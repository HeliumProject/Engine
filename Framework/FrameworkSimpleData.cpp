#include "FrameworkPch.h"
#include "FrameworkSimpleData.h"

#include "Reflect/ArchiveBinary.h"
#include "Reflect/ArchiveXML.h"

using namespace Helium;
using namespace Helium::Framework;
using namespace Helium::Reflect;

template <class T>
FrameworkSimpleData<T>::FrameworkSimpleData()
{

}

template <class T>
FrameworkSimpleData<T>::~FrameworkSimpleData()
{

}

template <class T>
void FrameworkSimpleData<T>::ConnectData(void* data)
{
    m_Data.Connect( data );
}

template <class T>
bool FrameworkSimpleData<T>::Set(Data* src, uint32_t flags)
{
    const SimpleDataT* rhs = SafeCast<SimpleDataT>(src);
    if (!rhs)
    {
        return false;
    }

    *m_Data = *rhs->m_Data;

    return true;
}

template <class T>
bool FrameworkSimpleData<T>::Equals(Object* object)
{
    const SimpleDataT* rhs = SafeCast<SimpleDataT>(object);
    if (!rhs)
    {
        return false;
    }

    return *rhs->m_Data == *m_Data;
}

template <class T>
void FrameworkSimpleData<T>::Serialize(ArchiveBinary& archive)
{
    const T* data = m_Data;
    archive.GetStream().Write( data );
}

template <class T>
void FrameworkSimpleData<T>::Deserialize(ArchiveBinary& archive)
{
    T* data = m_Data;
    archive.GetStream().Read( data );
}

template <class T>
void FrameworkSimpleData<T>::Serialize(ArchiveXML& archive)
{
    archive.GetStream() << *m_Data;
}

template <class T>
void FrameworkSimpleData<T>::Deserialize(ArchiveXML& archive)
{
    archive.GetStream() >> *m_Data;
}

template <class T>
tostream& FrameworkSimpleData<T>::operator>>(tostream& stream) const
{
    stream << *m_Data;

    return stream;
}

template <class T>
tistream& FrameworkSimpleData<T>::operator<<(tistream& stream)
{
    stream >> *m_Data;

    if ( m_Instance && m_Field && m_Field->m_Composite->GetReflectionType() == ReflectionTypes::Class )
    {
        Object* object = static_cast< Object* >( m_Instance );
        object->RaiseChanged( m_Field );
    }

    return stream;
}


template FrameworkSimpleData<Vector2>;
template FrameworkSimpleData<Vector3>;
template FrameworkSimpleData<Vector4>;

template FrameworkSimpleData<Matrix3>;
template FrameworkSimpleData<Matrix4>;

template FrameworkSimpleData<Color3>;
template FrameworkSimpleData<Color4>;

template FrameworkSimpleData<HDRColor3>;
template FrameworkSimpleData<HDRColor4>;


REFLECT_DEFINE_OBJECT( Helium::Framework::Vector2Data );
REFLECT_DEFINE_OBJECT( Helium::Framework::Vector3Data );
REFLECT_DEFINE_OBJECT( Helium::Framework::Vector4Data );

REFLECT_DEFINE_OBJECT( Helium::Framework::Matrix3Data );
REFLECT_DEFINE_OBJECT( Helium::Framework::Matrix4Data );

REFLECT_DEFINE_OBJECT( Helium::Framework::Color3Data );
REFLECT_DEFINE_OBJECT( Helium::Framework::Color4Data );

REFLECT_DEFINE_OBJECT( Helium::Framework::HDRColor3Data );
REFLECT_DEFINE_OBJECT( Helium::Framework::HDRColor4Data );



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
FrameworkSimpleStlVectorData<T>::FrameworkSimpleStlVectorData()
{

}

template < class T >
FrameworkSimpleStlVectorData<T>::~FrameworkSimpleStlVectorData()
{

}

template < class T >
void FrameworkSimpleStlVectorData<T>::ConnectData(void* data)
{
    m_Data.Connect( data );
}

template < class T >
size_t FrameworkSimpleStlVectorData<T>::GetSize() const
{
    return m_Data->size();
}

template < class T >
void FrameworkSimpleStlVectorData<T>::SetSize(size_t size)
{
    return m_Data->resize(size);
}

template < class T >
void FrameworkSimpleStlVectorData<T>::Clear()
{
    return m_Data->clear();
}

template < class T >
const Class* FrameworkSimpleStlVectorData<T>::GetItemClass() const
{
    return Reflect::GetDataClass<T>();
}

template < class T >
DataPtr FrameworkSimpleStlVectorData<T>::GetItem(size_t at)
{
    return Data::Bind(m_Data->at(at), m_Instance, m_Field);
}

template < class T >
void FrameworkSimpleStlVectorData<T>::SetItem(size_t at, Data* value)
{
    Data::GetValue(value, m_Data->at(at));
}

template < class T >
void FrameworkSimpleStlVectorData<T>::Insert( size_t at, Data* value )
{
    T temp;
    Data::GetValue( value, temp );
    m_Data->insert( m_Data->begin() + at, temp );
}

template < class T >
void FrameworkSimpleStlVectorData<T>::Remove( size_t at )
{
    m_Data->erase( m_Data->begin() + at );
}

template < class T >
void FrameworkSimpleStlVectorData<T>::MoveUp( std::set< size_t >& selectedIndices )
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
void FrameworkSimpleStlVectorData<T>::MoveDown( std::set< size_t >& selectedIndices )
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
bool FrameworkSimpleStlVectorData<T>::Set(Data* src, uint32_t flags)
{
    const FrameworkSimpleStlVectorData<T>* rhs = SafeCast<FrameworkSimpleStlVectorData<T>>(src);
    if (!rhs)
    {
        return false;
    }

    *m_Data = *rhs->m_Data;

    return true;
}

template < class T >
bool FrameworkSimpleStlVectorData<T>::Equals(Object* object)
{
    const FrameworkSimpleStlVectorData<T>* rhs = SafeCast< FrameworkSimpleStlVectorData<T> >(object);
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

// Wrapping in anonymous namespace so that we don't collide with the similarly named functions in SimpleData.cpp
// All this code is temporary until we have a better solution, so at some point this garbage will all go away
namespace
{
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
} // End anonymous namespace

template < class T >
void FrameworkSimpleStlVectorData<T>::Serialize(ArchiveBinary& archive)
{
    WriteVector( *m_Data, archive.GetStream() );
}

template < class T >
void FrameworkSimpleStlVectorData<T>::Deserialize(ArchiveBinary& archive)
{
    // if we are referring to a real field, clear its contents
    m_Data->clear();
    ReadVector( *m_Data, archive.GetStream() );
}

template < class T >
void FrameworkSimpleStlVectorData<T>::Serialize(ArchiveXML& archive)
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
void FrameworkSimpleStlVectorData<T>::Deserialize(ArchiveXML& archive)
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
tostream& FrameworkSimpleStlVectorData<T>::operator>>(tostream& stream) const
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
tistream& FrameworkSimpleStlVectorData<T>::operator<<(tistream& stream)
{
    m_Data->clear();

    tstring str;
    std::streamsize size = stream.rdbuf()->in_avail();
    str.resize( (size_t) size );
    stream.read( const_cast< tchar_t* >( str.c_str() ), size );

    Tokenize<T, T>( str, *m_Data, s_ContainerItemDelimiter );

    return stream;
}


template FrameworkSimpleStlVectorData<Vector2>;
template FrameworkSimpleStlVectorData<Vector3>;
template FrameworkSimpleStlVectorData<Vector4>;

template FrameworkSimpleStlVectorData<Matrix3>;
template FrameworkSimpleStlVectorData<Matrix4>;

template FrameworkSimpleStlVectorData<Color3>;
template FrameworkSimpleStlVectorData<Color4>;

template FrameworkSimpleStlVectorData<HDRColor3>;
template FrameworkSimpleStlVectorData<HDRColor4>;

REFLECT_DEFINE_OBJECT( Vector2StlVectorData );
REFLECT_DEFINE_OBJECT( Vector3StlVectorData );
REFLECT_DEFINE_OBJECT( Vector4StlVectorData );

REFLECT_DEFINE_OBJECT( Matrix3StlVectorData );
REFLECT_DEFINE_OBJECT( Matrix4StlVectorData );

REFLECT_DEFINE_OBJECT( Color3StlVectorData );
REFLECT_DEFINE_OBJECT( Color4StlVectorData );

REFLECT_DEFINE_OBJECT( HDRColor3StlVectorData );
REFLECT_DEFINE_OBJECT( HDRColor4StlVectorData );