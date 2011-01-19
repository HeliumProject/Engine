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

void ObjectStlVectorData::ConnectData(Helium::HybridPtr<void> data)
{
    m_Data.Connect( Helium::HybridPtr<DataType> (data.Address(), data.State()) );
}

size_t ObjectStlVectorData::GetSize() const 
{ 
    return m_Data->size(); 
}

void ObjectStlVectorData::Clear()
{ 
    return m_Data->clear(); 
}

bool ObjectStlVectorData::Set(const Data* src, uint32_t flags)
{
    const ObjectStlVectorData* rhs = SafeCast<ObjectStlVectorData>(src);
    if (!rhs)
    {
        return false;
    }

    m_Data->resize(rhs->m_Data->size());

    if (flags & DataFlags::Shallow)
    {
        m_Data.Ref() = rhs->m_Data.Ref();
    }
    else
    {
        std::vector< ObjectPtr >::const_iterator itr = rhs->m_Data->begin();
        std::vector< ObjectPtr >::const_iterator end = rhs->m_Data->end();
        for ( int index = 0; itr != end; ++itr )
        {
            Object* object = *itr;
            m_Data.Ref()[index++] = ( object ? object->Clone() : NULL );
        }
    }

    return true;
}

bool ObjectStlVectorData::Equals(const Object* object) const
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

void ObjectStlVectorData::Serialize(Archive& archive) const
{
    archive.Serialize(m_Data.Get());
}

void ObjectStlVectorData::Deserialize(Archive& archive)
{
    // if we are referring to a real field, clear its contents
    m_Data->clear();

    archive.Deserialize(m_Data.Ref());
}

void ObjectStlVectorData::Accept(Visitor& visitor)
{
    std::vector< ObjectPtr >::iterator itr = const_cast<Data::Pointer<DataType>&>(m_Data)->begin();
    std::vector< ObjectPtr >::iterator end = const_cast<Data::Pointer<DataType>&>(m_Data)->end();
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