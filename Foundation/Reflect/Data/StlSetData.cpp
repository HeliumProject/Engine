#include "Foundation/Reflect/Data/StlSetData.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_ABSTRACT(StlSetData)

// If you use this and your data can have spaces, you must make an explicit
// instantiation.  See the the string example below.
// 
// String tokenizer adapted from:
// http://www.oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
template< typename T >
void Tokenize( const tstring& str, std::set< T >& tokens, const tstring& delimiters )
{
    // Skip delimiters at beginning.
    tstring::size_type lastPos = str.find_first_not_of( delimiters, 0 );
    // Find first "non-delimiter".
    tstring::size_type pos     = str.find_first_of( delimiters, lastPos );

    T temp;
    while ( tstring::npos != pos || tstring::npos != lastPos )
    {
        // Found a token, convert it to the proper type for our vector
        tstringstream inStream (str.substr( lastPos, pos - lastPos ));
        inStream >> temp; // NOTE: Stream operator stops at spaces!
        if ( !inStream.fail() )
        {
            // Add the token to the set
            tokens.insert( temp );
            // Skip delimiters.  Note the "not_of"
            lastPos = str.find_first_not_of( delimiters, pos );
            // Find next "non-delimiter"
            pos = str.find_first_of( delimiters, lastPos );
        }
        else
        {
            HELIUM_BREAK();
        }
    }
}

// Explicit implementation for strings, that gets around the stream operator stopping
// at spaces by not using a stream at all.
template<>
inline void Tokenize( const tstring& str, std::set< tstring >& tokens, const tstring& delimiters )
{
    // Skip delimiters at beginning.
    tstring::size_type lastPos = str.find_first_not_of( delimiters, 0 );
    // Find first "non-delimiter".
    tstring::size_type pos     = str.find_first_of( delimiters, lastPos );

    while ( tstring::npos != pos || tstring::npos != lastPos )
    {
        // Add the token to the vector
        tokens.insert( str.substr( lastPos, pos - lastPos ) );
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of( delimiters, pos );
        // Find next "non-delimiter"
        pos = str.find_first_of( delimiters, lastPos );
    }
}

template < class DataT, class DataSer >
SimpleStlSetData<DataT, DataSer>::SimpleStlSetData()
{

}

template < class DataT, class DataSer >
SimpleStlSetData<DataT, DataSer>::~SimpleStlSetData()
{

}

template < class DataT, class DataSer >
void SimpleStlSetData<DataT, DataSer>::ConnectData(Helium::HybridPtr<void> data)
{
    __super::ConnectData( data );

    m_Data.Connect( Helium::HybridPtr<DataType> (data.Address(), data.State()) );
}

template < class DataT, class DataSer >
size_t SimpleStlSetData<DataT, DataSer>::GetSize() const
{
    return m_Data->size();
}

template < class DataT, class DataSer >
void SimpleStlSetData<DataT, DataSer>::Clear()
{
    return m_Data->clear();
}

template < class DataT, class DataSer >
int32_t SimpleStlSetData<DataT, DataSer>::GetItemType() const
{
    return Reflect::GetData<DataT>();
}

template < class DataT, class DataSer >
void SimpleStlSetData<DataT, DataSer>::GetItems(std::vector< ConstDataPtr >& items) const
{
    items.resize( m_Data->size() );
    DataType::const_iterator itr = m_Data->begin();
    DataType::const_iterator end = m_Data->end();
    for ( size_t index=0; itr != end; ++itr, ++index )
    {
        items[index] = static_cast< const ConstDataPtr& >( Data::Bind( *itr, m_Instance, m_Field ) );
    }
}

template < class DataT, class DataSer >
void SimpleStlSetData<DataT, DataSer>::AddItem(const Data* value)
{
    DataT dataValue;
    Data::GetValue(value, dataValue);
    m_Data->insert( dataValue );
}

template < class DataT, class DataSer >
void SimpleStlSetData<DataT, DataSer>::RemoveItem(const Data* value)
{
    DataT dataValue;
    Data::GetValue(value, dataValue);
    m_Data->erase( dataValue );
}

template < class DataT, class DataSer >
bool SimpleStlSetData<DataT, DataSer>::ContainsItem(const Data* value) const
{
    DataT dataValue;
    Data::GetValue(value, dataValue);
    return m_Data->find( dataValue ) != m_Data->end();
}

template < class DataT, class DataSer >
bool SimpleStlSetData<DataT, DataSer>::Set(const Data* src, uint32_t flags)
{
    const StlSetDataT* rhs = ConstObjectCast<StlSetDataT>(src);
    if (!rhs)
    {
        return false;
    }

    m_Data.Set( rhs->m_Data.Get() );

    return true;
}

template < class DataT, class DataSer >
bool SimpleStlSetData<DataT, DataSer>::Equals(const Data* s) const
{
    const StlSetDataT* rhs = ConstObjectCast<StlSetDataT>(s);
    if (!rhs)
    {
        return false;
    }

    return m_Data.Get() == rhs->m_Data.Get();
}

template < class DataT, class DataSer >
void SimpleStlSetData<DataT, DataSer>::Serialize(Archive& archive) const
{
    int i = 0;
    std::vector< ElementPtr > components;
    components.resize( m_Data->size() );

    {
        DataType::const_iterator itr = m_Data->begin();
        DataType::const_iterator end = m_Data->end();
        for ( ; itr != end; ++itr )
        {
            ElementPtr dataElem;

            // query cache for a serializer of this type
            archive.GetCache().Create( Reflect::GetType<DataSer>(), dataElem );

            // downcast to serializer type
            DataSer* dataSer = DangerousCast<DataSer>(dataElem);

            // connect to our map data memory address
            dataSer->ConnectData(const_cast<DataT*>(&(*itr)));

            // serialize to the archive stream
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

template < class DataT, class DataSer >
void SimpleStlSetData<DataT, DataSer>::Deserialize(Archive& archive)
{
    std::vector< ElementPtr > components;
    archive.Deserialize(components);

    // if we are referring to a real field, clear its contents
    m_Data->clear();

    std::vector< ElementPtr >::iterator itr = components.begin();
    std::vector< ElementPtr >::iterator end = components.end();
    for ( ; itr != end; ++itr )
    {
        DataSer* data = ObjectCast<DataSer>(*itr);
        if (!data)
        {
            throw LogisticException( TXT( "Set value type has changed, this is unpossible" ) );
        }

        m_Data->insert(data->m_Data.Get());
    }
}

template < class DataT, class DataSer >
tostream& SimpleStlSetData<DataT, DataSer>::operator>> (tostream& stream) const
{
    DataType::const_iterator itr = m_Data->begin();
    DataType::const_iterator end = m_Data->end();
    for ( ; itr != end; ++itr )
    {
        if ( itr != m_Data->begin() )
        {
            stream << s_ContainerItemDelimiter;
        }

        stream << *itr;
    }

    return stream;
}

template < class DataT, class DataSer >
tistream& SimpleStlSetData<DataT, DataSer>::operator<< (tistream& stream)
{
    m_Data->clear();

    tstring str;
    std::streamsize size = stream.rdbuf()->in_avail();
    str.resize( (size_t) size);
    stream.read( const_cast< tchar_t* >( str.c_str() ), size );

    Tokenize< DataT >( str, m_Data.Ref(), s_ContainerItemDelimiter );

    return stream;
}  

template SimpleStlSetData< tstring, StringData >;
template SimpleStlSetData< uint32_t, U32Data >;
template SimpleStlSetData< uint64_t, U64Data >;
template SimpleStlSetData< float32_t, F32Data >;
template SimpleStlSetData< Helium::GUID, GUIDData >;
template SimpleStlSetData< Helium::TUID, TUIDData >;
template SimpleStlSetData< Helium::Path, PathData >;

REFLECT_DEFINE_CLASS( StringStlSetData );
REFLECT_DEFINE_CLASS( U32StlSetData );
REFLECT_DEFINE_CLASS( U64StlSetData );
REFLECT_DEFINE_CLASS( F32StlSetData );
REFLECT_DEFINE_CLASS( GUIDStlSetData );
REFLECT_DEFINE_CLASS( TUIDStlSetData );
REFLECT_DEFINE_CLASS( PathStlSetData );