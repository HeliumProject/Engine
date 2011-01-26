#include "Foundation/Reflect/Data/ObjectStlSetData.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium::Reflect;

REFLECT_DEFINE_OBJECT(ObjectStlSetData);

ObjectStlSetData::ObjectStlSetData()
{

}

ObjectStlSetData::~ObjectStlSetData()
{

}

void ObjectStlSetData::ConnectData(void* data)
{
    m_Data.Connect( data );
}

size_t ObjectStlSetData::GetSize() const
{
    return m_Data->size();
}

void ObjectStlSetData::Clear()
{
    return m_Data->clear();
}

bool ObjectStlSetData::Set(Data* src, uint32_t flags)
{
    const ObjectStlSetData* rhs = SafeCast<ObjectStlSetData>(src);
    if (!rhs)
    {
        return false;
    }

    int index = 0;

    if (flags & DataFlags::Shallow)
    {
        *m_Data = *rhs->m_Data;
    }
    else
    {
        DataType::const_iterator itr = rhs->m_Data->begin();
        DataType::const_iterator end = rhs->m_Data->end();
        for ( ; itr != end; ++itr )
        {
            Object* object = *itr;
            m_Data->insert( object ? object->Clone() : NULL );
        }
    }

    return true;
}

bool ObjectStlSetData::Equals(Object* object)
{
    const ObjectStlSetData* rhs = SafeCast<ObjectStlSetData>(object);
    if (!rhs)
    {
        return false;
    }

    if (m_Data->size() != rhs->m_Data->size())
    {
        return false;
    }

    DataType::const_iterator itrLHS = m_Data->begin();
    DataType::const_iterator endLHS = m_Data->end();
    DataType::const_iterator itrRHS = rhs->m_Data->begin();
    DataType::const_iterator endRHS = rhs->m_Data->end();
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

void ObjectStlSetData::Serialize(Archive& archive)
{
    std::vector< ObjectPtr > components;

    DataType::const_iterator itr = m_Data->begin();
    DataType::const_iterator end = m_Data->end();
    for ( ; itr != end; ++itr )
    {
        components.push_back(*itr);
    }

    archive.Serialize(components);
}

void ObjectStlSetData::Deserialize(Archive& archive)
{
    std::vector< ObjectPtr > components;
    archive.Deserialize(components);

    // if we are referring to a real field, clear its contents
    m_Data->clear();

    std::vector< ObjectPtr >::const_iterator itr = components.begin();
    std::vector< ObjectPtr >::const_iterator end = components.end();
    for ( ; itr != end; ++itr )
    {
        m_Data->insert(*itr);
    }
}

void ObjectStlSetData::Accept(Visitor& visitor)
{
    DataType::iterator itr = const_cast<DataPointer<DataType>&>(m_Data)->begin();
    DataType::iterator end = const_cast<DataPointer<DataType>&>(m_Data)->end();
    for ( ; itr != end; ++itr )
    {
        if (!itr->ReferencesObject())
        {
            continue;
        }

        // just a note, this code is problematic with STLPort, but i wasn't 
        // able to figure out how to fix it ... works fine with msvc native iterators
        // i wish i had saved the compile error; geoff suspects it is const-ness related
        //
        if (!visitor.VisitPointer(*itr))
        {
            continue;
        }

        (*itr)->Accept( visitor );
    }
}
