/*
    Copyright 2005-2010 Intel Corporation.  All Rights Reserved.

    This file is part of Threading Building Blocks.

    Threading Building Blocks is free software; you can redistribute it
    and/or modify it under the terms of the GNU General Public License
    version 2 as published by the Free Software Foundation.

    Threading Building Blocks is distributed in the hope that it will be
    useful, but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Threading Building Blocks; if not, write to the Free Software
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

    As a special exception, you may use this file as part of a free software
    library without restriction.  Specifically, if other files instantiate
    templates or use macros or inline functions from this file, or you compile
    this file and link it with other files to produce an executable, this
    file does not by itself cause the resulting executable to be covered by
    the GNU General Public License.  This exception does not however
    invalidate any other reasons why the executable file might be covered by
    the GNU General Public License.
*/

//#define _CRT_SECURE_NO_DEPRECATE
#define VIDEO_WINMAIN_ARGS
#include "../../common/gui/video.h"
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>
#include <cassert>
#include <math.h>
#include "tbb/task_scheduler_init.h"
#include "tbb/blocked_range.h"
#include "tbb/parallel_for.h"
#include "tbb/tick_count.h"

using namespace std;

#ifdef _MSC_VER
// warning C4068: unknown pragma
#pragma warning(disable: 4068)
#endif

#define DEFAULT_NUMBER_OF_FRAMES 100
int number_of_frames = -1;
const size_t MAX_WIDTH = 1024;
const size_t MAX_HEIGHT = 512;

int UniverseHeight=MAX_HEIGHT;
int UniverseWidth=MAX_WIDTH;

typedef float value;

//! Velocity at each grid point
static value V[MAX_HEIGHT][MAX_WIDTH];

//! Horizontal stress
static value S[MAX_HEIGHT][MAX_WIDTH];

//! Vertical stress
static value T[MAX_HEIGHT][MAX_WIDTH];

//! Coefficient related to modulus
static value M[MAX_HEIGHT][MAX_WIDTH];

//! Coefficient related to lightness
static value L[MAX_HEIGHT][MAX_WIDTH];

//! Damping coefficients
static value D[MAX_HEIGHT][MAX_WIDTH];

/** Affinity is an argument to parallel_for to hint that an iteration of a loop 
    is best replayed on the same processor for each execution of the loop. 
    It is a global object because it must remember where the iterations happened
    in previous executions. */
static tbb::affinity_partitioner Affinity;

enum MaterialType {
    WATER=0,
    SANDSTONE=1,
    SHALE=2
};

//! Values are MaterialType, cast to an unsigned char to save space.
static unsigned char Material[MAX_HEIGHT][MAX_WIDTH];

static const colorcomp_t MaterialColor[4][3] = { // BGR
    {96,0,0},     // WATER
    {0,48,48},    // SANDSTONE
    {32,32,23}    // SHALE
};

static const int DamperSize = 32;

static const int ColorMapSize = 1024;
static color_t ColorMap[4][ColorMapSize];

static int PulseTime = 100;
static int PulseCounter;
static int PulseX = UniverseWidth/3;
static int PulseY = UniverseHeight/4;

static bool InitIsParallel = true;
const char *titles[2] = {"Seismic Simulation: Serial", "Seismic Simulation: Parallel"};
//! It is used for console mode for test with different number of threads and also has
//! meaning for gui: threads_low  - use sepatate event/updating loop thread (>0) or not (0).
//!                  threads_high - initialization value for scheduler
int threads_low = 0, threads_high = tbb::task_scheduler_init::automatic;

static void UpdatePulse() {
    if( PulseCounter>0 ) {
        value t = (PulseCounter-PulseTime/2)*0.05f;
        V[PulseY][PulseX] += 64*sqrt(M[PulseY][PulseX])*exp(-t*t);
        --PulseCounter;
    }
}

static void SerialUpdateStress() {
    drawing_area drawing(0, 0, UniverseWidth, UniverseHeight);
    for( int i=1; i<UniverseHeight-1; ++i ) {
        drawing.set_pos(1, i);
#pragma ivdep
        for( int j=1; j<UniverseWidth-1; ++j ) {
            S[i][j] += M[i][j]*(V[i][j+1]-V[i][j]);
            T[i][j] += M[i][j]*(V[i+1][j]-V[i][j]);
            int index = (int)(V[i][j]*(ColorMapSize/2)) + ColorMapSize/2;
            if( index<0 ) index = 0;
            if( index>=ColorMapSize ) index = ColorMapSize-1;
            color_t* c = ColorMap[Material[i][j]];
            drawing.put_pixel(c[index]);
        }
    }
}

struct UpdateStressBody {
    void operator()( const tbb::blocked_range<int>& range ) const {
        drawing_area drawing(0, range.begin(), UniverseWidth, range.end()-range.begin());
        int i_end = range.end();
        for( int y = 0, i=range.begin(); i!=i_end; ++i,y++ ) {
            drawing.set_pos(1, y);
#pragma ivdep
            for( int j=1; j<UniverseWidth-1; ++j ) {
                S[i][j] += M[i][j]*(V[i][j+1]-V[i][j]);
                T[i][j] += M[i][j]*(V[i+1][j]-V[i][j]);
                int index = (int)(V[i][j]*(ColorMapSize/2)) + ColorMapSize/2;
                if( index<0 ) index = 0;
                if( index>=ColorMapSize ) index = ColorMapSize-1;
                color_t* c = ColorMap[Material[i][j]];
                drawing.put_pixel(c[index]);
            }
        }
    }
};

static void ParallelUpdateStress() {
    tbb::parallel_for( tbb::blocked_range<int>( 1, UniverseHeight-1 ), // Index space for loop
                       UpdateStressBody(),                             // Body of loop
                       Affinity );                                     // Affinity hint
}

static void SerialUpdateVelocity() {
    for( int i=1; i<UniverseHeight-1; ++i ) 
#pragma ivdep
        for( int j=1; j<UniverseWidth-1; ++j ) 
            V[i][j] = D[i][j]*(V[i][j] + L[i][j]*(S[i][j] - S[i][j-1] + T[i][j] - T[i-1][j]));
}

struct UpdateVelocityBody {
    void operator()( const tbb::blocked_range<int>& range ) const {
        int i_end = range.end();
        for( int i=range.begin(); i!=i_end; ++i ) 
#pragma ivdep
            for( int j=1; j<UniverseWidth-1; ++j ) 
                V[i][j] = D[i][j]*(V[i][j] + L[i][j]*(S[i][j] - S[i][j-1] + T[i][j] - T[i-1][j]));
    }
};

static void ParallelUpdateVelocity() {
    tbb::parallel_for( tbb::blocked_range<int>( 1, UniverseHeight-1 ), // Index space for loop
                       UpdateVelocityBody(),                           // Body of loop
                       Affinity );                                     // Affinity hint
}

void SerialUpdateUniverse() {
    UpdatePulse();
    SerialUpdateStress();
    SerialUpdateVelocity();
}

void ParallelUpdateUniverse() {
    UpdatePulse();
    ParallelUpdateStress();
    ParallelUpdateVelocity();
}

class seismic_video : public video
{
    void on_mouse(int x, int y, int key) {
        if(key == 1 && PulseCounter == 0) {
            PulseCounter = PulseTime;
            PulseX = x; PulseY = y;
        }
    }
    void on_key(int key) {
        key &= 0xff;
        if(char(key) == ' ') InitIsParallel = !InitIsParallel;
        else if(char(key) == 'p') InitIsParallel = true;
        else if(char(key) == 's') InitIsParallel = false;
        else if(char(key) == 'e') updating = true;
        else if(char(key) == 'd') updating = false;
        else if(key == 27) running = false;
        title = InitIsParallel?titles[1]:titles[0];
    }
    void on_process() {
        tbb::task_scheduler_init Init(threads_high);
        do {
            if( InitIsParallel )
                ParallelUpdateUniverse();
            else
                SerialUpdateUniverse();
            if( number_of_frames > 0 ) --number_of_frames;
        } while(next_frame() && number_of_frames);
    }
} video;

void InitializeUniverse() {
    PulseCounter = PulseTime;
    // Initialize V, S, and T to slightly non-zero values, in order to avoid denormal waves.
    for( int i=0; i<UniverseHeight; ++i ) 
#pragma ivdep
        for( int j=0; j<UniverseWidth; ++j ) {
            T[i][j] = S[i][j] = V[i][j] = value(1.0E-6);
        }
    for( int i=1; i<UniverseHeight-1; ++i ) {
        for( int j=1; j<UniverseWidth-1; ++j ) {
            float x = float(j-UniverseWidth/2)/(UniverseWidth/2);
            value t = (value)i/UniverseHeight;
            MaterialType m;
            D[i][j] = 1.0;
            // Coefficient values are fictitious, and chosen to visually exaggerate 
            // physical effects such as Rayleigh waves.  The fabs/exp line generates
            // a shale layer with a gentle upwards slope and an anticline.
            if( t<0.3f ) {
                m = WATER;
                M[i][j] = 0.125;
                L[i][j] = 0.125;
            } else if( fabs(t-0.7+0.2*exp(-8*x*x)+0.025*x)<=0.1 ) {
                m = SHALE;
                M[i][j] = 0.5;
                L[i][j] = 0.6;
            } else {
                m = SANDSTONE;
                M[i][j] = 0.3;
                L[i][j] = 0.4;
            } 
            Material[i][j] = m;
        }
    }
    value scale = 2.0f/ColorMapSize;
    for( int k=0; k<4; ++k ) {
        for( int i=0; i<ColorMapSize; ++i ) {
            colorcomp_t c[3];
            value t = (i-ColorMapSize/2)*scale;
            value r = t>0 ? t : 0;
            value b = t<0 ? -t : 0;
            value g = 0.5f*fabs(t);
            memcpy(c, MaterialColor[k], sizeof(c));
            c[2] = colorcomp_t(r*(255-c[2])+c[2]);
            c[1] = colorcomp_t(g*(255-c[1])+c[1]);
            c[0] = colorcomp_t(b*(255-c[0])+c[0]);
            ColorMap[k][i] = video.get_color(c[2], c[1], c[0]);
        }
    }
    // Set damping coefficients around border to reduce reflections from boundaries.
    value d = 1.0;
    for( int k=DamperSize-1; k>0; --k ) {
        d *= 1-1.0f/(DamperSize*DamperSize);
        for( int j=1; j<UniverseWidth-1; ++j ) {
            D[k][j] *= d;
            D[UniverseHeight-1-k][j] *= d;
        }
        for( int i=1; i<UniverseHeight-1; ++i ) {
            D[i][k] *= d;
            D[i][UniverseWidth-1-k] *= d;
        }
    }
}

//////////////////////////////// Interface ////////////////////////////////////
#ifdef _WINDOWS
#include "msvs/resource.h"
#endif

int main(int argc, char *argv[])
{
    // threads number init
    if(argc > 1 && isdigit(argv[1][0])) {
        char* end; threads_high = threads_low = (int)strtol(argv[1],&end,0);
        switch( *end ) {
            case ':': threads_high = (int)strtol(end+1,0,0); break;
            case '\0': break;
            default: printf("unexpected character = %c\n",*end);
        }
    }
    if (argc > 2 && isdigit(argv[2][0])){
        number_of_frames = (int)strtol(argv[2],0,0);
    }
    // video layer init
    video.title = InitIsParallel?titles[1]:titles[0];
#ifdef _WINDOWS
    #define MAX_LOADSTRING 100
    TCHAR szWindowClass[MAX_LOADSTRING];    // the main window class name
    LoadStringA(video::win_hInstance, IDC_SEISMICSIMULATION, szWindowClass, MAX_LOADSTRING);
    LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
    WNDCLASSEX wcex; memset(&wcex, 0, sizeof(wcex));
    wcex.lpfnWndProc    = (WNDPROC)WndProc;
    wcex.hIcon          = LoadIcon(video::win_hInstance, MAKEINTRESOURCE(IDI_SEISMICSIMULATION));
    wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = LPCTSTR(IDC_SEISMICSIMULATION);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(video::win_hInstance, MAKEINTRESOURCE(IDI_SMALL));
    video.win_set_class(wcex); // ascii convention here
    video.win_load_accelerators(IDC_SEISMICSIMULATION);
#endif
    if(video.init_window(UniverseWidth, UniverseHeight)) {
        video.calc_fps = true;
        video.threaded = threads_low > 0;
        // video is ok, init universe
        InitializeUniverse();
        // main loop
        video.main_loop();
    }
    else if(video.init_console()) {
        // do console mode
        if(number_of_frames <= 0) number_of_frames = DEFAULT_NUMBER_OF_FRAMES;
        if(threads_high == tbb::task_scheduler_init::automatic) threads_high = 4;
        if(threads_high < threads_low) threads_high = threads_low;
        for( int p = threads_low; p <= threads_high; ++p ) {
            InitializeUniverse();
            tbb::task_scheduler_init init(tbb::task_scheduler_init::deferred);
            if( p > 0 )
                init.initialize( p );
            tbb::tick_count t0 = tbb::tick_count::now();
            if( p > 0 )
                for( int i=0; i<number_of_frames; ++i )
                    ParallelUpdateUniverse();
            else
                for( int i=0; i<number_of_frames; ++i )
                    SerialUpdateUniverse();
            tbb::tick_count t1 = tbb::tick_count::now();
            printf("%.1f frame per sec", number_of_frames/(t1-t0).seconds());
            if( p > 0 ) 
                printf(" with %d way parallelism\n",p);
            else
                printf(" with serial version\n"); 
        }
    }
    video.terminate();
    return 0;
}

#ifdef _WINDOWS
//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_INITDIALOG: return TRUE;
    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, LOWORD(wParam));
            return TRUE;
        }
        break;
    }
    return FALSE;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int wmId, wmEvent;
    switch (message) {
    case WM_COMMAND:
        wmId    = LOWORD(wParam); 
        wmEvent = HIWORD(wParam); 
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(video::win_hInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, (DLGPROC)About);
            break;
        case IDM_EXIT:
            PostQuitMessage(0);
            break;
        case ID_FILE_PARALLEL:
            if( !InitIsParallel ) {
                InitIsParallel = true;
                video.title = titles[1];
            }
            break;
        case ID_FILE_SERIAL:
            if( InitIsParallel ) {
                InitIsParallel = false;
                video.title = titles[0];
            }
            break;
        case ID_FILE_ENABLEGUI:
            video.updating = true;
            break;
        case ID_FILE_DISABLEGUI:
            video.updating = false;
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

#endif
