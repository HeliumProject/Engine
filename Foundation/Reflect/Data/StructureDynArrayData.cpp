#include "FoundationPch.h"
#include "Foundation/Reflect/Data/StructureDynArrayData.h"

#include "Foundation/Reflect/Data/DataDeduction.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"
#include "Foundation/Reflect/Structure.h"

REFLECT_DEFINE_OBJECT( Helium::Reflect::StructureDynArrayData );

using namespace Helium;
using namespace Helium::Reflect;

StructureDynArrayData::StructureDynArrayData()
{
}

StructureDynArrayData::~StructureDynArrayData()
{
}

// Inlined so must be at top of file
const Structure* Helium::Reflect::StructureDynArrayData::GetInternalStructure()
{
    return ReflectionCast< Structure >( m_Field->m_Type );
}

// Inlined so must be at top of file
void * Helium::Reflect::StructureDynArrayData::GetInternalPtr( const Structure* structure )
{
    size_t dyn_array_byte_size = structure->m_DynArrayAdapter->GetDynArrayByteSize();
    return m_Data.Get(static_cast<uint32_t>(dyn_array_byte_size));
}

void StructureDynArrayData::ConnectData( void* data )
{
    if (data)
    {
        const Structure* structure = ReflectionCast< Structure >( m_Field->m_Type );
        HELIUM_ASSERT( structure );

        m_Data.Connect( data, structure->m_DynArrayAdapter->GetDynArrayByteSize() );
    }
    else
    {
        m_Data.Connect( NULL, 0 );
    }
}

size_t StructureDynArrayData::GetSize() const 
{
    const Structure* structure = ReflectionCast< Structure >( m_Field->m_Type );
    HELIUM_ASSERT( structure );

    size_t dyn_array_byte_size = structure->m_DynArrayAdapter->GetDynArrayByteSize();

    return structure->m_DynArrayAdapter->GetSize(m_Data.Get(static_cast<uint32_t>(dyn_array_byte_size)));
}

void StructureDynArrayData::SetSize( size_t size )
{
    const Structure* structure = ReflectionCast< Structure >( m_Field->m_Type );
    HELIUM_ASSERT( structure );

    size_t dyn_array_byte_size = structure->m_DynArrayAdapter->GetDynArrayByteSize();

    return structure->m_DynArrayAdapter->Resize(m_Data.Get(static_cast<uint32_t>(dyn_array_byte_size)), size);
}

void StructureDynArrayData::Clear()
{ 
    const Structure* structure = ReflectionCast< Structure >( m_Field->m_Type );
    HELIUM_ASSERT( structure );

    size_t dyn_array_byte_size = structure->m_DynArrayAdapter->GetDynArrayByteSize();

    return structure->m_DynArrayAdapter->Clear(m_Data.Get(static_cast<uint32_t>(dyn_array_byte_size))); 
}

const Class* StructureDynArrayData::GetItemClass() const
{
    //return Reflect::GetDataClass< Reflect::Object >();
    return Reflect::GetDataClass< Reflect::Structure >();
}

DataPtr StructureDynArrayData::GetItem( size_t at )
{
    const Structure* structure = ReflectionCast< Structure >( m_Field->m_Type );
    HELIUM_ASSERT( structure );

    size_t dyn_array_byte_size = structure->m_DynArrayAdapter->GetDynArrayByteSize();
    return structure->m_DynArrayAdapter->GetItem(m_Data.Get(static_cast<uint32_t>(dyn_array_byte_size)), at, m_Instance, m_Field);
}

void StructureDynArrayData::SetItem( size_t at, Data* value )
{
    const Structure* structure = ReflectionCast< Structure >( m_Field->m_Type );
    HELIUM_ASSERT( structure );

    size_t dyn_array_byte_size = structure->m_DynArrayAdapter->GetDynArrayByteSize();
    structure->m_DynArrayAdapter->SetItem(m_Data.Get(static_cast<uint32_t>(dyn_array_byte_size)), at, value, m_Instance, m_Field);
}

void StructureDynArrayData::Insert( size_t at, Data* value )
{
    const Structure *structure = GetInternalStructure();
    structure->m_DynArrayAdapter->Insert(GetInternalPtr(structure), at, value);
}

void StructureDynArrayData::Remove( size_t at )
{
    const Structure *structure = GetInternalStructure();
    structure->m_DynArrayAdapter->Remove(GetInternalPtr(structure), at);
}

void StructureDynArrayData::MoveUp( std::set< size_t >& selectedIndices )
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

        structure->m_DynArrayAdapter->Swap(GetInternalPtr(structure), *itr - 1, *itr);

        newSelectedIndices.insert( *itr - 1 );
    }

    selectedIndices = newSelectedIndices;
}

void StructureDynArrayData::MoveDown( std::set< size_t >& selectedIndices )
{
    const Structure *structure = GetInternalStructure();
    size_t size = structure->m_DynArrayAdapter->GetSize(GetInternalPtr(structure));

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

        structure->m_DynArrayAdapter->Swap(GetInternalPtr(structure), *itr + 1, *itr);

        newSelectedIndices.insert( *itr + 1 );
    }

    selectedIndices = newSelectedIndices;
}


bool StructureDynArrayData::Set( Data* src, uint32_t flags )
{
    StructureDynArrayData* rhs = SafeCast< StructureDynArrayData >(src);
    if (!rhs)
    {
        return false;
    }

    if (rhs->m_Field->m_Type != m_Field->m_Type)
    {
        return false;
    }

    const Structure *structure = GetInternalStructure();

    return structure->m_DynArrayAdapter->Set(GetInternalPtr(structure), rhs->GetInternalPtr(structure), flags);
}

bool StructureDynArrayData::Equals( Object* object )
{
    StructureDynArrayData* rhs = SafeCast< StructureDynArrayData >(object);
    if (!rhs)
    {
        return false;
    }

    if (rhs->m_Field->m_Type != m_Field->m_Type)
    {
        return false;
    }
    
    const Structure *structure = GetInternalStructure();
    return structure->m_DynArrayAdapter->Equals(GetInternalPtr(structure), rhs->GetInternalPtr(structure));
}

void StructureDynArrayData::Accept( Visitor& visitor )
{
    const Structure *structure = GetInternalStructure();
    structure->m_DynArrayAdapter->Accept(structure, GetInternalPtr(structure), visitor);
}

void StructureDynArrayData::Serialize( ArchiveBinary& archive )
{
    Serialize<ArchiveBinary>( archive );
}

void StructureDynArrayData::Deserialize( ArchiveBinary& archive )
{
    Deserialize<ArchiveBinary>( archive );
}

void StructureDynArrayData::Serialize( ArchiveXML& archive )
{
    Serialize<ArchiveXML>( archive );
}

void StructureDynArrayData::Deserialize( ArchiveXML& archive )
{
    Deserialize<ArchiveXML>( archive );
}

template< class ArchiveT >
void StructureDynArrayData::Serialize( ArchiveT& archive )
{
    const Structure *structure = GetInternalStructure();

    DynArray< ObjectPtr > components;
    components.Resize( structure->m_DynArrayAdapter->GetSize(GetInternalPtr(structure)) );

    for (size_t i = 0; i < components.GetSize(); ++i)
    {
        components[i] = structure->m_DynArrayAdapter->GetItem(GetInternalPtr(structure), i, m_Instance, m_Field);
    }

    archive.SerializeArray( components );
}

template< class ArchiveT >
void StructureDynArrayData::Deserialize( ArchiveT& archive )
{
    const Structure *structure = GetInternalStructure();

    DynArray< ObjectPtr > components;
    archive.DeserializeArray(components);

    // if we are referring to a real field, clear its contents
    structure->m_DynArrayAdapter->Resize(GetInternalPtr(structure), components.GetSize());

    for (size_t i = 0; i < components.GetSize(); ++i)
    {
        Data* data = SafeCast<Data>(components[i]);
        structure->m_DynArrayAdapter->SetItem(GetInternalPtr(structure), i, data, m_Instance, m_Field);
    }
}
