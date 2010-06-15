#include "Debug/Exception.h"
#include "Application/Application.h"
#include "Platform/Windows/Windows.h"

#include "Foundation/InitializerStack.h"
#include "Foundation/Version.h"
#include "Foundation/Log.h"
#include "RCS/RCS.h"
#include "Foundation/File/Path.h"

#include "Reflect/Archive.h"
#include "Reflect/Version.h"

using namespace Reflect;

bool g_RCS = false;
bool g_XML = false;
bool g_Binary = false;
bool g_Verify = false;
std::string g_Batch = "";
std::string g_Input = "";
std::string g_Output = "";

enum RebuildCodes
{
    REBUILD_SUCCESS = 0,
    REBUILD_BAD_INPUT,
    REBUILD_BAD_READ,
    REBUILD_BAD_WRITE,
    REBUILD_CODE_COUNT,
};

const char* g_RebuildStrings[REBUILD_CODE_COUNT] = 
{
    "Success",
    "Bad Input",
    "Bad Read",
    "Bad Write",
};

int g_RebuildTotals[REBUILD_CODE_COUNT] =
{
    0,
    0,
    0,
    0,
};

V_string g_RebuildResults[REBUILD_CODE_COUNT];

namespace Reflect
{
    __declspec(dllimport) extern bool g_OverrideCRC;
}

int ProcessFile(const std::string& input, const std::string& output)
{
    //
    // Verify only
    //

    if (input == output && (g_Verify && !g_XML && !g_Binary))
    {
        V_Element elements;

        if ( Application::IsDebuggerPresent() )
        {
            Reflect::PrintStatus status;
            Archive::FromFile( input, elements, &status );
        }
        else
        {
            try
            {
                Reflect::PrintStatus status;
                Archive::FromFile( input, elements, &status );
            }
            catch (Nocturnal::Exception& ex)
            {
                Log::Error("Verify FAILED: %s\n", ex.what());
                return REBUILD_BAD_READ;
            }
        }

        Log::Print("\n" );
        Log::Print("Verified OK\n");
        return REBUILD_SUCCESS;
    }


    //
    // Read input
    //

    V_Element spool;

    if ( Application::IsDebuggerPresent() )
    {
        Reflect::PrintStatus status;
        Archive::FromFile( input, spool, &status );
    }
    else
    {
        try
        {
            Reflect::PrintStatus status;
            Archive::FromFile( input, spool, &status );
        }
        catch (Nocturnal::Exception& ex)
        {
            Log::Error("%s\n", ex.what());
            return REBUILD_BAD_READ;
        }
    }

    if ( spool.empty() )
    {
        return REBUILD_SUCCESS;
    }


    //
    // Reset version
    //

    VersionPtr version = NULL;

    if ( spool.front()->HasType(Reflect::GetType<Version>()) )
    {
        version = Reflect::DangerousCast<Version>(spool[0]);
        spool.erase(spool.begin());
    }
    else
    {
        version = new Version ();
    }

    version->m_Source = "rebuild";
    version->m_SourceVersion = NOCTURNAL_VERSION_STRING;


    //
    // Write output
    //

    // RCS requires absolute paths, this will resolve the output using cwd if necessary
    Nocturnal::Path absolute( output );
    absolute.Set( absolute.Absolute() );

    if (g_RCS)
    {
        if (RCS::PathIsManaged( absolute ))
        {
            try
            {
                RCS::File rcsFile( absolute );
                rcsFile.Open();
            }
            catch (RCS::Exception& ex)
            {
                Log::Error("%s\n", ex.what());
                return REBUILD_BAD_WRITE;
            }
        }
    }

    if ( Application::IsDebuggerPresent() )
    {
        Reflect::PrintStatus status;
        Archive::ToFile( spool, absolute, version, &status );
    }
    else
    {
        try
        {
            Reflect::PrintStatus status;
            Archive::ToFile( spool, absolute, version, &status );
        }
        catch (Nocturnal::Exception& ex)
        {
            Log::Error("%s\n", ex.what());
            return REBUILD_BAD_WRITE;
        }
    }


    //
    // Verify output
    //

    if (g_Verify)
    {
        if ( Application::IsDebuggerPresent() )
        {
            V_Element duplicates;
            Reflect::PrintStatus status;
            Archive::FromFile( absolute, duplicates, &status );
        }
        else
        {
            try
            {
                V_Element duplicates;
                Reflect::PrintStatus status;
                Archive::FromFile( absolute, duplicates, &status );
            }
            catch (Nocturnal::Exception& ex)
            {
                Log::Print("Verify FAILED: %s\n", ex.what());
                return REBUILD_BAD_WRITE;
            }
        }
    }

    Log::Print("\n" );
    Log::Print("Verified OK\n");
    return REBUILD_SUCCESS;
}

int Main(int argc, const char** argv)
{
    if ( argc < 2 )
    {
        Log::Print("rebuild - Update Utility for Insomniac Games Reflect File Format\n\n");
        Log::Print(" rebuild <input> [output]\n\n");
        return REBUILD_BAD_INPUT;
    }

    for ( int i=1; i<argc; ++i )
    {
        if ( !stricmp(argv[i], "-crc") )
        {
            Reflect::g_OverrideCRC = true;
        }
        else if ( !stricmp(argv[i], "-rcs") )
        {
            g_RCS = true;
        }
        else if ( !stricmp(argv[i], "-xml") )
        {
            g_XML = true;
        }
        else if ( !stricmp(argv[i], "-binary") )
        {
            g_Binary = true;
        }
        else if ( !stricmp(argv[i], "-verify") )
        {
            g_Verify = true;
        }
        else if ( !stricmp(argv[i], "-batch") && i+1 < argc )
        {
            g_Batch = argv[++i];
        }
        else 
        {
            if ( g_Input.empty() )
            {
                g_Input = argv[i];
            }
            else if ( g_Output.empty() )
            {
                g_Output = argv[i];
            }
        }
    }

    int result = REBUILD_SUCCESS;

    Nocturnal::InitializerStack initializerStack( true );
    initializerStack.Push( Reflect::Initialize, Reflect::Cleanup );

#pragma TODO("Need to init modules here -Geoff")

    if ( g_Batch.empty() && !g_Input.empty() )
    {
        if ( g_Output.empty() )
        {
            g_Output = g_Input;
        }
        result = ProcessFile( g_Input, g_Output );
    }
    else
    {
        std::fstream batchfile;
        batchfile.open(g_Batch.c_str(), std::ios_base::in);

        if (!batchfile.is_open())
        {
            Log::Error( "Unable to open file '%s' for read\n", g_Batch.c_str() );
        }
        else
        {
            while (!batchfile.eof())
            {       
                std::string line, input, output;
                std::getline(batchfile, line);

                size_t off = line.find_first_of('|');
                if ( off != std::string::npos )
                {
                    input = line.substr(0, off);
                    output = line.substr(off+1);
                }
                else
                {
                    input = line;
                    output = line;
                }

                Nocturnal::Path inputPath( input );
                Nocturnal::Path outputPath( output );

                if (!input.empty() && inputPath.Exists() )
                {
                    if (g_XML)
                    {
                        outputPath.ReplaceExtension( "xml" );
                    }

                    if (g_Binary)
                    {
                        outputPath.ReplaceExtension( "rb" );
                    }

                    // do work
                    int code = ProcessFile(inputPath.Get(), outputPath.Get());

                    // verify result code
                    NOC_ASSERT(code >= 0 && code < REBUILD_CODE_COUNT);

                    // store result
                    g_RebuildTotals[code]++;
                    g_RebuildResults[code].push_back(outputPath.Get());
                }
            }

            batchfile.close();

            Log::Print("Rebuild Report:\n");
            for (int i=0; i<REBUILD_CODE_COUNT; i++)
            {
                Log::Print(" %s: %d\n", g_RebuildStrings[i], g_RebuildTotals[i]);
                if (i > 0)
                {
                    V_string::const_iterator itr = g_RebuildResults[i].begin();
                    V_string::const_iterator end = g_RebuildResults[i].end();
                    for ( int count = 0; itr != end; ++itr, ++count )
                    {
                        Log::Print("  [%d]: %s\n", count, itr->c_str());
                    }
                }
            }

            if (result != 0)
            {
                result = -1;
            }
        }
    }

    return result;
}

int main(int argc, const char** argv)
{
    return Application::StandardMain( &Main, argc, argv );
}
