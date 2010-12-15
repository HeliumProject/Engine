#pragma once

#include <vector>
#include <hash_map>

#include "Platform/Types.h"

#include "API.h"
#include "ArchiveStream.h" 

//
//  String Pool Binary Format:
//
//  struct String
//  {
//      int32_t size;           // string length
//      char[] data;        // characters
//  };
//
//  struct StringPool
//  {
//      int32_t count;          // string count
//      String[] strings;   // string data
//      int32_t term;           // -1
//  };
//

namespace Helium
{
    namespace Reflect
    {
        //
        // String pool for serializing string data in binary
        //

        class FOUNDATION_API StringPool
        {
        public:
            typedef stdext::hash_map<tstring, int> M_StringToIndex;

            M_StringToIndex         m_Indices; 
            std::vector< tstring >  m_Strings;

            int32_t Insert( const tstring& str );
            const tstring& Get( int32_t index );

            void SerializeDirect( CharStream& stream ); 
            void DeserializeDirect( CharStream& stream, CharacterEncoding encoding ); 

            void SerializeCompressed( CharStream& stream ); 
            void DeserializeCompressed( CharStream& stream, CharacterEncoding encoding ); 

            void Serialize( class ArchiveBinary* archive );
            void Deserialize( class ArchiveBinary* archive, CharacterEncoding encoding );
        };
    }
}