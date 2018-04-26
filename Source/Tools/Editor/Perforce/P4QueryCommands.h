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
                : Command ( provider, "where" )
                , m_File( File )
            {

            }

            virtual void Run();
            virtual void OutputStat( StrDict *dict );

            RCS::File* m_File;
        };

        class FStatCommand : public Command
        {
        public:
            FStatCommand( Provider* provider, const char* command = "", RCS::File* File = NULL )
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
                : FStatCommand ( provider, "fstat", File )
            {
            }

            virtual void Run() override;
            virtual void OutputStat( StrDict *dict ) override;
        };

        class MultiFStatCommand : public FStatCommand
        {
        public:
            MultiFStatCommand( Provider* provider, const std::string& folder, RCS::V_File* files, bool recursive, uint32_t fileData = RCS::FileData::All, uint32_t actionData = RCS::ActionData::All )
                : FStatCommand ( provider, "fstat", NULL )
                , m_Folder ( folder )
                , m_Files ( files )
                , m_Recursive ( recursive )
                , m_FileData ( fileData )
                , m_ActionData ( actionData )
            {

            }

            virtual void Run() override;
            virtual void OutputStat( StrDict *dict ) override;

        protected:
            std::string m_Folder;
            RCS::V_File* m_Files;
            bool m_Recursive;
            uint32_t m_FileData;
            uint32_t m_ActionData;
        };

        class FileLogCommand : public Command
        {
        public:
            FileLogCommand( Provider* provider, RCS::File* File, bool getIntegrationHistory = false )
                : Command ( provider, "filelog" )
                , m_File( File )
            {
                if ( getIntegrationHistory )
                {
                    AddArg( "-i" );
                }
            }

            virtual void Run() override;
            virtual void OutputStat( StrDict *dict ) override;
            virtual void HandleError( Error* error ) override;

            RCS::File* m_File;
        };
    }
}