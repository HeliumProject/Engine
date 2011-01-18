#include "Foundation/Reflect/Data/ObjectDynArrayData.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium::Reflect;

REFLECT_DEFINE_OBJECT( ObjectDynArrayData );

ObjectDynArrayData::ObjectDynArrayData()
{
}

ObjectDynArrayData::~ObjectDynArrayData()
{
}

void ObjectDynArrayData::ConnectData( Helium::HybridPtr< void > data )
{
    m_Data.Connect( Helium::HybridPtr< DataType >( data.Address(), data.State() ) );
}

size_t ObjectDynArrayData::GetSize() const 
{ 
    return m_Data->GetSize(); 
}

void ObjectDynArrayData::Clear()
{ 
    return m_Data->Clear(); 
}

bool ObjectDynArrayData::Set( const Data* src, uint32_t flags )
{
    const ObjectDynArrayData* rhs = SafeCast< ObjectDynArrayData >(src);
    if (!rhs)
    {
        return false;
    }

    size_t size = rhs->m_Data->GetSize();
    m_Data->Resize( size );

    if( flags & DataFlags::Shallow )
    {
        m_Data.Ref() = rhs->m_Data.Ref();
    }
    else
    {
        DynArray< ObjectPtr >::ConstIterator itr = rhs->m_Data->Begin();
        DynArray< ObjectPtr >::ConstIterator end = rhs->m_Data->End();
        for( size_t index = 0; itr != end; ++itr, ++index )
        {
            Object* object = *itr;
            m_Data->GetElement( index ) = ( object ? object->Clone() : NULL );
        }
    }

    return true;
}

bool ObjectDynArrayData::Equals( const Object* object ) const
{
    const ObjectDynArrayData* rhs = SafeCast< ObjectDynArrayData >( object );
    if (!rhs)
    {
        return false;
    }

    if( m_Data->GetSize() != rhs->m_Data->GetSize() )
    {
        return false;
    }

    DynArray< ObjectPtr >::ConstIterator itrLHS = m_Data->Begin();
    DynArray< ObjectPtr >::ConstIterator endLHS = m_Data->End();
    DynArray< ObjectPtr >::ConstIterator itrRHS = rhs->m_Data->Begin();
    DynArray< ObjectPtr >::ConstIterator endRHS = rhs->m_Data->End();
    for( ; itrLHS != endLHS; ++itrLHS, ++itrRHS )
    {
        HELIUM_ASSERT( itrRHS != endRHS );

        Object* objectLHS = *itrLHS;
        Object* objectRHS = *itrRHS;

        if( objectLHS != objectRHS && ( !objectLHS || !objectRHS || !objectLHS->Equals( objectRHS ) ) )
        {
            return false;
        }
    }

    return true;
}

void ObjectDynArrayData::Serialize( Archive& archive ) const
{
    archive.Serialize( m_Data.Get() );
}

void ObjectDynArrayData::Deserialize( Archive& archive )
{
    // if we are referring to a real field, clear its contents
    m_Data->Clear();

    archive.Deserialize( m_Data.Ref() );
}

void ObjectDynArrayData::Accept( Visitor& visitor )
{
    DynArray< ObjectPtr >::Iterator itr = m_Data->Begin();
    DynArray< ObjectPtr >::Iterator end = m_Data->End();
    for ( ; itr != end; ++itr )
    {
        ObjectPtr& object = *itr;
        if( !object.ReferencesObject() )
        {
            continue;
        }

        if( !visitor.VisitPointer( object ) )
        {
            continue;
        }

        object->Accept( visitor );
    }
}
