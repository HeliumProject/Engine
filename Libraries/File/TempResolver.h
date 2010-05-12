#pragma once

#include "API.h"
#include "CacheDB.h" 
#include "ManagedFile.h"
#include "Exceptions.h"

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"
#include "EventSystem/EventSystem.h"

#include "TUID/TUID.h"

namespace File
{
  class TempResolver;
  typedef Nocturnal::SmartPtr< TempResolver > TempResolverPtr;

  class FILE_API TempResolver : public Nocturnal::RefCountBase< TempResolver >
  {
  public:
    TempResolver();
    virtual ~TempResolver();

    void ClearCache();

    tuid AddEntry( const std::string &filePath, const tuid tryId = TUID::Null );
    tuid AddEntry( ManagedFilePtr& file );
    tuid UpdateEntry( const ManagedFilePtr& file, const std::string &newFilePath );
    void DeleteEntry( const tuid id );

    ManagedFilePtr GetFile( const std::string& filePath );
    ManagedFilePtr GetFile( const tuid id );
    tuid GetFileID( const std::string &filePath );
    void GetFilePath( const tuid id, std::string &filePath );

  private:
    std::string         m_Username;

    typedef std::map< tuid, ManagedFilePtr > M_TuidManagedFile;
    typedef std::map< std::string, ManagedFilePtr > M_PathManagedFile;

    M_TuidManagedFile   m_TuidManagedFiles;
    M_PathManagedFile   m_PathManagedFiles;
    
    void InsertManagedFile( ManagedFilePtr& managedFile );
  };
}
