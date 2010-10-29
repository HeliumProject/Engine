#pragma once

#include "P4Command.h"

namespace Helium
{
    namespace Perforce
    {
        class WhereCommand : public Command
        {
        public:
            WhereCommand( Provider* provider, RCS::File* File )
                : Command ( provider, TXT( "where" ) )
                , m_File( File )
            {

            }

            virtual void Run();
            virtual void OutputStat( StrDict *dict );

            RCS::File* m_File;
        };

        class HaveCommand : public Command
        {
        public:
            HaveCommand( Provider* provider )
                : Command ( provider, TXT( "have" ) )
            {

            }

            virtual void OutputStat( StrDict *dict );

        };

        class DirsCommand : public Command
        {
        public:
            DirsCommand( Provider* provider )
                : Command ( provider, TXT( "dirs" ) )
            {

            }

            virtual void OutputStat( StrDict *dict );

        };

        class FilesCommand : public Command
        {
        public:
            FilesCommand( Provider* provider )
                : Command ( provider, TXT( "files" ) )
            {

            }

            virtual void OutputStat( StrDict *dict );

        };

        class FStatCommand : public Command
        {
        public:
            FStatCommand( Provider* provider, const tchar* command = TXT( "" ), RCS::File* File = NULL )
                : Command( provider, command )
                , m_File ( File )
            {
                AddArg( "-Ol" );
            }

            virtual void OutputStat( StrDict *dict );
            virtual void HandleError( Error* error );

        protected:
            RCS::File* m_File;
        };

        class SingleFStatCommand : public FStatCommand
        {
        public:
            SingleFStatCommand( Provider* provider, RCS::File* File )
                : FStatCommand ( provider, TXT( "fstat" ), File )
            {
            }

            virtual void Run();
            virtual void OutputStat( StrDict *dict ) HELIUM_OVERRIDE;
        };

        class MultiFStatCommand : public FStatCommand
        {
        public:
            MultiFStatCommand( Provider* provider, const tstring& folder, RCS::V_File* files, bool recursive, uint32_t fileData = RCS::FileData::All, uint32_t actionData = RCS::ActionData::All )
                : FStatCommand ( provider, TXT( "fstat" ), NULL )
                , m_Folder ( folder )
                , m_Files ( files )
                , m_Recursive ( recursive )
                , m_FileData ( fileData )
                , m_ActionData ( actionData )
            {

            }

            virtual void Run();
            virtual void OutputStat( StrDict *dict ) HELIUM_OVERRIDE;

        protected:
            tstring m_Folder;
            RCS::V_File* m_Files;
            bool m_Recursive;
            uint32_t m_FileData;
            uint32_t m_ActionData;
        };

        class FileLogCommand : public Command
        {
        public:
            FileLogCommand( Provider* provider, RCS::File* File, bool getIntegrationHistory = false )
                : Command ( provider, TXT( "filelog" ) )
                , m_File( File )
            {
                if ( getIntegrationHistory )
                {
                    AddArg( "-i" );
                }
            }

            virtual void Run();
            virtual void OutputStat( StrDict *dict );
            virtual void HandleError( Error* error );

            RCS::File* m_File;
        };
    }
}