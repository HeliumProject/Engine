#include "ReflectPch.h"
#include "Reflect/Data/MapData.h"

#include "Reflect/Data/DataDeduction.h"
#include "Reflect/ArchiveBinary.h"
#include "Reflect/ArchiveXML.h"

REFLECT_DEFINE_ABSTRACT( Helium::Reflect::MapData )

using namespace Helium;
using namespace Helium::Reflect;

// Tokenizer adapted from:
// http://www.oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
// str should contain a string with map object separated by the specified delimiters argument.
// str will be parsed into key-value pairs and each pair will be inserted into tokens.
template< typename TKey, typename TVal, typename TEqualKey, typename TAllocator >
inline void Tokenize( const String& str, Map< TKey, TVal, TEqualKey, TAllocator >& tokens, const tchar_t* delimiters )
{
    // Skip delimiters at beginning.
    size_t lastPos = str.FindNone( delimiters );
    // Find first "non-delimiter".
    size_t pos     = str.FindAny( delimiters, lastPos );

    while ( IsValid( pos ) || IsValid( lastPos ) )
    {
        tstringstream kStream( *str.Substring( lastPos, pos - lastPos ) );

        // Skip delimiters.  Note the "not_of"
        lastPos = str.FindNone( delimiters, pos );
        // Find next "non-delimiter"
        pos = str.FindAny( delimiters, lastPos );

        if ( IsValid( pos ) || IsValid( lastPos ) )
        {
            tstringstream vStream( *str.Substring( lastPos, pos - lastPos ) );

            // At this point, we have the key and value.  Build the map entry.
            // Note that the stream operator stops at spaces.
            TKey k;
            kStream >> k;
            TVal v;
            vStream >> v;
            tokens[ k ] = v;

            // Skip delimiters.  Note the "not_of"
            lastPos = str.FindNone( delimiters, pos );

            // Find next "non-delimiter"
            pos = str.FindAny( delimiters, lastPos );
        }
        else
        {
            // Delimited map is messed up. It should alternate key-value pairs, but there is
            // a key without a value.
            HELIUM_BREAK();
        }
    }
}

// Partial specialization for strings as TVal, that gets around the stream operator stopping
// at spaces by not using a stream at all.
template< typename TKey, typename TVal, typename TEqualKey, typename TAllocator >
inline void Tokenize( const String& str, Map< TKey, String, TEqualKey, TAllocator >& tokens, const tchar_t* delimiters )
{
    // Skip delimiters at beginning.
    size_t lastPos = str.FindNone( delimiters );
    // Find first "non-delimiter".
    size_t pos     = str.FindAny( delimiters, lastPos );

    while ( IsValid( pos ) || IsValid( lastPos ) )
    {
        tstringstream kStream( *str.Substring( lastPos, pos - lastPos ) );

        // Skip delimiters.  Note the "not_of"
        lastPos = str.FindNone( delimiters, pos );
        // Find next "non-delimiter"
        pos = str.FindAny( delimiters, lastPos );

        if ( IsValid( pos ) || IsValid( lastPos ) )
        {
            // At this point, we have the key and value.  Build the map entry.
            TKey k;
            kStream >> k;
            str.Substring( tokens[ k ], lastPos, pos - lastPos );

            // Skip delimiters.  Note the "not_of"
            lastPos = str.FindNone( delimiters, pos );

            // Find next "non-delimiter"
            pos = str.FindAny( delimiters, lastPos );
        }
        else
        {
            // Delimited map is messed up. It should alternate key-value pairs, but there is
            // a key without a value.
            HELIUM_BREAK();
        }
    }
}

// Partial specialization for strings as TKey, that gets around the stream operator stopping
// at spaces by not using a stream at all.
template< typename TKey, typename TVal, typename TEqualKey, typename TAllocator >
inline void Tokenize( const String& str, Map< String, TVal, TEqualKey, TAllocator >& tokens, const tchar_t* delimiters )
{
    // Skip delimiters at beginning.
    size_t lastPos = str.FindNone( delimiters );
    // Find first "non-delimiter".
    size_t pos     = str.FindAny( delimiters, lastPos );

    while ( IsValid( pos ) || IsValid( lastPos ) )
    {
        String key;
        str.Substring( key, lastPos, pos - lastPos );

        // Skip delimiters.  Note the "not_of"
        lastPos = str.FindNone( delimiters, pos );
        // Find next "non-delimiter"
        pos = str.FindAny( delimiters, lastPos );

        if ( IsValid( pos ) || IsValid( lastPos ) )
        {
            tstringstream vStream( *str.Substring( lastPos, pos - lastPos ) );

            // At this point, we have the key and value.  Build the map entry.
            TVal v;
            vStream >> v;
            tokens[ key ] = v;

            // Skip delimiters.  Note the "not_of"
            lastPos = str.FindNone( delimiters, pos );

            // Find next "non-delimiter"
            pos = str.FindAny( delimiters, lastPos );
        }
        else
        {
            // Delimited map is messed up. It should alternate key-value pairs, but there is
            // a key without a value.
            HELIUM_BREAK();
        }
    }
}

// Explicit implementation for strings, that gets around the stream operator stopping
// at spaces by not using a stream at all.
template< typename TKey, typename TVal, typename TEqualKey, typename TAllocator >
inline void Tokenize( const String& str, Map< String, String, TEqualKey, TAllocator >& tokens, const tchar_t* delimiters )
{
    // Skip delimiters at beginning.
    size_t lastPos = str.FindNone( delimiters );
    // Find first "non-delimiter".
    size_t pos     = str.FindAny( delimiters, lastPos );

    while ( IsValid( pos ) || IsValid( lastPos ) )
    {
        String key;
        str.Substring( key, lastPos, pos - lastPos );

        // Skip delimiters.  Note the "not_of"
        lastPos = str.FindNone( delimiters, pos );
        // Find next "non-delimiter"
        pos = str.FindAny( delimiters, lastPos );

        if ( IsValid( pos ) || IsValid( lastPos ) )
        {
            // At this point, we have the key and value.  Build the map entry.
            str.Substring( tokens[ key ], lastPos, pos - lastPos );

            // Skip delimiters.  Note the "not_of"
            lastPos = str.FindNone( delimiters, pos );

            // Find next "non-delimiter"
            pos = str.FindAny( delimiters, lastPos );
        }
        else
        {
            // Delimited map is messed up. It should alternate key-value pairs, but there is
            // a key without a value.
            HELIUM_BREAK();
        }
    }
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::SimpleMapData()
{

}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::~SimpleMapData()
{

}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
size_t SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::GetSize() const
{
    return m_Data->GetSize();
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
void SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::Clear()
{
    return m_Data->Clear();
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
void SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::ConnectData(void* data)
{
    m_Data.Connect( data );
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
const Class* SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::GetKeyClass() const
{
    return Reflect::GetDataClass< KeyT >();
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
const Class* SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::GetValueClass() const
{
    return Reflect::GetDataClass< ValueT >();
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
void SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::GetItems( A_ValueType& items )
{
    items.Clear();
    items.Reserve( m_Data->GetSize() );

    DataType::ConstIterator itr = m_Data->Begin();
    DataType::ConstIterator end = m_Data->End();
    for ( ; itr != end; ++itr )
    {
        HELIUM_VERIFY( items.New(
            Data::Bind( const_cast< KeyT& >( itr->First() ), m_Instance, m_Field ),
            Data::Bind( const_cast< ValueT& >( itr->Second() ), m_Instance, m_Field ) ) );
    }
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
DataPtr SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::GetItem( Data* key )
{
    KeyT keyValue;
    Data::GetValue( key, keyValue );

    DataType::ConstIterator found = m_Data->Find( keyValue );
    if ( found != m_Data->End() )
    {
        return Data::Bind( const_cast< ValueT& >( found->Second() ), m_Instance, m_Field );
    }

    return NULL;
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
void SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::SetItem( Data* key, Data* value )
{
    KeyT keyValue;
    Data::GetValue( key, keyValue );

    Data::GetValue( value, (*m_Data)[ keyValue ] );
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
void SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::RemoveItem( Data* key )
{
    KeyT keyValue;
    Data::GetValue( key, keyValue );

    m_Data->Remove( keyValue );
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
bool SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::Set( Data* src, uint32_t flags )
{
    const MapDataT* rhs = SafeCast< MapDataT >( src );
    if ( !rhs )
    {
        return false;
    }

    *m_Data = *rhs->m_Data;

    return true;
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
bool SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::Equals( Object* object )
{
    const MapDataT* rhs = SafeCast< MapDataT >( object );
    if ( !rhs )
    {
        return false;
    }

    if ( m_Data->GetSize() != rhs->m_Data->GetSize() )
    {
        return false;
    }

    const DataType& rhsData = *m_Data;

    DataType::ConstIterator itrLHS = m_Data->Begin();
    DataType::ConstIterator endLHS = m_Data->End();
    DataType::ConstIterator endRHS = rhsData.End();
    for ( ; itrLHS != endLHS; ++itrLHS )
    {
        DataType::ConstIterator itrRHS = rhsData.Find( itrLHS->First() );
        if( itrRHS == endRHS || itrLHS->Second() != itrRHS->Second() )
        {
            return false;
        }
    }

    return true;
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
void SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::Serialize( ArchiveBinary& archive )
{
    Serialize<ArchiveBinary>( archive );
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
void SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::Deserialize( ArchiveBinary& archive )
{
    Deserialize<ArchiveBinary>( archive );
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
void SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::Serialize( ArchiveXML& archive )
{
    Serialize<ArchiveXML>( archive );
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
void SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::Deserialize( ArchiveXML& archive )
{
    Deserialize<ArchiveXML>( archive );
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
tostream& SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::operator>>( tostream& stream ) const
{
    DataType::ConstIterator itr = m_Data->Begin();
    DataType::ConstIterator end = m_Data->End();
    for ( ; itr != end; ++itr )
    {
        if ( itr != m_Data->Begin() )
        {
            stream << s_ContainerItemDelimiter;
        }

        stream << itr->First() << s_ContainerItemDelimiter << itr->Second();
    }

    return stream;
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT >
tistream& SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::operator<<( tistream& stream )
{
    m_Data->Clear();

    String str;
    std::streamsize size = stream.rdbuf()->in_avail();
    str.Reserve( static_cast< size_t >( size ) );
    str.Resize( static_cast< size_t >( size ) );
    stream.read( &str[ 0 ], size );

    Tokenize< KeyT, ValueT, EqualKeyT, AllocatorT >( str, *m_Data, s_ContainerItemDelimiter );

    return stream;
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT > template< class ArchiveT >
void SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::Serialize( ArchiveT& archive )
{
    DynArray< ObjectPtr > components;
    components.Reserve( m_Data->GetSize() * 2 );

    {
        DataType::ConstIterator itr = m_Data->Begin();
        DataType::ConstIterator end = m_Data->End();
        for ( ; itr != end; ++itr )
        {
            ObjectPtr keyElem = Registry::GetInstance()->CreateInstance( Reflect::GetDataClass< KeyT >() );
            ObjectPtr dataElem = Registry::GetInstance()->CreateInstance( Reflect::GetDataClass< ValueT >() );

            Data* keySer = AssertCast< Data >( keyElem );
            Data* dataSer = AssertCast< Data >( dataElem );

            // connect to our map key memory address
            keySer->ConnectData( const_cast< KeyT* >( &itr->First() ) );

            // connect to our map data memory address
            dataSer->ConnectData( const_cast< ValueT* >( &itr->Second() ) );

            // serialize to the archive stream
            HELIUM_VERIFY( components.New( keySer ) );
            HELIUM_VERIFY( components.New( dataSer ) );
        }
    }

    archive.SerializeArray( components );

    DynArray< ObjectPtr >::Iterator itr = components.Begin();
    DynArray< ObjectPtr >::Iterator end = components.End();
    for ( ; itr != end; ++itr )
    {
        Data* ser = AssertCast< Data >( *itr );
        HELIUM_ASSERT( ser );
        ser->Disconnect();

        // might be useful to cache the data object here
    }
}

template< typename KeyT, typename ValueT, typename EqualKeyT, typename AllocatorT > template< class ArchiveT >
void SimpleMapData< KeyT, ValueT, EqualKeyT, AllocatorT >::Deserialize( ArchiveT& archive )
{
    DynArray< ObjectPtr > components;
    archive.DeserializeArray( components, ArchiveFlags::Sparse );

    size_t componentCount = components.GetSize();
    if ( componentCount % 2 != 0 )
    {
        throw Reflect::DataFormatException( TXT( "Unmatched map objects" ) );
    }

    // if we are referring to a real field, clear its contents
    m_Data->Clear();
    m_Data->Reserve( componentCount / 2 );

    DynArray< ObjectPtr >::Iterator itr = components.Begin();
    DynArray< ObjectPtr >::Iterator end = components.End();
    while( itr != end )
    {
        Data* key = SafeCast< Data >( *itr );
        ++itr;
        Data* value = SafeCast< Data >( *itr );
        ++itr;

        if ( key && value )
        {
            KeyT k;
            Data::GetValue( key, k );
            Data::GetValue( value, (*m_Data)[ k ] );
        }
    }
}

template SimpleMapData< String, String >;
template SimpleMapData< String, bool >;
template SimpleMapData< String, uint32_t >;
template SimpleMapData< String, int32_t >;

template SimpleMapData< uint32_t, String >;
template SimpleMapData< uint32_t, uint32_t >;
template SimpleMapData< uint32_t, int32_t >;
template SimpleMapData< uint32_t, uint64_t >;

template SimpleMapData< int32_t, String >;
template SimpleMapData< int32_t, uint32_t >;
template SimpleMapData< int32_t, int32_t >;
template SimpleMapData< int32_t, uint64_t >;

template SimpleMapData< uint64_t, String >;
template SimpleMapData< uint64_t, uint32_t >;
template SimpleMapData< uint64_t, uint64_t >;

template SimpleMapData< Helium::GUID, uint32_t >;
template SimpleMapData< Helium::TUID, uint32_t >;

REFLECT_DEFINE_OBJECT( Helium::Reflect::StringStringMapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::StringBoolMapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::StringUInt32MapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::StringInt32MapData );

REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt32StringMapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt32UInt32MapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt32Int32MapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt32UInt64MapData );

REFLECT_DEFINE_OBJECT( Helium::Reflect::Int32StringMapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Int32UInt32MapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Int32Int32MapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Int32UInt64MapData );

REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt64StringMapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt64UInt32MapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt64UInt64MapData );

REFLECT_DEFINE_OBJECT( Helium::Reflect::GUIDUInt32MapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::TUIDUInt32MapData );
