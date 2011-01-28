#include "Foundation/Reflect/Data/StructureData.h"
#include "Foundation/Reflect/Structure.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_OBJECT( StructureData );

StructureData::StructureData()
{

}

StructureData::~StructureData()
{

}

void StructureData::ConnectData(void* data)
{
    const Structure* structure = NULL;
    
    if ( data )
    {
        structure = ReflectionCast< Structure >( m_Field->m_Type );
        HELIUM_ASSERT( structure );
    }

    m_Data.Connect( data, structure ? structure->m_Size : 0x0 );
}

bool StructureData::Set(Data* data, uint32_t flags)
{
    StructureData* rhs = SafeCast<StructureData>(data);
    if (!rhs)
    {
        return false;
    }

    const Structure* structure = ReflectionCast< Structure >( m_Field->m_Type );
    HELIUM_ASSERT( structure );

    structure->Copy( rhs->m_Data.Get( structure->m_Size ), m_Data.Get( structure->m_Size ) );
    return true;
}

bool StructureData::Equals(Object* object)
{
    StructureData* rhs = SafeCast<StructureData>(object);
    if (!rhs)
    {
        return false;
    }

    const Structure* structure = ReflectionCast< Structure >( m_Field->m_Type );
    HELIUM_ASSERT( structure );

    return structure->Equals( rhs->m_Data.Get( structure->m_Size ), m_Data.Get( structure->m_Size ) );
}

void StructureData::Accept(Visitor& visitor)
{
    const Structure* structure = ReflectionCast< Structure >( m_Field->m_Type );
    HELIUM_ASSERT( structure );

    structure->Visit( m_Data.Get( structure->m_Size ), visitor );
}

void StructureData::Serialize(Archive& archive)
{
    const Structure* structure = ReflectionCast< Structure >( m_Field->m_Type );
    HELIUM_ASSERT( structure );

    archive.Serialize( m_Data.Get( structure->m_Size ), structure );
}

void StructureData::Deserialize(Archive& archive)
{
    const Structure* structure = ReflectionCast< Structure >( m_Field->m_Type );
    HELIUM_ASSERT( structure );

    archive.Deserialize( m_Data.Get( structure->m_Size ), structure );
}