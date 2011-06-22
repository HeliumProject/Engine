#include "FoundationPch.h"
#include "Utilities.h"
#include "Platform/Environment.h"
#include "Platform/Assert.h"

using namespace Helium;

// 8192 is conservative here, its xp's max command line, most other OSes are higher
#define ARG_MAX (8192)

// the fully processed argc/argv data
int            g_Argc = 0;
const tchar_t**   g_Argv = NULL;
tchar_t           g_CmdLine[ ARG_MAX ] = { '\0' };
const tchar_t*    Helium::CmdLineDelimiters = TXT( "-/" );

void Helium::SetCmdLine( int argc, const tchar_t** argv )
{
    for ( int i=0; i<argc; i++ )
    {
        bool quote = _tcsstr( argv[i], TXT( " " ) ) != NULL;

        if ( quote )
        {
            _tcsncat( g_CmdLine, TXT( "\"" ), sizeof( g_CmdLine ) - _tcslen( g_CmdLine ) );
        }

        _tcsncat( g_CmdLine, argv[ i ], sizeof( g_CmdLine ) - _tcslen( g_CmdLine ) );   

        if ( quote )
        {
            _tcsncat( g_CmdLine, TXT( "\"" ), sizeof( g_CmdLine ) - _tcslen( g_CmdLine ) );
        }

        if ( i+1 < argc )
        {
            _tcsncat( g_CmdLine, TXT( " " ), sizeof( g_CmdLine ) - _tcslen( g_CmdLine ) );
        }
    }

    ProcessCmdLine( g_CmdLine, g_Argc, g_Argv );
}

const tchar_t* Helium::GetCmdLine()
{
    return g_CmdLine;
}

void Helium::ReleaseCmdLine()
{
    delete[] g_Argv;
    g_Argv = NULL;
    g_Argc = 0;
}

void Helium::ProcessCmdLine(const tchar_t* command, int& argc, const tchar_t**& argv)
{
    tchar_t* _argv;

    tchar_t a;
    unsigned len;
    unsigned i, j;

    bool in_QM;
    bool in_TEXT;
    bool in_SPACE;

    len = (unsigned)_tcslen(command);
    i = ((len+2)/2)*sizeof(void*) + sizeof(void*);

    argv = (const tchar_t**)(new tchar_t**[i + (len+2)*sizeof(tchar_t)]);
    _argv = (tchar_t*)(((tchar_t*)argv)+i);

    argc = 0;
    argv[argc] = _argv;
    in_QM = false;
    in_TEXT = false;
    in_SPACE = true;
    i = 0;
    j = 0;

    while( a = command[i] )
    {
        if(in_QM)
        {
            if(a == '\"')
            {
                in_QM = false;
            }
            else
            {
                _argv[j] = a;
                j++;
            }
        }
        else
        {
            switch(a)
            {
            case '\"':
                in_QM = true;
                in_TEXT = true;
                if(in_SPACE)
                {
                    argv[argc] = _argv+j;
                    argc++;
                }
                in_SPACE = false;
                break;

            case ' ':
            case '\t':
            case '\n':
            case '\r':
                if(in_TEXT)
                {
                    _argv[j] = '\0';
                    j++;
                }
                in_TEXT = false;
                in_SPACE = true;
                break;

            default:
                in_TEXT = true;
                if(in_SPACE)
                {
                    argv[argc] = _argv+j;
                    argc++;
                }
                _argv[j] = a;
                j++;
                in_SPACE = false;
                break;
            }
        }

        i++;
    }

    _argv[j] = '\0';
    argv[argc] = NULL;
}

const tchar_t** Helium::GetCmdLine( int& argc )
{
    argc = g_Argc;
    return g_Argv;
}

const tchar_t* Helium::GetCmdLineArg( const tchar_t* arg )
{
    int delims = (int)_tcslen( Helium::CmdLineDelimiters );

    // for each arg
    for ( int i=0; i<g_Argc; ++i )
    {
        // this will be the arg w/o delims
        const tchar_t* name = NULL;

        // for each delimiter
        for ( int j=0; j<delims; j++ )
        {
            // if this arg has that delimiter
            if ( g_Argv[i][0] == Helium::CmdLineDelimiters[j] )
            {
                // we have a delimited arg, set it
                name = g_Argv[i];

                // skip all the delims
                while ( *name == Helium::CmdLineDelimiters[j] )
                {
                    name++;
                }

                break;
            }
        }

        // if we have a valid arg, return the next one (if it exists)
        if ( name && !_tcsicmp(arg, name) )
        {
            if ( i+1 < g_Argc )
            {
                return g_Argv[ i+1 ];
            }
            else
            {
                return TXT( "" );
            }
        }
    }

    return NULL;
}

#pragma TODO ( "Deprecate Helium::GetCmdLineFlag. All commandline options should be defined and parsed once in the application, we shouldn't be parsing the entire commandline everytime!" )
bool Helium::GetCmdLineFlag( const tchar_t* arg )
{
    bool explicitValue;
    if ( GetCmdLineArg( arg, explicitValue ) )
    {
        return explicitValue;
    }

    if ( GetCmdLineArg( arg ) )
    {
        return true; // just '-flag'
    }

    return false;
}
