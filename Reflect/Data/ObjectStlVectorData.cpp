#include "ReflectPch.h"
#include "Reflect/Data/ObjectStlVectorData.h"

#include "Reflect/Data/DataDeduction.h"
#include "Reflect/ArchiveBinary.h"
#include "Reflect/ArchiveXML.h"

REFLECT_DEFINE_OBJECT( Helium::Reflect::ObjectStlVectorData );

using namespace Helium;
using namespace Helium::Reflect;

ObjectStlVectorData::ObjectStlVectorData()
{

}

ObjectStlVectorData::~ObjectStlVectorData()
{

}

void ObjectStlVectorData::ConnectData(void* data)
{
    m_Data.Connect( data );
}

size_t ObjectStlVectorData::GetSize() const 
{ 
    return m_Data->size(); 
}

void ObjectStlVectorData::Clear()
{ 
    return m_Data->clear(); 
}

bool ObjectStlVectorData::Set(Data* src, uint32_t flags)
{
    const ObjectStlVectorData* rhs = SafeCast<ObjectStlVectorData>(src);
    if (!rhs)
    {
        return false;
    }

    m_Data->resize(rhs->m_Data->size());

    if (flags & DataFlags::Shallow)
    {
        *m_Data = *rhs->m_Data;
    }
    else
    {
        std::vector< ObjectPtr >::const_iterator itr = rhs->m_Data->begin();
        std::vector< ObjectPtr >::const_iterator end = rhs->m_Data->end();
        for ( int index = 0; itr != end; ++itr )
        {
            Object* object = *itr;
            m_Data->at( index++ ) = ( object ? object->Clone() : NULL );
        }
    }

    return true;
}

bool ObjectStlVectorData::Equals(Object* object)
{
    const ObjectStlVectorData* rhs = SafeCast<ObjectStlVectorData>(object);
    if (!rhs)
    {
        return false;
    }

    if (m_Data->size() != rhs->m_Data->size())
    {
        return false;
    }

    std::vector< ObjectPtr >::const_iterator itrLHS = m_Data->begin();
    std::vector< ObjectPtr >::const_iterator endLHS = m_Data->end();
    std::vector< ObjectPtr >::const_iterator itrRHS = rhs->m_Data->begin();
    std::vector< ObjectPtr >::const_iterator endRHS = rhs->m_Data->end();
    for ( ; itrLHS != endLHS && itrRHS != endRHS; ++itrLHS, ++itrRHS )
    {
        if ((*itrLHS) == (*itrRHS))
        {
            continue;
        }

        if (!(*itrLHS)->Equals(*itrRHS))
        {
            return false;
        }
    }

    return true;
}

void ObjectStlVectorData::Accept(Visitor& visitor)
{
    std::vector< ObjectPtr >::iterator itr = const_cast<DataPointer<DataType>&>(m_Data)->begin();
    std::vector< ObjectPtr >::iterator end = const_cast<DataPointer<DataType>&>(m_Data)->end();
    for ( ; itr != end; ++itr )
    {
        if (!itr->ReferencesObject())
        {
            continue;
        }

        if (!visitor.VisitPointer(*itr))
        {
            continue;
        }

        (*itr)->Accept( visitor );
    }
}

void ObjectStlVectorData::Serialize(ArchiveBinary& archive)
{
    Serialize<ArchiveBinary>( archive );
}

void ObjectStlVectorData::Deserialize(ArchiveBinary& archive)
{
    Deserialize<ArchiveBinary>( archive );
}

void ObjectStlVectorData::Serialize(ArchiveXML& archive)
{
    Serialize<ArchiveXML>( archive );
}

void ObjectStlVectorData::Deserialize(ArchiveXML& archive)
{
    Deserialize<ArchiveXML>( archive );
}

template< class ArchiveT >
void ObjectStlVectorData::Serialize(ArchiveT& archive)
{
    archive.SerializeArray( *m_Data );
}

template< class ArchiveT >
void ObjectStlVectorData::Deserialize(ArchiveT& archive)
{
    // if we are referring to a real field, clear its contents
    m_Data->clear();

    archive.DeserializeArray( *m_Data );
}
