#pragma once

#include "API.h"
#include "File.h"
#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"

namespace RCS
{
  class RCS_API Changeset : public Nocturnal::RefCountBase< Changeset >
  {
  public:
    u64                m_Id;
    std::string        m_Description;
    V_File             m_Files;

    Changeset( const std::string& description = std::string( "" ), int changesetId = DefaultChangesetId )
      : m_Id( changesetId )
      , m_Description( description )
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
    void Delete( RCS::File& file, const RCS::OpenFlag flags = RCS::OpenFlags::Default ) const;

    void Reopen( RCS::File& file, const RCS::OpenFlag flags = RCS::OpenFlags::Default ) const;

    bool operator==( const RCS::Changeset& rhs ) const
    {
      return m_Id == rhs.m_Id;
    }
  }; 

  typedef std::vector< Changeset > V_Changeset;
  typedef Nocturnal::SmartPtr< Changeset > ChangesetPtr;
  typedef std::vector< ChangesetPtr > V_ChangesetPtr;
}