#pragma once

#include "API.h"
#include "Exceptions.h"

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"

#include "FileSystem/File.h"

#include "Reflect/Registry.h"
#include "Reflect/Serializers.h"

#include "TUID/TUID.h"

namespace File
{ 
  class FILE_API Reference : public Reflect::Element
  {
  public:

    Reference( const std::string& path = "" );
    virtual ~Reference();

  public:
    tuid              m_Id;

    std::string       m_LastUsername;

  private:

    // reference info
    u64               m_CreationTime;
    u64               m_ModifiedTime;

    // file info
    std::string       m_Path;
    std::string       m_Signature;
    u64               m_FileModifiedTime;
    u64               m_FileCreationTime;
    u64               m_FileSize;

  public:
    REFLECT_DECLARE_CLASS( Reference, Reflect::Element );

    static void EnumerateClass( Reflect::Compositor< Reference >& comp );

  public:

    std::string AsString( bool verbose = false );

    bool IsValid();

    bool operator<( const Reference& rhs ) const
    {
      return ( _stricmp( m_Path.c_str(), rhs.m_Path.c_str() ) < 0 );
    }

    bool operator==( const Reference& rhs ) const
    {
      return ( m_Id == rhs.m_Id );
    }


    std::string GetPath()
    {
      Update();
      return m_Path;
    }
    void SetPath( const std::string& path );

    std::string GetSignature()
    {
      Update();
      return m_Signature;
    }

  private:
    void Update();

  };

  typedef Nocturnal::SmartPtr< Reference > ReferencePtr;
  typedef std::vector< ReferencePtr > V_Reference;
}