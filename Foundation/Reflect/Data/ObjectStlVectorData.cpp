#include "Foundation/Reflect/Data/ObjectStlVectorData.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium::Reflect;

REFLECT_DEFINE_OBJECT(ObjectStlVectorData);

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

void ObjectStlVectorData::Serialize(Archive& archive)
{
    archive.Serialize( *m_Data );
}

void ObjectStlVectorData::Deserialize(Archive& archive)
{
    // if we are referring to a real field, clear its contents
    m_Data->clear();

    archive.Deserialize( *m_Data );
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