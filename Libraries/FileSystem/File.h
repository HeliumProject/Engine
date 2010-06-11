#pragma once

#include "API.h"
#include "Exceptions.h"

#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"

namespace FileSystem
{
  namespace FileModes
  {
    enum FileMode
    {
      Read              = 1 << 0,
      Write             = 1 << 1,
      Append            = 1 << 2,
      Truncate          = 1 << 3,
      RandomAccess      = 1 << 4,
      SequentialAccess  = 1 << 5,
    };
  }
  typedef FileModes::FileMode FileMode;

  namespace FileOffsets
  {
    enum FileOffset
    {
      Beginning,
      Current,
      End,
    };
  }
  typedef FileOffsets::FileOffset FileOffset;

  typedef void* FileHandle;
  typedef i64 FileLocation;

  //
  // FileSystem::File class
  //
  //  Note: This class is 'sealed' - there is one kind of file on disk, so there
  //  is one file class. Rather then extended this class, other File-like classes
  //  should have a File object member.
  //
  //   1 - This class throws what are noted in the declarations below
  //   2 - Exception means you are probably using the class wrong (eg Read() before Open())
  //   3 - FileOperationException means that something you tried to do to a file failed
  //   4 - This class does not have to be heap allocated, but if you pass
  //       it through a smart pointer API, it will die before the API call returns
  //   5 - All Verify*() prototypes will throw on failure, but have
  //       non-throwing counterparts (should be obvious)
  //
  //  FIXME: Track file usage by placing events on file open, close, etc...
  //
  class FILESYSTEM_API File
  {
  public:
    File();
    File( const std::string& path );
    ~File();

  private:
    File( const File& rhs )
    {
      // No Copy Allowed
    }

  public:
    const std::string& GetPath();
    void SetPath(const std::string& path);

    bool IsOpen();
    void VerifyOpen();

    void Open( FileMode mode );
    void Open( const std::string& path, FileMode mode );
    void Close();
    
    const u32         GetSize();
    const FileHandle  GetHandle(){ return m_Handle; }
    u32 Read(u8* buffer, size_t amount);
    void Write(u8* buffer, size_t amount);

    FileLocation Tell();
    void Seek( FileLocation location, FileOffset offsetType = FileOffsets::Beginning );

    static void GenerateCRC( const std::string& filePath, std::string &hashString );
    static bool VerifyCRC( const std::string& filePath, const std::string &hashString );

    static void GenerateMD5( const std::string& filePath, std::string &hashString );
    static bool VerifyMD5( const std::string& filePath, const std::string &hashString );

	private:
    std::string m_Path;
    FileHandle m_Handle;
  };
}
