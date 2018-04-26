#pragma once

#include "P4Command.h"

namespace Helium
{
    namespace Perforce
    {
        class SyncCommand : public Command
        {
        public:
            SyncCommand( Provider* provider, RCS::File* file = NULL, const uint64_t timestamp = 0 )
                : Command ( provider, "sync" )
                , m_File ( file )
                , m_SyncTime( timestamp )
            {
            }

            virtual void OutputStat( StrDict* dict ) override;
            virtual void HandleError( Error* error ) override;
            virtual void Run() override;

        protected:
            RCS::File* m_File;
            uint64_t m_SyncTime;
        };

        class OpenCommand : public Command
        {
        public:
            OpenCommand( Provider* provider, const char* command = "", RCS::File* file = NULL )
                : Command ( provider, command )
                , m_File ( file )
            {

            }

            virtual void OutputStat( StrDict* dict ) override;
            virtual void Run() override;

        protected:
            RCS::File* m_File;
        };

        class IntegrateCommand : public Command
        {
        public:
            IntegrateCommand( Provider* provider, RCS::File* source, RCS::File* dest )
                : Command( provider, "integrate" )
                , m_Source( source )
                , m_Dest( dest )
            {
            }

            virtual void OutputStat( StrDict* dict ) override;
            virtual void Run() override;

        protected:
            RCS::File* m_Source;
            RCS::File* m_Dest;
        };
    }
}