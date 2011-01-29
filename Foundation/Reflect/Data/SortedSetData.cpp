#include "Foundation/Reflect/Data/SortedSetData.h"

#include "Foundation/Reflect/Data/DataDeduction.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_ABSTRACT( SortedSetData )

// If you use this and your data can have spaces, you must make an explicit
// instantiation.  See the the string example below.
// 
// String tokenizer adapted from:
// http://www.oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
template< typename TKey, typename TCompareKey, typename TAllocator >
void Tokenize( const String& str, SortedSet< TKey, TCompareKey, TAllocator >& tokens, const tchar_t* delimiters )
{
    // Skip delimiters at beginning.
    size_t lastPos = str.FindNone( delimiters );
    // Find first "non-delimiter".
    size_t pos     = str.FindAny( delimiters, lastPos );

    TKey temp;
    while ( IsValid( pos ) || IsValid( lastPos ) )
    {
        // Found a token, convert it to the proper type for our vector
        tstringstream inStream( *str.Substring( lastPos, pos - lastPos ) );
        inStream >> temp; // NOTE: Stream operator stops at spaces!
        if ( !inStream.fail() )
        {
            // Add the token to the set
            tokens.Insert( temp );
            // Skip delimiters.  Note the "not_of"
            lastPos = str.FindNone( delimiters, pos );
            // Find next "non-delimiter"
            pos = str.FindAny( delimiters, lastPos );
        }
        else
        {
            HELIUM_BREAK();
        }
    }
}

// Explicit implementation for strings, that gets around the stream operator stopping
// at spaces by not using a stream at all.
template< typename TCompareKey, typename TAllocator >
inline void Tokenize( const String& str, SortedSet< String, TCompareKey, TAllocator >& tokens, const tchar_t* delimiters )
{
    // Skip delimiters at beginning.
    size_t lastPos = str.FindNone( delimiters );
    // Find first "non-delimiter".
    size_t pos     = str.FindAny( delimiters, lastPos );

    while ( IsValid( pos ) || IsValid( lastPos ) )
    {
        // Add the token to the vector
        tokens.Insert( str.SubString( lastPos, pos - lastPos ) );
        // Skip delimiters.  Note the "not_of"
        lastPos = str.FindNone( delimiters, pos );
        // Find next "non-delimiter"
        pos = str.FindAny( delimiters, lastPos );
    }
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::SimpleSortedSetData()
{
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::~SimpleSortedSetData()
{
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::ConnectData( void* data )
{
    m_Data.Connect( data );
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
size_t SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::GetSize() const
{
    return m_Data->GetSize();
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::Clear()
{
    return m_Data->Clear();
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
const Class* SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::GetItemClass() const
{
    return Reflect::GetDataClass< KeyT >();
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::GetItems( DynArray< DataPtr >& items ) const
{
    items.Clear();
    items.Reserve( m_Data->GetSize() );

    DataType::ConstIterator itr = m_Data->Begin();
    DataType::ConstIterator end = m_Data->End();
    for ( ; itr != end; ++itr )
    {
        HELIUM_VERIFY( items.New( Data::Bind( const_cast< KeyT& >( *itr ), m_Instance, m_Field ) ) );
    }
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::AddItem( Data* value )
{
    KeyT dataValue;
    Data::GetValue( value, dataValue );
    m_Data->Insert( dataValue );
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::RemoveItem( Data* value )
{
    KeyT dataValue;
    Data::GetValue( value, dataValue );
    m_Data->Remove( dataValue );
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
bool SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::ContainsItem( Data* value ) const
{
    KeyT dataValue;
    Data::GetValue( value, dataValue );
    return m_Data->Find( dataValue ) != m_Data->End();
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
bool SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::Set( Data* src, uint32_t flags )
{
    const SortedSetDataT* rhs = SafeCast< SortedSetDataT >( src );
    if ( !rhs )
    {
        return false;
    }

    *m_Data = *rhs->m_Data;

    return true;
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
bool SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::Equals( Object* object )
{
    const SortedSetDataT* rhs = SafeCast< SortedSetDataT >( object );
    if ( !rhs )
    {
        return false;
    }

    return *m_Data == *rhs->m_Data;
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::Serialize( ArchiveBinary& archive )
{
    Serialize( static_cast< Archive& >( archive ) );
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::Deserialize( ArchiveBinary& archive )
{
    Deserialize( static_cast< Archive& >( archive ) );
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::Serialize( ArchiveXML& archive )
{
    Serialize( static_cast< Archive& >( archive ) );
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::Deserialize( ArchiveXML& archive )
{
    Deserialize( static_cast< Archive& >( archive ) );
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::Serialize( Archive& archive )
{
    DynArray< ObjectPtr > components;
    components.Reserve( m_Data->GetSize() );

    {
        DataType::ConstIterator itr = m_Data->Begin();
        DataType::ConstIterator end = m_Data->End();
        for ( ; itr != end; ++itr )
        {
            ObjectPtr dataElem = Registry::GetInstance()->CreateInstance( Reflect::GetDataClass< KeyT >() );

            // downcast to data type
            Data* dataSer = AssertCast< Data >( dataElem );

            // connect to our map data memory address
            dataSer->ConnectData( const_cast< KeyT* >( &( *itr ) ) );

            // serialize to the archive stream
            HELIUM_VERIFY( components.New( dataSer ) );
        }
    }

    archive.Serialize( components );

    DynArray< ObjectPtr >::Iterator itr = components.Begin();
    DynArray< ObjectPtr >::Iterator end = components.End();
    for ( ; itr != end; ++itr )
    {
        Data* ser = AssertCast< Data >( *itr );
        ser->Disconnect();

        // might be useful to cache the data object here
    }
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::Deserialize( Archive& archive )
{
    DynArray< ObjectPtr > components;
    archive.Deserialize( components );

    // if we are referring to a real field, clear its contents
    m_Data->Clear();
    m_Data->Reserve( components.GetSize() );

    DynArray< ObjectPtr >::Iterator itr = components.Begin();
    DynArray< ObjectPtr >::Iterator end = components.End();
    for ( ; itr != end; ++itr )
    {
        Data* data = SafeCast< Data >( *itr );
        if ( !data )
        {
            throw LogisticException( TXT( "SortedSet value type has changed, this is unpossible" ) );
        }

        KeyT k;
        Data::GetValue( data, k );
        m_Data->Insert( k );
    }
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
tostream& SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::operator>>( tostream& stream ) const
{
    DataType::ConstIterator itr = m_Data->Begin();
    DataType::ConstIterator end = m_Data->End();
    for ( ; itr != end; ++itr )
    {
        if ( itr != m_Data->Begin() )
        {
            stream << s_ContainerItemDelimiter;
        }

        stream << *itr;
    }

    return stream;
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
tistream& SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT >::operator<<( tistream& stream )
{
    m_Data->Clear();

    String str;
    std::streamsize size = stream.rdbuf()->in_avail();
    str.Reserve( static_cast< size_t >( size ) );
    str.Resize( static_cast< size_t >( size ) );
    stream.read( &str[ 0 ], size );

    Tokenize< KeyT, CompareKeyT, AllocatorT >( str, *m_Data, s_ContainerItemDelimiter );

    return stream;
}  

template SimpleSortedSetData< String >;
template SimpleSortedSetData< uint32_t >;
template SimpleSortedSetData< uint64_t >;
template SimpleSortedSetData< Helium::GUID >;
template SimpleSortedSetData< Helium::TUID >;
template SimpleSortedSetData< Helium::Path >;

REFLECT_DEFINE_OBJECT( StringSortedSetData );
REFLECT_DEFINE_OBJECT( UInt32SortedSetData );
REFLECT_DEFINE_OBJECT( UInt64SortedSetData );
REFLECT_DEFINE_OBJECT( GUIDSortedSetData );
REFLECT_DEFINE_OBJECT( TUIDSortedSetData );
REFLECT_DEFINE_OBJECT( PathSortedSetData );
