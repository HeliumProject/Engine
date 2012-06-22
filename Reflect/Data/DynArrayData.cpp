#include "ReflectPch.h"
#include "Reflect/Data/DynArrayData.h"

#include "Reflect/Data/DataDeduction.h"
#include "Reflect/ArchiveBinary.h"
#include "Reflect/ArchiveXML.h"

REFLECT_DEFINE_ABSTRACT( Helium::Reflect::DynArrayData )

using namespace Helium;
using namespace Helium::Reflect;

// If you use this and your data can have spaces, you must make an explicit
// instantiation.  See the the string example below.
// 
// String tokenizer adapted from:
// http://www.oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
template< typename T, typename I >
void Tokenize( const String& str, DynArray< T >& tokens, const tchar_t* delimiters )
{
    // Skip delimiters at beginning.
    size_t lastPos = str.FindNone( delimiters );
    // Find first "non-delimiter".
    size_t pos     = str.FindAny( delimiters, lastPos );

    I temp;
    while ( IsValid( pos ) || IsValid( lastPos ) )
    {
        // Found a token, convert it to the proper type for our vector
        tstringstream stream( *str.Substring( lastPos, pos - lastPos ) );
        stream >> temp; // NOTE: Stream operator stops at spaces!
        if ( !stream.fail() )
        {
            // Add the token to the vector
            tokens.Push( static_cast< T >( temp ) );
        }
        else
        {
            HELIUM_BREAK();
        }
        // Skip delimiters.  Note the "not_of"
        lastPos = str.FindNone( delimiters, pos );
        // Find next "non-delimiter"
        pos = str.FindAny( delimiters, lastPos );
    }
}

// Explicit implementation for strings, that gets around the stream operator stopping
// at spaces by not using a stream at all.
template<>
inline void Tokenize< String, String >( const String& str, DynArray< String >& tokens, const tchar_t* delimiters )
{
    // Skip delimiters at beginning.
    size_t lastPos = str.FindNone( delimiters );
    // Find first "non-delimiter".
    size_t pos     = str.FindAny( delimiters, lastPos );

    while ( IsValid( pos ) || IsValid( lastPos ) )
    {
        // Add the token to the vector
        String* element = tokens.New();
        HELIUM_ASSERT( element );
        str.Substring( *element, lastPos, pos - lastPos );
        // Skip delimiters.  Note the "not_of"
        lastPos = str.FindNone( delimiters, pos );
        // Find next "non-delimiter"
        pos = str.FindAny( delimiters, lastPos );
    }
}

template< class T >
SimpleDynArrayData< T >::SimpleDynArrayData()
{

}

template< class T >
SimpleDynArrayData< T >::~SimpleDynArrayData()
{

}

template< class T >
void SimpleDynArrayData< T >::ConnectData(void* data)
{
    m_Data.Connect( data );
}

template< class T >
size_t SimpleDynArrayData< T >::GetSize() const
{ 
    return m_Data->GetSize(); 
}

template< class T >
void SimpleDynArrayData< T >::SetSize( size_t size )
{
    return m_Data->Resize( size );
}

template< class T >
void SimpleDynArrayData< T >::Clear()
{
    return m_Data->Clear();
}

template< class T >
const Class* SimpleDynArrayData< T >::GetItemClass() const
{
    return Reflect::GetDataClass< T >();
}

template< class T >
DataPtr SimpleDynArrayData< T >::GetItem( size_t at )
{
    return Data::Bind( m_Data->GetElement( at ), m_Instance, m_Field );
}

template< class T >
void SimpleDynArrayData< T >::SetItem( size_t at, Data* value )
{
    Data::GetValue( value, m_Data->GetElement( at ) );
}

template< class T >
void SimpleDynArrayData< T >::Insert( size_t at, Data* value )
{
    T temp;
    Data::GetValue( value, temp );
    m_Data->Insert( at, temp );
}

template< class T >
void SimpleDynArrayData< T >::Remove( size_t at )
{
    m_Data->Remove( at );
}

template< class T >
void SimpleDynArrayData< T >::MoveUp( std::set< size_t >& selectedIndices )
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

        Swap( m_Data->GetElement( *itr - 1 ), m_Data->GetElement( *itr ) );

        newSelectedIndices.insert( *itr - 1 );
    }

    selectedIndices = newSelectedIndices;
}

template< class T >
void SimpleDynArrayData< T >::MoveDown( std::set< size_t >& selectedIndices )
{
    std::set< size_t > newSelectedIndices;

    std::set< size_t >::const_reverse_iterator itr = selectedIndices.rbegin();
    std::set< size_t >::const_reverse_iterator end = selectedIndices.rend();

    for( ; itr != end; ++itr )
    {
        if ( ( (*itr) == m_Data->GetSize() - 1 ) || ( newSelectedIndices.find( (*itr) + 1 ) != newSelectedIndices.end() ) )
        {
            newSelectedIndices.insert( *itr );
            continue;
        }

        Swap( m_Data->GetElement( *itr + 1 ), m_Data->GetElement( *itr ) );

        newSelectedIndices.insert( *itr + 1 );
    }

    selectedIndices = newSelectedIndices;
}

template< class T >
bool SimpleDynArrayData< T >::Set( Data* src, uint32_t flags )
{
    const SimpleDynArrayData< T >* rhs = SafeCast< SimpleDynArrayData< T > >( src );
    if (!rhs)
    {
        return false;
    }

    *m_Data = *rhs->m_Data;

    return true;
}

template< class T >
bool SimpleDynArrayData< T >::Equals( Object* object )
{
    const SimpleDynArrayData< T >* rhs = SafeCast< SimpleDynArrayData< T > >( object );
    if (!rhs)
    {
        return false;
    }

    return *m_Data == *rhs->m_Data;
}

template< class T >
void SimpleDynArrayData< T >::Serialize( ArchiveBinary& archive )
{
    Reflect::CharStream& stream = archive.GetStream();

    size_t countActual = m_Data->GetSize();
    HELIUM_ASSERT( countActual <= UINT32_MAX );
    uint32_t count = static_cast< uint32_t >( countActual );
    stream.Write( &count );

    for( size_t index = 0; index < countActual; ++index )
    {
        stream.Write( &m_Data->GetElement( index ) );
    }
}

template< class T >
void SimpleDynArrayData< T >::Deserialize( ArchiveBinary& archive )
{
    // if we are referring to a real field, clear its contents
    m_Data->Clear();

    Reflect::CharStream& stream = archive.GetStream();

    uint32_t count = 0;
    stream.Read( &count );

    m_Data->Reserve( count );

    uint_fast32_t countFast = count;
    for( uint_fast32_t index = 0; index < countFast; ++index )
    {
        T* pElement = m_Data->New();
        HELIUM_ASSERT( pElement );
        stream.Read( pElement );
    }
}

template< class T >
void SimpleDynArrayData< T >::Serialize( ArchiveXML& archive )
{
    archive.GetIndent().Push();

    // foreach datum
    for (size_t i=0; i<m_Data->GetSize(); i++)
    {
        // indent
        archive.GetIndent().Get(archive.GetStream());

        // write
        archive.GetStream() << (*m_Data)[i];

        // newline
        archive.GetStream() << "\n";
    }

    archive.GetIndent().Pop();
}

template< class T >
void SimpleDynArrayData< T >::Deserialize( ArchiveXML& archive )
{
    // if we are referring to a real field, clear its contents
    m_Data->Clear();

    T value;
    archive.GetStream().SkipWhitespace(); 

    while (!archive.GetStream().Done())
    {
        // read data
        archive.GetStream() >> value;

        // copy onto vector
        m_Data->Push(value);

        // read to next non-whitespace char
        archive.GetStream().SkipWhitespace(); 
    }
}

template< class T >
tostream& SimpleDynArrayData< T >::operator>>( tostream& stream ) const
{
    size_t elementCount = m_Data->GetSize();

    if( elementCount != 0 )
    {
        stream << m_Data->GetElement( 0 );

        for( size_t index = 1; index < elementCount; ++index )
        {
            stream << s_ContainerItemDelimiter;
            stream << m_Data->GetElement( index );
        }
    }

    return stream;
}

template< class T >
tistream& SimpleDynArrayData< T >::operator<<( tistream& stream )
{
    m_Data->Clear();

    String str;
    std::streamsize size = stream.rdbuf()->in_avail();
    str.Reserve( static_cast< size_t >( size ) );
    str.Resize( static_cast< size_t >( size ) );
    stream.read( &str[ 0 ], size );

    Tokenize< T, T >( str, *m_Data, s_ContainerItemDelimiter );

    return stream;
}

//
// Specializations
//

void SerializeStringDynArray( ArchiveXML& archive, DynArray<String> data )
{
    archive.GetIndent().Push();
    archive.GetIndent().Get(archive.GetStream());

    // start our CDATA section, this prevents XML from parsing its escapes in this cdata section
    archive.GetStream() << TXT("<![CDATA[\n");

    for (size_t i=0; i<data.GetSize(); i++)
    {
        archive.GetIndent().Get(archive.GetStream());

        // output the escape-code free character sequence between double qutoes
        archive.GetStream() << TXT('\"') << (data)[i] << TXT('\"') << s_ContainerItemDelimiter;
    }

    // end our CDATA escape section
    archive.GetIndent().Get(archive.GetStream());
    archive.GetStream() << TXT("]]>\n");
    archive.GetIndent().Pop();
}

void DeserializeStringDynArray( ArchiveXML& archive, DynArray<String> data )
{
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
            String* string = data.New();
            HELIUM_ASSERT( string );
            if (start != end-1)
            {
                *string = value.substr( start + 1, end - start - 1).c_str();
            }
        }
        else
        {
            start = value.find_first_not_of( TXT( " \t\n" ) );

            if ( start != std::string::npos )
            {
                HELIUM_VERIFY( data.New( value.substr(start).c_str() ) );
            }
        }

        archive.GetStream().SkipWhitespace(); 
    }
}

template<>
void StringDynArrayData::Serialize( ArchiveBinary& archive )
{
    CharStream& stream = archive.GetStream();

    size_t countActual = m_Data->GetSize();
    HELIUM_ASSERT( countActual <= UINT32_MAX );
    uint32_t count = static_cast< uint32_t >( countActual );
    stream.Write( &count );

    for( size_t index = 0; index < countActual; ++index )
    {
        stream.WriteString( m_Data->GetElement( index ) );
    }
}

template<>
void StringDynArrayData::Deserialize( ArchiveBinary& archive )
{
    m_Data->Clear();

    CharStream& stream = archive.GetStream();

    uint32_t count = 0;
    stream.Read( &count );

    m_Data->Reserve( count );

    uint_fast32_t countFast = count;
    for( uint32_t index = 0; index < countFast; index++ )
    {
        String* element = m_Data->New();
        HELIUM_ASSERT( element );
        stream.ReadString( *element ); 
    }
}

template<>
void StringDynArrayData::Serialize( ArchiveXML& archive )
{
    SerializeStringDynArray(archive, *m_Data);
}

template<>
void StringDynArrayData::Deserialize( ArchiveXML& archive )
{
    DeserializeStringDynArray(archive, *m_Data);
}

template<>
void NameDynArrayData::Serialize( ArchiveBinary& archive )
{
    CharStream& stream = archive.GetStream();

    size_t countActual = m_Data->GetSize();
    HELIUM_ASSERT( countActual <= UINT32_MAX );
    uint32_t count = static_cast< uint32_t >( countActual );
    stream.Write( &count );

    for( size_t index = 0; index < countActual; ++index )
    {
        stream.WriteString( *m_Data->GetElement( index ) );
    }
}

template<>
void NameDynArrayData::Deserialize( ArchiveBinary& archive )
{
    m_Data->Clear();

    CharStream& stream = archive.GetStream();

    uint32_t count = 0;
    stream.Read( &count );

    m_Data->Reserve( count );

    uint_fast32_t countFast = count;
    for( uint32_t index = 0; index < countFast; index++ )
    {
        Name* element = m_Data->New();
        HELIUM_ASSERT( element );
        String name_as_str;
        stream.ReadString(name_as_str);
        element->Set(name_as_str);
    }
}

template<>
void NameDynArrayData::Serialize( ArchiveXML& archive )
{
    DynArray<String> names_as_str;
    names_as_str.Resize(m_Data->GetSize());

    for( size_t index = 0; index < m_Data->GetSize(); ++index )
    {
        names_as_str[index] = *m_Data->GetElement( index );
    }

    SerializeStringDynArray(archive, names_as_str);
}

template<>
void NameDynArrayData::Deserialize( ArchiveXML& archive )
{
    DynArray<String> names_as_str;
    DeserializeStringDynArray(archive, names_as_str);

    m_Data->Resize(names_as_str.GetSize());
    
    for ( size_t index = 0; index < m_Data->GetSize(); ++index )
    {
        m_Data->GetElement(index).Set(names_as_str.GetElement(index));
    }
}


#if HELIUM_UNICODE

//
// When unicode is active the XML streams are made of wchar_t, and C++ stdlib won't do the conversion for uint8_t/int8_t
//  So we explicitly specialize some functions to to the conversion via a uint16_t/int16_t
//

template<>
tistream& SimpleDynArrayData< uint8_t >::operator<<( tistream& stream )
{
    m_Data->Clear();

    String str;
    std::streamsize size = stream.rdbuf()->in_avail();
    str.Reserve( static_cast< size_t >( size ) );
    str.Resize( static_cast< size_t >( size ) );
    stream.read( &str[ 0 ], size );

    Tokenize< uint8_t, uint16_t >( str, *m_Data, s_ContainerItemDelimiter );

    return stream;
}

template<>
tistream& SimpleDynArrayData< int8_t >::operator<<( tistream& stream )
{
    m_Data->Clear();

    String str;
    std::streamsize size = stream.rdbuf()->in_avail();
    str.Reserve( static_cast< size_t >( size ) );
    str.Resize( static_cast< size_t >( size ) );
    stream.read( &str[ 0 ], size );

    Tokenize< int8_t, int16_t >( str, *m_Data, s_ContainerItemDelimiter );

    return stream;
}

#endif // UNICODE

template SimpleDynArrayData< Name >;
template SimpleDynArrayData< String >;
template SimpleDynArrayData< bool >;
template SimpleDynArrayData< uint8_t >;
template SimpleDynArrayData< int8_t >;
template SimpleDynArrayData< uint16_t >;
template SimpleDynArrayData< int16_t >;
template SimpleDynArrayData< uint32_t >;
template SimpleDynArrayData< int32_t >;
template SimpleDynArrayData< uint64_t >;
template SimpleDynArrayData< int64_t >;
template SimpleDynArrayData< float32_t >;
template SimpleDynArrayData< float64_t >;
template SimpleDynArrayData< Helium::GUID >;
template SimpleDynArrayData< Helium::TUID >;
template SimpleDynArrayData< Helium::Path >;

REFLECT_DEFINE_OBJECT( Helium::Reflect::NameDynArrayData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::StringDynArrayData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::BoolDynArrayData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt8DynArrayData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Int8DynArrayData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt16DynArrayData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Int16DynArrayData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt32DynArrayData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Int32DynArrayData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::UInt64DynArrayData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Int64DynArrayData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Float32DynArrayData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::Float64DynArrayData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::GUIDDynArrayData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::TUIDDynArrayData );
REFLECT_DEFINE_OBJECT( Helium::Reflect::PathDynArrayData );