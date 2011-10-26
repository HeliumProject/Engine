#include "FoundationPch.h"
#include "Foundation/Reflect/Data/StlVectorData.h"

#include "Foundation/Reflect/Data/DataDeduction.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"

REFLECT_DEFINE_ABSTRACT( Helium::Reflect::StlVectorData )

using namespace Helium;
using namespace Helium::Reflect;

// If you use this and your data can have spaces, you must make an explicit
// instantiation.  See the the string example below.
// 
// String tokenizer adapted from:
// http://www.oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
template< typename T, typename I >
void Tokenize( const tstring& str, std::vector< T >& tokens, const tstring& delimiters )
{
    // Skip delimiters at beginning.
    tstring::size_type lastPos = str.find_first_not_of( delimiters, 0 );
    // Find first "non-delimiter".
    tstring::size_type pos     = str.find_first_of( delimiters, lastPos );

    I temp;
    while ( tstring::npos != pos || tstring::npos != lastPos )
    {
        // Found a token, convert it to the proper type for our vector
        tstringstream stream (str.substr( lastPos, pos - lastPos ));
        stream >> temp; // NOTE: Stream operator stops at spaces!
        if ( !stream.fail() )
        {
            // Add the token to the vector
            tokens.push_back( (T)temp );
        }
        else
        {
            HELIUM_BREAK();
        }
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of( delimiters, pos );
        // Find next "non-delimiter"
        pos = str.find_first_of( delimiters, lastPos );
    }
}

// Explicit implementation for strings, that gets around the stream operator stopping
// at spaces by not using a stream at all.
template<>
inline void Tokenize<tstring, tstring>( const tstring& str, std::vector< tstring >& tokens, const tstring& delimiters )
{
    // Skip delimiters at beginning.
    tstring::size_type lastPos = str.find_first_not_of( delimiters, 0 );
    // Find first "non-delimiter".
    tstring::size_type pos     = str.find_first_of( delimiters, lastPos );

    while ( tstring::npos != pos || tstring::npos != lastPos )
    {
        // Add the token to the vector
        tokens.push_back( str.substr( lastPos, pos - lastPos ) );
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of( delimiters, pos );
        // Find next "non-delimiter"
        pos = str.find_first_of( delimiters, lastPos );
    }
}

template < class T >
SimpleStlVectorData<T>::SimpleStlVectorData()
{

}

template < class T >
SimpleStlVectorData<T>::~SimpleStlVectorData()
{

}

template < class T >
void SimpleStlVectorData<T>::ConnectData(void* data)
{
    m_Data.Connect( data );
}

template < class T >
size_t SimpleStlVectorData<T>::GetSize() const
{ 
    return m_Data->size(); 
}

template < class T >
void SimpleStlVectorData<T>::SetSize(size_t size)
{
    return m_Data->resize(size);
}

template < class T >
void SimpleStlVectorData<T>::Clear()
{
    return m_Data->clear();
}

template < class T >
const Class* SimpleStlVectorData<T>::GetItemClass() const
{
    return Reflect::GetDataClass<T>();
}

template < class T >
DataPtr SimpleStlVectorData<T>::GetItem(size_t at)
{
    return Data::Bind(m_Data->at(at), m_Instance, m_Field);
}

template < class T >
void SimpleStlVectorData<T>::SetItem(size_t at, Data* value)
{
    Data::GetValue(value, m_Data->at(at));
}

template < class T >
void SimpleStlVectorData<T>::Insert( size_t at, Data* value )
{
    T temp;
    Data::GetValue( value, temp );
    m_Data->insert( m_Data->begin() + at, temp );
}

template < class T >
void SimpleStlVectorData<T>::Remove( size_t at )
{
    m_Data->erase( m_Data->begin() + at );
}

template < class T >
void SimpleStlVectorData<T>::MoveUp( std::set< size_t >& selectedIndices )
{
    std::set< size_t > newSelectedIndices;

    std::set< size_t >::const_iterator itr = selectedIndices.begin();
    std::set< size_t >::const_iterator end = selectedIndices.end();

    for( ; itr != end; ++itr )
    {
        if ( (*itr) == 0 || ( newSelectedIndices.find( (*itr) - 1 ) != newSelectedIndices.end() ) )
        {
            newSelectedIndices.insert( *itr );
            continue;
        }
        
        T temp = m_Data->at( (*itr) - 1 );
        m_Data->at( (*itr) - 1 ) = m_Data->at( (*itr) );
        m_Data->at( (*itr) ) = temp;

        newSelectedIndices.insert( *itr - 1 );
    }

    selectedIndices = newSelectedIndices;
}

template < class T >
void SimpleStlVectorData<T>::MoveDown( std::set< size_t >& selectedIndices )
{
    std::set< size_t > newSelectedIndices;

    std::set< size_t >::const_reverse_iterator itr = selectedIndices.rbegin();
    std::set< size_t >::const_reverse_iterator end = selectedIndices.rend();

    for( ; itr != end; ++itr )
    {
        if ( ( (*itr) == m_Data->size() - 1 ) || ( newSelectedIndices.find( (*itr) + 1 ) != newSelectedIndices.end() ) )
        {
            newSelectedIndices.insert( *itr );
            continue;
        }
        
        T temp = m_Data->at( (*itr) + 1 );
        m_Data->at( (*itr) + 1 ) = m_Data->at( (*itr) );
        m_Data->at( (*itr) ) = temp;

        newSelectedIndices.insert( *itr + 1 );
    }

    selectedIndices = newSelectedIndices;
}

template < class T >
bool SimpleStlVectorData<T>::Set(Data* src, uint32_t flags)
{
    const SimpleStlVectorData<T>* rhs = SafeCast<SimpleStlVectorData<T>>(src);
    if (!rhs)
    {
        return false;
    }

    *m_Data = *rhs->m_Data;

    return true;
}

template < class T >
bool SimpleStlVectorData<T>::Equals(Object* object)
{
    const SimpleStlVectorData<T>* rhs = SafeCast< SimpleStlVectorData<T> >(object);
    if (!rhs)
    {
        return false;
    }

    return *m_Data == *rhs->m_Data;
}

//
// Stl specializes std::vector<bool> using bitfields...
//  in general this isn't very efficient, but isn't used often
//

template < class T >
void WriteVector( const std::vector< T >& v, Reflect::CharStream& stream )
{
    int32_t count = (int32_t)v.size();
    stream.Write(&count); 

    if (count > 0)
    {
        stream.WriteBuffer( (const void*)&(v.front()), sizeof(T) * count); 
    }

}

template <>
void WriteVector( const std::vector< bool >& v, Reflect::CharStream& stream )
{
    int32_t count = (int32_t)v.size();
    stream.Write(&count); 

    for ( std::vector< bool >::const_iterator itr = v.begin(), end = v.end(); itr != end; ++itr )
    {
        bool value = *itr;
        stream.Write( &value ); 
    }

}

template < class T >
void ReadVector( std::vector< T >& v, Reflect::CharStream& stream )
{
    int32_t count = 0;
    stream.Read(&count); 
    v.resize(count);

    if(count > 0)
    {
        stream.ReadBuffer( (void*)&(v.front()), sizeof(T) * count );
    }
}

template <>
void ReadVector( std::vector< bool >& v, Reflect::CharStream& stream )
{
    int32_t count = 0;
    stream.Read(&count); 
    v.reserve(count);

    while( --count > 0 )
    {
        bool value;
        stream.Read( &value );
        v.push_back( value );
    }
}

template < class T >
void SimpleStlVectorData<T>::Serialize(ArchiveBinary& archive)
{
    WriteVector( *m_Data, archive.GetStream() );
}

template < class T >
void SimpleStlVectorData<T>::Deserialize(ArchiveBinary& archive)
{
    // if we are referring to a real field, clear its contents
    m_Data->clear();
    ReadVector( *m_Data, archive.GetStream() );
}

template < class T >
void SimpleStlVectorData<T>::Serialize(ArchiveXML& archive)
{
    archive.GetIndent().Push();

    // foreach datum
    for (size_t i=0; i<m_Data->size(); i++)
    {
        // indent
        archive.GetIndent().Get(archive.GetStream());

        // write
        archive.GetStream() << m_Data->at( i );

        // newline
        archive.GetStream() << "\n";
    }

    archive.GetIndent().Pop();
}

template < class T >
void SimpleStlVectorData<T>::Deserialize(ArchiveXML& archive)
{
    // if we are referring to a real field, clear its contents
    m_Data->clear();

    T value;
    archive.GetStream().SkipWhitespace(); 

    while (!archive.GetStream().Done())
    {
        // read data
        archive.GetStream() >> value;

        // copy onto vector
        m_Data->push_back(value);

        // read to next non-whitespace char
        archive.GetStream().SkipWhitespace(); 
    }
}

template < class T >
tostream& SimpleStlVectorData<T>::operator>>(tostream& stream) const
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

template < class T >
tistream& SimpleStlVectorData<T>::operator<<(tistream& stream)
{
    m_Data->clear();

    tstring str;
    std::streamsize size = stream.rdbuf()->in_avail();
    str.resize( (size_t) size );
    stream.read( const_cast< tchar_t* >( str.c_str() ), size );

    Tokenize<T, T>( str, *m_Data, s_ContainerItemDelimiter );

    return stream;
}

//
// Specializations
//

template <>
void StlStringStlVectorData::Serialize(ArchiveBinary& archive)
{
    uint32_t count = (uint32_t)m_Data->size();
    archive.GetStream().Write( &count ); 

    for (size_t i=0; i<m_Data->size(); i++)
    {
        archive.GetStream().WriteString( m_Data->at( i ) ); 
    }
}

template <>
void StlStringStlVectorData::Deserialize(ArchiveBinary& archive)
{
    m_Data->clear();

    uint32_t count = (uint32_t)m_Data->size();
    archive.GetStream().Read( &count ); 

    m_Data->resize(count);
    for ( uint32_t i=0; i<count; i++ )
    {
        archive.GetStream().ReadString( m_Data->at( i ) ); 
    }
}

template <>
void StlStringStlVectorData::Serialize(ArchiveXML& archive)
{
    archive.GetIndent().Push();
    archive.GetIndent().Get(archive.GetStream());

    // start our CDATA section, this prevents XML from parsing its escapes in this cdata section
    archive.GetStream() << TXT("<![CDATA[\n");

    for (size_t i=0; i<m_Data->size(); i++)
    {
        archive.GetIndent().Get(archive.GetStream());

        // output the escape-code free character sequence between double qutoes
        archive.GetStream() << TXT('\"') << m_Data->at( i ) << TXT('\"') << s_ContainerItemDelimiter;
    }

    // end our CDATA escape section
    archive.GetIndent().Get(archive.GetStream());
    archive.GetStream() << TXT("]]>\n");

    archive.GetIndent().Pop();
}

template <>
void StlStringStlVectorData::Deserialize(ArchiveXML& archive)
{
    m_Data->clear();

    archive.GetStream().SkipWhitespace(); 
    tstring value;

    while (!archive.GetStream().Done())
    {
        std::getline( archive.GetStream().GetInternal(), value ); 

        size_t start = value.find_first_of('\"');
        size_t end = value.find_last_of('\"');

        // if we found a pair of quotes
        if (start != std::string::npos && end != std::string::npos && start != end)
        {
            // if all we have are open/close quotes, push a blank string
            if (start == end-1)
            {
                m_Data->push_back(tstring ());
            }
            // else we have some non-null string data
            else
            {
                m_Data->push_back(value.substr(start + 1, end - start - 1));
            }
        }
        else
        {
            start = value.find_first_not_of( TXT( " \t\n" ) );

            if (start != std::string::npos)
                m_Data->push_back(value.substr(start));
        }

        archive.GetStream().SkipWhitespace(); 
    }
}

#ifdef UNICODE

//
// When unicode is active the XML streams are made of wchar_t, and C++ stdlib won't do the conversion for uint8_t/int8_t
//  So we explicitly specialize some functions to to the conversion via a uint16_t/int16_t
//

template <>
tistream& SimpleStlVectorData<uint8_t>::operator<<(tistream& stream)
{
    m_Data->clear();

    tstring str;
    std::streamsize size = stream.rdbuf()->in_avail();
    str.resize( (size_t) size );
    stream.read(const_cast< tchar_t* >( str.c_str() ), size );

    Tokenize<uint8_t, uint16_t>( str, *m_Data, s_ContainerItemDelimiter );

    return stream;
}

template <>
tistream& SimpleStlVectorData<int8_t>::operator<<(tistream& stream)
{
    m_Data->clear();

    tstring str;
    std::streamsize size = stream.rdbuf()->in_avail();
    str.resize( (size_t) size );
    stream.read(const_cast< tchar_t* >( str.c_str() ), size );

    Tokenize<int8_t, int16_t>( str, *m_Data, s_ContainerItemDelimiter );

    return stream;
}

#endif // UNICODE

template SimpleStlVectorData<tstring>;
template SimpleStlVectorData<bool>;
template SimpleStlVectorData<uint8_t>;
template SimpleStlVectorData<int8_t>;
template SimpleStlVectorData<uint16_t>;
template SimpleStlVectorData<int16_t>;
template SimpleStlVectorData<uint32_t>;
template SimpleStlVectorData<int32_t>;
template SimpleStlVectorData<uint64_t>;
template SimpleStlVectorData<int64_t>;
template SimpleStlVectorData<float32_t>;
template SimpleStlVectorData<float64_t>;
template SimpleStlVectorData<Helium::GUID>;
template SimpleStlVectorData<Helium::TUID>;
template SimpleStlVectorData< Helium::Path >;

template SimpleStlVectorData<Vector2>;
template SimpleStlVectorData<Vector3>;
template SimpleStlVectorData<Vector4>;
template SimpleStlVectorData<Matrix3>;
template SimpleStlVectorData<Matrix4>;

template SimpleStlVectorData<Color3>;
template SimpleStlVectorData<Color4>;
template SimpleStlVectorData<HDRColor3>;
template SimpleStlVectorData<HDRColor4>;

REFLECT_DEFINE_OBJECT( Helium::Reflect::StlStringStlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::BoolStlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt8StlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Int8StlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt16StlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Int16StlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt32StlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Int32StlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt64StlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Int64StlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Float32StlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Float64StlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::GUIDStlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::TUIDStlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::PathStlVectorData  );

REFLECT_DEFINE_OBJECT( Helium::Reflect::Vector2StlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Vector3StlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Vector4StlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Matrix3StlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Matrix4StlVectorData );

REFLECT_DEFINE_OBJECT( Helium::Reflect::Color3StlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Color4StlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::HDRColor3StlVectorData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::HDRColor4StlVectorData );
