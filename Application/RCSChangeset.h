#pragma once

#include "Platform/Types.h"

#include "Application/API.h"
#include "Foundation/SmartPtr.h"
#include "Application/RCSFile.h"

namespace Helium
{
    namespace RCS
    {
        class HELIUM_APPLICATION_API Changeset : public Helium::RefCountBase< Changeset >
        {
        public:
            uint64_t                m_Id;
            tstring            m_Description;
            V_File             m_Files;

            Changeset( const tstring& description = TXT( "" ), int changesetId = DefaultChangesetId )
                : m_Id( changesetId )
                , m_Description( description )
            {
            }
            virtual ~Changeset()
            {
            }

            inline void Clear()
            {
                new (this) Changeset();
            }

            void Create();
            void Commit();
            void Revert( const OpenFlag flags = OpenFlags::Default );

            void Open( RCS::File& file, const RCS::OpenFlag flags = RCS::OpenFlags::Default ) const;
            void Edit( RCS::File& file, const RCS::OpenFlag flags = RCS::OpenFlags::Default ) const;
            void Copy( RCS::File& source, RCS::File& target, const RCS::OpenFlag flags = RCS::OpenFlags::Default ) const;
            void Rename( RCS::File& source, RCS::File& target, const RCS::OpenFlag flags = RCS::OpenFlags::Default ) const;
            void Delete( RCS::File& file, const RCS::OpenFlag flags = RCS::OpenFlags::Default ) const;

            void Reopen( RCS::File& file, const RCS::OpenFlag flags = RCS::OpenFlags::Default ) const;

            bool operator==( const RCS::Changeset& rhs ) const
            {
                return m_Id == rhs.m_Id;
            }
        }; 

        typedef std::vector< Changeset > V_Changeset;
        typedef Helium::SmartPtr< Changeset > ChangesetPtr;
        typedef std::vector< ChangesetPtr > V_ChangesetPtr;
    }
}