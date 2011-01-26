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
void SimpleObjectStlMapData<KeyT>::ConnectData(void* data)
{
    m_Data.Connect( data );
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
        items[index].first = Data::Bind( const_cast< KeyT& >( itr->first ), m_Instance, m_Field );
        items[index].second = &itr->second;
    }
}

template < class KeyT >
ObjectPtr* SimpleObjectStlMapData<KeyT>::GetItem(Data* key)
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
void SimpleObjectStlMapData<KeyT>::SetItem(Data* key, Object* value)
{
    KeyT keyValue;
    Data::GetValue(key, keyValue);
    (*m_Data)[keyValue] = value;
}

template < class KeyT >
void SimpleObjectStlMapData<KeyT>::RemoveItem(Data* key)
{
    KeyT keyValue;
    Data::GetValue(key, keyValue);

    (*m_Data).erase(keyValue);
}

template < class KeyT >
bool SimpleObjectStlMapData<KeyT>::Set(Data* src, uint32_t flags)
{
    const ObjectStlMapDataT* rhs = SafeCast<ObjectStlMapDataT>(src);
    if (!rhs)
    {
        return false;
    }

    m_Data->clear();

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
            Object* object = itr->second;
            (*m_Data)[itr->first] = ( object ? object->Clone() : NULL );
        }
    }

    return true;
}

template < class KeyT >
bool SimpleObjectStlMapData<KeyT>::Equals(Object* object)
{
    const ObjectStlMapDataT* rhs = SafeCast<ObjectStlMapDataT>( object );
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
            ObjectPtr elem = Registry::GetInstance()->CreateInstance( Reflect::GetDataClass<KeyT>() );

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
            ++itr;

            // might be useful to cache the data object here
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
        Data* key = SafeCast<Data>(*itr);
        Object* value = *(++itr);
        if ( key )  // The object value can be null, so don't check it here.
        {
            KeyT k;
            Data::GetValue( key, k );
            (*m_Data)[ k ] = value;
        }
    }
}

template < class KeyT >
void SimpleObjectStlMapData<KeyT>::Accept(Visitor& visitor)
{
    DataType::iterator itr = const_cast<DataPointer<DataType>&>(m_Data)->begin();
    DataType::iterator end = const_cast<DataPointer<DataType>&>(m_Data)->end();
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

REFLECT_DEFINE_OBJECT(TypeIDObjectStlMapData);
REFLECT_DEFINE_OBJECT(StlStringObjectStlMapData);
REFLECT_DEFINE_OBJECT(UInt32ObjectStlMapData);
REFLECT_DEFINE_OBJECT(Int32ObjectStlMapData);
REFLECT_DEFINE_OBJECT(UInt64ObjectStlMapData);
REFLECT_DEFINE_OBJECT(Int64ObjectStlMapData);
REFLECT_DEFINE_OBJECT(GUIDObjectStlMapData);
REFLECT_DEFINE_OBJECT(TUIDObjectStlMapData);