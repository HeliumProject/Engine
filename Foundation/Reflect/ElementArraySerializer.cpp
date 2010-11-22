#include "Foundation/Reflect/ElementArraySerializer.h"
#include "Foundation/Reflect/Serializers.h"

using namespace Helium::Reflect;

REFLECT_DEFINE_CLASS(ElementArraySerializer);

ElementArraySerializer::ElementArraySerializer()
{

}

ElementArraySerializer::~ElementArraySerializer()
{

}

void ElementArraySerializer::ConnectData(Helium::HybridPtr<void> data)
{
    __super::ConnectData( data );

    m_Data.Connect( Helium::HybridPtr<DataType> (data.Address(), data.State()) );
}

size_t ElementArraySerializer::GetSize() const 
{ 
    return m_Data->size(); 
}

void ElementArraySerializer::Clear()
{ 
    return m_Data->clear(); 
}

bool ElementArraySerializer::Set(const Serializer* src, uint32_t flags)
{
    const ElementArraySerializer* rhs = ConstObjectCast<ElementArraySerializer>(src);
    if (!rhs)
    {
        return false;
    }

    m_Data->resize(rhs->m_Data->size());

    std::vector< ElementPtr >::const_iterator itr = rhs->m_Data->begin();
    std::vector< ElementPtr >::const_iterator end = rhs->m_Data->end();
    for ( int index = 0; itr != end; ++itr )
    {
        if (flags & SerializerFlags::Shallow)
        {
            m_Data.Ref()[index++] = *itr;
        }
        else
        {
            m_Data.Ref()[index++] = (*itr)->Clone();
        }
    }

    return true;
}

bool ElementArraySerializer::Equals(const Serializer* s) const
{
    const ElementArraySerializer* rhs = ConstObjectCast<ElementArraySerializer>(s);
    if (!rhs)
    {
        return false;
    }

    if (m_Data->size() != rhs->m_Data->size())
    {
        return false;
    }

    std::vector< ElementPtr >::const_iterator itrLHS = m_Data->begin();
    std::vector< ElementPtr >::const_iterator endLHS = m_Data->end();
    std::vector< ElementPtr >::const_iterator itrRHS = rhs->m_Data->begin();
    std::vector< ElementPtr >::const_iterator endRHS = rhs->m_Data->end();
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

void ElementArraySerializer::Serialize(Archive& archive) const
{
    archive.Serialize(m_Data.Get());
}

void ElementArraySerializer::Deserialize(Archive& archive)
{
    // if we are referring to a real field, clear its contents
    m_Data->clear();

    archive.Deserialize(m_Data.Ref());
}

void ElementArraySerializer::Accept(Visitor& visitor)
{
    std::vector< ElementPtr >::iterator itr = const_cast<Serializer::DataPtr<DataType>&>(m_Data)->begin();
    std::vector< ElementPtr >::iterator end = const_cast<Serializer::DataPtr<DataType>&>(m_Data)->end();
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