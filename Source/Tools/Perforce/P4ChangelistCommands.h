#pragma once

#include "P4Command.h"

namespace Helium
{
    namespace Perforce
    {
        class GetChangelistsCommand : public Command
        {
        public:
            GetChangelistsCommand( Provider* provider, RCS::V_Changeset* changesets )
                : Command ( provider, "changes" )
                , m_Changesets( changesets )
            {
            }

            virtual void OutputStat( StrDict* dict ) override;
            virtual void Run() override;

        protected:
            RCS::V_Changeset* m_Changesets;
        };

        class CreateChangelistCommand : public Command
        {
        public:
            CreateChangelistCommand( Provider* provider, RCS::Changeset* changeset )
                : Command ( provider, "change" )
                , m_Changeset( changeset )
            {
            }

            virtual void Run() override;
            virtual void InputData( StrBuf *buf, Error *e ) override;
            virtual void OutputInfo( char level, const char* data ) override;

        protected:
            RCS::Changeset* m_Changeset;
        };

        class DeleteChangelistCommand : public Command
        {
        public:
            DeleteChangelistCommand( Provider* provider )
                : Command ( provider, "change" )
                , m_Changelist ( RCS::InvalidChangesetId )
            {
            }

            uint64_t GetChangelist()
            {
                return m_Changelist;
            }

        protected:
            uint64_t m_Changelist;
        };

        class OpenedCommand : public Command
        {
        public:
            OpenedCommand( Provider* provider, RCS::V_File* files )
                : Command ( provider, "opened" )
                , m_FileList( files )
            {
            }

            virtual void OutputStat( StrDict* dict ) override;

            RCS::V_File* m_FileList;
        };

        class SubmitCommand : public Command
        {
        public:
            SubmitCommand( Provider* provider, RCS::Changeset* changeset = NULL )
                : Command ( provider, "submit" )
                , m_Changeset( changeset )
            {
            }

            virtual void OutputStat( StrDict* dict );

            RCS::Changeset* m_Changeset;
        };

        class RevertCommand : public Command
        {
        public:
            RevertCommand( Provider* provider, RCS::File* file = NULL )
                : Command ( provider, "revert" )
                , m_File ( file )
            {
            }

            virtual void OutputStat( StrDict* dict );

        protected:
            RCS::File* m_File;
        };
    }
}