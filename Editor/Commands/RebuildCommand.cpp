#include "EditorPch.h"

#include "RebuildCommand.h"

#include "Platform/Exception.h"

#include "Foundation/Log.h"
#include "Foundation/FilePath.h"

#include "Reflect/Archive.h"
#include "Reflect/Version.h"

#include "Application/InitializerStack.h"
#include "Application/CmdLineProcessor.h"
#include "Application/Startup.h"
#include "Application/RCS.h"

using namespace Helium;
using namespace Helium::Editor;
using namespace Helium::CommandLine;
using namespace Helium::Reflect;

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
            }
            else if ( m_Output.empty() )
            {
                m_Output = arg;
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

                Helium::FilePath inputPath( input );
                Helium::FilePath outputPath( output );

                if (!input.empty() && inputPath.Exists() )
                {
                    if (m_XML)
                    {
                        outputPath.ReplaceExtension( TXT( "xml" ) );
                    }

                    if (m_Binary)
                    {
                        outputPath.ReplaceExtension( TXT( "hrb" ) );
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
        std::vector< ObjectPtr > elements;

        if ( Helium::IsDebuggerPresent() )
        {
            Reflect::ArchivePtr archive = Reflect::GetArchive( input );
            archive->e_Status.AddMethod( this, &RebuildCommand::ArchiveStatus );
            archive->Get( elements );
        }
        else
        {
            try
            {
                Reflect::ArchivePtr archive = Reflect::GetArchive( input );
                archive->e_Status.AddMethod( this, &RebuildCommand::ArchiveStatus );
                archive->Get( elements );
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

    std::vector< ObjectPtr > spool;

    if ( Helium::IsDebuggerPresent() )
    {
        Reflect::ArchivePtr archive = Reflect::GetArchive( input );
        archive->e_Status.AddMethod( this, &RebuildCommand::ArchiveStatus );
        archive->Get( spool );
    }
    else
    {
        try
        {
            Reflect::ArchivePtr archive = Reflect::GetArchive( input );
            archive->e_Status.AddMethod( this, &RebuildCommand::ArchiveStatus );
            archive->Get( spool );
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
    // Write output
    //

    // RCS requires absolute paths, this will resolve the output using cwd if necessary
    Helium::FilePath absolute( output );
    absolute.Set( absolute.Absolute() );

    if (m_RCS)
    {
        if ( RCS::PathIsManaged( absolute ) )
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

    if ( Helium::IsDebuggerPresent() )
    {
        Reflect::ArchivePtr archive = Reflect::GetArchive( absolute );
        archive->e_Status.AddMethod( this, &RebuildCommand::ArchiveStatus );
        archive->Put( spool );
        archive->Close();
    }
    else
    {
        try
        {
            Reflect::ArchivePtr archive = Reflect::GetArchive( absolute );
            archive->e_Status.AddMethod( this, &RebuildCommand::ArchiveStatus );
            archive->Put( spool );
            archive->Close();
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
        if ( Helium::IsDebuggerPresent() )
        {
            std::vector< ObjectPtr > duplicates;
            Reflect::ArchivePtr archive = Reflect::GetArchive( absolute );
            archive->e_Status.AddMethod( this, &RebuildCommand::ArchiveStatus );
            archive->Get( duplicates );
        }
        else
        {
            try
            {
                std::vector< ObjectPtr > duplicates;
                Reflect::ArchivePtr archive = Reflect::GetArchive( absolute );
                archive->e_Status.AddMethod( this, &RebuildCommand::ArchiveStatus );
                archive->Get( duplicates );
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

void RebuildCommand::ArchiveStatus( const Reflect::ArchiveStatus& info )
{
    switch ( info.m_State )
    {
    case Reflect::ArchiveStates::Starting:
        {
            m_Timer = Helium::TimerGetClock();

            const char* verb = info.m_Archive.GetMode() == ArchiveModes::Read ? "Reading" : "Writing";
            const char* type = info.m_Archive.GetType() == ArchiveTypes::XML ? "XML" : "Binary";

            if (info.m_Archive.GetPath().empty())
            {
                Log::Bullet bullet( TXT( "%s %s stream\n" ), verb, type );
            }
            else
            {
                Log::Bullet bullet( TXT( "%s %s file '%s'\n" ), verb, type, info.m_Archive.GetPath().c_str() );

                if (info.m_Archive.GetMode() == ArchiveModes::Read)
                {
					Status status;
					status.Read( info.m_Archive.GetPath().Get().c_str() );
                    uint64_t size = status.m_Size;
                    if ( size > 1000 )
                    {
                        Log::Bullet bullet( TXT( "Size: %dk\n" ),  size / 1000);
                    }
                    else
                    {
                        Log::Bullet bullet( TXT( "Size: %d\n" ), size );
                    }
                }
            }

            break;
        }

    case Reflect::ArchiveStates::PostProcessing:
        {
            Log::Bullet bullet( TXT( "Processing...\n" ) );
            break;
        }

    case Reflect::ArchiveStates::Complete:
        {
            Log::Bullet bullet( TXT( "Completed in %.2f ms\n" ), Helium::CyclesToMillis(Helium::TimerGetClock() - m_Timer));
            break;
        }

    case Reflect::ArchiveStates::Publishing:
        {
            Log::Bullet bullet( TXT( "Publishing to %s\n" ), info.m_Archive.GetPath().c_str());
            break;
        }
    }
}
