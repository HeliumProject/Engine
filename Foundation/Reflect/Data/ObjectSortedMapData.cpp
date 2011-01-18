#include "Foundation/Reflect/Data/ObjectSortedMapData.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_ABSTRACT( ObjectSortedMapData );

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT >::SimpleObjectSortedMapData()
{
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT >::~SimpleObjectSortedMapData()
{

}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT >::ConnectData( Helium::HybridPtr< void > data )
{
    m_Data.Connect( Helium::HybridPtr< DataType >( data.Address(), data.State() ) );
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
size_t SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT >::GetSize() const
{
    return m_Data->GetSize();
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT >::Clear()
{
    return m_Data->Clear();
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
const Class* SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT >::GetKeyClass() const
{
    return Reflect::GetDataClass< KeyT >();
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT >::GetItems( A_ValueType& items )
{
    items.Clear();
    items.Reserve( m_Data->GetSize() );

    DataType::Iterator itr = m_Data->Begin();
    DataType::Iterator end = m_Data->End();
    for ( ; itr != end; ++itr )
    {
        HELIUM_VERIFY( items.New(
            static_cast< const ConstDataPtr& >( Data::Bind( itr->First(), m_Instance, m_Field ) ),
            &itr->Second() ) );
    }
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT >::GetItems( A_ConstValueType& items ) const
{
    items.Clear();
    items.Reserve( m_Data->GetSize() );

    DataType::ConstIterator itr = m_Data->Begin();
    DataType::ConstIterator end = m_Data->End();
    for ( ; itr != end; ++itr )
    {
        HELIUM_VERIFY( items.New(
            static_cast< const ConstDataPtr& >( Data::Bind( itr->First(), m_Instance, m_Field ) ),
            &itr->Second() ) );
    }
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
ObjectPtr* SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT >::GetItem( const Data* key )
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

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
const ObjectPtr* SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT >::GetItem( const Data* key ) const
{
    KeyT keyValue;
    Data::GetValue( key, keyValue );

    DataType::ConstIterator found = m_Data->Find( keyValue );
    if ( found != m_Data->End() )
    {
        return &found->Second();
    }

    return NULL;
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT >::SetItem( const Data* key, const Object* value )
{
    KeyT keyValue;
    Data::GetValue( key, keyValue );

#pragma TODO( "Fix const correctness." )
    m_Data.Ref()[ keyValue ] = const_cast< Object* >( value );
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT >::RemoveItem( const Data* key )
{
    KeyT keyValue;
    Data::GetValue( key, keyValue );

    m_Data->Remove( keyValue );
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
bool SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT >::Set( const Data* src, uint32_t flags )
{
    const ObjectSortedMapDataT* rhs = SafeCast< ObjectSortedMapDataT >( src );
    if ( !rhs )
    {
        return false;
    }

    m_Data->Clear();

    if ( flags & DataFlags::Shallow )
    {
        m_Data.Ref() = rhs->m_Data.Ref();
    }
    else
    {
        DataType::ConstIterator itr = rhs->m_Data->Begin();
        DataType::ConstIterator end = rhs->m_Data->End();
        for ( ; itr != end; ++itr )
        {
            Object* object = itr->Second();
            m_Data.Ref()[ itr->First() ] = ( object ? object->Clone() : NULL );
        }
    }

    return true;
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
bool SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT >::Equals( const Object* object ) const
{
    const ObjectSortedMapDataT* rhs = SafeCast< ObjectSortedMapDataT >( object );
    if ( !rhs )
    {
        return false;
    }

    if ( m_Data->GetSize() != rhs->m_Data->GetSize() )
    {
        return false;
    }

    const DataType& rhsData = m_Data.Ref();

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

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT >::Serialize( Archive& archive ) const
{
    DynArray< ObjectPtr > components;
    components.Reserve( m_Data->GetSize() * 2 );

    {
        DataType::ConstIterator itr = m_Data->Begin();
        DataType::ConstIterator end = m_Data->End();
        for ( ; itr != end; ++itr )
        {
            Object* object = itr->Second();
            if ( !object )
            {
                continue;
            }

            ObjectPtr elem = Registry::GetInstance()->CreateInstance( Reflect::GetDataClass< KeyT >() );

            Data* ser = AssertCast< Data >( elem.Ptr() );
            ser->ConnectData( &itr->First() );

            HELIUM_VERIFY( components.New( ser ) );
            HELIUM_VERIFY( components.New( object ) );
        }
    }

    archive.Serialize( components );

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

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT >::Deserialize( Archive& archive )
{
    DynArray< ObjectPtr > components;
    archive.Deserialize( components, ArchiveFlags::Sparse );

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

        if ( key && value )
        {
            KeyT k;
            Data::GetValue( key, k );
            m_Data.Ref()[ k ] = value;
        }
    }
}

template< typename KeyT, typename CompareKeyT, typename AllocatorT >
void SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT >::Accept( Visitor& visitor )
{
    DataType::Iterator itr = const_cast< Data::Pointer< DataType >& >( m_Data )->Begin();
    DataType::Iterator end = const_cast< Data::Pointer< DataType >& >( m_Data )->End();
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

template SimpleObjectSortedMapData< TypeID >;
template SimpleObjectSortedMapData< String >;
template SimpleObjectSortedMapData< uint32_t >;
template SimpleObjectSortedMapData< int32_t >;
template SimpleObjectSortedMapData< uint64_t >;
template SimpleObjectSortedMapData< int64_t >;
template SimpleObjectSortedMapData< Helium::GUID >;
template SimpleObjectSortedMapData< Helium::TUID >;

REFLECT_DEFINE_OBJECT( TypeIDObjectSortedMapData );
REFLECT_DEFINE_OBJECT( StringObjectSortedMapData );
REFLECT_DEFINE_OBJECT( UInt32ObjectSortedMapData );
REFLECT_DEFINE_OBJECT( Int32ObjectSortedMapData );
REFLECT_DEFINE_OBJECT( UInt64ObjectSortedMapData );
REFLECT_DEFINE_OBJECT( Int64ObjectSortedMapData );
REFLECT_DEFINE_OBJECT( GUIDObjectSortedMapData );
REFLECT_DEFINE_OBJECT( TUIDObjectSortedMapData );
