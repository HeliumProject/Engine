#include "stdafx.h"
#include "ElementMapSerializer.h"

using namespace Reflect;

template < class KeyT >
SimpleElementMapSerializer<KeyT>::SimpleElementMapSerializer()
{

}

template < class KeyT >
SimpleElementMapSerializer<KeyT>::~SimpleElementMapSerializer()
{

}

template < class KeyT >
void SimpleElementMapSerializer<KeyT>::ConnectData(Nocturnal::HybridPtr<void> data)
{
    __super::ConnectData( data );

    m_Data.Connect( Nocturnal::HybridPtr<DataType> (data.Address(), data.State()) );
}

template < class KeyT >
size_t SimpleElementMapSerializer<KeyT>::GetSize() const
{
    return m_Data->size();
}

template < class KeyT >
void SimpleElementMapSerializer<KeyT>::Clear()
{
    return m_Data->clear();
}

template < class KeyT >
i32 SimpleElementMapSerializer<KeyT>::GetKeyType() const
{
    return Serializer::DeduceType<KeyT>();
}

template < class KeyT >
void SimpleElementMapSerializer<KeyT>::GetItems(V_ValueType& items)
{
    items.resize(m_Data->size());
    DataType::iterator itr = m_Data->begin();
    DataType::iterator end = m_Data->end();
    for ( size_t index=0; itr != end; ++itr, ++index )
    {
        items[index].first = Serializer::Bind( itr->first, m_Instance, m_Field );
        items[index].second = &itr->second;
    }
}

template < class KeyT >
void SimpleElementMapSerializer<KeyT>::GetItems(V_ConstValueType& items) const
{
    items.resize(m_Data->size());
    DataType::const_iterator itr = m_Data->begin();
    DataType::const_iterator end = m_Data->end();
    for ( size_t index=0; itr != end; ++itr, ++index )
    {
        items[index].first = Serializer::Bind( itr->first, m_Instance, m_Field );
        items[index].second = &itr->second;
    }
}

template < class KeyT >
ElementPtr* SimpleElementMapSerializer<KeyT>::GetItem(const Serializer* key)
{
    KeyT keyValue;
    Serializer::GetValue(key, keyValue);

    DataType::iterator found = m_Data->find( keyValue );
    if ( found != m_Data->end() )
    {
        return &found->second;
    }

    return NULL;
}

template < class KeyT >
const ElementPtr* SimpleElementMapSerializer<KeyT>::GetItem(const Serializer* key) const
{
    KeyT keyValue;
    Serializer::GetValue(key, keyValue);

    DataType::const_iterator found = m_Data->find( keyValue );
    if ( found != m_Data->end() )
    {
        return &found->second;
    }

    return NULL;
}

template < class KeyT >
void SimpleElementMapSerializer<KeyT>::SetItem(const Serializer* key, const Element* value)
{
    KeyT keyValue;
    Serializer::GetValue(key, keyValue);

    (m_Data.Ref())[keyValue] = value;
}

template < class KeyT >
void SimpleElementMapSerializer<KeyT>::RemoveItem(const Serializer* key)
{
    KeyT keyValue;
    Serializer::GetValue(key, keyValue);

    (m_Data.Ref()).erase(keyValue);
}

template < class KeyT >
bool SimpleElementMapSerializer<KeyT>::Set(const Serializer* src, u32 flags)
{
    const ElementMapSerializerT* rhs = ConstObjectCast<ElementMapSerializerT>(src);
    if (!rhs)
    {
        return false;
    }

    m_Data->clear();

    DataType::const_iterator itr = rhs->m_Data->begin();
    DataType::const_iterator end = rhs->m_Data->end();
    for ( ; itr != end; ++itr )
    {
        if (flags & SerializerFlags::Shallow)
        {
            m_Data.Ref()[itr->first] = itr->second;
        }
        else
        {
            m_Data.Ref()[itr->first] = itr->second->Clone();
        }
    }

    return true;
}

template < class KeyT >
bool SimpleElementMapSerializer<KeyT>::Equals(const Serializer* s) const
{
    const ElementMapSerializerT* rhs = ConstObjectCast<ElementMapSerializerT>(s);
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
        if ( itrLHS->first != itrRHS->first )
        {
            return false;
        }

        if ( !itrLHS->second->Equals(itrRHS->second))
        {
            return false;
        }
    }

    return true;
}

template < class KeyT >
void SimpleElementMapSerializer<KeyT>::Serialize(Archive& archive) const
{
    V_Element components;
    components.resize(m_Data->size() * 2);

    {
        DataType::const_iterator itr = m_Data->begin();
        DataType::const_iterator end = m_Data->end();
        for ( int i = 0; itr != end; ++itr )
        {
            if (!itr->second.ReferencesObject())
            {
                continue;
            }

            ElementPtr elem;
            archive.GetCache().Create( Serializer::DeduceType<KeyT>(), elem );

            Serializer* ser = AssertCast<Serializer>(elem.Ptr());
            ser->ConnectData((void*)&(itr->first));

            components[i++] = ser;
            components[i++] = itr->second;
        }
    }

    archive.Serialize(components);

    {
        V_Element::iterator itr = components.begin();
        V_Element::iterator end = components.end();
        for ( ; itr != end; ++itr )
        {
            Serializer* ser = AssertCast<Serializer>(*itr);
            ser->Disconnect();
            archive.GetCache().Free(ser);
            ++itr;
        }
    }
}

template < class KeyT >
void SimpleElementMapSerializer<KeyT>::Deserialize(Archive& archive)
{
    V_Element components;
    archive.Deserialize(components, ArchiveFlags::Sparse);

    if (components.size() % 2 != 0)
    {
        throw Reflect::DataFormatException("Unmatched map objects");
    }

    // if we are referring to a real field, clear its contents
    m_Data->clear();

    V_Element::iterator itr = components.begin();
    V_Element::iterator end = components.end();
    for ( ; itr != end; ++itr )
    {
        Serializer* key = ObjectCast<Serializer>(*itr);
        Element* value = *(++itr);
        if ( key && value )
        {
            KeyT k;
            Serializer::GetValue( key, k );
            m_Data.Ref()[ k ] = value;
        }
    }
}

template < class KeyT >
void SimpleElementMapSerializer<KeyT>::Host(Visitor& visitor)
{
    DataType::iterator itr = const_cast<Serializer::DataPtr<DataType>&>(m_Data)->begin();
    DataType::iterator end = const_cast<Serializer::DataPtr<DataType>&>(m_Data)->end();
    for ( ; itr != end; ++itr )
    {
        if (!itr->second.ReferencesObject())
        {
            continue;
        }

        if (!visitor.VisitPointer(itr->second))
        {
            continue;
        }

        itr->second->Host( visitor );
    }
}

template SimpleElementMapSerializer<TypeID>;
template SimpleElementMapSerializer<std::string>;
template SimpleElementMapSerializer<u32>;
template SimpleElementMapSerializer<i32>;
template SimpleElementMapSerializer<u64>;
template SimpleElementMapSerializer<i64>;
template SimpleElementMapSerializer<Nocturnal::GUID>;
template SimpleElementMapSerializer<Nocturnal::TUID>;

REFLECT_DEFINE_CLASS(TypeIDElementMapSerializer);
REFLECT_DEFINE_CLASS(StringElementMapSerializer);
REFLECT_DEFINE_CLASS(U32ElementMapSerializer);
REFLECT_DEFINE_CLASS(I32ElementMapSerializer);
REFLECT_DEFINE_CLASS(U64ElementMapSerializer);
REFLECT_DEFINE_CLASS(I64ElementMapSerializer);
REFLECT_DEFINE_CLASS(GUIDElementMapSerializer);
REFLECT_DEFINE_CLASS(TUIDElementMapSerializer);