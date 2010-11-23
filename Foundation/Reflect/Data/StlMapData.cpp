#include "Foundation/Reflect/Data/StlMapData.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_ABSTRACT(StlMapData)

// Tokenizer adapted from:
// http://www.oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
// str should contain a string with map element separated by the specified delimiters argument.
// str will be parsed into key-value pairs and each pair will be inserted into tokens.
template< typename TKey, typename TVal >
inline void Tokenize( const tstring& str, std::map< TKey, TVal >& tokens, const tstring& delimiters )
{
    // Skip delimiters at beginning.
    tstring::size_type lastPos = str.find_first_not_of( delimiters, 0 );
    // Find first "non-delimiter".
    tstring::size_type pos     = str.find_first_of( delimiters, lastPos );

    while ( tstring::npos != pos || tstring::npos != lastPos )
    {
        tstringstream kStream( str.substr( lastPos, pos - lastPos ) );

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of( delimiters, pos );
        // Find next "non-delimiter"
        pos = str.find_first_of( delimiters, lastPos );

        if ( tstring::npos != pos || tstring::npos != lastPos )
        {
            tstringstream vStream( str.substr( lastPos, pos - lastPos ) );

            // At this point, we have the key and value.  Build the map entry.
            // Note that the stream operator stops at spaces.
            TKey k;
            kStream >> k;
            TVal v;
            vStream >> v;
            tokens.insert( std::map< TKey, TVal >::value_type( k, v ) );

            // Skip delimiters.  Note the "not_of"
            lastPos = str.find_first_not_of( delimiters, pos );

            // Find next "non-delimiter"
            pos = str.find_first_of( delimiters, lastPos );
        }
        else
        {
            // Delimited map is messed up. It should alternate key-value pairs, but there is
            // a key without a value.
            HELIUM_BREAK();
        }
    }
}

// Partial specialization for strings as TVal, that gets around the stream operator stopping
// at spaces by not using a stream at all.
template< typename TKey, typename TVal >
inline void Tokenize( const tstring& str, std::map< TKey, tstring >& tokens, const tstring& delimiters )
{
    // Skip delimiters at beginning.
    tstring::size_type lastPos = str.find_first_not_of( delimiters, 0 );
    // Find first "non-delimiter".
    tstring::size_type pos     = str.find_first_of( delimiters, lastPos );

    while ( tstring::npos != pos || tstring::npos != lastPos )
    {
        tstringstream kStream( str.substr( lastPos, pos - lastPos ) );

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of( delimiters, pos );
        // Find next "non-delimiter"
        pos = str.find_first_of( delimiters, lastPos );

        if ( tstring::npos != pos || tstring::npos != lastPos )
        {
            // At this point, we have the key and value.  Build the map entry.
            TKey k;
            kStream >> k;
            tokens.insert( std::map< TKey, TVal >::value_type( k, str.substr( lastPos, pos - lastPos ) ) );

            // Skip delimiters.  Note the "not_of"
            lastPos = str.find_first_not_of( delimiters, pos );

            // Find next "non-delimiter"
            pos = str.find_first_of( delimiters, lastPos );
        }
        else
        {
            // Delimited map is messed up. It should alternate key-value pairs, but there is
            // a key without a value.
            HELIUM_BREAK();
        }
    }
}

// Partial specialization for strings as TKey, that gets around the stream operator stopping
// at spaces by not using a stream at all.
template< typename TKey, typename TVal >
inline void Tokenize( const tstring& str, std::map< tstring, TVal >& tokens, const tstring& delimiters )
{
    // Skip delimiters at beginning.
    tstring::size_type lastPos = str.find_first_not_of( delimiters, 0 );
    // Find first "non-delimiter".
    tstring::size_type pos     = str.find_first_of( delimiters, lastPos );

    while ( tstring::npos != pos || tstring::npos != lastPos )
    {
        tstring key( str.substr( lastPos, pos - lastPos ) );

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of( delimiters, pos );
        // Find next "non-delimiter"
        pos = str.find_first_of( delimiters, lastPos );

        if ( tstring::npos != pos || tstring::npos != lastPos )
        {
            tstringstream vStream( str.substr( lastPos, pos - lastPos ) );

            // At this point, we have the key and value.  Build the map entry.
            TVal v;
            vStream >> v;
            tokens.insert( std::map< TKey, TVal >::value_type( key, v ) );

            // Skip delimiters.  Note the "not_of"
            lastPos = str.find_first_not_of( delimiters, pos );

            // Find next "non-delimiter"
            pos = str.find_first_of( delimiters, lastPos );
        }
        else
        {
            // Delimited map is messed up. It should alternate key-value pairs, but there is
            // a key without a value.
            HELIUM_BREAK();
        }
    }
}

// Explicit implementation for strings, that gets around the stream operator stopping
// at spaces by not using a stream at all.
template< typename TKey, typename TVal >
inline void Tokenize( const tstring& str, std::map< tstring, tstring >& tokens, const tstring& delimiters )
{
    // Skip delimiters at beginning.
    tstring::size_type lastPos = str.find_first_not_of( delimiters, 0 );
    // Find first "non-delimiter".
    tstring::size_type pos     = str.find_first_of( delimiters, lastPos );

    while ( tstring::npos != pos || tstring::npos != lastPos )
    {
        tstring key( str.substr( lastPos, pos - lastPos ) );

        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of( delimiters, pos );
        // Find next "non-delimiter"
        pos = str.find_first_of( delimiters, lastPos );

        if ( tstring::npos != pos || tstring::npos != lastPos )
        {
            // At this point, we have the key and value.  Build the map entry.
            tokens.insert( std::map< tstring, tstring >::value_type( key, str.substr( lastPos, pos - lastPos ) ) );

            // Skip delimiters.  Note the "not_of"
            lastPos = str.find_first_not_of( delimiters, pos );

            // Find next "non-delimiter"
            pos = str.find_first_of( delimiters, lastPos );
        }
        else
        {
            // Delimited map is messed up. It should alternate key-value pairs, but there is
            // a key without a value.
            HELIUM_BREAK();
        }
    }
}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::SimpleStlMapData()
{

}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::~SimpleStlMapData()
{

}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
size_t SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::GetSize() const
{
    return m_Data->size();
}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
void SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::Clear()
{
    return m_Data->clear();
}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
void SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::ConnectData(Helium::HybridPtr<void> data)
{
    __super::ConnectData( data );

    m_Data.Connect( Helium::HybridPtr<DataType> (data.Address(), data.State()) );
}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
int32_t SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::GetKeyType() const
{
    return Reflect::GetData<KeyT>();
}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
int32_t SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::GetValueType() const
{
    return Reflect::GetData<ValueT>();
}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
void SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::GetItems(V_ValueType& items)
{
    items.resize(m_Data->size());
    DataType::const_iterator itr = m_Data->begin();
    DataType::const_iterator end = m_Data->end();
    for ( size_t index=0; itr != end; ++itr, ++index )
    {
        items[index].first = static_cast< const ConstDataPtr& >( Data::Bind( itr->first, m_Instance, m_Field ) );
        items[index].second = static_cast< const ConstDataPtr& >( Data::Bind( itr->second, m_Instance, m_Field ) );
    }
}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
void SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::GetItems(V_ConstValueType& items) const
{
    items.resize(m_Data->size());
    DataType::const_iterator itr = m_Data->begin();
    DataType::const_iterator end = m_Data->end();
    for ( size_t index=0; itr != end; ++itr, ++index )
    {
        items[index].first = static_cast< const ConstDataPtr& >( Data::Bind( itr->first, m_Instance, m_Field ) );
        items[index].second = static_cast< const ConstDataPtr& >( Data::Bind( itr->second, m_Instance, m_Field ) );
    }
}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
DataPtr SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::GetItem(const Data* key)
{
    KeyT keyValue;
    Data::GetValue(key, keyValue);

    DataType::const_iterator found = m_Data->find( keyValue );
    if ( found != m_Data->end() )
    {
        return Data::Bind( found->second, m_Instance, m_Field );
    }

    return NULL;
}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
ConstDataPtr SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::GetItem(const Data* key) const
{
    KeyT keyValue;
    Data::GetValue(key, keyValue);

    DataType::const_iterator found = m_Data->find( keyValue );
    if ( found != m_Data->end() )
    {
        return Data::Bind( found->second, m_Instance, m_Field );
    }

    return NULL;
}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
void SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::SetItem(const Data* key, const Data* value)
{
    KeyT keyValue;
    Data::GetValue(key, keyValue);

    ValueT valueValue;
    Data::GetValue(value, valueValue);

    (m_Data.Ref())[keyValue] = valueValue;
}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
void SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::RemoveItem(const Data* key)
{
    KeyT keyValue;
    Data::GetValue(key, keyValue);

    (m_Data.Ref()).erase(keyValue);
}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
bool SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::Set(const Data* src, uint32_t flags)
{
    const StlMapDataT* rhs = ConstObjectCast<StlMapDataT>(src);
    if (!rhs)
    {
        return false;
    }

    m_Data.Set( rhs->m_Data.Get() );

    return true;
}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
bool SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::Equals(const Data* s) const
{
    const StlMapDataT* rhs = ConstObjectCast<StlMapDataT>(s);
    if (!rhs)
    {
        return false;
    }

    return m_Data.Get() == rhs->m_Data.Get();
}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
void SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::Serialize(Archive& archive) const
{
    int i = 0;
    std::vector< ElementPtr > components;
    components.resize( m_Data->size() * 2 );

    {
        DataType::const_iterator itr = m_Data->begin();
        DataType::const_iterator end = m_Data->end();
        for ( ; itr != end; ++itr )
        {
            ElementPtr keyElem;
            ElementPtr dataElem;

            // query cache for a serializer of this type
            archive.GetCache().Create( Reflect::GetType<KeySer>(), keyElem );
            archive.GetCache().Create( Reflect::GetType<ValueSer>(), dataElem );

            // downcast to serializer type
            KeySer* keySer = DangerousCast<KeySer>(keyElem);
            ValueSer* dataSer = DangerousCast<ValueSer>(dataElem);

            // connect to our map key memory address
            keySer->ConnectData(const_cast<KeyT*>(&(itr->first)));

            // connect to our map data memory address
            dataSer->ConnectData(const_cast<ValueT*>(&(itr->second)));

            // serialize to the archive stream
            components[i++] = keySer;
            components[i++] = dataSer;
        }
    }

    archive.Serialize(components);

    std::vector< ElementPtr >::iterator itr = components.begin();
    std::vector< ElementPtr >::iterator end = components.end();
    for ( ; itr != end; ++itr )
    {
        // downcast to serializer type
        Data* ser = DangerousCast<Data>(*itr);

        // disconnect from memory
        ser->Disconnect();

        // restore serializer to the cache
        archive.GetCache().Free( ser );
    }
}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
void SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::Deserialize(Archive& archive)
{
    std::vector< ElementPtr > components;
    archive.Deserialize(components, ArchiveFlags::Sparse);

    if (components.size() % 2 != 0)
    {
        throw Reflect::DataFormatException( TXT( "Unmatched map objects" ) );
    }

    // if we are referring to a real field, clear its contents
    m_Data->clear();

    std::vector< ElementPtr >::iterator itr = components.begin();
    std::vector< ElementPtr >::iterator end = components.end();
    for ( ; itr != end; ++itr )
    {
        KeySer* key = ObjectCast<KeySer>( *itr );
        ValueSer* value = ObjectCast<ValueSer>( *(++itr) );

        if (key && value)
        {
            m_Data.Ref()[ key->m_Data.Get() ] = value->m_Data.Get();
        }
    }
}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
tostream& SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::operator>> (tostream& stream) const
{
    DataType::const_iterator itr = m_Data->begin();
    DataType::const_iterator end = m_Data->end();
    for ( ; itr != end; ++itr )
    {
        if ( itr != m_Data->begin() )
        {
            stream << s_ContainerItemDelimiter;
        }

        stream << itr->first << s_ContainerItemDelimiter << itr->second;
    }

    return stream;
}

template < class KeyT, class KeySer, class ValueT, class ValueSer >
tistream& SimpleStlMapData<KeyT, KeySer, ValueT, ValueSer>::operator<< (tistream& stream)
{
    m_Data->clear();

    tstring str;
    std::streamsize size = stream.rdbuf()->in_avail();
    str.resize( (size_t) size);
    stream.read( const_cast< tchar_t* >( str.c_str() ), size );

    Tokenize< KeyT, ValueT >( str, m_Data.Ref(), s_ContainerItemDelimiter );

    return stream;
}  

template SimpleStlMapData<tstring, StringData, tstring, StringData>;
template SimpleStlMapData<tstring, StringData, bool, BoolData>;
template SimpleStlMapData<tstring, StringData, uint32_t, U32Data>;
template SimpleStlMapData<tstring, StringData, int32_t, I32Data>;

template SimpleStlMapData<uint32_t, U32Data, tstring, StringData>;
template SimpleStlMapData<uint32_t, U32Data, uint32_t, U32Data>;
template SimpleStlMapData<uint32_t, U32Data, int32_t, I32Data>;
template SimpleStlMapData<uint32_t, U32Data, uint64_t, U64Data>;

template SimpleStlMapData<int32_t, I32Data, tstring, StringData>;
template SimpleStlMapData<int32_t, I32Data, uint32_t, U32Data>;
template SimpleStlMapData<int32_t, I32Data, int32_t, I32Data>;
template SimpleStlMapData<int32_t, I32Data, uint64_t, U64Data>;

template SimpleStlMapData<uint64_t, U64Data, tstring, StringData>;
template SimpleStlMapData<uint64_t, U64Data, uint32_t, U32Data>;
template SimpleStlMapData<uint64_t, U64Data, uint64_t, U64Data>;
template SimpleStlMapData<uint64_t, U64Data, Matrix4, Matrix4Data>;

template SimpleStlMapData<Helium::GUID, GUIDData, uint32_t, U32Data>;
template SimpleStlMapData<Helium::GUID, GUIDData, Matrix4, Matrix4Data>;
template SimpleStlMapData<Helium::TUID, TUIDData, uint32_t, U32Data>;
template SimpleStlMapData<Helium::TUID, TUIDData, Matrix4, Matrix4Data>;

REFLECT_DEFINE_CLASS(StringStringStlMapData);
REFLECT_DEFINE_CLASS(StringBoolStlMapData);
REFLECT_DEFINE_CLASS(StringU32StlMapData);
REFLECT_DEFINE_CLASS(StringI32StlMapData);

REFLECT_DEFINE_CLASS(U32StringStlMapData);
REFLECT_DEFINE_CLASS(U32U32StlMapData);
REFLECT_DEFINE_CLASS(U32I32StlMapData);
REFLECT_DEFINE_CLASS(U32U64StlMapData);

REFLECT_DEFINE_CLASS(I32StringStlMapData);
REFLECT_DEFINE_CLASS(I32U32StlMapData);
REFLECT_DEFINE_CLASS(I32I32StlMapData);
REFLECT_DEFINE_CLASS(I32U64StlMapData);

REFLECT_DEFINE_CLASS(U64StringStlMapData);
REFLECT_DEFINE_CLASS(U64U32StlMapData);
REFLECT_DEFINE_CLASS(U64U64StlMapData);
REFLECT_DEFINE_CLASS(U64Matrix4StlMapData);

REFLECT_DEFINE_CLASS(GUIDU32StlMapData);
REFLECT_DEFINE_CLASS(GUIDMatrix4StlMapData);
REFLECT_DEFINE_CLASS(TUIDU32StlMapData);
REFLECT_DEFINE_CLASS(TUIDMatrix4StlMapData);
