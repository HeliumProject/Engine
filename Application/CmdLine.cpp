#include "ApplicationPch.h"
#include "CmdLine.h"

#include "Platform/Process.h"
#include "Platform/Assert.h"

using namespace Helium;

// the fully processed argc/argv data
int              g_Argc = 0;
const char**     g_Argv = NULL;
std::string      g_CmdLine;
const char*      Helium::CmdLineDelimiters = "-/";

void Helium::SetCmdLine( int argc, const char** argv )
{
    for ( int i=0; i<argc; i++ )
    {
        bool quote = FindCharacter( argv[i], ' ' ) != NULL;
        if ( quote )
        {
            g_CmdLine.append( "\"" );
        }

        g_CmdLine.append( argv[ i ] );   

        if ( quote )
        {
            g_CmdLine.append( "\"" );
        }

        if ( i+1 < argc )
        {
            g_CmdLine.append( " " );
        }
    }

	ProcessCmdLine( g_CmdLine.c_str(), g_Argc, g_Argv );
}

const char* Helium::GetCmdLine()
{
	return g_CmdLine.c_str();
}

void Helium::ReleaseCmdLine()
{
    delete[] g_Argv;
    g_Argv = NULL;
    g_Argc = 0;
}

void Helium::ProcessCmdLine(const char* command, int& argc, const char**& argv)
{
    char* _argv;

    char a;
    unsigned len;
    unsigned i, j;

    bool in_QM;
    bool in_TEXT;
    bool in_SPACE;

    len = (unsigned)StringLength(command);
    i = ((len+2)/2)*sizeof(void*) + sizeof(void*);

    argv = (const char**)(new char**[i + (len+2)*sizeof(char)]);
    _argv = (char*)(((char*)argv)+i);

    argc = 0;
    argv[argc] = _argv;
    in_QM = false;
    in_TEXT = false;
    in_SPACE = true;
    i = 0;
    j = 0;

    while( ( a = command[i] ) )
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

const char** Helium::GetCmdLine( int& argc )
{
    argc = g_Argc;
    return g_Argv;
}

const char* Helium::GetCmdLineArg( const char* arg )
{
    int delims = (int)StringLength( Helium::CmdLineDelimiters );

    // for each arg
    for ( int i=0; i<g_Argc; ++i )
    {
        // this will be the arg w/o delims
        const char* name = NULL;

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
        if ( name && !CaseInsensitiveCompareString(arg, name) )
        {
            if ( i+1 < g_Argc )
            {
                return g_Argv[ i+1 ];
            }
            else
            {
                return "";
            }
        }
    }

    return NULL;
}

bool Helium::GetCmdLineFlag( const char* arg )
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
