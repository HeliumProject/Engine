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

/*
 *  Based on "OpenGL Image" example from http://developer.apple.com/samplecode/OpenGL_Image/
 */

#include "video.h"
#include <sched.h>
#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <pthread.h>

#include <AvailabilityMacros.h>
#undef DEPRECATED_ATTRIBUTE
#define DEPRECATED_ATTRIBUTE
#include <Carbon/Carbon.h>
#include <AGL/agl.h>
#include <OpenGL/gl.h>    // for OpenGL API
#include <OpenGL/glext.h> // for OpenGL extension support 

unsigned int *      g_pImg = 0;
int                 g_sizex, g_sizey;
WindowRef           g_window = 0;
static video *      g_video = 0;
static int          g_fps = 0;
struct timeval      g_time;
static pthread_mutex_t  g_mutex = PTHREAD_MUTEX_INITIALIZER;


static OSStatus     AppEventHandler( EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon );
WindowRef           HandleNew();
static OSStatus     WindowEventHandler( EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon );

static IBNibRef     sNibRef;

//-------------------------------------------------------------------------------------------- 

// structure for creating a fullscreen context
struct structGLInfo // storage for setup info
{
    SInt16 width;               // input: width of drawable (screen width in full screen mode), return: actual width allocated
    SInt16 height;              // input: height of drawable (screen height in full screen mode), return: actual height allocated
    UInt32 pixelDepth;          // input: requested pixel depth
    Boolean fDepthMust;         // input: pixel depth must be set (if false then current depth will be used if able)
    Boolean fAcceleratedMust;   // input: must renderer be accelerated?
    GLint aglAttributes[64];    // input: pixel format attributes always required (reset to what was actually allocated)
    SInt32 VRAM;                // input: minimum VRAM; output: actual (if successful otherwise input)
    SInt32 textureRAM;          // input: amount of texture RAM required on card; output: same (used in allcoation to ensure enough texture
    AGLPixelFormat    fmt;      // input: none; output pixel format...
};
typedef struct structGLInfo structGLInfo;
typedef struct structGLInfo * pstructGLInfo;

// structure for creating a context from a window
struct structGLWindowInfo // storage for setup info
{
    Boolean fAcceleratedMust;   // input: must renderer be accelerated?
    GLint aglAttributes[64];    // input: pixel format attributes always required (reset to what was actually allocated)
    SInt32 VRAM;                // input: minimum VRAM; output: actual (if successful otherwise input)
    SInt32 textureRAM;          // input: amount of texture RAM required on card; output: same (used in allcoation to ensure enough texture
    AGLPixelFormat    fmt;      // input: none; output pixel format...
    Boolean fDraggable;         // input: is window going to be dragable, 
                                //        if so renderer check (accel, VRAM, textureRAM) will look at all renderers vice just the current one
                                //        if window is not dragable renderer check will either check the single device or short 
                                //            circuit to software if window spans multiple devices 
                                //        software renderer is consider to have unlimited VRAM, unlimited textureRAM and to not be accelerated
};
typedef struct structGLWindowInfo structGLWindowInfo;
typedef struct structGLWindowInfo * pstructGLWindowInfo;

//-------------------------------------------------------------------------------------------- 

struct recGLCap // structure to store minimum OpenGL capabilites across all displays and GPUs
{
    Boolean f_ext_texture_rectangle; // is texture rectangle extension supported
    Boolean f_ext_client_storage; // is client storage extension supported
    Boolean f_ext_packed_pixel; // is packed pixel extension supported
    Boolean f_ext_texture_edge_clamp; // is SGI texture edge clamp extension supported
    Boolean f_gl_texture_edge_clamp; // is OpenGL texture edge clamp support (1.2+)
    unsigned long edgeClampParam; // the param that is passed to the texturing parmeteres
    long maxTextureSize; // the minimum max texture size across all GPUs
    long maxNOPTDTextureSize; // the minimum max texture size across all GPUs that support non-power of two texture dimensions
};
typedef struct recGLCap recGLCap;
typedef recGLCap * pRecGLCap;

struct recImage // OpenGL and image information associated with each window
{
    // genric OpenGL stuff
    structGLWindowInfo glInfo;  // gl info used with SetupGL to build context
    AGLContext aglContext;      // the OpenGL context (read: state)
    GLuint fontList;            // the display list storing the bitmap font created for the context to display info
    
    Boolean fAGPTexturing;      // 10.1+ only: texture from AGP memory without loading to GPU
    
    // texture display stuff
    Boolean fNPOTTextures; // are we using Non-Power Of Two (NPOT) textures?
    Boolean fTileTextures; // are multiple tiled textures used to display image?
    Boolean fOverlapTextures; // do tiled textures overlapped to create correct filtering between tiles? (only applies if using tiled textures)
    Boolean fClientTextures; // 10.1+ only: texture from client memory
        
    unsigned char * pImageBuffer; // image buffer that contains data for image (disposed after loading into texture if not using client textures)
    long imageWidth; // height of orginal image
    long imageHeight; // width of orginal image
    float imageAspect; // width / height or aspect ratio of orginal image
    long imageDepth; // depth of image (after loading into gworld, will be either 32 or 16 bits)
    long textureX; // number of horizontal textures
    long textureY; // number of vertical textures
    long maxTextureSize; // max texture size for image
    GLuint * pTextureName; // array for texture names (# = textureX * textureY)
    long textureWidth; // total width of texels with cover image (including any border on image, but not internal texture overlaps)
    long textureHeight; // total height of texels with cover image (including any border on image, but not internal texture overlaps)
    float zoomX; // zoom from on texel = one pixel is 1.0
    float zoomY; // zoom from on texel = one pixel is 1.0
 };
typedef struct recImage recImage; // typedef for easy declaration
typedef recImage * pRecImage; // pointer type

// ==================================

// public function declarations -------------------------------------

// Destroys drawable and context
// Ouputs: *paglDraw, *paglContext should be 0 on exit
// destorys a context that was associated with an existing window, window is left intacted
OSStatus DestroyGLFromWindow (AGLContext* paglContext, pstructGLWindowInfo pcontextInfo);

short FindGDHandleFromWindow (WindowPtr pWindow, GDHandle * phgdOnThisDevice);

// disposes OpenGL context, and associated texture list
OSStatus DisposeGLForWindow (WindowRef window);

// builds the GL context and associated state for the window
// loads image into a texture or textures
// disposes of GWorld and image buffer when finished loading textures
OSStatus BuildGLForWindow (WindowRef window);

// Handle updating context for window moves and resizing
OSStatus ResizeMoveGLWindow (WindowRef window);

// main GL drawing routine, should be valid window passed in (will setupGL if require).  Draw image
void DrawGL (WindowRef window);

pRecGLCap gpOpenGLCaps; 

// prototypes (internal/private) --------------------------------------------

static Boolean CheckRenderer (GDHandle hGD, long *VRAM, long *textureRAM, GLint*  , Boolean fAccelMust);
static Boolean CheckAllDeviceRenderers (long* pVRAM, long* pTextureRAM, GLint* pDepthSizeSupport, Boolean fAccelMust);
static void DumpCurrent (AGLDrawable* paglDraw, AGLContext* paglContext, pstructGLInfo pcontextInfo);
static OSStatus BuildGLonWindow (WindowPtr pWindow, AGLContext* paglContext, pstructGLWindowInfo pcontextInfo, AGLContext aglShareContext);

static long GetNextTextureSize (long textureDimension, long maxTextureSize, Boolean textureRectangle);
static long GetTextureNumFromTextureDim (long textureDimension, long maxTextureSize, Boolean texturesOverlap, Boolean textureRectangle);

// ----------------------------------------------------------------------------------------

// functions (internal/private) ---------------------------------------------

#pragma mark -
// --------------------------------------------------------------------------

// central error reporting

void ReportErrorNum (char * strError, long numError)
{
    char errMsgPStr [257];
    
    errMsgPStr[0] = (char)snprintf (errMsgPStr+1, 255, "%s %ld (0x%lx)\n", strError, numError, numError); 

    // ensure we are faded in
    DebugStr ( (ConstStr255Param) errMsgPStr );
}

// --------------------------------------------------------------------------

void ReportError (char * strError)
{
    char errMsgPStr [257];
    
    errMsgPStr[0] = (char)snprintf (errMsgPStr+1, 255, "%s\n", strError); 

    // ensure we are faded in
    DebugStr ( (ConstStr255Param) errMsgPStr );
}

//-----------------------------------------------------------------------------------------------------------------------

// if error dump agl errors to debugger string, return error

OSStatus aglReportError (void)
{
    GLenum err = aglGetError();
    if (AGL_NO_ERROR != err)
        ReportError ((char *)aglErrorString(err));
    // ensure we are returning an OSStatus noErr if no error condition
    if (err == AGL_NO_ERROR)
        return noErr;
    else
        return (OSStatus) err;
}

//-----------------------------------------------------------------------------------------------------------------------

// if error dump gl errors to debugger string, return error

OSStatus glReportError (void)
{
    GLenum err = glGetError();
    switch (err)
    {
        case GL_NO_ERROR:
            break;
        case GL_INVALID_ENUM:
            ReportError ("GL Error: Invalid enumeration");
            break;
        case GL_INVALID_VALUE:
            ReportError ("GL Error: Invalid value");
            break;
        case GL_INVALID_OPERATION:
            ReportError ("GL Error: Invalid operation");
            break;
        case GL_STACK_OVERFLOW:
            ReportError ("GL Error: Stack overflow");
            break;
        case GL_STACK_UNDERFLOW:
            ReportError ("GL Error: Stack underflow");
            break;
        case GL_OUT_OF_MEMORY:
            ReportError ("GL Error: Out of memory");
            break;
    }
    // ensure we are returning an OSStatus noErr if no error condition
    if (err == GL_NO_ERROR)
        return noErr;
    else
        return (OSStatus) err;
}

//--------------------------------------------------------------------------------------------

// functions (internal/private) ---------------------------------------------

// CheckRenderer

// looks at renderer attributes it has at least the VRAM is accelerated

// Inputs:     hGD: GDHandle to device to look at
//            pVRAM: pointer to VRAM in bytes required; out is actual VRAM if a renderer was found, otherwise it is the input parameter
//            pTextureRAM:  pointer to texture RAM in bytes required; out is same (implementation assume VRAM returned by card is total so we add texture and VRAM)
//            fAccelMust: do we check for acceleration

// Returns: true if renderer for the requested device complies, false otherwise

static Boolean CheckRenderer (GDHandle hGD, long* pVRAM, long* pTextureRAM, GLint* pDepthSizeSupport, Boolean fAccelMust)
{
    AGLRendererInfo info, head_info;
    GLint inum;
    GLint dAccel = 0;
    GLint dVRAM = 0, dMaxVRAM = 0;
    Boolean canAccel = false, found = false;
    head_info = aglQueryRendererInfo(&hGD, 1);
    aglReportError ();
    if(!head_info)
    {
        ReportError ("aglQueryRendererInfo error");
        return false;
    }
    else
    {
        info = head_info;
        inum = 0;
        // see if we have an accelerated renderer, if so ignore non-accelerated ones
        // this prevents returning info on software renderer when actually we'll get the hardware one
        while (info)
        {    
            aglDescribeRenderer(info, AGL_ACCELERATED, &dAccel);
            aglReportError ();
            if (dAccel)
                canAccel = true;
            info = aglNextRendererInfo(info);
            aglReportError ();
            inum++;
        }
            
        info = head_info;
        inum = 0;
        while (info)
        {
            aglDescribeRenderer (info, AGL_ACCELERATED, &dAccel);
            aglReportError ();
            // if we can accel then we will choose the accelerated renderer 
            // how about compliant renderers???
            if ((canAccel && dAccel) || (!canAccel && (!fAccelMust || dAccel)))
            {
                aglDescribeRenderer (info, AGL_VIDEO_MEMORY, &dVRAM);    // we assume that VRAM returned is total thus add texture and VRAM required
                aglReportError ();
                if (dVRAM >= (*pVRAM + *pTextureRAM))
                {
                    if (dVRAM >= dMaxVRAM) // find card with max VRAM
                    {
                        aglDescribeRenderer (info, AGL_DEPTH_MODES, pDepthSizeSupport);    // which depth buffer modes are supported
                        aglReportError ();
                        dMaxVRAM = dVRAM; // store max
                        found = true;
                    }
                }
            }
            info = aglNextRendererInfo(info);
            aglReportError ();
            inum++;
        }
    }
    aglDestroyRendererInfo(head_info);
    if (found) // if we found a card that has enough VRAM and meets the accel criteria
    {
        *pVRAM = dMaxVRAM; // return VRAM
        return true;
    }
    // VRAM will remain to same as it did when sent in
    return false;
}

//-----------------------------------------------------------------------------------------------------------------------

// CheckAllDeviceRenderers 

// looks at renderer attributes and each device must have at least one renderer that fits the profile

// Inputs:     pVRAM: pointer to VRAM in bytes required; out is actual min VRAM of all renderers found, otherwise it is the input parameter
//            pTextureRAM:  pointer to texture RAM in bytes required; out is same (implementation assume VRAM returned by card is total so we add texture and VRAM)
//            fAccelMust: do we check fro acceleration

// Returns: true if any renderer for on each device complies (not necessarily the same renderer), false otherwise

static Boolean CheckAllDeviceRenderers (long* pVRAM, long* pTextureRAM, GLint* pDepthSizeSupport, Boolean fAccelMust)
{
    AGLRendererInfo info, head_info;
    GLint inum;
    GLint dAccel = 0;
    GLint dVRAM = 0, dMaxVRAM = 0;
    Boolean canAccel = false, found = false, goodCheck = true; // can the renderer accelerate, did we find a valid renderer for the device, are we still successfully on all the devices looked at
    long MinVRAM = 0x8FFFFFFF; // max long
    GDHandle hGD = GetDeviceList (); // get the first screen
    while (hGD && goodCheck)
    {
        head_info = aglQueryRendererInfo(&hGD, 1);
        aglReportError ();
        if(!head_info)
        {
            ReportError ("aglQueryRendererInfo error");
            return false;
        }
        else
        {
            info = head_info;
            inum = 0;
            // see if we have an accelerated renderer, if so ignore non-accelerated ones
            // this prevents returning info on software renderer when actually we'll get the hardware one
            while (info)
            {
                aglDescribeRenderer(info, AGL_ACCELERATED, &dAccel);
                aglReportError ();
                if (dAccel)
                    canAccel = true;
                info = aglNextRendererInfo(info);
                aglReportError ();
                inum++;
            }
                
            info = head_info;
            inum = 0;
            while (info)
            {    
                aglDescribeRenderer(info, AGL_ACCELERATED, &dAccel);
                aglReportError ();
                // if we can accel then we will choose the accelerated renderer 
                // how about compliant renderers???
                if ((canAccel && dAccel) || (!canAccel && (!fAccelMust || dAccel)))
                {
                    aglDescribeRenderer(info, AGL_VIDEO_MEMORY, &dVRAM);    // we assume that VRAM returned is total thus add texture and VRAM required
                    aglReportError ();
                    if (dVRAM >= (*pVRAM + *pTextureRAM))
                    {
                        if (dVRAM >= dMaxVRAM) // find card with max VRAM
                        {
                            aglDescribeRenderer(info, AGL_DEPTH_MODES, pDepthSizeSupport);    // which depth buffer modes are supported
                            aglReportError ();
                            dMaxVRAM = dVRAM; // store max
                            found = true;
                        }
                    }
                }
                info = aglNextRendererInfo(info);
                aglReportError ();
                inum++;
            }
        }
        aglDestroyRendererInfo(head_info);
        if (found) // if we found a card that has enough VRAM and meets the accel criteria
        {
            if (MinVRAM > dMaxVRAM)
                MinVRAM = dMaxVRAM; // return VRAM
            
        }
        else
            goodCheck = false; // one device failed thus entire requirement fails
        hGD = GetNextDevice (hGD); // get next device
    } // while
    if (goodCheck) // we check all devices and each was good
    {
        *pVRAM = MinVRAM; // return VRAM
        return true;
    }
    return false; //at least one device failed to have mins
}

//-----------------------------------------------------------------------------------------------------------------------

// DumpCurrent

// Kills currently allocated context
// does not care about being pretty (assumes display is likely faded)

// Inputs:     paglDraw, paglContext: things to be destroyed

void DumpCurrent (AGLDrawable* paglDraw, AGLContext* paglContext, pstructGLInfo pcontextInfo)
{
    if (*paglContext)
    {
        aglSetCurrentContext (NULL);
        aglReportError ();
        aglSetDrawable (*paglContext, NULL);
        aglReportError ();
        aglDestroyContext (*paglContext);
        aglReportError ();
        *paglContext = NULL;
    }
    
    if (pcontextInfo->fmt)
    {
        aglDestroyPixelFormat (pcontextInfo->fmt); // pixel format is no longer needed
        aglReportError ();
    }
    pcontextInfo->fmt = 0;

    if (*paglDraw) // do not destory a window on DSp since there is no window built in X
        DisposeWindow (GetWindowFromPort (*paglDraw));
    *paglDraw = NULL;
}

#pragma mark -
// --------------------------------------------------------------------------

// BuildGLonWindow

static OSStatus BuildGLonWindow (WindowPtr pWindow, AGLContext* paglContext, pstructGLWindowInfo pcontextInfo, AGLContext aglShareContext)
{
    GDHandle hGD = NULL;
    GrafPtr cgrafSave = NULL;
    short numDevices;
    GLint depthSizeSupport;
    OSStatus err = noErr;
    
    if (!pWindow || !pcontextInfo)
    {
        ReportError ("NULL parameter passed to BuildGLonWindow.");
        return paramErr;
    }
    
    GetPort (&cgrafSave);
    SetPortWindowPort(pWindow);

    // check renderere VRAM and acceleration
    numDevices = FindGDHandleFromWindow (pWindow, &hGD);
    if (!pcontextInfo->fDraggable)     // if numDevices > 1 then we will only be using the software renderer otherwise check only window device
    {
        if ((numDevices > 1) || (numDevices == 0)) // this window spans mulitple devices thus will be software only
        {
            // software renderer
            // infinite VRAM, infinite textureRAM, not accelerated
            if (pcontextInfo->fAcceleratedMust)
            {
                ReportError ("Unable to accelerate window that spans multiple devices");
                return err;
            }
        }
        else // not draggable on single device
        {
            if (!CheckRenderer (hGD, &(pcontextInfo->VRAM), &(pcontextInfo->textureRAM), &depthSizeSupport, pcontextInfo->fAcceleratedMust))
            {
                ReportError ("Renderer check failed");
                return err;
            }
        }
    }
    // else draggable so must check all for support (each device should have at least one renderer that meets the requirements)
    else if (!CheckAllDeviceRenderers (&(pcontextInfo->VRAM), &(pcontextInfo->textureRAM), &depthSizeSupport, pcontextInfo->fAcceleratedMust))
    {
        ReportError ("Renderer check failed");
        return err;
    }
    
    // do agl
    if ((Ptr) kUnresolvedCFragSymbolAddress == (Ptr) aglChoosePixelFormat) // check for existance of OpenGL
    {
        ReportError ("OpenGL not installed");
        return noErr;
    }    
    // we successfully passed the renderer check

    if ((!pcontextInfo->fDraggable && (numDevices == 1)))  // not draggable on a single device
        pcontextInfo->fmt = aglChoosePixelFormat (&hGD, 1, pcontextInfo->aglAttributes); // get an appropriate pixel format
    else
        pcontextInfo->fmt = aglChoosePixelFormat (NULL, 0, pcontextInfo->aglAttributes); // get an appropriate pixel format
    aglReportError ();
    if (NULL == pcontextInfo->fmt) 
    {
        ReportError("Could not find valid pixel format");
        return noErr;
    }

    *paglContext = aglCreateContext (pcontextInfo->fmt, aglShareContext); // Create an AGL context
    if (AGL_BAD_MATCH == aglGetError())
        *paglContext = aglCreateContext (pcontextInfo->fmt, 0); // unable to sahre context, create without sharing
    aglReportError ();
    if (NULL == *paglContext) 
    {
        ReportError ("Could not create context");
        return noErr;
    }
    
    if (!aglSetDrawable (*paglContext, GetWindowPort (pWindow))) // attach the CGrafPtr to the context
        return aglReportError ();
    
    if(!aglSetCurrentContext (*paglContext)) // make the context the current context
        return aglReportError ();

    SetPort (cgrafSave);

    return err;
}

#pragma mark -

// functions (public) -------------------------------------------------------

// DestroyGLFromWindow

// Destroys context that waas allocated with BuildGLonWindow
// Ouputs: *paglContext should be NULL on exit

OSStatus DestroyGLFromWindow (AGLContext* paglContext, pstructGLWindowInfo pcontextInfo)
{
    OSStatus err;
    
    if ((!paglContext) || (!*paglContext))
        return paramErr; // not a valid context
    glFinish ();
    aglSetCurrentContext (NULL);
    err = aglReportError ();
    aglSetDrawable (*paglContext, NULL);
    err = aglReportError ();
    aglDestroyContext (*paglContext);
    err = aglReportError ();
    *paglContext = NULL;

    if (pcontextInfo->fmt)
    {
        aglDestroyPixelFormat (pcontextInfo->fmt); // pixel format is no longer valid
        err = aglReportError ();
    }
    pcontextInfo->fmt = 0;
    
    return err;
}

//-----------------------------------------------------------------------------------------------------------------------

// GetWindowDevice

// Inputs:    a valid WindowPtr

// Outputs:    the GDHandle that that window is mostly on

// returns the number of devices that the windows content touches

short FindGDHandleFromWindow (WindowPtr pWindow, GDHandle * phgdOnThisDevice)
{
    GrafPtr pgpSave;
    Rect rectWind, rectSect;
    long greatestArea, sectArea;
    short numDevices = 0;
    GDHandle hgdNthDevice;
    
    if (!pWindow || !phgdOnThisDevice)
        return 0;
        
    *phgdOnThisDevice = NULL;
    
    GetPort (&pgpSave);
    SetPortWindowPort (pWindow);
    

    GetWindowPortBounds (pWindow, &rectWind);
    LocalToGlobal ((Point*)& rectWind.top);    // convert to global coordinates
    LocalToGlobal ((Point*)& rectWind.bottom);
    hgdNthDevice = GetDeviceList ();
    greatestArea = 0;
    // check window against all gdRects in gDevice list and remember 
    //  which gdRect contains largest area of window}
    while (hgdNthDevice)
    {
        if (TestDeviceAttribute (hgdNthDevice, screenDevice))
            if (TestDeviceAttribute (hgdNthDevice, screenActive))
            {
                // The SectRect routine calculates the intersection 
                //  of the window rectangle and this gDevice 
                //  rectangle and returns TRUE if the rectangles intersect, 
                //  FALSE if they don't.
                SectRect (&rectWind, &(**hgdNthDevice).gdRect, &rectSect);
                // determine which screen holds greatest window area
                //  first, calculate area of rectangle on current device
                sectArea = (long) (rectSect.right - rectSect.left) * (rectSect.bottom - rectSect.top);
                if (sectArea > 0)
                    numDevices++;
                if (sectArea > greatestArea)
                {
                    greatestArea = sectArea; // set greatest area so far
                    *phgdOnThisDevice = hgdNthDevice; // set zoom device
                }
                hgdNthDevice = GetNextDevice(hgdNthDevice);
            }
    }
    
    SetPort (pgpSave);
    return numDevices;
}

//--------------------------------------------------------------------------------------------
// private

// returns the largest power of 2 texture <= textureDimension
// or in the case of texture rectangle returns the next texture size (can be non-power of two)

static long GetNextTextureSize (long textureDimension, long maxTextureSize, Boolean textureRectangle)
{
    long targetTextureSize = maxTextureSize; // start at max texture size
    if (textureRectangle)
    {
        if (textureDimension >= targetTextureSize) // the texture dimension is greater than the target texture size (i.e., it fits)
            return targetTextureSize; // return corresponding texture size
        else
            return textureDimension; // jusr return the dimension
    }
    else
    {
        do // while we have txture sizes check for texture value being equal or greater
        {  
            if (textureDimension >= targetTextureSize) // the texture dimension is greater than the target texture size (i.e., it fits)
                return targetTextureSize; // return corresponding texture size
        }
        while (targetTextureSize >>= 1); // step down to next texture size smaller
    }
    return 0; // no textures fit so return zero
}

// ---------------------------------

// returns the nuber of textures need to represent a size of textureDimension given
// requirement for power of 2 textures as the maximum texture size
// for the overlap case each texture effectively covers two less pixels so must iterate through using whole statement

static long GetTextureNumFromTextureDim (long textureDimension, long maxTextureSize, Boolean texturesOverlap, Boolean textureRectangle) 
{
    // start at max texture size 
    // loop through each texture size, removing textures in turn which are less than the remaining texture dimension
    // each texture has 2 pixels of overlap (one on each side) thus effective texture removed is 2 less than texture size
    
    long i = 0; // initially no textures
    long bitValue = maxTextureSize; // start at max texture size
    long texOverlapx2 = texturesOverlap ? 2 : 0;
    textureDimension -= texOverlapx2; // ignore texture border since we are using effective texure size (by subtracting 2 from the initial size)
    if (textureRectangle)
    {
        // count number of full textures
        while (textureDimension > (bitValue - texOverlapx2)) // while our texture dimension is greater than effective texture size (i.e., minus the border)
        {
            i++; // count a texture
            textureDimension -= bitValue - texOverlapx2; // remove effective texture size
        }
        // add one partial texture
        i++; 
    }
    else
    {
        do
        {
            while (textureDimension >= (bitValue - texOverlapx2)) // while our texture dimension is greater than effective texture size (i.e., minus the border)
            {
                i++; // count a texture
                textureDimension -= bitValue - texOverlapx2; // remove effective texture size
            }
        }
        while ((bitValue >>= 1) > texOverlapx2); // step down to next texture while we are greater than two (less than 4 can't be used due to 2 pixel overlap)
    if (textureDimension > 0x0) // if any textureDimension is left there is an error, because we can't texture these small segments and in anycase should not have image pixels left
        ReportErrorNum ("GetTextureNumFromTextureDim error: Texture to small to draw, should not ever get here, texture size remaining:", textureDimension);
    }
    return i; // return textures counted
} 

#pragma mark -
// ==================================
// public

// disposes OpenGL context, and associated texture list

OSStatus DisposeGLForWindow (WindowRef window)
{
    if (window)
    {
        pRecImage pWindowInfo = (pRecImage) GetWRefCon (window); // get gl data stored in refcon
        SetWRefCon (window, 0); // ensure the refcon is not used again
        if (NULL == pWindowInfo) // if this is non-existant
            return paramErr; // then drop out
        if (NULL != pWindowInfo->aglContext)
        {
            aglSetCurrentContext (pWindowInfo->aglContext); // ensaure the context we are working with is set to current
            aglUpdateContext (pWindowInfo->aglContext); // ensaure the context we are working with is set to current
            glFinish (); // ensure all gl commands are complete
            glDeleteTextures (pWindowInfo->textureX * pWindowInfo->textureY, pWindowInfo->pTextureName); // delete the complete set of textures used for the window
            DestroyGLFromWindow (&pWindowInfo->aglContext, &pWindowInfo->glInfo); // preoperly destroy GL context and any associated structures
            pWindowInfo->aglContext = NULL; // ensure we don't use invlad context
        }
        if (NULL != pWindowInfo->pTextureName)
        {
            DisposePtr ((Ptr) pWindowInfo->pTextureName); // dispose of the allocate4d texture name storage
            pWindowInfo->pTextureName = NULL; // ensure we do not use it again
        }
        if (pWindowInfo->pImageBuffer) // MUST preserve the buffer if texturing from client memory
        {
            //DisposePtr ((Ptr) pWindowInfo->pImageBuffer); // or image buffer
            pWindowInfo->pImageBuffer = NULL;
        }
        DisposePtr ((Ptr) pWindowInfo);
        return noErr; // we are good to go
    }
    else
        return paramErr; // NULL window ref passed in
}

// ---------------------------------

// builds the GL context and associated state for the window
// loads image into a texture or textures
// disposes of GWorld and image buffer when finished loading textures

OSStatus BuildGLForWindow (WindowRef window)
{
    GrafPtr portSave = NULL; // port which is set on entrance to this routine
    pRecImage pWindowInfo = (pRecImage) GetWRefCon (window); // the info structure for the window stored in the refcon
    short i; // iterator
    GLenum textureTarget = GL_TEXTURE_2D;
   
    if (!pWindowInfo->aglContext) // if we get here and do not have a context built, build one
    {
        GetPort (&portSave);    // save current port
        SetPort ((GrafPtr) GetWindowPort (window)); // set port to the current window
        // set parameters for Carbon SetupGL
        pWindowInfo->glInfo.fAcceleratedMust = false; // must renderer be accelerated?
        pWindowInfo->glInfo.VRAM = 0 * 1048576; // minimum VRAM (if not zero this is always required)
        pWindowInfo->glInfo.textureRAM = 0 * 1048576; // minimum texture RAM (if not zero this is always required)
           pWindowInfo->glInfo.fDraggable = true; // is this a draggable window
        pWindowInfo->glInfo.fmt = 0; // output pixel format
        
        i = 0; // first attribute in array
        pWindowInfo->glInfo.aglAttributes [i++] = AGL_RGBA; // RGB + Alpha pixels
        pWindowInfo->glInfo.aglAttributes [i++] = AGL_DOUBLEBUFFER; // doble buffered context
        pWindowInfo->glInfo.aglAttributes [i++] = AGL_ACCELERATED; // require hardware acceleration
        pWindowInfo->glInfo.aglAttributes [i++] = AGL_NO_RECOVERY; // 10.0.4 has problems with the GL (disregards UNPACK_ROW_LENGTH) resulting from using no recovery
                                                                   // normally we would use no recovery to ensure the minimum pixel size textures are stored by GL.
        pWindowInfo->glInfo.aglAttributes [i++] = AGL_NONE; // end parameter list
        BuildGLonWindow (window, &(pWindowInfo->aglContext), &(pWindowInfo->glInfo), NULL); // build opengl context for our window
        if (!pWindowInfo->aglContext) // if could not create context
            DestroyGLFromWindow (&pWindowInfo->aglContext, &pWindowInfo->glInfo); // ensure context is destroyed correctly
        else // we have a valid context
        {
            GLint swap = 0; // swap interval (i.e., VBL sync) setting 1 = sync, 0 = no sync
            Rect rectPort; // window port rectangle
            long width = pWindowInfo->imageWidth, height = pWindowInfo->imageHeight; // image width and height
            GDHandle device; // GDevice to find the constrain the window to
            Rect deviceRect, availRect, rect; // rect of device which window is on (mostly, area wise at least). avialable area for window (minus dock and menu if req), working rect

            GetWindowGreatestAreaDevice (window, kWindowContentRgn, &device, &deviceRect); // find device the window is mostly on
            GetAvailableWindowPositioningBounds (device, &availRect); //  get the geretest available area for te windoew (mminus doc and menu if applicable)
            if (width > (availRect.right - availRect.left)) // adjust window width if it is greater than available area (orginally set to image width, see above)
                width = (availRect.right - availRect.left);
            if (height > (availRect.bottom - availRect.top)) // adjust window height if it is greater than available area (orginally set to image width, see above)
                height = (availRect.bottom - availRect.top);
            SizeWindow (window, (short) width, (short) height, true); // size the window to new width and height
            ConstrainWindowToScreen(window, kWindowStructureRgn, kWindowConstrainMayResize, NULL, &rect); // ensure window structure region is on the screen
            GetWindowPortBounds (window, &rectPort); // get port rect for viewport reset

            aglSetCurrentContext (pWindowInfo->aglContext); // set our GL context to this one
            aglUpdateContext (pWindowInfo->aglContext); // update the context to account for the resize
            InvalWindowRect (window, &rectPort); // inval the entire window to ensure we get a redraw
            glViewport (0, 0, rectPort.right - rectPort.left, rectPort.bottom - rectPort.top); // reset viewport to entier window area

            aglSetInteger (pWindowInfo->aglContext, AGL_SWAP_INTERVAL, &swap); // set swap interval to account for vbl syncing or not
            
            // set correct texture target // if building on 10.0 or 9 this will be undefined
        #ifdef GL_TEXTURE_RECTANGLE_EXT
            if (pWindowInfo->fNPOTTextures)
                textureTarget = GL_TEXTURE_RECTANGLE_EXT;
        #endif
            
            // Set texture mapping parameters
            glEnable (textureTarget); // enable texturing
                
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f); // set clear color buffer to dark gray
            glClear (GL_COLOR_BUFFER_BIT); // clear just to color buffer
            aglSwapBuffers (pWindowInfo->aglContext); // swap the cleared buffer to front
    
            //GetFNum ("\pMonaco", &fNum); // get font number for named font
            //pWindowInfo->fontList = BuildFontGL (pWindowInfo->aglContext, fNum, normal, 9); // build display list for fonts for this context (see aglString for more info)
            
            // if we can use texture rectangle // if building on 10.0 or 9 this will be undefined
        #ifdef GL_TEXTURE_RECTANGLE_EXT
            if (pWindowInfo->fNPOTTextures)
                glEnable(GL_TEXTURE_RECTANGLE_EXT);
        #endif
            if (pWindowInfo->fAGPTexturing)
                glTextureRangeAPPLE(textureTarget, pWindowInfo->textureHeight * pWindowInfo->textureWidth * (pWindowInfo->imageDepth >> 3), pWindowInfo->pImageBuffer);
            glPixelStorei (GL_UNPACK_ROW_LENGTH, pWindowInfo->textureWidth); // set image width in groups (pixels), accounts for border this ensures proper image alignment row to row
            // get number of textures x and y
                // extract the number of horiz. textures needed to tile image
            pWindowInfo->textureX = GetTextureNumFromTextureDim (pWindowInfo->textureWidth, pWindowInfo->maxTextureSize, pWindowInfo->fOverlapTextures, pWindowInfo->fNPOTTextures); 
                // extract the number of horiz. textures needed to tile image
            pWindowInfo->textureY = GetTextureNumFromTextureDim (pWindowInfo->textureHeight, pWindowInfo->maxTextureSize, pWindowInfo->fOverlapTextures, pWindowInfo->fNPOTTextures); 
            pWindowInfo->pTextureName = (GLuint *) NewPtrClear ((long) sizeof (GLuint) * pWindowInfo->textureX * pWindowInfo->textureY); // allocate storage for texture name lists
            glGenTextures (pWindowInfo->textureX * pWindowInfo->textureY, pWindowInfo->pTextureName); // generate textures names need to support tiling
            {
                long x, y, k = 0, offsetY, offsetX = 0, currWidth, currHeight; // texture iterators, texture name iterator, image offsets for tiling, current texture width and height
                for (x = 0; x < pWindowInfo->textureX; x++) // for all horizontal textures
                {
                    currWidth = GetNextTextureSize (pWindowInfo->textureWidth - offsetX, pWindowInfo->maxTextureSize, pWindowInfo->fNPOTTextures); // use remaining to determine next texture size 
                                                                                                                    // (basically greatest power of 2 which fits into remaining space)
                    offsetY = 0; // reset vertical offest for every column
                    for (y = 0; y < pWindowInfo->textureY; y++) // for all vertical textures
                    {
                        // buffer pointer is at base + rows * row size + columns
                        unsigned char * pBuffer = pWindowInfo->pImageBuffer + 
                                                   offsetY * pWindowInfo->textureWidth * (pWindowInfo->imageDepth >> 3) + 
                                                   offsetX * (pWindowInfo->imageDepth >> 3);
                        currHeight = GetNextTextureSize (pWindowInfo->textureHeight - offsetY, pWindowInfo->maxTextureSize, pWindowInfo->fNPOTTextures); // use remaining to determine next texture size
                        glBindTexture (textureTarget, pWindowInfo->pTextureName[k++]);
                        if (pWindowInfo->fAGPTexturing) {
                            glTexParameterf (textureTarget, GL_TEXTURE_PRIORITY, 0.0f); // AGP texturing
                            glTexParameteri (textureTarget, GL_TEXTURE_STORAGE_HINT_APPLE, GL_STORAGE_SHARED_APPLE);
                        }
                        else
                            glTexParameterf (textureTarget, GL_TEXTURE_PRIORITY, 1.0f);
                            
                    #ifdef GL_UNPACK_CLIENT_STORAGE_APPLE
                        if (pWindowInfo->fClientTextures)
                            glPixelStorei (GL_UNPACK_CLIENT_STORAGE_APPLE, 1);
                        else
                            glPixelStorei (GL_UNPACK_CLIENT_STORAGE_APPLE, 0);
                    #endif

                        glTexParameteri (textureTarget, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                        glTexParameteri (textureTarget, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
                        glTexParameteri (textureTarget, GL_TEXTURE_WRAP_S, gpOpenGLCaps->edgeClampParam);
                        glTexParameteri (textureTarget, GL_TEXTURE_WRAP_T, gpOpenGLCaps->edgeClampParam);
                        glReportError (); // report any errors so far
                        glTexImage2D (textureTarget, 0, GL_RGBA, currWidth, currHeight, 0, 
                                      GL_BGRA_EXT, pWindowInfo->imageDepth == 32 ? GL_UNSIGNED_INT_8_8_8_8_REV : GL_UNSIGNED_SHORT_1_5_5_5_REV, 
                                      pBuffer); // texture with current width and height at pBuffer location in image buffer with image size as GL_UNPACK_ROW_LENGTH
                        glReportError (); // report any errors
                        offsetY += currHeight - 2 * pWindowInfo->fOverlapTextures; // offset in for the amount of texture used, 
                                                                                       //  since we are overlapping the effective texture used is 2 texels less than texture width
                    }
                    offsetX += currWidth - 2 * pWindowInfo->fOverlapTextures; // offset in for the amount of texture used, 
                                                                              //  since we are overlapping the effective texture used is 2 texels less than texture width
                }
            }
            if (false == pWindowInfo->fClientTextures) // MUST preserve the buffer if texturing from client memory
            {
                DisposePtr ((Ptr) pWindowInfo->pImageBuffer); // or image buffer
                pWindowInfo->pImageBuffer = NULL;
            }
        }
        SetPort (portSave); //reset port
    }
    return noErr; // we done
}

// ---------------------------------

// Handle updating context for window moves and resizing

OSStatus ResizeMoveGLWindow (WindowRef window)
{
    OSStatus err = noErr; // no errors to start
    Rect rectPort; // new port rect
    pRecImage pWindowInfo = (pRecImage) GetWRefCon (window); // get GL info;
    if (window && pWindowInfo) // if we have a window
    {
        GetWindowPortBounds (window, &rectPort);
        pWindowInfo->zoomX = (float) (rectPort.right - rectPort.left) / (float) pWindowInfo->imageWidth;
        pWindowInfo->zoomY = (float) (rectPort.bottom - rectPort.top) / (float) pWindowInfo->imageHeight;

        if (!aglUpdateContext (pWindowInfo->aglContext)) // update the context to ensure gl knows about the move or resize
               aglReportError (); // report any error with update context
        if (noErr != err)
            ReportErrorNum ("ResizeMoveGLWindow error with InvalWindowRect on window: ", err);  // should not get erro here, but who knows
        err = InvalWindowRect (window, &rectPort);
    }
    else
        err = paramErr; // bad window
    return err; // return any error 
}

// ---------------------------------

// main GL drawing routine, should be valid window passed in (will setupGL if require).  Draw image

void DrawGL (WindowRef window)
{
    Rect rectPort; // rectangle for port
    pRecImage pWindowInfo; // the gl info for the target window 
    long width, height; // width and height or the port and the row of the raster position
    long effectiveTextureMod = 0; // texture size modification (inset) to account for borders
    long x, y, k = 0, offsetY, offsetX = 0, currTextureWidth, currTextureHeight;
    GLenum textureTarget = GL_TEXTURE_2D;
    
    if (NULL == window) // if we do not have a window
        return; // drop out
    pWindowInfo = (pRecImage) GetWRefCon (window); // get the gl info for the window
    if (NULL == pWindowInfo) // if this is non-existant
        return; // then drop out
    if (NULL == pWindowInfo->aglContext) // try to buld the context if we don't have one (safety check)
        BuildGLForWindow (window);
    if (NULL == pWindowInfo->aglContext) // if we still don't have one then drop out
        return;
        
    if (pWindowInfo->fOverlapTextures)
        effectiveTextureMod = 2; // if we overlap then we need to inset the textures passed to the drawing code
    // set texture target
#ifdef GL_TEXTURE_RECTANGLE_EXT
    if (pWindowInfo->fNPOTTextures)
        textureTarget = GL_TEXTURE_RECTANGLE_EXT;
#endif

    aglSetCurrentContext (pWindowInfo->aglContext); // ensaure the context we are working with is set to current
    aglUpdateContext (pWindowInfo->aglContext); // ensaure the context we are working with is set to current

    GetWindowPortBounds (window, &rectPort); // get the current port (window) bounds
    width = rectPort.right - rectPort.left; // find width
    height = rectPort.bottom - rectPort.top; // and height
    glViewport (0, 0, width, height); // set the viewport to cover entire window
    
    glMatrixMode (GL_PROJECTION); // set projection matrix
    glLoadIdentity (); // to indetity
    glMatrixMode (GL_MODELVIEW); // set modelview matrix
    glLoadIdentity (); // to identity
    glReportError (); // report any GL errors so far
    
    // set the model view matrix for an orthographic view scaled to one screen pixel equal image pixel (independent of image zoom)
    glScalef (2.0f / width, -2.0f /  height, 1.0f); // scale to port per pixel scale
    //glTranslatef (pWindowInfo->centerX, pWindowInfo->centerY, 0.0f); // translate for image movement
    //glRotatef (0.0f, 0.0f, 0.0f, 1.0f); // ratate matrix for image rotation
    glReportError (); // report any GL errors
    
    glClear (GL_COLOR_BUFFER_BIT); // clear the color buffer before drawing

    // draw image
    glEnable (textureTarget); // enable texturing
    glColor3f (1.0f, 1.0f, 1.0f); // white polygons
    // offset x and y are used to draw the polygon and need to represent the texture effective edges (without borders)
    // so walk the texture size images adjusting for each border
    for (x = 0; x < pWindowInfo->textureX; x++) // for all horizontal textures
    {
        // use remaining to determine next texture size
        currTextureWidth = GetNextTextureSize (pWindowInfo->textureWidth - offsetX, pWindowInfo->maxTextureSize, pWindowInfo->fNPOTTextures) - effectiveTextureMod; // current effective texture width for drawing
        offsetY = 0; // start at top
        for (y = 0; y < pWindowInfo->textureY; y++) // for a complete column
        {
            // use remaining to determine next texture size
            currTextureHeight = GetNextTextureSize (pWindowInfo->textureHeight - offsetY, pWindowInfo->maxTextureSize, pWindowInfo->fNPOTTextures) - effectiveTextureMod; // effective texture height for drawing
            glBindTexture(textureTarget, pWindowInfo->pTextureName[k++]); // work through textures in same order as stored, setting each texture name as current in turn
            if (!pWindowInfo->fAGPTexturing)
                glTexSubImage2D(textureTarget, 0, 0, 0, currTextureWidth, currTextureHeight, GL_BGRA, pWindowInfo->imageDepth == 32 ? GL_UNSIGNED_INT_8_8_8_8_REV : GL_UNSIGNED_SHORT_1_5_5_5_REV, pWindowInfo->pImageBuffer);
            glReportError (); // report any errors
            {
                float endX = pWindowInfo->fTileTextures ? currTextureWidth + offsetX : pWindowInfo->imageWidth;
                float endY = pWindowInfo->fTileTextures ? currTextureHeight + offsetY : pWindowInfo->imageHeight;
                float startXDraw = (offsetX - pWindowInfo->imageWidth * 0.5f) * pWindowInfo->zoomX; // left edge of poly: offset is in image local coordinates convert to world coordinates
                float endXDraw = (endX - pWindowInfo->imageWidth * 0.5f) * pWindowInfo->zoomX; // right edge of poly: offset is in image local coordinates convert to world coordinates
                float startYDraw = (offsetY - pWindowInfo->imageHeight * 0.5f) * pWindowInfo->zoomY; // top edge of poly: offset is in image local coordinates convert to world coordinates
                float endYDraw = (endY - pWindowInfo->imageHeight * 0.5f) * pWindowInfo->zoomY; // bottom edge of poly: offset is in image local coordinates convert to world coordinates
                float texOverlap =  pWindowInfo->fOverlapTextures ? 1.0f : 0.0f; // size of texture overlap, switch based on whether we are using overlap or not
                float startXTexCoord = texOverlap / (currTextureWidth + 2.0f * texOverlap); // texture right edge coordinate (stepped in one pixel for border if required)
                float endXTexCoord = 1.0f - startXTexCoord; // texture left edge coordinate (stepped in one pixel for border if required)
                float startYTexCoord = texOverlap / (currTextureHeight + 2.0f * texOverlap); // texture top edge coordinate (stepped in one pixel for border if required)
                float endYTexCoord = 1.0f - startYTexCoord; // texture bottom edge coordinate (stepped in one pixel for border if required)
                if (pWindowInfo->fNPOTTextures)
                {
                    startXTexCoord = texOverlap; // texture right edge coordinate (stepped in one pixel for border if required)
                    endXTexCoord = currTextureWidth + texOverlap; // texture left edge coordinate (stepped in one pixel for border if required)
                    startYTexCoord = texOverlap; // texture top edge coordinate (stepped in one pixel for border if required)
                    endYTexCoord = currTextureHeight + texOverlap; // texture bottom edge coordinate (stepped in one pixel for border if required)
                }
                if (endX > (pWindowInfo->imageWidth + 0.5)) // handle odd image sizes, (+0.5 is to ensure there is no fp resolution problem in comparing two fp numbers)
                {
                    endXDraw = (pWindowInfo->imageWidth * 0.5f) * pWindowInfo->zoomX; // end should never be past end of image, so set it there
                    if (pWindowInfo->fNPOTTextures)
                        endXTexCoord -= 1.0f;
                    else
                        endXTexCoord = 1.0f -  2.0f * startXTexCoord; // for the last texture in odd size images there are two texels of padding so step in 2
                }
                if (endY > (pWindowInfo->imageHeight + 0.5f)) // handle odd image sizes, (+0.5 is to ensure there is no fp resolution problem in comparing two fp numbers)
                {
                    endYDraw = (pWindowInfo->imageHeight * 0.5f) * pWindowInfo->zoomY; // end should never be past end of image, so set it there
                    if (pWindowInfo->fNPOTTextures)
                        endYTexCoord -= 1.0f;
                    else
                        endYTexCoord = 1.0f -  2.0f * startYTexCoord; // for the last texture in odd size images there are two texels of padding so step in 2
                }
                
                glBegin (GL_TRIANGLE_STRIP); // draw either tri strips of line strips
                    glTexCoord2f (startXTexCoord, startYTexCoord); // draw upper left in world coordinates
                    glVertex3d (startXDraw, startYDraw, 0.0);

                    glTexCoord2f (endXTexCoord, startYTexCoord); // draw lower left in world coordinates
                    glVertex3d (endXDraw, startYDraw, 0.0);

                    glTexCoord2f (startXTexCoord, endYTexCoord); // draw upper right in world coordinates
                    glVertex3d (startXDraw, endYDraw, 0.0);

                    glTexCoord2f (endXTexCoord, endYTexCoord); // draw lower right in world coordinates
                    glVertex3d (endXDraw, endYDraw, 0.0);
                glEnd();
                
            }

            //////////////////////

            glReportError (); // report any errors
            offsetY += currTextureHeight; // offset drawing position for next texture vertically
        }
        offsetX += currTextureWidth; // offset drawing position for next texture horizontally
    }
    glReportError (); // report any errors

    glDisable (textureTarget); // done with texturing
        
    aglSwapBuffers (pWindowInfo->aglContext);
}

// finds the minimum OpenGL capabilites across all displays and GPUs attached to machine.

static void FindMinimumOpenGLCapabilities (pRecGLCap pOpenGLCaps)
{
    WindowPtr pWin = NULL; 
    Rect rectWin = {0, 0, 10, 10};
    GLint attrib[] = { AGL_RGBA, AGL_NONE };
    AGLPixelFormat fmt = NULL;
    AGLContext ctx = NULL;
    GLint deviceMaxTextureSize = 0, NPOTDMaxTextureSize = 0;
    
    if (NULL != gpOpenGLCaps)
    {
        // init desired caps to max values
        pOpenGLCaps->f_ext_texture_rectangle = true;
        pOpenGLCaps->f_ext_client_storage = true;
        pOpenGLCaps->f_ext_packed_pixel = true;
        pOpenGLCaps->f_ext_texture_edge_clamp = true;
        pOpenGLCaps->f_gl_texture_edge_clamp = true;
        pOpenGLCaps->maxTextureSize = 0x7FFFFFFF;
        pOpenGLCaps->maxNOPTDTextureSize = 0x7FFFFFFF;

        // build window
        pWin = NewCWindow (0L, &rectWin, NULL, false,
                plainDBox, (WindowPtr) -1L, true, 0L);
                
        // build context
        fmt = aglChoosePixelFormat(NULL, 0, attrib);
        if (fmt)
            ctx = aglCreateContext(fmt, NULL);
        if (ctx)
        {
            GDHandle hgdNthDevice;
            
            aglSetDrawable(ctx, GetWindowPort (pWin));
            aglSetCurrentContext(ctx);
            
            // for each display
            hgdNthDevice = GetDeviceList ();
            while (hgdNthDevice)
            {
                if (TestDeviceAttribute (hgdNthDevice, screenDevice))
                    if (TestDeviceAttribute (hgdNthDevice, screenActive))
                    {
                        // move window to display
                        MoveWindow (pWin, (**hgdNthDevice).gdRect.left + 5, (**hgdNthDevice).gdRect.top + 5, false);
                        aglUpdateContext(ctx);
                        
                        // for each cap (this can obviously be expanded)
                        // if this driver/GPU/display is less capable
                            // save this minimum capability
                        {
                            // get strings
                            enum { kShortVersionLength = 32 };
                            const GLubyte * strVersion = glGetString (GL_VERSION); // get version string
                            const GLubyte * strExtension = glGetString (GL_EXTENSIONS);    // get extension string
                            
                            // get just the non-vendor specific part of version string
                            GLubyte strShortVersion [kShortVersionLength];
                            short i = 0;
                            while ((((strVersion[i] <= '9') && (strVersion[i] >= '0')) || (strVersion[i] == '.')) && (i < kShortVersionLength)) // get only basic version info (until first space)
                                strShortVersion [i] = strVersion[i++];
                            strShortVersion [i] = 0; //truncate string
                            
                            // compare capabilities based on extension string and GL version
                            pOpenGLCaps->f_ext_texture_rectangle = 
                                pOpenGLCaps->f_ext_texture_rectangle && (NULL != strstr ((const char *) strExtension, "GL_EXT_texture_rectangle"));
                            pOpenGLCaps->f_ext_client_storage = 
                                pOpenGLCaps->f_ext_client_storage && (NULL != strstr ((const char *) strExtension, "GL_APPLE_client_storage"));
                            pOpenGLCaps->f_ext_packed_pixel = 
                                pOpenGLCaps->f_ext_packed_pixel && (NULL != strstr ((const char *) strExtension, "GL_APPLE_packed_pixel"));
                            pOpenGLCaps->f_ext_texture_edge_clamp = 
                                pOpenGLCaps->f_ext_texture_edge_clamp && (NULL != strstr ((const char *) strExtension, "GL_SGIS_texture_edge_clamp"));
                            pOpenGLCaps->f_gl_texture_edge_clamp = 
                                pOpenGLCaps->f_gl_texture_edge_clamp && (!strstr ((const char *) strShortVersion, "1.0") && !strstr ((const char *) strShortVersion, "1.1")); // if not 1.0 and not 1.1 must be 1.2 or greater
                            
                            // get device max texture size
                            glGetIntegerv (GL_MAX_TEXTURE_SIZE, &deviceMaxTextureSize);
                            if (deviceMaxTextureSize < pOpenGLCaps->maxTextureSize)
                                pOpenGLCaps->maxTextureSize = deviceMaxTextureSize;
                            // get max size of non-power of two texture on devices which support
                            if (NULL != strstr ((const char *) strExtension, "GL_EXT_texture_rectangle"))
                            {
                            #ifdef GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT
                                glGetIntegerv (GL_MAX_RECTANGLE_TEXTURE_SIZE_EXT, &NPOTDMaxTextureSize);
                                if (NPOTDMaxTextureSize < pOpenGLCaps->maxNOPTDTextureSize)
                                    pOpenGLCaps->maxNOPTDTextureSize = NPOTDMaxTextureSize;
                            #endif
                            }
                        }
                        // next display
                        hgdNthDevice = GetNextDevice(hgdNthDevice);
                    }
            }
            aglDestroyContext( ctx );
        }
        else
        { // could not build context set caps to min
            pOpenGLCaps->f_ext_texture_rectangle = false;
            pOpenGLCaps->f_ext_client_storage = false;
            pOpenGLCaps->f_ext_packed_pixel = false;
            pOpenGLCaps->f_ext_texture_edge_clamp = false;
            pOpenGLCaps->f_gl_texture_edge_clamp = false;
            pOpenGLCaps->maxTextureSize = 0;
        }
        
        // set clamp param based on retrieved capabilities
        if (pOpenGLCaps->f_gl_texture_edge_clamp) // if OpenGL 1.2 or later and texture edge clamp is supported natively
                    pOpenGLCaps->edgeClampParam = GL_CLAMP_TO_EDGE;  // use 1.2+ constant to clamp texture coords so as to not sample the border color
        else if (pOpenGLCaps->f_ext_texture_edge_clamp) // if GL_SGIS_texture_edge_clamp extension supported
            pOpenGLCaps->edgeClampParam = GL_CLAMP_TO_EDGE_SGIS; // use extension to clamp texture coords so as to not sample the border color
        else
            pOpenGLCaps->edgeClampParam = GL_CLAMP; // clamp texture coords to [0, 1]

        aglDestroyPixelFormat( fmt );
        DisposeWindow( pWin );
    }
}

//--------------------------------------------------------------------------------------------

static OSStatus
WindowEventHandler( EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon )
{
    OSStatus    err = eventNotHandledErr;
    WindowRef    window = (WindowRef) inRefcon;

    if( GetEventClass(inEvent) == kEventClassMouse )
    {
        Point mousePoint; // UInt32 modifiers;
        verify_noerr( GetEventParameter(inEvent, kEventParamMouseLocation, typeQDPoint, NULL, sizeof(Point), NULL, &mousePoint) );
        pRecImage pWindowInfo = (pRecImage) GetWRefCon (window); // get the gl info for the window
        if(pWindowInfo) {
            SetPortWindowPort(window);
            GlobalToLocal (&mousePoint); //convert mouse coordinates to local coordintes prior to recording
            mousePoint.h /= pWindowInfo->zoomX; mousePoint.v /= pWindowInfo->zoomY;
            if(mousePoint.h >= 0 && mousePoint.h < pWindowInfo->imageWidth && mousePoint.v >= 0 && mousePoint.v < pWindowInfo->imageHeight)
                g_video->on_mouse(mousePoint.h, mousePoint.v, GetEventKind(inEvent) == kEventMouseUp?-1:1), err = noErr;
        }
    }
    else if( GetEventClass(inEvent) == kEventClassKeyboard )
    {
        char ch;
        verify_noerr( GetEventParameter( inEvent, kEventParamKeyMacCharCodes, typeChar, NULL, sizeof( ch ), NULL, &ch ) );
        if(g_video)
            g_video->on_key(ch);
    }
    else //if( GetEventClass(inEvent) == kEventClassWindow )
    {
        if (GetEventKind(inEvent) == kEventWindowDrawContent)
        {
            //DrawGL(window);
            err = noErr;
        }
        else if (GetEventKind(inEvent) == kEventWindowClose)
        {
            if (window)
            {
                g_video->running = false;
            }
            err = noErr;
        }
        else if (GetEventKind(inEvent) == kEventWindowShowing)
        {
            err = BuildGLForWindow (window);
        }
        else if ((GetEventKind(inEvent) == kEventWindowResizeCompleted) || (GetEventKind(inEvent) == kEventWindowDragCompleted))
        {
            err = ResizeMoveGLWindow (window);
        }
        else if (GetEventKind(inEvent) == kEventWindowZoomed)
        {
            err = ResizeMoveGLWindow (window);
        }
    }
    
    return err;
}
//--------------------------------------------------------------------------------------------
DEFINE_ONE_SHOT_HANDLER_GETTER( WindowEventHandler )

//--------------------------------------------------------------------------------------------
WindowRef HandleNew()
{
    OSStatus  err;
    WindowRef window;
    pRecImage pWindowInfo = NULL;
    static const EventTypeSpec    kWindowEvents[] =
    {
       { kEventClassMouse, kEventMouseUp },
       { kEventClassMouse, kEventMouseDown },
       { kEventClassKeyboard, kEventRawKeyDown },
//       { kEventClassCommand, kEventCommandProcess },
       { kEventClassWindow, kEventWindowShowing },
       { kEventClassWindow, kEventWindowClose },
       { kEventClassWindow, kEventWindowDrawContent },
       { kEventClassWindow, kEventWindowResizeCompleted },
       { kEventClassWindow, kEventWindowDragCompleted },
       { kEventClassWindow, kEventWindowZoomed}
    };
    if (!gpOpenGLCaps)
    {
        gpOpenGLCaps = (pRecGLCap) NewPtrClear (sizeof (recGLCap));
        FindMinimumOpenGLCapabilities (gpOpenGLCaps);
    }

    // Create a window. "MainWindow" is the name of the window object. This name is set in 
    // InterfaceBuilder when the nib is created.
    err = CreateWindowFromNib( sNibRef, CFSTR("MainWindow"), &window );
    require_noerr( err, CantCreateWindow );
    // We don't need the nib reference anymore.
    DisposeNibReference(sNibRef);
    
    pWindowInfo = (recImage *) NewPtrClear (sizeof (recImage));
    pWindowInfo->textureWidth = pWindowInfo->imageWidth = g_sizex;
    pWindowInfo->textureHeight = pWindowInfo->imageHeight = g_sizey;
    pWindowInfo->imageDepth = 32;
    pWindowInfo->fTileTextures = true;
    pWindowInfo->fOverlapTextures = false; // TODO: ???
    pWindowInfo->maxTextureSize = gpOpenGLCaps->maxTextureSize;
    pWindowInfo->fNPOTTextures = gpOpenGLCaps->f_ext_texture_rectangle;
    pWindowInfo->fClientTextures = gpOpenGLCaps->f_ext_client_storage; // texture from client memory if available
    pWindowInfo->fAGPTexturing = false; // if AGP texturing selected
    pWindowInfo->pImageBuffer = (unsigned char*) g_pImg;
    // set default parameters for this image
    pWindowInfo->zoomX = 1.0f; // pixel 1 to 1 size
    pWindowInfo->zoomY = 1.0f; // pixel 1 to 1 size
    SetWRefCon (window, (long) pWindowInfo);
    char buffer[256]; buffer[0] = snprintf(buffer+1, 255, "%s", g_video->title);
    SetWTitle (window, (ConstStr255Param)buffer);
    // Install a command handler on the window. We don't use this handler yet, but nearly all
    // Carbon apps will need to handle commands, so this saves everyone a little typing.
    InstallStandardEventHandler(GetWindowEventTarget(window));
    InstallWindowEventHandler( window, GetWindowEventHandlerUPP(),
                               GetEventTypeCount( kWindowEvents ), kWindowEvents, window, NULL );
    if (noErr != BuildGLForWindow (window))
    {
        DisposeGLForWindow (window);
        DisposeWindow (window);
        return 0;
    }

    // Position new windows in a staggered arrangement on the main screen
    RepositionWindow( window, NULL, kWindowCascadeOnMainScreen );
    
    // The window was created hidden, so show it
    ShowWindow( window );
    return window;
  
CantCreateWindow:
    return 0;
}


//--------------------------------------------------------------------------------------------
static OSStatus
AppEventHandler( EventHandlerCallRef inCaller, EventRef inEvent, void* inRefcon )
{
    OSStatus    result = eventNotHandledErr;

    return result;
}

//=======================================================================================================

video::video()
    : red_mask(0xff0000), red_shift(16), green_mask(0xff00),
      green_shift(8), blue_mask(0xff), blue_shift(0), depth(24)
{
    assert(g_video == 0);
    g_video = this; title = "Video"; updating = true; calc_fps = false;
}

bool video::init_window(int x, int y)
{
    g_sizex = x; g_sizey = y; g_window = 0;
    g_pImg = new unsigned int[x*y];

    // Check for graphics availability
    if( CGGetOnlineDisplayList(0, NULL, NULL) ) {
        running = true; // console mode
        return false;
    }

    OSStatus                    err;
    static const EventTypeSpec    kAppEvents[] =
    {
        { kEventClassCommand, kEventCommandProcess }
    };

    // Create a Nib reference, passing the name of the nib file (without the .nib extension).
    // CreateNibReference only searches into the application bundle.
    err = CreateNibReference( CFSTR("main"), &sNibRef );
    require_noerr( err, ReturnLabel );

    // Install our handler for common commands on the application target
    // Register for standard event handlers
    InstallStandardEventHandler(GetApplicationEventTarget()); // Doesn't work?
    verify_noerr( InstallApplicationEventHandler( NewEventHandlerUPP( AppEventHandler ),
                                    GetEventTypeCount( kAppEvents ), kAppEvents, 0, NULL ) );
    
    // Once the nib reference is created, set the menu bar. "MainMenu" is the name of the menu bar
    // object. This name is set in InterfaceBuilder when the nib is created.
    //err = SetMenuBarFromNib( sNibRef, CFSTR("MenuBar") );
    //require_noerr( err, ReturnLabel );
    InstallStandardEventHandler(GetMenuEventTarget(AcquireRootMenu()));

    // Create a new window. A full-fledged application would do this from an AppleEvent handler for kAEOpenApplication.
    g_window = HandleNew();

ReturnLabel:
    return running = g_window != 0;
}

bool video::init_console()
{
    running = true;
    return true;
}

void video::terminate()
{
    g_video = 0; running = false;
    if(g_pImg) { delete[] g_pImg; g_pImg = 0; }
    if(g_window) {
        DisposeGLForWindow (g_window);
        DisposeWindow (g_window);
        g_window = 0;
    }
}

video::~video()
{
    if(g_video) terminate();
}

//! Count and display FPS count in titlebar
bool video::next_frame()
{
	if(!running) return false;
	if(!g_window) return running;
    //! try acquire mutex if threaded code, returns on failure
    if(threaded && pthread_mutex_trylock(&g_mutex))
        return running;
    g_fps++;
    struct timezone tz; struct timeval now_time; gettimeofday(&now_time, &tz);
    double sec = (now_time.tv_sec+1.0*now_time.tv_usec/1000000.0) - (g_time.tv_sec+1.0*g_time.tv_usec/1000000.0);
    if(sec > 1) {
        memcpy(&g_time, &now_time, sizeof(g_time));
        if(calc_fps) {
            double fps = g_fps; g_fps = 0;
            char buffer[256]; buffer[0] = snprintf(buffer+1, 255, "%s%s: %d fps", title, updating?"":" (no updating)", int(fps/sec));
            SetWTitle (g_window, (ConstStr255Param) buffer );
        }
      }

    EventRef theEvent;
    EventTargetRef theTarget;
    OSStatus                    err;
    // Run the event loop

    theTarget = GetEventDispatcherTarget();
    while( (err = ReceiveNextEvent(0, NULL, kEventDurationNoWait, true, &theEvent)) == noErr)
    {
        SendEventToEventTarget(theEvent, theTarget);
        ReleaseEvent(theEvent);
    }
    if(err != eventLoopTimedOutErr) running = false;
    if(updating) {
        pRecImage pWindowInfo = (pRecImage) GetWRefCon (g_window); // get the gl info for the window
        if(pWindowInfo) DrawGL(g_window);
    }
  	if(threaded) pthread_mutex_unlock(&g_mutex);
    return true;
}

//! Do standard loop
void video::main_loop()
{
    struct timezone tz; gettimeofday(&g_time, &tz);
    //RunApplicationEventLoop(); -- using another application loop model
    on_process();
}

//! Change window title
void video::show_title()
{
    char buffer[256]; buffer[0] = snprintf(buffer+1, 255, "%s", title);
    SetWTitle (g_window, (ConstStr255Param) buffer );
}

///////////////////////////////////////////// public methods of video class ///////////////////////

drawing_area::drawing_area(int x, int y, int sizex, int sizey)
    : start_x(x), start_y(y), size_x(sizex), size_y(sizey), pixel_depth(24),
    base_index(y*g_sizex + x), max_index(g_sizex*g_sizey), index_stride(g_sizex), ptr32(g_pImg)
{
    assert(x < g_sizex); assert(y < g_sizey);
    assert(x+sizex <= g_sizex); assert(y+sizey <= g_sizey);

    index = base_index; // current index
}

drawing_area::~drawing_area() {}
