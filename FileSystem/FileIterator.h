#pragma once

#include "API.h"
#include "Exceptions.h"
#include "Common/Types.h"

namespace FileSystem
{
  namespace IteratorFlags
  {
    enum Flags
    {
      // Default: Find both files and directories; Mark directories with a trailing slash;
      // prepend the root path; clean file names; Return an empty vector if no files are 
      // found; Does not catch unexpected exceptions; Sorts the result vector before returning.
      NoFiles        = 1 << 0,          // Skip files
      NoDirs         = 1 << 1,          // Ship directories

      NoMarkDir      = 1 << 2,          // Don't append trailing slash on directory
      NoPrependRoot  = 1 << 3,          // Preped each file with the root path
      NoClean        = 1 << 4,          // Don't clean the file names using StandardName
    };

    const u32 Default = 0;
  }
  typedef u32 FileIteratorFlags;

  typedef void* FileIteratorHandle;

  /////////////////////////////////////////////////////////////////////////////
  //
  // Fileterator class:
  //   1 - This class throws what are noted in the declarations below
  //   2 - Exception means you are probably using the class wrong
  //   3 - This class does not have to be heap allocated, but if you pass it through a smart pointer API,
  //       it will die before the API you called returns
  //   4 - All Verify*() prototypes will throw on failure, but have non-throwing counterparts (should be obvious)
  //
  // Usage: see UnitTest.cpp
  // 
  class FILESYSTEM_API FileIterator
  {
  public:
    FileIterator();
    FileIterator( const std::string &root, const std::string &spec = "*.*", const FileIteratorFlags flag = IteratorFlags::Default );
    ~FileIterator();

    bool Next();
    void Reset();
    bool IsDone();
    const std::string& Item();

  private:
    bool Open(const std::string &root, const std::string &spec = "*.*" , const FileIteratorFlags flag = IteratorFlags::Default);
    bool Find(bool first = false);
    void Close();

    std::string         m_Root;
    std::string         m_Spec;
    std::string         m_Query;
    FileIteratorFlags   m_Flags;
    FileIteratorHandle  m_Handle;
    std::string         m_Item;
    bool                m_Done;
  };
}