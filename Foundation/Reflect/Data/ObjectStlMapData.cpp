#include "Foundation/Reflect/Data/ObjectStlMapData.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_ABSTRACT( ObjectStlMapData );

template < class KeyT >
SimpleObjectStlMapData<KeyT>::SimpleObjectStlMapData()
{

}

template < class KeyT >
SimpleObjectStlMapData<KeyT>::~SimpleObjectStlMapData()
{

}

template < class KeyT >
void SimpleObjectStlMapData<KeyT>::ConnectData(Helium::HybridPtr<void> data)
{
    __super::ConnectData( data );

    m_Data.Connect( Helium::HybridPtr<DataType> (data.Address(), data.State()) );
}

template < class KeyT >
size_t SimpleObjectStlMapData<KeyT>::GetSize() const
{
    return m_Data->size();
}

template < class KeyT >
void SimpleObjectStlMapData<KeyT>::Clear()
{
    return m_Data->clear();
}

template < class KeyT >
const Class* SimpleObjectStlMapData<KeyT>::GetKeyClass() const
{
    return Reflect::GetDataClass<KeyT>();
}

template < class KeyT >
void SimpleObjectStlMapData<KeyT>::GetItems(V_ValueType& items)
{
    items.resize(m_Data->size());
    DataType::iterator itr = m_Data->begin();
    DataType::iterator end = m_Data->end();
    for ( size_t index=0; itr != end; ++itr, ++index )
    {
        items[index].first = static_cast< const ConstDataPtr& >( Data::Bind( itr->first, m_Instance, m_Field ) );
        items[index].second = &itr->second;
    }
}

template < class KeyT >
void SimpleObjectStlMapData<KeyT>::GetItems(V_ConstValueType& items) const
{
    items.resize(m_Data->size());
    DataType::const_iterator itr = m_Data->begin();
    DataType::const_iterator end = m_Data->end();
    for ( size_t index=0; itr != end; ++itr, ++index )
    {
        items[index].first = static_cast< const ConstDataPtr& >( Data::Bind( itr->first, m_Instance, m_Field ) );
        items[index].second = &itr->second;
    }
}

template < class KeyT >
ObjectPtr* SimpleObjectStlMapData<KeyT>::GetItem(const Data* key)
{
    KeyT keyValue;
    Data::GetValue(key, keyValue);

    DataType::iterator found = m_Data->find( keyValue );
    if ( found != m_Data->end() )
    {
        return &found->second;
    }

    return NULL;
}

template < class KeyT >
const ObjectPtr* SimpleObjectStlMapData<KeyT>::GetItem(const Data* key) const
{
    KeyT keyValue;
    Data::GetValue(key, keyValue);

    DataType::const_iterator found = m_Data->find( keyValue );
    if ( found != m_Data->end() )
    {
        return &found->second;
    }

    return NULL;
}

template < class KeyT >
void SimpleObjectStlMapData<KeyT>::SetItem(const Data* key, const Object* value)
{
    KeyT keyValue;
    Data::GetValue(key, keyValue);

#pragma TODO( "Fix const correctness." )
    (m_Data.Ref())[keyValue] = const_cast< Object* >( value );
}

template < class KeyT >
void SimpleObjectStlMapData<KeyT>::RemoveItem(const Data* key)
{
    KeyT keyValue;
    Data::GetValue(key, keyValue);

    (m_Data.Ref()).erase(keyValue);
}

template < class KeyT >
bool SimpleObjectStlMapData<KeyT>::Set(const Data* src, uint32_t flags)
{
    const ObjectStlMapDataT* rhs = ObjectCast<ObjectStlMapDataT>(src);
    if (!rhs)
    {
        return false;
    }

    m_Data->clear();

    DataType::const_iterator itr = rhs->m_Data->begin();
    DataType::const_iterator end = rhs->m_Data->end();
    for ( ; itr != end; ++itr )
    {
        if (flags & DataFlags::Shallow)
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
bool SimpleObjectStlMapData<KeyT>::Equals(const Object* object) const
{
    const ObjectStlMapDataT* rhs = ObjectCast<ObjectStlMapDataT>( object );
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
void SimpleObjectStlMapData<KeyT>::Serialize(Archive& archive) const
{
    std::vector< ObjectPtr > components;
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

            ObjectPtr elem;
            archive.GetCache().Create( Reflect::GetDataClass<KeyT>(), elem );

            Data* ser = AssertCast<Data>(elem.Ptr());
            ser->ConnectData((void*)&(itr->first));

            components[i++] = ser;
            components[i++] = itr->second;
        }
    }

    archive.Serialize(components);

    {
        std::vector< ObjectPtr >::iterator itr = components.begin();
        std::vector< ObjectPtr >::iterator end = components.end();
        for ( ; itr != end; ++itr )
        {
            Data* ser = AssertCast<Data>(*itr);
            ser->Disconnect();
            archive.GetCache().Free(ser);
            ++itr;
        }
    }
}

template < class KeyT >
void SimpleObjectStlMapData<KeyT>::Deserialize(Archive& archive)
{
    std::vector< ObjectPtr > components;
    archive.Deserialize(components, ArchiveFlags::Sparse);

    if (components.size() % 2 != 0)
    {
        throw Reflect::DataFormatException( TXT( "Unmatched map objects" ) );
    }

    // if we are referring to a real field, clear its contents
    m_Data->clear();

    std::vector< ObjectPtr >::iterator itr = components.begin();
    std::vector< ObjectPtr >::iterator end = components.end();
    for ( ; itr != end; ++itr )
    {
        Data* key = ObjectCast<Data>(*itr);
        Object* value = *(++itr);
        if ( key && value )
        {
            KeyT k;
            Data::GetValue( key, k );
            m_Data.Ref()[ k ] = value;
        }
    }
}

template < class KeyT >
void SimpleObjectStlMapData<KeyT>::Accept(Visitor& visitor)
{
    DataType::iterator itr = const_cast<Data::Pointer<DataType>&>(m_Data)->begin();
    DataType::iterator end = const_cast<Data::Pointer<DataType>&>(m_Data)->end();
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

        itr->second->Accept( visitor );
    }
}

template SimpleObjectStlMapData<TypeID>;
template SimpleObjectStlMapData<std::string>;
template SimpleObjectStlMapData<uint32_t>;
template SimpleObjectStlMapData<int32_t>;
template SimpleObjectStlMapData<uint64_t>;
template SimpleObjectStlMapData<int64_t>;
template SimpleObjectStlMapData<Helium::GUID>;
template SimpleObjectStlMapData<Helium::TUID>;

REFLECT_DEFINE_CLASS(TypeIDObjectStlMapData);
REFLECT_DEFINE_CLASS(StlStringObjectStlMapData);
REFLECT_DEFINE_CLASS(UInt32ObjectStlMapData);
REFLECT_DEFINE_CLASS(Int32ObjectStlMapData);
REFLECT_DEFINE_CLASS(UInt64ObjectStlMapData);
REFLECT_DEFINE_CLASS(Int64ObjectStlMapData);
REFLECT_DEFINE_CLASS(GUIDObjectStlMapData);
REFLECT_DEFINE_CLASS(TUIDObjectStlMapData);