#include "ReflectPch.h"
#include "Reflect/Data/ObjectMapData.h"

#include "Reflect/Data/DataDeduction.h"
#include "Reflect/ArchiveBinary.h"
#include "Reflect/ArchiveXML.h"

REFLECT_DEFINE_ABSTRACT( Helium::Reflect::ObjectMapData );

using namespace Helium;
using namespace Helium::Reflect;

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::SimpleObjectMapData()
{
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::~SimpleObjectMapData()
{

}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
void SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::ConnectData( void* data )
{
    m_Data.Connect( data );
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
size_t SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::GetSize() const
{
    return m_Data->GetSize();
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
void SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::Clear()
{
    return m_Data->Clear();
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
const Class* SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::GetKeyClass() const
{
    return Reflect::GetDataClass< KeyT >();
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
void SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::GetItems( A_ValueType& items )
{
    items.Clear();
    items.Reserve( m_Data->GetSize() );

    DataType::Iterator itr = m_Data->Begin();
    DataType::Iterator end = m_Data->End();
    for ( ; itr != end; ++itr )
    {
        HELIUM_VERIFY( items.New( Data::Bind( const_cast< KeyT& >( itr->First() ), m_Instance, m_Field ), &itr->Second() ) );
    }
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
ObjectPtr* SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::GetItem( Data* key )
{
    KeyT keyValue;
    Data::GetValue( key, keyValue );

    DataType::Iterator found = m_Data->Find( keyValue );
    if ( found != m_Data->End() )
    {
        return &found->Second();
    }

    return NULL;
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
void SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::SetItem( Data* key, Object* value )
{
    KeyT keyValue;
    Data::GetValue( key, keyValue );
    (*m_Data)[ keyValue ] = value;
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
void SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::RemoveItem( Data* key )
{
    KeyT keyValue;
    Data::GetValue( key, keyValue );

    m_Data->Remove( keyValue );
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
bool SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::Set( Data* src, uint32_t flags )
{
    const ObjectMapDataT* rhs = SafeCast< ObjectMapDataT >( src );
    if ( !rhs )
    {
        return false;
    }

    m_Data->Clear();

    if ( flags & DataFlags::Shallow )
    {
        *m_Data = *rhs->m_Data;
    }
    else
    {
        DataType::ConstIterator itr = rhs->m_Data->Begin();
        DataType::ConstIterator end = rhs->m_Data->End();
        for ( ; itr != end; ++itr )
        {
            Object* object = itr->Second();
            (*m_Data)[ itr->First() ] = ( object ? object->Clone() : NULL );
        }
    }

    return true;
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
bool SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::Equals( Object* object )
{
    const ObjectMapDataT* rhs = SafeCast< ObjectMapDataT >( object );
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
        if( itrRHS == endRHS )
        {
            return false;
        }

        Object* objectLHS = itrLHS->Second();
        Object* objectRHS = itrRHS->Second();
        if( objectLHS != objectRHS && ( !objectLHS || !objectRHS || !objectLHS->Equals( objectRHS ) ) )
        {
            return false;
        }
    }

    return true;
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
void SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::Accept( Visitor& visitor )
{
    DataType::Iterator itr = const_cast< DataPointer< DataType >& >( m_Data )->Begin();
    DataType::Iterator end = const_cast< DataPointer< DataType >& >( m_Data )->End();
    for ( ; itr != end; ++itr )
    {
        ObjectPtr& object = itr->Second();
        if ( !object.Get() )
        {
            continue;
        }

        if ( !visitor.VisitPointer( object ) )
        {
            continue;
        }

        object->Accept( visitor );
    }
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
void SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::Serialize( ArchiveBinary& archive )
{
    Serialize<ArchiveBinary>( archive );
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
void SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::Deserialize( ArchiveBinary& archive )
{
    Deserialize<ArchiveBinary>( archive );
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
void SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::Serialize( ArchiveXML& archive )
{
    Serialize<ArchiveXML>( archive );
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT >
void SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::Deserialize( ArchiveXML& archive )
{
    Deserialize<ArchiveXML>( archive );
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT > template< class ArchiveT >
void SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::Serialize( ArchiveT& archive )
{
    DynArray< ObjectPtr > components;
    components.Reserve( m_Data->GetSize() * 2 );

    {
        DataType::ConstIterator itr = m_Data->Begin();
        DataType::ConstIterator end = m_Data->End();
        for ( ; itr != end; ++itr )
        {
            ObjectPtr elem = Registry::GetInstance()->CreateInstance( Reflect::GetDataClass< KeyT >() );

            Data* ser = AssertCast< Data >( elem.Ptr() );
            ser->ConnectData( const_cast< KeyT* >( &itr->First() ) );

            HELIUM_VERIFY( components.New( ser ) );
            HELIUM_VERIFY( components.New( itr->Second() ) );
        }
    }

    archive.SerializeArray( components );

    {
        DynArray< ObjectPtr >::Iterator itr = components.Begin();
        DynArray< ObjectPtr >::Iterator end = components.End();
        for ( ; itr != end; ++itr )
        {
            Data* ser = AssertCast< Data >( *itr );
            ser->Disconnect();
            ++itr;  // Skip over the object reference.

            // might be useful to cache the data object here
        }
    }
}

template< typename KeyT, typename EqualKeyT, typename AllocatorT > template< class ArchiveT >
void SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT >::Deserialize( ArchiveT& archive )
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
    while ( itr != end )
    {
        Data* key = SafeCast< Data >( *itr );
        ++itr;
        Object* value = *itr;
        ++itr;

        if ( key )  // The object value can be null, so don't check it here.
        {
            KeyT k;
            Data::GetValue( key, k );
            (*m_Data)[ k ] = value;
        }
    }
}

template SimpleObjectMapData< TypeID >;
template SimpleObjectMapData< String >;
template SimpleObjectMapData< uint32_t >;
template SimpleObjectMapData< int32_t >;
template SimpleObjectMapData< uint64_t >;
template SimpleObjectMapData< int64_t >;
template SimpleObjectMapData< Helium::TUID >;

REFLECT_DEFINE_OBJECT( Helium::Reflect::TypeIDObjectMapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::StringObjectMapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt32ObjectMapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Int32ObjectMapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt64ObjectMapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Int64ObjectMapData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::TUIDObjectMapData );
