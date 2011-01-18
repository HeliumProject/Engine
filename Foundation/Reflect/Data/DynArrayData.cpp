#include "Foundation/Reflect/Data/DynArrayData.h"

#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"
#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_ABSTRACT( DynArrayData )

// If you use this and your data can have spaces, you must make an explicit
// instantiation.  See the the string example below.
// 
// String tokenizer adapted from:
// http://www.oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
template< typename T, typename I >
void Tokenize( const tstring& str, DynArray< T >& tokens, const tstring& delimiters )
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
            tokens.Push( static_cast< T >( temp ) );
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
inline void Tokenize< tstring, tstring >( const tstring& str, DynArray< tstring >& tokens, const tstring& delimiters )
{
    // Skip delimiters at beginning.
    tstring::size_type lastPos = str.find_first_not_of( delimiters, 0 );
    // Find first "non-delimiter".
    tstring::size_type pos     = str.find_first_of( delimiters, lastPos );

    while ( tstring::npos != pos || tstring::npos != lastPos )
    {
        // Add the token to the vector
        tokens.Push( str.substr( lastPos, pos - lastPos ) );
        // Skip delimiters.  Note the "not_of"
        lastPos = str.find_first_not_of( delimiters, pos );
        // Find next "non-delimiter"
        pos = str.find_first_of( delimiters, lastPos );
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
void SimpleDynArrayData< T >::ConnectData(Helium::HybridPtr<void> data)
{
    m_Data.Connect( Helium::HybridPtr<DataType> (data.Address(), data.State()) );
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
ConstDataPtr SimpleDynArrayData< T >::GetItem( size_t at ) const
{
    return Data::Bind( m_Data->GetElement( at ), m_Instance, m_Field );
}

template< class T >
void SimpleDynArrayData< T >::SetItem( size_t at, const Data* value )
{
    Data::GetValue(value, m_Data->GetElement( at ) );
}

template< class T >
void SimpleDynArrayData< T >::Insert( size_t at, const Data* value )
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
bool SimpleDynArrayData< T >::Set( const Data* src, uint32_t flags )
{
    const SimpleDynArrayData< T >* rhs = SafeCast< SimpleDynArrayData< T > >( src );
    if (!rhs)
    {
        return false;
    }

    m_Data.Set( rhs->m_Data.Get() );

    return true;
}

template< class T >
bool SimpleDynArrayData< T >::Equals( const Object* object ) const
{
    const SimpleDynArrayData< T >* rhs = SafeCast< SimpleDynArrayData< T > >( object );
    if (!rhs)
    {
        return false;
    }

    return m_Data.Get() == rhs->m_Data.Get();
}

template< class T >
void SimpleDynArrayData< T >::Serialize( Archive& archive ) const
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            xml.GetIndent().Push();

            // foreach datum
            for (size_t i=0; i<m_Data->GetSize(); i++)
            {
                // indent
                xml.GetIndent().Get(xml.GetStream());

                // write
                xml.GetStream() << m_Data.Get()[i];

                // newline
                xml.GetStream() << "\n";
            }

            xml.GetIndent().Pop();
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary( static_cast< ArchiveBinary& >( archive ) );
            Reflect::CharStream& stream = binary.GetStream();

            size_t countActual = m_Data->GetSize();
            HELIUM_ASSERT( countActual <= UINT32_MAX );
            uint32_t count = static_cast< uint32_t >( countActual );
            stream.Write( &count );

            for( size_t index = 0; index < countActual; ++index )
            {
#pragma TODO( "Fix support for writing out non-POD types (i.e. strings)" )
                stream.Write( &m_Data->GetElement( index ) );
            }

            break;
        }
    }
}

template< class T >
void SimpleDynArrayData< T >::Deserialize( Archive& archive )
{
    // if we are referring to a real field, clear its contents
    m_Data->Clear();

    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            T value;
            xml.GetStream().SkipWhitespace(); 

            while (!xml.GetStream().Done())
            {
                // read data
                xml.GetStream() >> value;

                // copy onto vector
                m_Data->Push(value);

                // read to next non-whitespace char
                xml.GetStream().SkipWhitespace(); 
            }
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary( static_cast< ArchiveBinary& >( archive ) );
            Reflect::CharStream& stream = binary.GetStream();

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

            break;
        }
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

    tstring str;
    std::streamsize size = stream.rdbuf()->in_avail();
    str.resize( static_cast< size_t >( size ) );
    stream.read( const_cast< tchar_t* >( str.c_str() ), size );

    Tokenize< T, T >( str, m_Data.Ref(), s_ContainerItemDelimiter );

    return stream;
}

//
// Specializations
//

// keep reading the string until we run out of buffer
template<>
void StringDynArrayData::Serialize( Archive& archive ) const
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            xml.GetIndent().Push();
            xml.GetIndent().Get(xml.GetStream());

            // start our CDATA section, this prevents XML from parsing its escapes in this cdata section
            xml.GetStream() << TXT("<![CDATA[\n");

            for (size_t i=0; i<m_Data->GetSize(); i++)
            {
                xml.GetIndent().Get(xml.GetStream());

                // output the escape-code free character sequence between double qutoes
                xml.GetStream() << TXT('\"') << *m_Data.Get()[i] << TXT('\"') << s_ContainerItemDelimiter;
            }

            // end our CDATA escape section
            xml.GetIndent().Get(xml.GetStream());
            xml.GetStream() << TXT("]]>\n");

            xml.GetIndent().Pop();
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary( static_cast< ArchiveBinary& >( archive ) );
            CharStream& stream = binary.GetStream();

            size_t countActual = m_Data->GetSize();
            HELIUM_ASSERT( countActual <= UINT32_MAX );
            uint32_t count = static_cast< uint32_t >( countActual );
            stream.Write( &count );

            for( size_t index = 0; index < countActual; ++index )
            {
                stream.WriteString( m_Data->GetElement( index ) );
            }

            break;
        }
    }
}

// must escape strings to account for special "evil" characters... like ", &, `, etc...
template<>
void StringDynArrayData::Deserialize( Archive& archive )
{
    m_Data->Clear();

    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            xml.GetStream().SkipWhitespace(); 
            tstring value;

            while (!xml.GetStream().Done())
            {
                std::getline( xml.GetStream().GetInternal(), value ); 

                size_t start = value.find_first_of('\"');
                size_t end = value.find_last_of('\"');

                // if we found a pair of quotes
                if (start != std::string::npos && end != std::string::npos && start != end)
                {
                    // if all we have are open/close quotes, push a blank string
                    String* string = m_Data->New();
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
                        HELIUM_VERIFY( m_Data->New( value.substr(start).c_str() ) );
                    }
                }

                xml.GetStream().SkipWhitespace(); 
            }
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary( static_cast< ArchiveBinary& >( archive ) );
            CharStream& stream = binary.GetStream();

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

            break;
        }
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

    tstring str;
    std::streamsize size = stream.rdbuf()->in_avail();
    str.resize( static_cast< size_t >( size ) );
    stream.read( const_cast< tchar_t* >( str.c_str() ), size );

    Tokenize< uint8_t, uint16_t >( str, m_Data.Ref(), s_ContainerItemDelimiter );

    return stream;
}

template<>
tistream& SimpleDynArrayData< int8_t >::operator<<( tistream& stream )
{
    m_Data->Clear();

    tstring str;
    std::streamsize size = stream.rdbuf()->in_avail();
    str.resize( static_cast< size_t >( size ) );
    stream.read( const_cast< tchar_t* >( str.c_str() ), size );

    Tokenize< int8_t, int16_t >( str, m_Data.Ref(), s_ContainerItemDelimiter );

    return stream;
}
#endif // UNICODE

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

template SimpleDynArrayData< Vector2 >;
template SimpleDynArrayData< Vector3 >;
template SimpleDynArrayData< Vector4 >;
template SimpleDynArrayData< Matrix3 >;
template SimpleDynArrayData< Matrix4 >;

template SimpleDynArrayData< Color3 >;
template SimpleDynArrayData< Color4 >;
template SimpleDynArrayData< HDRColor3 >;
template SimpleDynArrayData< HDRColor4 >;

REFLECT_DEFINE_OBJECT( StringDynArrayData );
REFLECT_DEFINE_OBJECT( BoolDynArrayData );
REFLECT_DEFINE_OBJECT( UInt8DynArrayData );
REFLECT_DEFINE_OBJECT( Int8DynArrayData );
REFLECT_DEFINE_OBJECT( UInt16DynArrayData );
REFLECT_DEFINE_OBJECT( Int16DynArrayData );
REFLECT_DEFINE_OBJECT( UInt32DynArrayData );
REFLECT_DEFINE_OBJECT( Int32DynArrayData );
REFLECT_DEFINE_OBJECT( UInt64DynArrayData );
REFLECT_DEFINE_OBJECT( Int64DynArrayData );
REFLECT_DEFINE_OBJECT( Float32DynArrayData );
REFLECT_DEFINE_OBJECT( Float64DynArrayData );
REFLECT_DEFINE_OBJECT( GUIDDynArrayData );
REFLECT_DEFINE_OBJECT( TUIDDynArrayData );
REFLECT_DEFINE_OBJECT( PathDynArrayData );

REFLECT_DEFINE_OBJECT( Vector2DynArrayData );
REFLECT_DEFINE_OBJECT( Vector3DynArrayData );
REFLECT_DEFINE_OBJECT( Vector4DynArrayData );
REFLECT_DEFINE_OBJECT( Matrix3DynArrayData );
REFLECT_DEFINE_OBJECT( Matrix4DynArrayData );

REFLECT_DEFINE_OBJECT( Color3DynArrayData );
REFLECT_DEFINE_OBJECT( Color4DynArrayData );
REFLECT_DEFINE_OBJECT( HDRColor3DynArrayData );
REFLECT_DEFINE_OBJECT( HDRColor4DynArrayData );
