#include "StringPool.h"

#include "Serializer.h" 
#include "ArchiveBinary.h" 
#include "CompressionUtilities.h" 

#include "Platform/Assert.h"
#include "Foundation/Memory/ArrayPtr.h" 
#include "Foundation/Log.h"

using Nocturnal::ArrayPtr; 

#include <strstream>
#include <sstream>

using namespace Reflect;

Profile::Accumulator g_StringPoolSerialize("Reflect String Pool Serialize"); 
Profile::Accumulator g_StringPoolDeserialize("Reflect String Pool Deserialize"); 
Profile::Accumulator g_StringPoolLookup("Reflect String Pool Lookup"); 
Profile::Accumulator g_StringPoolInsert("Reflect String Pool Insert"); 

int StringPool::GetIndex(const std::string& str)
{
    PROFILE_SCOPE_ACCUM(g_StringPoolLookup); 

    M_StringToIndex::iterator found = m_Indices.find(str);

    if (found != m_Indices.end())
    {
        return found->second;
    }
    else
    {
        return -1;
    }
}

int StringPool::AssignIndex(const std::string& str)
{
    int index = GetIndex(str);

    if (index < 0)
    {
        PROFILE_SCOPE_ACCUM(g_StringPoolInsert); 

        index = (int)m_Strings.size();
        m_Indices.insert( M_StringToIndex::value_type(str, index));
        m_Strings.push_back(str);
    }

    return index;
}

const std::string& StringPool::GetString(int index)
{
    PROFILE_SCOPE_ACCUM(g_StringPoolLookup); 

    NOC_ASSERT(index >=0 && index < (int)m_Strings.size());
    return m_Strings[ index ];
}

void StringPool::Serialize(ArchiveBinary* archive)
{
    PROFILE_SCOPE_ACCUM(g_StringPoolSerialize); 

    Reflect::Stream& stream = archive->GetOutput(); 

    NOC_ASSERT(m_Strings.size() == m_Indices.size());
    return SerializeCompressed(stream); 
}

void StringPool::Deserialize(ArchiveBinary* archive)
{
    PROFILE_SCOPE_ACCUM(g_StringPoolDeserialize); 

    Reflect::Stream& stream = archive->GetInput(); 

    if(archive->GetVersion() >= ArchiveBinary::FIRST_VERSION_WITH_STRINGPOOL_COMPRESSION)
    {
        return DeserializeCompressed(stream); 
    }
    else
    {
        return DeserializeDirect(stream); 
    }

    NOC_ASSERT(m_Strings.size() == m_Indices.size());
}

void StringPool::SerializeDirect(Reflect::Stream& stream)
{
#ifdef REFLECT_ARCHIVE_VERBOSE
    Log::Debug("Serializing %d strings\n", m_Strings.size());
#endif

    i32 size = (i32)m_Strings.size();
    stream.Write(&size); 

    std::vector<std::string>::iterator itr = m_Strings.begin();
    std::vector<std::string>::iterator end = m_Strings.end();
    for ( int index=0; itr != end; ++itr, ++index )
    {
        size = (i32)itr->length();
        const std::string& str (*itr);

#ifdef REFLECT_ARCHIVE_VERBOSE
        Log::Debug(" [%d] : %s\n", index, str.c_str());
#endif

        stream.Write(&size); 
        stream.WriteBuffer(str.c_str(), size);
    }

    size = -1;
    stream.Write(&size); 
}

void StringPool::DeserializeDirect(Reflect::Stream& stream)
{
    i32 size;
    stream.Read(&size);

#ifdef REFLECT_ARCHIVE_VERBOSE
    Log::Debug("Deserializing %d strings\n", size);
#endif

    m_Strings.resize(size);
    for (i32 i=0; i<size; ++i)
    {
        i32 string_size;
        stream.Read(&string_size); 

        // read the bytes directly into the string
        std::string& outputString = m_Strings[i]; 
        outputString.resize(string_size); 
        stream.ReadBuffer(&outputString[0], string_size); 

        // log the index
        m_Indices[outputString] = i; 

#ifdef REFLECT_ARCHIVE_VERBOSE
        Log::Debug(" [%d] : %s\n", i, outputString.c_str());
#endif
    }

    stream.Read(&size); 
    NOC_ASSERT(size == -1);

}

void StringPool::SerializeCompressed(Reflect::Stream& stream)
{
    // in bytes... 
    u32 originalSize = 0; 
    u32 compressedSize = 0; 

    // save a place for the originalSize and compressedSize
    std::streamoff startOffset = stream.TellWrite(); 
    stream.Write(&originalSize);
    stream.Write(&compressedSize); 

    // serialize the strings to a temp buffer
    std::strstream memoryStream; 
    Reflect::Stream tempStream(&memoryStream, false); 
    SerializeDirect(tempStream); 

    // get the pointer
    const char* stringBuff = memoryStream.str(); 

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

void StringPool::DeserializeCompressed(Reflect::Stream& stream)
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
        throw Reflect::StreamException("StringPool failed to read compressed data"); 
    }

    std::strstream memoryStream(originalData, originalSize); 
    Reflect::Stream tempStream(&memoryStream, false); 

    DeserializeDirect(tempStream); 
}
