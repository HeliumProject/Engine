#include "StringPool.h"

#include "Serializer.h" 
#include "ArchiveBinary.h" 
#include "Compression.h" 

#include "Platform/Assert.h"
#include "Platform/String.h"
#include "Foundation/Memory/ArrayPtr.h" 
#include "Foundation/Log.h"

using Nocturnal::ArrayPtr; 

#include <strstream>
#include <sstream>

using namespace Reflect;

Profile::Accumulator g_StringPoolSerialize( "Reflect String Pool Serialize"); 
Profile::Accumulator g_StringPoolDeserialize( "Reflect String Pool Deserialize"); 
Profile::Accumulator g_StringPoolLookup( "Reflect String Pool Lookup"); 
Profile::Accumulator g_StringPoolInsert( "Reflect String Pool Insert"); 

int StringPool::Insert( const std::string& str )
{
    PROFILE_SCOPE_ACCUM(g_StringPoolInsert); 

    M_CharStringToIndex::iterator found = m_CharIndices.find( str );
    if ( found != m_CharIndices.end() )
    {
        return found->second;
    }

    int index = (int)m_CharStrings.size();
    m_CharIndices.insert( M_CharStringToIndex::value_type(str, index));
    m_CharStrings.push_back(str);

    return index;
}

int StringPool::Insert( const std::wstring& str )
{
    PROFILE_SCOPE_ACCUM(g_StringPoolInsert); 

    M_WideStringToIndex::iterator found = m_WideIndices.find( str );
    if ( found != m_WideIndices.end() )
    {
        return found->second;
    }

    int index = (int)m_WideStrings.size();

    m_WideIndices.insert( M_WideStringToIndex::value_type( str, index ) );
    m_WideStrings.push_back( str );

    return index;
}

tstring StringPool::GetString( int index )
{
    PROFILE_SCOPE_ACCUM(g_StringPoolLookup); 

    tstring result;

    if ( index >= 0 )
    {
        if( index >= (int)m_CharStrings.size() )
        {
            throw Reflect::LogisticException( TXT( "String index out of range in StringPool" ) );
        }

        Platform::ConvertString( m_CharStrings[ index ], result );
    }
    else
    {
        index &= 0xEFFFFFFF; // make out the high bit

        if( index < 0 || index >= (int)m_WideStrings.size() )
        {
            throw Reflect::LogisticException( TXT( "Wide string index out of range in StringPool" ) );
        }

        Platform::ConvertString( m_WideStrings[ index ], result );
    }

    return result;
}

void StringPool::Serialize(ArchiveBinary* archive)
{
    PROFILE_SCOPE_ACCUM(g_StringPoolSerialize); 

    Reflect::CharStream& stream = archive->GetStream(); 

    NOC_ASSERT(m_CharStrings.size() == m_CharIndices.size());
    return SerializeCompressed(stream); 
}

void StringPool::Deserialize(ArchiveBinary* archive)
{
    PROFILE_SCOPE_ACCUM(g_StringPoolDeserialize); 

    Reflect::CharStream& stream = archive->GetStream(); 

    if(archive->GetVersion() >= ArchiveBinary::FIRST_VERSION_WITH_STRINGPOOL_COMPRESSION)
    {
        return DeserializeCompressed(stream); 
    }
    else
    {
        return DeserializeDirect(stream); 
    }

    NOC_ASSERT(m_CharStrings.size() == m_CharIndices.size());
}

void StringPool::SerializeDirect(CharStream& stream)
{
#ifdef REFLECT_ARCHIVE_VERBOSE
    Log::Debug(TXT("Serializing %d strings\n"), m_CharStrings.size());
#endif

    i32 size = (i32)m_CharStrings.size();
    stream.Write(&size); 

    std::vector<std::string>::iterator itr = m_CharStrings.begin();
    std::vector<std::string>::iterator end = m_CharStrings.end();
    for ( int index=0; itr != end; ++itr, ++index )
    {
        size = (i32)itr->length();
        const std::string& str (*itr);

#ifdef REFLECT_ARCHIVE_VERBOSE
        Log::Debug(TXT(" [%d] : %s\n"), index, str.c_str());
#endif

        stream.Write(&size); 
        stream.WriteBuffer(str.c_str(), size);
    }

    size = -1;
    stream.Write(&size); 
}

void StringPool::DeserializeDirect(CharStream& stream)
{
    i32 size;
    stream.Read(&size);

#ifdef REFLECT_ARCHIVE_VERBOSE
    Log::Debug(TXT("Deserializing %d strings\n"), size);
#endif

    m_CharStrings.resize(size);
    for (i32 i=0; i<size; ++i)
    {
        i32 string_size;
        stream.Read(&string_size); 

        // read the bytes directly into the string
        std::string& outputString = m_CharStrings[i]; 
        outputString.resize(string_size); 
        stream.ReadBuffer(&outputString[0], string_size); 

        // log the index
        m_CharIndices[outputString] = i; 

#ifdef REFLECT_ARCHIVE_VERBOSE
        Log::Debug(TXT(" [%d] : %s\n"), i, outputString.c_str());
#endif
    }

    stream.Read(&size); 
    NOC_ASSERT(size == -1);
}

void StringPool::SerializeCompressed(CharStream& stream)
{
    // in bytes... 
    u32 originalSize = 0; 
    u32 compressedSize = 0; 

    // save a place for the originalSize and compressedSize
    std::streamoff startOffset = stream.TellWrite(); 
    stream.Write(&originalSize);
    stream.Write(&compressedSize); 

    // serialize the strings to a temp buffer
    std::stringstream memoryStream; 
    Reflect::CharStream tempStream(&memoryStream, false); 
    SerializeDirect(tempStream); 

    // get the pointer
    const char* stringBuff = memoryStream.str().c_str();

    // we own that buffer now... 
    ArrayPtr<const char> helper(stringBuff); 

    originalSize   = (u32) memoryStream.tellp(); 
    compressedSize = CompressToStream(stream, stringBuff, originalSize); 

    // go back and record the size information in the stream. 
    stream.SeekWrite(startOffset, std::ios_base::beg); 
    stream.Write(&originalSize); 
    stream.Write(&compressedSize); 
    stream.SeekWrite(0, std::ios_base::end); 
}

void StringPool::DeserializeCompressed(CharStream& stream)
{
    u32 originalSize = 0; 
    u32 compressedSize = 0; 

    stream.Read(&originalSize); 
    stream.Read(&compressedSize); 

    ArrayPtr<char> helper(new char[originalSize]); 

    char* originalData = helper.Ptr(); 
    int   inflatedSize = DecompressFromStream(stream, compressedSize, originalData, originalSize); 

    if(inflatedSize != originalSize)
    {
        throw Reflect::StreamException( TXT( "StringPool failed to read compressed data" ) ); 
    }

    std::stringstream memoryStream(originalData, originalSize); 
    Reflect::CharStream tempStream(&memoryStream, false); 

    DeserializeDirect(tempStream); 
}
