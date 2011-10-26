#include "FoundationPch.h"
#include "Foundation/Reflect/Data/PointerData.h"

#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"

REFLECT_DEFINE_OBJECT( Helium::Reflect::PointerData );

using namespace Helium;
using namespace Helium::Reflect;

PointerData::PointerData()
{

}

PointerData::~PointerData()
{

}

void PointerData::ConnectData(void* data)
{
    m_Data.Connect( data );
}

bool PointerData::Set(Data* data, uint32_t flags)
{
    const PointerData* rhs = SafeCast<PointerData>(data);
    if (!rhs)
    {
        return false;
    }

    if (flags & DataFlags::Shallow)
    {
        *m_Data = *rhs->m_Data;
    }
    else
    {
        *m_Data = (*rhs->m_Data).ReferencesObject() ? (*rhs->m_Data)->Clone() : NULL;
    }

    return true;
}

bool PointerData::Equals(Object* object)
{
    const PointerData* rhs = SafeCast<PointerData>(object);
    
    if (!rhs)
    {
        return false;
    }

    // if the pointers are equal we are done
    if ( *m_Data == *rhs->m_Data )
    {
        return true;
    }
    // if they are not equal but one is null we are done
    else if ( (*m_Data).ReferencesObject() || !(*rhs->m_Data).ReferencesObject() )
    {
        return false;
    }

    // pointers aren't equal so we have to do deep equality test
    return (*m_Data)->Equals( *rhs->m_Data );
}

void PointerData::Accept(Visitor& visitor)
{
    if ( !visitor.VisitPointer( *m_Data ) )
    {
        return;
    }

    if ( *m_Data )
    {
        (*m_Data)->Accept( visitor );
    }
}

void PointerData::Serialize(ArchiveBinary& archive)
{
    Serialize<ArchiveBinary>( archive );
}

void PointerData::Deserialize(ArchiveBinary& archive)
{
    Deserialize<ArchiveBinary>( archive );
}

void PointerData::Serialize(ArchiveXML& archive)
{
    Serialize<ArchiveXML>( archive );
}

void PointerData::Deserialize(ArchiveXML& archive)
{
    Deserialize<ArchiveXML>( archive );
}

template< class ArchiveT >
void PointerData::Serialize(ArchiveT& archive)
{
    archive.SerializeInstance( *m_Data );
}

template< class ArchiveT >
void PointerData::Deserialize(ArchiveT& archive)
{
    *m_Data = NULL;

    archive.DeserializeInstance( *m_Data );
}