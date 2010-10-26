#include "StringPool.h"

#include "Serializer.h" 
#include "ArchiveBinary.h" 
#include "Compression.h" 

#include "Platform/Assert.h"
#include "Platform/String.h"
#include "Foundation/Memory/ArrayPtr.h" 
#include "Foundation/Log.h"

using Helium::ArrayPtr; 

#include <strstream>
#include <sstream>

using namespace Helium;
using namespace Helium::Reflect;

HELIUM_COMPILE_ASSERT( Reflect::CharacterEncodings::ASCII == 0 );
HELIUM_COMPILE_ASSERT( Reflect::CharacterEncodings::UTF_16 == 1 );

Profile::Accumulator g_StringPoolSerialize( "Reflect String Pool Serialize"); 
Profile::Accumulator g_StringPoolDeserialize( "Reflect String Pool Deserialize"); 
Profile::Accumulator g_StringPoolLookup( "Reflect String Pool Lookup"); 
Profile::Accumulator g_StringPoolInsert( "Reflect String Pool Insert"); 

int StringPool::Insert( const tstring& str )
{
    PROFILE_SCOPE_ACCUM(g_StringPoolInsert); 

    M_StringToIndex::iterator found = m_Indices.find( str );
    if ( found != m_Indices.end() )
    {
        return found->second;
    }

    int index = (int)m_Strings.size();
    m_Indices.insert( M_StringToIndex::value_type( str, index ) );
    m_Strings.push_back( str );

    return index;
}

const tstring& StringPool::Get( int index )
{
    PROFILE_SCOPE_ACCUM( g_StringPoolLookup );

    if ( index < 0 || index >= (int)m_Strings.size() )
    {
        throw Reflect::LogisticException( TXT( "String index out of range in StringPool" ) );
    }

    return m_Strings[ index ];
}

void StringPool::SerializeDirect( CharStream& stream )
{
#ifdef REFLECT_ARCHIVE_VERBOSE
    Log::Debug( TXT( "Serializing %d strings\n" ), m_Strings.size() );
#endif

    i32 size = (i32)m_Strings.size();
    stream.Write( &size ); 

    std::vector< tstring >::iterator itr = m_Strings.begin();
    std::vector< tstring >::iterator end = m_Strings.end();
    for ( int index=0; itr != end; ++itr, ++index )
    {
        size = (i32)itr->length();
        const tstring& str( *itr );

#ifdef REFLECT_ARCHIVE_VERBOSE
        Log::Debug( TXT(" [%d] : %s\n"), index, str.c_str() );
#endif

        stream.Write( &size );
        stream.WriteBuffer( str.c_str(), size * sizeof(tstring::traits_type::char_type) );
    }

    size = -1;
    stream.Write(&size); 
}

void StringPool::DeserializeDirect( CharStream& stream, CharacterEncoding encoding )
{
    i32 stringCount = 0;
    stream.Read( &stringCount );

#ifdef REFLECT_ARCHIVE_VERBOSE
    Log::Debug( TXT( "Deserializing %d strings\n" ), stringCount );
#endif

    m_Strings.resize( stringCount );
    for ( i32 i=0; i < stringCount; ++i )
    {
        i32 stringLength = 0;
        stream.Read( &stringLength );

        tstring& outputString = m_Strings[ i ]; 

        switch ( encoding )
        {
        case CharacterEncodings::ASCII:
            {
#ifdef UNICODE
                std::string temp;
                temp.resize( stringLength );
                stream.ReadBuffer( &temp[ 0 ], stringLength );
                Helium::ConvertString( temp, outputString );
#else
                // read the bytes directly into the string
                outputString.resize( stringLength ); 
                stream.ReadBuffer( &outputString[ 0 ], stringLength ); 
#endif
                break;
            }

        case CharacterEncodings::UTF_16:
            {
#ifdef UNICODE
                // read the bytes directly into the string
                outputString.resize( stringLength ); 
                stream.ReadBuffer( &outputString[ 0 ], stringLength * 2 ); 
#else
                std::wstring temp;
                temp.resize( stringLength );
                stream.ReadBuffer( &temp[ 0 ], stringLength * 2 ); 
                Helium::ConvertString( temp, outputString );
#endif
                break;
            }
        }

        // log the index
        m_Indices[ outputString ] = i; 

#ifdef REFLECT_ARCHIVE_VERBOSE
        Log::Debug( TXT( " [%d] : %s\n" ), i, outputString.c_str() );
#endif
    }

    stream.Read( &stringCount ); 
    HELIUM_ASSERT( stringCount == -1 );
}

void StringPool::SerializeCompressed( CharStream& stream )
{
    // in bytes... 
    u32 originalSize = 0; 
    u32 compressedSize = 0; 

    // save a place for the originalSize and compressedSize
    std::streamoff startOffset = stream.TellWrite(); 
    stream.Write( &originalSize );
    stream.Write( &compressedSize ); 

    // serialize the strings to a temp buffer
    std::stringstream memoryStream;
    Reflect::CharStream tempStream( &memoryStream ); 
    SerializeDirect( tempStream ); 

    // get the pointer
    originalSize   = (u32) memoryStream.tellp(); 
    compressedSize = CompressToStream( stream, memoryStream.str().c_str(), originalSize ); 

    // go back and record the size information in the stream. 
    stream.SeekWrite( startOffset, std::ios_base::beg ); 
    stream.Write( &originalSize ); 
    stream.Write( &compressedSize ); 
    stream.SeekWrite( 0, std::ios_base::end ); 
}

void StringPool::DeserializeCompressed( CharStream& stream, CharacterEncoding encoding )
{
    u32 originalSize = 0; 
    u32 compressedSize = 0; 

    stream.Read( &originalSize ); 
    stream.Read( &compressedSize ); 

    ArrayPtr<char> helper( new char[ originalSize ] );
    char* originalData = helper.Ptr(); 

    int inflatedSize = DecompressFromStream( stream, compressedSize, originalData, originalSize ); 
    if ( inflatedSize != originalSize )
    {
        throw Reflect::StreamException( TXT( "StringPool failed to read compressed data" ) ); 
    }

    std::stringstream memoryStream;
    std::streamsize copied = memoryStream.rdbuf()->sputn( originalData, originalSize );
    HELIUM_ASSERT( copied == originalSize );

    Reflect::CharStream tempStream( &memoryStream ); 
    DeserializeDirect( tempStream, encoding ); 
}

void StringPool::Serialize( ArchiveBinary* archive )
{
    PROFILE_SCOPE_ACCUM( g_StringPoolSerialize ); 

    Reflect::CharStream& stream = archive->GetStream(); 

    HELIUM_ASSERT( m_Strings.size() == m_Indices.size() );
    return SerializeCompressed( stream ); 
}

void StringPool::Deserialize( ArchiveBinary* archive, CharacterEncoding encoding )
{
    PROFILE_SCOPE_ACCUM( g_StringPoolDeserialize ); 

    Reflect::CharStream& stream = archive->GetStream(); 

    return DeserializeCompressed( stream, encoding ); 

    HELIUM_ASSERT( m_Strings.size() == m_Indices.size() );
}
