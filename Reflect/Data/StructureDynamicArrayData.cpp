#include "ReflectPch.h"
#include "Reflect/Data/StructureDynamicArrayData.h"

#include "Reflect/Data/DataDeduction.h"
#include "Reflect/ArchiveBinary.h"
#include "Reflect/ArchiveXML.h"
#include "Reflect/Structure.h"

REFLECT_DEFINE_OBJECT( Helium::Reflect::StructureDynamicArrayData );

using namespace Helium;
using namespace Helium::Reflect;

StructureDynamicArrayData::StructureDynamicArrayData()
{
}

StructureDynamicArrayData::~StructureDynamicArrayData()
{
}

// Inlined so must be at top of file
const Structure* Helium::Reflect::StructureDynamicArrayData::GetInternalStructure()
{
    return ReflectionCast< Structure >( m_Field->m_Type );
}

// Inlined so must be at top of file
void * Helium::Reflect::StructureDynamicArrayData::GetInternalPtr( const Structure* structure )
{
    size_t dyn_array_byte_size = structure->m_DynamicArrayAdapter->GetDynamicArrayByteSize();
    return m_Data.Get(static_cast<uint32_t>(dyn_array_byte_size));
}

void StructureDynamicArrayData::ConnectData( void* data )
{
    if (data)
    {
        const Structure* structure = ReflectionCast< Structure >( m_Field->m_Type );
        HELIUM_ASSERT( structure );

        m_Data.Connect( data, structure->m_DynamicArrayAdapter->GetDynamicArrayByteSize() );
    }
    else
    {
        m_Data.Connect( NULL, 0 );
    }
}

size_t StructureDynamicArrayData::GetSize() const 
{
    const Structure* structure = ReflectionCast< Structure >( m_Field->m_Type );
    HELIUM_ASSERT( structure );

    size_t dyn_array_byte_size = structure->m_DynamicArrayAdapter->GetDynamicArrayByteSize();

    return structure->m_DynamicArrayAdapter->GetSize(m_Data.Get(static_cast<uint32_t>(dyn_array_byte_size)));
}

void StructureDynamicArrayData::SetSize( size_t size )
{
    const Structure* structure = ReflectionCast< Structure >( m_Field->m_Type );
    HELIUM_ASSERT( structure );

    size_t dyn_array_byte_size = structure->m_DynamicArrayAdapter->GetDynamicArrayByteSize();

    return structure->m_DynamicArrayAdapter->Resize(m_Data.Get(static_cast<uint32_t>(dyn_array_byte_size)), size);
}

void StructureDynamicArrayData::Clear()
{ 
    const Structure* structure = ReflectionCast< Structure >( m_Field->m_Type );
    HELIUM_ASSERT( structure );

    size_t dyn_array_byte_size = structure->m_DynamicArrayAdapter->GetDynamicArrayByteSize();

    return structure->m_DynamicArrayAdapter->Clear(m_Data.Get(static_cast<uint32_t>(dyn_array_byte_size))); 
}

const Class* StructureDynamicArrayData::GetItemClass() const
{
    //return Reflect::GetDataClass< Reflect::Object >();
    return Reflect::GetDataClass< Reflect::Structure >();
}

DataPtr StructureDynamicArrayData::GetItem( size_t at )
{
    const Structure* structure = ReflectionCast< Structure >( m_Field->m_Type );
    HELIUM_ASSERT( structure );

    size_t dyn_array_byte_size = structure->m_DynamicArrayAdapter->GetDynamicArrayByteSize();
    return structure->m_DynamicArrayAdapter->GetItem(m_Data.Get(static_cast<uint32_t>(dyn_array_byte_size)), at, m_Instance, m_Field);
}

void StructureDynamicArrayData::SetItem( size_t at, Data* value )
{
    const Structure* structure = ReflectionCast< Structure >( m_Field->m_Type );
    HELIUM_ASSERT( structure );

    size_t dyn_array_byte_size = structure->m_DynamicArrayAdapter->GetDynamicArrayByteSize();
    structure->m_DynamicArrayAdapter->SetItem(m_Data.Get(static_cast<uint32_t>(dyn_array_byte_size)), at, value, m_Instance, m_Field);
}

void StructureDynamicArrayData::Insert( size_t at, Data* value )
{
    const Structure *structure = GetInternalStructure();
    structure->m_DynamicArrayAdapter->Insert(GetInternalPtr(structure), at, value);
}

void StructureDynamicArrayData::Remove( size_t at )
{
    const Structure *structure = GetInternalStructure();
    structure->m_DynamicArrayAdapter->Remove(GetInternalPtr(structure), at);
}

void StructureDynamicArrayData::MoveUp( std::set< size_t >& selectedIndices )
{
    const Structure *structure = GetInternalStructure();

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

        structure->m_DynamicArrayAdapter->Swap(GetInternalPtr(structure), *itr - 1, *itr);

        newSelectedIndices.insert( *itr - 1 );
    }

    selectedIndices = newSelectedIndices;
}

void StructureDynamicArrayData::MoveDown( std::set< size_t >& selectedIndices )
{
    const Structure *structure = GetInternalStructure();
    size_t size = structure->m_DynamicArrayAdapter->GetSize(GetInternalPtr(structure));

    std::set< size_t > newSelectedIndices;

    std::set< size_t >::const_reverse_iterator itr = selectedIndices.rbegin();
    std::set< size_t >::const_reverse_iterator end = selectedIndices.rend();

    for( ; itr != end; ++itr )
    {
        if ( ( (*itr) == size - 1 ) || ( newSelectedIndices.find( (*itr) + 1 ) != newSelectedIndices.end() ) )
        {
            newSelectedIndices.insert( *itr );
            continue;
        }

        structure->m_DynamicArrayAdapter->Swap(GetInternalPtr(structure), *itr + 1, *itr);

        newSelectedIndices.insert( *itr + 1 );
    }

    selectedIndices = newSelectedIndices;
}


bool StructureDynamicArrayData::Set( Data* src, uint32_t flags )
{
    StructureDynamicArrayData* rhs = SafeCast< StructureDynamicArrayData >(src);
    if (!rhs)
    {
        return false;
    }

    if (rhs->m_Field->m_Type != m_Field->m_Type)
    {
        return false;
    }

    const Structure *structure = GetInternalStructure();

    return structure->m_DynamicArrayAdapter->Set(GetInternalPtr(structure), rhs->GetInternalPtr(structure), flags);
}

bool StructureDynamicArrayData::Equals( Object* object )
{
    StructureDynamicArrayData* rhs = SafeCast< StructureDynamicArrayData >(object);
    if (!rhs)
    {
        return false;
    }

    if (rhs->m_Field->m_Type != m_Field->m_Type)
    {
        return false;
    }
    
    const Structure *structure = GetInternalStructure();
    return structure->m_DynamicArrayAdapter->Equals(GetInternalPtr(structure), rhs->GetInternalPtr(structure));
}

void StructureDynamicArrayData::Accept( Visitor& visitor )
{
    const Structure *structure = GetInternalStructure();
    structure->m_DynamicArrayAdapter->Accept(structure, GetInternalPtr(structure), visitor);
}

void StructureDynamicArrayData::Serialize( ArchiveBinary& archive )
{
    Serialize<ArchiveBinary>( archive );
}

void StructureDynamicArrayData::Deserialize( ArchiveBinary& archive )
{
    Deserialize<ArchiveBinary>( archive );
}

void StructureDynamicArrayData::Serialize( ArchiveXML& archive )
{
    Serialize<ArchiveXML>( archive );
}

void StructureDynamicArrayData::Deserialize( ArchiveXML& archive )
{
    Deserialize<ArchiveXML>( archive );
}

template< class ArchiveT >
void StructureDynamicArrayData::Serialize( ArchiveT& archive )
{
    const Structure *structure = GetInternalStructure();

    DynamicArray< ObjectPtr > components;
    components.Resize( structure->m_DynamicArrayAdapter->GetSize(GetInternalPtr(structure)) );

    for (size_t i = 0; i < components.GetSize(); ++i)
    {
        components[i] = structure->m_DynamicArrayAdapter->GetItem(GetInternalPtr(structure), i, m_Instance, m_Field);
    }

    archive.SerializeArray( components );
}

template< class ArchiveT >
void StructureDynamicArrayData::Deserialize( ArchiveT& archive )
{
    const Structure *structure = GetInternalStructure();

    DynamicArray< ObjectPtr > components;
    archive.DeserializeArray(components);

    // if we are referring to a real field, clear its contents
    structure->m_DynamicArrayAdapter->Resize(GetInternalPtr(structure), components.GetSize());

    for (size_t i = 0; i < components.GetSize(); ++i)
    {
        Data* data = SafeCast<Data>(components[i]);
        structure->m_DynamicArrayAdapter->SetItem(GetInternalPtr(structure), i, data, m_Instance, m_Field);
    }
}
