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
  ////////////////////////////////////////////////////////////////////////////
  // ManagedFile class
  class FILE_API ManagedFile : public Reflect::Element
  {
  public:
    tuid              m_Id;
    u64               m_Created;
    u64               m_Modified;
    std::string       m_Path;
    u32               m_UserId;
    std::string       m_Username;
    bool              m_WasDeleted;

  public:
    ManagedFile();
    virtual ~ManagedFile();

    bool IsValid() const { return ( m_Id != TUID::Null ); }

    bool operator<( const ManagedFile& rhs ) const;
    bool operator==( const ManagedFile& rhs ) const;

  public:
    REFLECT_DECLARE_CLASS( ManagedFile, Reflect::Element );
    static void EnumerateClass( Reflect::Compositor<ManagedFile>& comp );
  };
  typedef Nocturnal::SmartPtr< ManagedFile > ManagedFilePtr;
  typedef std::vector< ManagedFilePtr > V_ManagedFilePtr;
  typedef std::set< ManagedFilePtr > S_ManagedFilePtr;
  

  /////////////////////////////////////////////////////////////////////////////
  // ManagedFileStash is for stashing managed file data in reflect files
  class FILE_API ManagedFileStash : public Reflect::Element
  {
  public:
    V_ManagedFilePtr m_Files;
    REFLECT_DECLARE_CLASS( ManagedFileStash, Reflect::Element );
    static void EnumerateClass( Reflect::Compositor<ManagedFileStash>& comp );
  };
  typedef Nocturnal::SmartPtr< ManagedFileStash > ManagedFileStashPtr;


} // namespace File