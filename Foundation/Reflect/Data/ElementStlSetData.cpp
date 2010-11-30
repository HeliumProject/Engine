#include "Foundation/Reflect/Data/ElementStlSetData.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium::Reflect;

REFLECT_DEFINE_CLASS(ElementStlSetData);

ElementStlSetData::ElementStlSetData()
{

}

ElementStlSetData::~ElementStlSetData()
{

}

void ElementStlSetData::ConnectData(Helium::HybridPtr<void> data)
{
    __super::ConnectData( data );

    m_Data.Connect( Helium::HybridPtr<DataType> (data.Address(), data.State()) );
}

size_t ElementStlSetData::GetSize() const
{
    return m_Data->size();
}

void ElementStlSetData::Clear()
{
    return m_Data->clear();
}

bool ElementStlSetData::Set(const Data* src, uint32_t flags)
{
    const ElementStlSetData* rhs = ConstObjectCast<ElementStlSetData>(src);
    if (!rhs)
    {
        return false;
    }

    int index = 0;

    DataType::const_iterator itr = rhs->m_Data->begin();
    DataType::const_iterator end = rhs->m_Data->end();
    for ( ; itr != end; ++itr )
    {
        if (flags & DataFlags::Shallow)
        {
            m_Data->insert(*itr);
        }
        else
        {
            m_Data->insert((*itr)->Clone());
        }
    }

    return true;
}

bool ElementStlSetData::Equals(const Data* s) const
{
    const ElementStlSetData* rhs = ConstObjectCast<ElementStlSetData>(s);
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

void ElementStlSetData::Serialize(Archive& archive) const
{
    std::vector< ElementPtr > components;

    DataType::const_iterator itr = m_Data->begin();
    DataType::const_iterator end = m_Data->end();
    for ( ; itr != end; ++itr )
    {
        if (!itr->ReferencesObject())
        {
            continue;
        }

        components.push_back(*itr);
    }

    archive.Serialize(components);
}

void ElementStlSetData::Deserialize(Archive& archive)
{
    std::vector< ElementPtr > components;
    archive.Deserialize(components);

    // if we are referring to a real field, clear its contents
    m_Data->clear();

    std::vector< ElementPtr >::const_iterator itr = components.begin();
    std::vector< ElementPtr >::const_iterator end = components.end();
    for ( ; itr != end; ++itr )
    {
        m_Data->insert(*itr);
    }
}

void ElementStlSetData::Accept(Visitor& visitor)
{
    DataType::iterator itr = const_cast<Data::Pointer<DataType>&>(m_Data)->begin();
    DataType::iterator end = const_cast<Data::Pointer<DataType>&>(m_Data)->end();
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
