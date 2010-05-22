#pragma once

#include "API.h"
#include "Exceptions.h"

#include "Common/Types.h"
#include "Common/Memory/SmartPtr.h"

namespace File
{ 
  class FILE_API Handle : public Nocturnal::RefCountBase< Handle >
  {
  public:
    Handle( const std::string& path, const char* mode = "r" );

    virtual ~Handle()
    {
      // we don't just call Close() because it is linked to Open() calls
      if ( m_FileHandle )
      {
        fclose( m_FileHandle );
      }
    }

    bool IsValid();

    bool Open();
    void Close();

    const std::string& GetPath() const
    {
      return m_Path;
    }

    const char* GetMode() const
    {
      return m_Mode;
    }

    FILE* Get() const
    {
      return m_FileHandle;
    }

  private:
    u32  m_OpenCount;
    std::string m_Path;
    const char* m_Mode;
    FILE* m_FileHandle;
  };

  typedef Nocturnal::SmartPtr< Handle > HandlePtr;
  typedef std::vector< HandlePtr > V_Handle;

}