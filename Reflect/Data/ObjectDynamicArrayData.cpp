#include "ReflectPch.h"
#include "Reflect/Data/ObjectDynamicArrayData.h"

#include "Reflect/Data/DataDeduction.h"
#include "Reflect/ArchiveBinary.h"
#include "Reflect/ArchiveXML.h"

REFLECT_DEFINE_OBJECT( Helium::Reflect::ObjectDynamicArrayData );

using namespace Helium;
using namespace Helium::Reflect;

ObjectDynamicArrayData::ObjectDynamicArrayData()
{
}

ObjectDynamicArrayData::~ObjectDynamicArrayData()
{
}

void ObjectDynamicArrayData::ConnectData( void* data )
{
    m_Data.Connect( data );
}

size_t ObjectDynamicArrayData::GetSize() const 
{ 
    return m_Data->GetSize(); 
}

void ObjectDynamicArrayData::SetSize( size_t size )
{
    return m_Data->Resize( size );
}

void ObjectDynamicArrayData::Clear()
{ 
    return m_Data->Clear(); 
}

const Class* ObjectDynamicArrayData::GetItemClass() const
{
    return Reflect::GetDataClass< Reflect::Object >();
}

DataPtr ObjectDynamicArrayData::GetItem( size_t at )
{
    return Data::Bind( m_Data->GetElement( at ), m_Instance, m_Field );
}

void ObjectDynamicArrayData::SetItem( size_t at, Data* value )
{
    Data::GetValue( value, m_Data->GetElement( at ) );
}

void ObjectDynamicArrayData::Insert( size_t at, Data* value )
{
    ObjectPtr temp;
    Data::GetValue( value, temp );
    m_Data->Insert( at, temp );
}

void ObjectDynamicArrayData::Remove( size_t at )
{
    m_Data->Remove( at );
}

void ObjectDynamicArrayData::MoveUp( std::set< size_t >& selectedIndices )
{
    std::set< size_t > newSelectedIndices;

    std::set< size_t >::const_iterator itr = selectedIndices.begin();
    std::set< size_t >::const_iterator end = selectedIndices.end();

    for( ; itr != end; ++itr )
    {
        if ( (*itr) == 0 || ( newSelectedIndices.find( (*itr) - 1 ) != newSelectedIndices.end() ) )
        {
            newSelectedIndices.insert( *itr );
            continue;
        }

        Swap( m_Data->GetElement( *itr - 1 ), m_Data->GetElement( *itr ) );

        newSelectedIndices.insert( *itr - 1 );
    }

    selectedIndices = newSelectedIndices;
}

void ObjectDynamicArrayData::MoveDown( std::set< size_t >& selectedIndices )
{
    std::set< size_t > newSelectedIndices;

    std::set< size_t >::const_reverse_iterator itr = selectedIndices.rbegin();
    std::set< size_t >::const_reverse_iterator end = selectedIndices.rend();

    for( ; itr != end; ++itr )
    {
        if ( ( (*itr) == m_Data->GetSize() - 1 ) || ( newSelectedIndices.find( (*itr) + 1 ) != newSelectedIndices.end() ) )
        {
            newSelectedIndices.insert( *itr );
            continue;
        }

        Swap( m_Data->GetElement( *itr + 1 ), m_Data->GetElement( *itr ) );

        newSelectedIndices.insert( *itr + 1 );
    }

    selectedIndices = newSelectedIndices;
}


bool ObjectDynamicArrayData::Set( Data* src, uint32_t flags )
{
    const ObjectDynamicArrayData* rhs = SafeCast< ObjectDynamicArrayData >(src);
    if (!rhs)
    {
        return false;
    }

    size_t size = rhs->m_Data->GetSize();
    m_Data->Resize( size );

    if( flags & DataFlags::Shallow )
    {
        *m_Data = *rhs->m_Data;
    }
    else
    {
        DynamicArray< ObjectPtr >::ConstIterator itr = rhs->m_Data->Begin();
        DynamicArray< ObjectPtr >::ConstIterator end = rhs->m_Data->End();
        for( size_t index = 0; itr != end; ++itr, ++index )
        {
            Object* object = *itr;
            m_Data->GetElement( index ) = ( object ? object->Clone() : NULL );
        }
    }

    return true;
}

bool ObjectDynamicArrayData::Equals( Object* object )
{
    const ObjectDynamicArrayData* rhs = SafeCast< ObjectDynamicArrayData >( object );
    if (!rhs)
    {
        return false;
    }

    if( m_Data->GetSize() != rhs->m_Data->GetSize() )
    {
        return false;
    }

    DynamicArray< ObjectPtr >::ConstIterator itrLHS = m_Data->Begin();
    DynamicArray< ObjectPtr >::ConstIterator endLHS = m_Data->End();
    DynamicArray< ObjectPtr >::ConstIterator itrRHS = rhs->m_Data->Begin();
    DynamicArray< ObjectPtr >::ConstIterator endRHS = rhs->m_Data->End();
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

void ObjectDynamicArrayData::Accept( Visitor& visitor )
{
    DynamicArray< ObjectPtr >::Iterator itr = m_Data->Begin();
    DynamicArray< ObjectPtr >::Iterator end = m_Data->End();
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

void ObjectDynamicArrayData::Serialize( ArchiveBinary& archive )
{
    Serialize<ArchiveBinary>( archive );
}

void ObjectDynamicArrayData::Deserialize( ArchiveBinary& archive )
{
    Deserialize<ArchiveBinary>( archive );
}

void ObjectDynamicArrayData::Serialize( ArchiveXML& archive )
{
    Serialize<ArchiveXML>( archive );
}

void ObjectDynamicArrayData::Deserialize( ArchiveXML& archive )
{
    Deserialize<ArchiveXML>( archive );
}

template< class ArchiveT >
void ObjectDynamicArrayData::Serialize( ArchiveT& archive )
{
    archive.SerializeArray( *m_Data );
}

template< class ArchiveT >
void ObjectDynamicArrayData::Deserialize( ArchiveT& archive )
{
    // if we are referring to a real field, clear its contents
    m_Data->Clear();

    archive.DeserializeArray( *m_Data );
}
