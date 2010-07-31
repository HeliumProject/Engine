#pragma once

#include <vector>
#include <hash_map>

#include "Platform/Types.h"

#include "API.h"
#include "Stream.h" 

//
//  String Pool Binary Format:
//
//  struct String
//  {
//      i32 size;           // string length
//      char[] data;        // characters
//  };
//
//  struct StringPool
//  {
//      i32 count;          // string count
//      String[] strings;   // string data
//      i32 term;           // -1
//  };
//

namespace Helium
{
    namespace Reflect
    {
        //
        // String pool for serializing string data in binary
        //

        namespace CharacterEncodings
        {
            enum CharacterEncoding
            {
                ASCII,  // default encoding, legacy 7-bit
                UTF_16, // used by windows' Unicode build
            };
        }
        typedef CharacterEncodings::CharacterEncoding CharacterEncoding;

        class FOUNDATION_API StringPool
        {
        public:
            typedef stdext::hash_map<tstring, int> M_StringToIndex;

            M_StringToIndex         m_Indices; 
            std::vector< tstring >  m_Strings;

            i32 Insert(const tstring& str);
            const tstring& Get(i32 index);

            void SerializeDirect(CharStream& stream); 
            void DeserializeDirect(CharStream& stream, CharacterEncoding encoding); 

            void SerializeCompressed(CharStream& stream); 
            void DeserializeCompressed(CharStream& stream, CharacterEncoding encoding); 

            void Serialize(class ArchiveBinary* archive); 
            void Deserialize(class ArchiveBinary* archive, CharacterEncoding encoding); 
        };
    }
}