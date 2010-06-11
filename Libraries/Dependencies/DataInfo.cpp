#include "DataInfo.h"

#include "API.h"
#include "Exceptions.h"

#include "Foundation/Flags.h"

#include "Finder/Finder.h"
#include "FileSystem/FileSystem.h"
#include "FileSystem/File.h"

#include "Foundation/Log.h"

#include "SQL/SQL.h"

#include <cryptlib.h>
#include <md5.h>
#include <filters.h>
#include <hex.h> 

#include "Foundation/String/Utilities.h"

namespace Dependencies
{
  /////////////////////////////////////////////////////////////////////////////
  DataInfo::DataInfo() 
    : DependencyInfo()

  {
    m_LastModified = 0;

  }


  /////////////////////////////////////////////////////////////////////////////
  DataInfo::DataInfo( const std::string &path, const Finder::FileSpec &fileSpec, const GraphConfigs graphConfigs ) 
    : DependencyInfo( path, fileSpec, graphConfigs )

  {
    m_LastModified = 0;
  }

  DataInfo::DataInfo( const std::string &path, const Finder::FileSpec &fileSpec, u8* data, 
    const size_t dataSize, const GraphConfigs graphConfigs ) 
    : DependencyInfo( path, fileSpec, graphConfigs )
    , m_Data(data)
    , m_DataSize(dataSize)
  {
    m_LastModified = 0;
  }
  
  /////////////////////////////////////////////////////////////////////////////
  // Used to determine if the file has changed on disk since the MD5 was generated.
  //
  bool DataInfo::IsMD5Valid()
  {
    
    if ( m_MD5.empty() )
    {
      m_IsMD5Valid = false;
    }

    return m_IsMD5Valid;
  }

  /////////////////////////////////////////////////////////////////////////////
  // Used force generate a new MD5
  //
  void DataInfo::GenerateMD5()
  {  
    
    CryptoPP::MD5 hash;
    CryptoPP::StringSource source( m_Data, m_DataSize, false, new CryptoPP::HashFilter( hash ) );

    // attached the HexEncoder
    source.Attach( new CryptoPP::HexEncoder );

    // PumpAll appends instead of overwriting
    m_MD5 = "";

    // attach the hash filter with string sink
    source.Attach( new CryptoPP::StringSink( m_MD5 ) );
    source.PumpAll();

    // ensure that the string is upper case
    toUpper( m_MD5 );

     m_IsMD5Valid = true;
     
  }

  /////////////////////////////////////////////////////////////////////////////
  // Determines if the file was modified since it was last cached.
  //
  // Returns TRUE if:
  //  - the FileSpec no longer exits
  //  - the file does not currently exist in the GraphDB (invalid RowID)
  //  - expected FormatVersion has changed (builder updated)
  //  - the file has changed on disk
  //    Determines if the file has been modified on disk since its info was cached.
  //    TRUE if:
  //    * file does not exist on disc
  //    * the last modified time and size have changed
  //    * the cached md5 is not the same as the md5 of the file on disk
  //
  bool DataInfo::WasModified()
  {
    // the file does not currently exist in the GraphDB (invalid RowID)
    if ( m_VersionRowID == SQL::InvalidRowID || m_RowID == SQL::InvalidRowID )
    {
      //Log::Bullet wasFileModifiedBullet( Log::Streams::Debug, Log::Levels::Verbose, "File was modified: the file does not currently exist in the GraphDB (invalid RowID)\n" );
      return true;
    }
    // expected FormatVersion has changed (builder updated)
    else if ( !m_Spec || ( m_FormatVersion != m_Spec->GetFormatVersion() ) )
    {
      //Log::Bullet wasFileModifiedBullet( Log::Streams::Debug, Log::Levels::Verbose, "File was modified: expected FormatVersion has changed (builder updated)\n" );
      return true;
    }

    if ( m_MD5.empty() )
    {
      return true;
    }
    else
    {
      FILEINFO_SCOPE_TIMER(("GenerateMD5"));

      std::string curMD5;
      CryptoPP::MD5 hash;
      CryptoPP::StringSource source( m_Data, m_DataSize, false, new CryptoPP::HashFilter( hash ) );

      // attached the HexEncoder
      source.Attach( new CryptoPP::HexEncoder );

      // attach the hash filter with string sink
      source.Attach( new CryptoPP::StringSink( curMD5 ) );
      source.PumpAll();

      // ensure that the string is upper case
      toUpper( curMD5 );

      //the cached md5 is not the same as the md5 of the file on disk
      if ( m_MD5 == curMD5 )
      {
        return false;
      }
      else
      {
        //Log::Bullet wasFileModifiedBullet( Log::Streams::Debug, Log::Levels::Verbose, "File was modified: the cached md5 is not the same as the md5 of the file on disk\n" );
        return true;
      }
    }
   
    return false;
  }

  /////////////////////////////////////////////////////////////////////////////
  bool DataInfo::AppendToSignature( CryptoPP::HashFilter* hashFilter, V_string& trace)
  {

    bool dirtyFile = false;
    if ( !Exists() )
    {
      Log::Bullet cacheGetGraph( Log::Streams::Debug, Log::Levels::Verbose, "CreateSignature is skipping optional input dependency (%s)\n", m_Path.c_str() );
      return dirtyFile;
    }

    std::string md5;

    if ( !IsMD5Valid() )
    {
      GenerateMD5();
      dirtyFile = true;
    }

    md5 = m_MD5;

    {
      DEPENDENCIES_SCOPE_TIMER(("Signature Hashing"));
      hashFilter->Put( ( byte const* ) md5.data(), md5.size() );
    }

    trace.push_back( md5 );
    trace.back() += " (" + m_Path + ")";

    return dirtyFile;
  }

  void DataInfo::CacheCopy( DependencyInfo& rhs )
  {
    __super::CacheCopy( rhs );

    DataInfo* info = dynamic_cast< DataInfo* >(&rhs);

    if( info )
    {
      m_Data = info->m_Data;
      m_DataSize = info->m_DataSize;
    }
  }


  /////////////////////////////////////////////////////////////////////////////
  DataInfo::~DataInfo()
  {

  }

  
  /////////////////////////////////////////////////////////////////////////////
  // Copy from one file to this
  void DataInfo::CopyFrom( const DependencyInfo &rhs )
  {
    if ( this != &rhs )
    {
      __super::CopyFrom( rhs );

      const DataInfo* dataInfo = dynamic_cast< const DataInfo* >(&rhs);
      if( dataInfo )
      {
        m_Data = dataInfo->m_Data;
        m_DataSize = dataInfo->m_DataSize;
      }
      
    }

  }

  /////////////////////////////////////////////////////////////////////////////
  // Determines if two dependencies are equal
  bool DataInfo::IsEqual( const DependencyInfo &rhs ) const
  {
    FILEINFO_SCOPE_TIMER((""));

    bool eq = true;

    eq = eq && __super::IsEqual( rhs );

    const DataInfo* dataInfo = dynamic_cast< const DataInfo* >(&rhs);
    if( dataInfo )
    {
      eq = eq && ( m_Data     == dataInfo->m_Data );
      eq = eq && ( m_DataSize == dataInfo->m_DataSize );
    }
    return eq;
  }
}

