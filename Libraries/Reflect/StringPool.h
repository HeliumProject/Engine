#pragma once

#include <vector>
#include <hash_map>

#include "Platform/Types.h"

#include "API.h"
#include "Stream.h" 

/*

 String Pool Binary Format:

  struct String
  {
    i32 size;           // string length
    char[] data;        // characters
  };

  struct StringPool
  {
    i32 count;          // string count
    String[] strings;   // string data
    i32 term;           // -1
  };

*/

namespace Reflect
{
  class ArchiveBinary; 

  //
  // String pool for serializing string data in binary
  //

  class REFLECT_API StringPool
  {
  public:
    typedef stdext::hash_map<std::string, int> M_StringToIndex; 
    
    std::vector<std::string> m_Strings;
    M_StringToIndex          m_Indices; 

    int GetIndex(const std::string& str);
    int AssignIndex(const std::string& str);
    const std::string& GetString(int index);

    void Serialize    (ArchiveBinary* archive); 
    void Deserialize  (ArchiveBinary* archive); 

  private: 
    void SerializeDirect(Reflect::Stream& stream); 
    void SerializeCompressed(Reflect::Stream& stream); 

    void DeserializeDirect(Stream& stream); 
    void DeserializeCompressed(Stream& stream); 
  };
}
