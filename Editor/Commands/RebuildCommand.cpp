#include "Precompile.h"

#include "RebuildCommand.h"

#include "Platform/Debug.h"
#include "Platform/Exception.h"
#include "Platform/Windows/Windows.h"

#include "Foundation/InitializerStack.h"
#include "Foundation/Log.h"
#include "Foundation/File/Path.h"
#include "Foundation/Reflect/Archive.h"
#include "Foundation/Reflect/Version.h"
#include "Foundation/CommandLine/Option.h"
#include "Foundation/CommandLine/Command.h"

#include "Application/Application.h"
#include "Application/RCS/RCS.h"

using namespace Helium::Editor;
using namespace Helium::CommandLine;
using namespace Helium::Reflect;

namespace Helium
{
    namespace Reflect
    {
        FOUNDATION_API extern bool g_OverrideCRC;
    }
}

const char* RebuildCommand::m_RebuildStrings[REBUILD_CODE_COUNT] = 
{
    "Success",
    "Bad Input",
    "Bad Read",
    "Bad Write",
};


RebuildCommand::RebuildCommand()
: Command( TXT( "rebuild" ), TXT( "[<INPUT>] [<OUTPUT>]" ), TXT( "Convertion utility for Helium Reflect file format" ) )
, m_HelpFlag( false )
, m_RCS( false )
, m_XML( false )
, m_Binary( false )
, m_Verify( false )
{
    for( int index = 0; index <= REBUILD_CODE_COUNT; ++index )	
    {
        m_RebuildTotals[index] = 0;
    }
}

RebuildCommand::~RebuildCommand()
{
}

bool RebuildCommand::Initialize( tstring& error ) 
{
    bool result = true;

    result &= AddOption( new FlagOption( &Reflect::g_OverrideCRC, TXT( "crc" ), TXT( "override crc" ) ), error );
    result &= AddOption( new FlagOption( &m_RCS, TXT( "rcs" ), TXT( "user rcs" ) ), error );
    result &= AddOption( new FlagOption( &m_XML, TXT( "xml" ), TXT( "" ) ), error );
    result &= AddOption( new FlagOption( &m_Binary, TXT( "binary" ), TXT( "" ) ), error );
    result &= AddOption( new FlagOption( &m_Verify, TXT( "verify" ), TXT( "" ) ), error );
    result &= AddOption( new SimpleOption<tstring>( &m_Batch, TXT( "batch" ), TXT( "<FILE>" ), TXT( "parse batch file" ) ), error );
    result &= AddOption( new FlagOption( &m_HelpFlag, TXT( "h|help" ), TXT( "print command usage" ) ), error );

    return result;
}

void RebuildCommand::Cleanup()
{
    m_InitializerStack.Cleanup();
}

bool RebuildCommand::Process( std::vector< tstring >::const_iterator& argsBegin, const std::vector< tstring >::const_iterator& argsEnd, tstring& error )
{
    if ( !ParseOptions( argsBegin, argsEnd, error ) )
    {
        return false;
    }

    if ( m_HelpFlag )
    {
        Log::Print( Help().c_str() );
        return true;
    }

    if ( ( m_Batch.empty() && argsBegin == argsEnd )
        || ( !m_Batch.empty() && argsBegin != argsEnd ) )
    {
        error = TXT( "Must supply either an input file OR batch file, but not both." );
        return false;
    }

    while ( argsBegin != argsEnd )
    {
        const tstring& arg = (*argsBegin);
        ++argsBegin;

        if ( arg.length() )
        {
            if ( m_Input.empty() )
            {
                m_Input = arg;
                Helium::Path::Normalize( m_Input );
            }
            else if ( m_Output.empty() )
            {
                m_Output = arg;
                Helium::Path::Normalize( m_Output );
            }
        }
    }


    int result = REBUILD_SUCCESS;

    m_InitializerStack.Push( Reflect::Initialize, Reflect::Cleanup );

#pragma TODO("Need to init modules here -Geoff")

    if ( m_Batch.empty() && !m_Input.empty() )
    {
        if ( m_Output.empty() )
        {
            m_Output = m_Input;
        }
        result = ProcessFile( m_Input, m_Output );
    }
    else
    {
        tfstream batchfile;
        batchfile.open(m_Batch.c_str(), std::ios_base::in);

        if (!batchfile.is_open())
        {
            error = TXT( "Unable to open file for read: " ) + m_Batch;
            return false;
        }
        else
        {
            while (!batchfile.eof())
            {       
                tstring line, input, output;
                std::getline(batchfile, line);

                size_t off = line.find_first_of('|');
                if ( off != tstring::npos )
                {
                    input = line.substr(0, off);
                    output = line.substr(off+1);
                }
                else
                {
                    input = line;
                    output = line;
                }

                Helium::Path inputPath( input );
                Helium::Path outputPath( output );

                if (!input.empty() && inputPath.Exists() )
                {
                    if (m_XML)
                    {
                        outputPath.ReplaceExtension( TXT( "xml" ) );
                    }

                    if (m_Binary)
                    {
                        outputPath.ReplaceExtension( TXT( "nrb" ) );
                    }

                    // do work
                    int code = ProcessFile(inputPath.Get(), outputPath.Get());

                    // verify result code
                    HELIUM_ASSERT(code >= 0 && code < REBUILD_CODE_COUNT);

                    // store result
                    m_RebuildTotals[code]++;
                    m_RebuildResults[code].push_back(outputPath.Get());
                }
            }

            batchfile.close();

            Log::Print( TXT( "Rebuild Report:\n" ) );
            for (int i=0; i<REBUILD_CODE_COUNT; i++)
            {
                Log::Print( TXT( " %s: %d\n" ), m_RebuildStrings[i], m_RebuildTotals[i]);
                if (i > 0)
                {
                    std::vector< tstring >::const_iterator itr = m_RebuildResults[i].begin();
                    std::vector< tstring >::const_iterator end = m_RebuildResults[i].end();
                    for ( int count = 0; itr != end; ++itr, ++count )
                    {
                        Log::Print( TXT( "  [%d]: %s\n" ), count, itr->c_str());
                    }
                }
            }

            if (result != 0)
            {
                result = -1;
            }
        }
    }

    return result == REBUILD_SUCCESS;
}


int RebuildCommand::ProcessFile(const tstring& input, const tstring& output)
{
    //
    // Verify only
    //

    if (input == output && (m_Verify && !m_XML && !m_Binary))
    {
        V_Element elements;

        if ( Platform::IsDebuggerPresent() )
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
            catch (Helium::Exception& ex)
            {
                Log::Error( TXT( "Verify FAILED: %s\n" ), ex.What());
                return REBUILD_BAD_READ;
            }
        }

        Log::Print( TXT( "\n" ) );
        Log::Print( TXT( "Verified OK\n" ) );
        return REBUILD_SUCCESS;
    }


    //
    // Read input
    //

    V_Element spool;

    if ( Platform::IsDebuggerPresent() )
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
        catch (Helium::Exception& ex)
        {
            Log::Error( TXT( "%s\n" ), ex.What());
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

    version->m_Source = TXT( "rebuild" );


    //
    // Write output
    //

    // RCS requires absolute paths, this will resolve the output using cwd if necessary
    Helium::Path absolute( output );
    absolute.Set( absolute.Absolute() );

    if (m_RCS)
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
                Log::Error( TXT( "%s\n" ), ex.What());
                return REBUILD_BAD_WRITE;
            }
        }
    }

    if ( Platform::IsDebuggerPresent() )
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
        catch (Helium::Exception& ex)
        {
            Log::Error( TXT( "%s\n" ), ex.What());
            return REBUILD_BAD_WRITE;
        }
    }


    //
    // Verify output
    //

    if (m_Verify)
    {
        if ( Platform::IsDebuggerPresent() )
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
            catch (Helium::Exception& ex)
            {
                Log::Print( TXT( "Verify FAILED: %s\n" ), ex.What());
                return REBUILD_BAD_WRITE;
            }
        }
    }

    Log::Print( TXT( "\n" ) );
    Log::Print( TXT( "Verified OK\n" ) );
    return REBUILD_SUCCESS;
}

