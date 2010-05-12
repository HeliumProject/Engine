#pragma once

#include "API.h"
#include "PatchOperations.h"

#include "Common/Types.h"

namespace File
{
  /////////////////////////////////////////////////////////////////////////////
  // Used to store managed file revision history
  class FILE_API FileHistory
  {
  public:
    u64            m_Timestamp;
    PatchOperation m_Operation;
    std::string    m_Username;
    std::string    m_Data;

    FileHistory( const u64 timestamp, const PatchOperation operation, const std::string& username, const std::string& data = std::string( "" ) )
      : m_Timestamp( timestamp )
      , m_Operation( operation )
      , m_Username( username )
      , m_Data( data )
    {

    }

    // this function is to support correct sorting in sets 
    inline bool operator<( const FileHistory &rhs ) const
    {
      return m_Timestamp < rhs.m_Timestamp;
    }
  };
  typedef FILE_API std::set< File::FileHistory > S_FileHistory;


  // map of modified times and computer/username who modified the file
  typedef FILE_API std::map< u64, std::pair< PatchOperation, std::string > > M_FileHistory;
}