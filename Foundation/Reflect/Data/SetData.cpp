#include "Foundation/Reflect/Data/SetData.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_ABSTRACT( SetData )

// If you use this and your data can have spaces, you must make an explicit
// instantiation.  See the the string example below.
// 
// String tokenizer adapted from:
// http://www.oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
template< typename TKey, typename TEqualKey, typename TAllocator >
void Tokenize( const String& str, Set< TKey, TEqualKey, TAllocator >& tokens, const tchar_t* delimiters )
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
template< typename TEqualKey, typename TAllocator >
inline void Tokenize( const String& str, Set< String, TEqualKey, TAllocator >& tokens, const tchar_t* delimiters )
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

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
SimpleSetData< KeyT, EqualKeyT, AllocatorT >::SimpleSetData()
{
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
SimpleSetData< KeyT, EqualKeyT, AllocatorT >::~SimpleSetData()
{
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
void SimpleSetData< KeyT, EqualKeyT, AllocatorT >::ConnectData( Helium::HybridPtr< void > data )
{
    m_Data.Connect( Helium::HybridPtr< DataType >( data.Address(), data.State() ) );
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
size_t SimpleSetData< KeyT, EqualKeyT, AllocatorT >::GetSize() const
{
    return m_Data->GetSize();
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
void SimpleSetData< KeyT, EqualKeyT, AllocatorT >::Clear()
{
    return m_Data->Clear();
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
const Class* SimpleSetData< KeyT, EqualKeyT, AllocatorT >::GetItemClass() const
{
    return Reflect::GetDataClass< KeyT >();
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
void SimpleSetData< KeyT, EqualKeyT, AllocatorT >::GetItems( DynArray< ConstDataPtr >& items ) const
{
    items.Clear();
    items.Reserve( m_Data->GetSize() );

    DataType::ConstIterator itr = m_Data->Begin();
    DataType::ConstIterator end = m_Data->End();
    for ( ; itr != end; ++itr )
    {
        HELIUM_VERIFY( items.New( static_cast< const ConstDataPtr& >( Data::Bind( *itr, m_Instance, m_Field ) ) ) );
    }
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
void SimpleSetData< KeyT, EqualKeyT, AllocatorT >::AddItem( const Data* value )
{
    KeyT dataValue;
    Data::GetValue( value, dataValue );
    m_Data->Insert( dataValue );
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
void SimpleSetData< KeyT, EqualKeyT, AllocatorT >::RemoveItem( const Data* value )
{
    KeyT dataValue;
    Data::GetValue( value, dataValue );
    m_Data->Remove( dataValue );
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
bool SimpleSetData< KeyT, EqualKeyT, AllocatorT >::ContainsItem( const Data* value ) const
{
    KeyT dataValue;
    Data::GetValue( value, dataValue );
    return m_Data->Find( dataValue ) != m_Data->End();
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
bool SimpleSetData< KeyT, EqualKeyT, AllocatorT >::Set( const Data* src, uint32_t flags )
{
    const SetDataT* rhs = SafeCast< SetDataT >( src );
    if ( !rhs )
    {
        return false;
    }

    *m_Data = *rhs->m_Data;

    return true;
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
bool SimpleSetData< KeyT, EqualKeyT, AllocatorT >::Equals( const Object* object ) const
{
    const SetDataT* rhs = SafeCast< SetDataT >( object );
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
        DataType::ConstIterator itrRHS = rhsData.Find( *itrLHS );
        if( itrRHS == endRHS )
        {
            return false;
        }
    }

    return true;
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
void SimpleSetData< KeyT, EqualKeyT, AllocatorT >::Serialize( Archive& archive ) const
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

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
void SimpleSetData< KeyT, EqualKeyT, AllocatorT >::Deserialize( Archive& archive )
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
            throw LogisticException( TXT( "Set value type has changed, this is unpossible" ) );
        }

        KeyT k;
        Data::GetValue( data, k );
        m_Data->Insert( k );
    }
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
tostream& SimpleSetData< KeyT, EqualKeyT, AllocatorT >::operator>>( tostream& stream ) const
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

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
tistream& SimpleSetData< KeyT, EqualKeyT, AllocatorT >::operator<<( tistream& stream )
{
    m_Data->Clear();

    String str;
    std::streamsize size = stream.rdbuf()->in_avail();
    str.Reserve( static_cast< size_t >( size ) );
    str.Resize( static_cast< size_t >( size ) );
    stream.read( &str[ 0 ], size );

    Tokenize< KeyT, EqualKeyT, AllocatorT >( str, *m_Data, s_ContainerItemDelimiter );

    return stream;
}  

template SimpleSetData< String >;
template SimpleSetData< uint32_t >;
template SimpleSetData< uint64_t >;
template SimpleSetData< Helium::GUID >;
template SimpleSetData< Helium::TUID >;
template SimpleSetData< Helium::Path >;

REFLECT_DEFINE_OBJECT( StringSetData );
REFLECT_DEFINE_OBJECT( UInt32SetData );
REFLECT_DEFINE_OBJECT( UInt64SetData );
REFLECT_DEFINE_OBJECT( GUIDSetData );
REFLECT_DEFINE_OBJECT( TUIDSetData );
REFLECT_DEFINE_OBJECT( PathSetData );
