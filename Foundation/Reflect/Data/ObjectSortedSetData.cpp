#include "Foundation/Reflect/Data/ObjectSortedSetData.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium::Reflect;

REFLECT_DEFINE_OBJECT( ObjectSortedSetData );

ObjectSortedSetData::ObjectSortedSetData()
{
}

ObjectSortedSetData::~ObjectSortedSetData()
{
}

void ObjectSortedSetData::ConnectData( Helium::HybridPtr< void > data )
{
    m_Data.Connect( Helium::HybridPtr< DataType >( data.Address(), data.State() ) );
}

size_t ObjectSortedSetData::GetSize() const
{
    return m_Data->GetSize();
}

void ObjectSortedSetData::Clear()
{
    return m_Data->Clear();
}

bool ObjectSortedSetData::Set( const Data* src, uint32_t flags )
{
    const ObjectSortedSetData* rhs = SafeCast< ObjectSortedSetData >( src );
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
            Object* object = *itr;
            m_Data->Insert( object ? object->Clone() : NULL );
        }
    }

    return true;
}

bool ObjectSortedSetData::Equals( const Object* object ) const
{
    const ObjectSortedSetData* rhs = SafeCast< ObjectSortedSetData >(object);
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
        Object* objectLHS = *itrLHS;

        DataType::ConstIterator itrRHS = rhsData.Begin();
        for( ; itrRHS != endRHS; ++itrRHS )
        {
            Object* objectRHS = *itrRHS;
            if( objectLHS == objectRHS || ( objectLHS && objectRHS && objectLHS->Equals( objectRHS ) ) )
            {
                break;
            }
        }

        if( itrRHS == endRHS )
        {
            return false;
        }
    }

    return true;
}

void ObjectSortedSetData::Serialize( Archive& archive ) const
{
    DynArray< ObjectPtr > components;
    components.Reserve( m_Data->GetSize() );

    DataType::ConstIterator itr = m_Data->Begin();
    DataType::ConstIterator end = m_Data->End();
    for ( ; itr != end; ++itr )
    {
        components.Push( *itr );
    }

    archive.Serialize( components );
}

void ObjectSortedSetData::Deserialize( Archive& archive )
{
    DynArray< ObjectPtr > components;
    archive.Deserialize( components );

    // if we are referring to a real field, clear its contents
    m_Data->Clear();
    m_Data->Reserve( components.GetSize() );

    DynArray< ObjectPtr >::ConstIterator itr = components.Begin();
    DynArray< ObjectPtr >::ConstIterator end = components.End();
    for ( ; itr != end; ++itr )
    {
        m_Data->Insert( *itr );
    }
}

void ObjectSortedSetData::Accept( Visitor& visitor )
{
    DataType::Iterator itr = const_cast< Data::Pointer< DataType >& >( m_Data )->Begin();
    DataType::Iterator end = const_cast< Data::Pointer< DataType >& >( m_Data )->End();
    for ( ; itr != end; ++itr )
    {
        // SortedSet keys are immutable, so we need to const_cast here to get VisitPointer()'s non-const reference later.
        ObjectPtr& object = const_cast< ObjectPtr& >( *itr );
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
