#include "ArraySerializer.h"
#include "CompressionUtilities.h" 
#include "ArchiveBinary.h"
#include "ArchiveXML.h"

using namespace Reflect;

REFLECT_DEFINE_ABSTRACT(ArraySerializer)

// If you use this and your data can have spaces, you must make an explicit
// instantiation.  See the the string example below.
// 
// String tokenizer adapted from:
// http://www.oopweb.com/CPP/Documents/CPPHOWTO/Volume/C++Programming-HOWTO-7.html
template< typename T >
void Tokenize( const std::string& str, std::vector< T >& tokens, const std::string& delimiters )
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of( delimiters, 0 );
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of( delimiters, lastPos );

    T temp;
    while ( std::string::npos != pos || std::string::npos != lastPos )
    {
        // Found a token, convert it to the proper type for our vector
        std::stringstream stream (str.substr( lastPos, pos - lastPos ));
        stream >> temp; // NOTE: Stream operator stops at spaces!
        if ( !stream.fail() )
        {
            // Add the token to the vector
            tokens.push_back( temp );
        }
        else
        {
            NOC_BREAK();
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
inline void Tokenize( const std::string& str, std::vector< std::string >& tokens, const std::string& delimiters )
{
    // Skip delimiters at beginning.
    std::string::size_type lastPos = str.find_first_not_of( delimiters, 0 );
    // Find first "non-delimiter".
    std::string::size_type pos     = str.find_first_of( delimiters, lastPos );

    while ( std::string::npos != pos || std::string::npos != lastPos )
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
SimpleArraySerializer<T>::SimpleArraySerializer()
{

}

template < class T >
SimpleArraySerializer<T>::~SimpleArraySerializer()
{

}

template < class T >
void SimpleArraySerializer<T>::ConnectData(Nocturnal::HybridPtr<void> data)
{
    __super::ConnectData( data );

    m_Data.Connect( Nocturnal::HybridPtr<DataType> (data.Address(), data.State()) );
}

template < class T >
size_t SimpleArraySerializer<T>::GetSize() const
{ 
    return m_Data->size(); 
}

template < class T >
void SimpleArraySerializer<T>::SetSize(size_t size)
{
    return m_Data->resize(size);
}

template < class T >
void SimpleArraySerializer<T>::Clear()
{
    return m_Data->clear();
}

template < class T >
i32 SimpleArraySerializer<T>::GetItemType() const
{
    return Serializer::DeduceType<T>();
}

template < class T >
SerializerPtr SimpleArraySerializer<T>::GetItem(size_t at)
{
    return Serializer::Bind(m_Data->at(at), m_Instance, m_Field);
}

template < class T >
ConstSerializerPtr SimpleArraySerializer<T>::GetItem(size_t at) const
{
    return Serializer::Bind(m_Data->at(at), m_Instance, m_Field);
}

template < class T >
void SimpleArraySerializer<T>::SetItem(size_t at, const Serializer* value)
{
    Serializer::GetValue(value, m_Data->at(at));
}

template < class T >
bool SimpleArraySerializer<T>::Set(const Serializer* src, u32 flags)
{
    const SimpleArraySerializer<T>* rhs = ConstObjectCast<SimpleArraySerializer<T>>(src);
    if (!rhs)
    {
        return false;
    }

    m_Data.Set( rhs->m_Data.Get() );

    return true;
}

template < class T >
bool SimpleArraySerializer<T>::Equals(const Serializer* s) const
{
    const SimpleArraySerializer<T>* rhs = ConstObjectCast<SimpleArraySerializer<T>>(s);
    if (!rhs)
    {
        return false;
    }

    return m_Data.Get() == rhs->m_Data.Get();
}

template < class T >
void SimpleArraySerializer<T>::Serialize(Archive& archive) const
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
#ifdef REFLECT_XML_SUPPORT
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            xml.GetIndent().Push();

            // foreach datum
            for (size_t i=0; i<m_Data->size(); i++)
            {
                // indent
                xml.GetIndent().Get(archive.GetStream());

                // write
                archive.GetStream() << m_Data.Get()[i];

                // newline
                archive.GetStream() << "\n";
            }

            xml.GetIndent().Pop();
#endif
            break;
        }

    case ArchiveTypes::Binary:
        {
            i32 count = (i32)m_Data->size();
            archive.GetStream().Write(&count); 

            if(count > 0)
            {
                // current offset in stream... 
                i32 offset       = (i32) archive.GetStream().TellWrite(); 
                i32 bytesWritten = 0; 
                archive.GetStream().Write(&bytesWritten); 

                const T& front = m_Data->front();
                bytesWritten   = CompressToStream(archive.GetStream(), (const char*) &front, sizeof(T) * count); 

                archive.GetStream().SeekWrite(offset, std::ios_base::beg); 
                archive.GetStream().Write(&bytesWritten); 
                archive.GetStream().SeekWrite(0, std::ios_base::end); 

            }
            break;
        }
    }
}

template < class T >
void SimpleArraySerializer<T>::Deserialize(Archive& archive)
{
    // if we are referring to a real field, clear its contents
    m_Data->clear();

    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
#ifdef REFLECT_XML_SUPPORT
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
#endif
            break;
        }

    case ArchiveTypes::Binary:
        {
            i32 count = -1;
            archive.GetStream().Read(&count); 

            m_Data->resize(count);

            if(count > 0)
            {
                // if we have array compression, decompress from the stream
                // otherwise, read count * sizeof(T) bytes 
                // 
                ArchiveBinary* archiveBinary = static_cast<ArchiveBinary*>(&archive); 
                if(archiveBinary->GetVersion() >= ArchiveBinary::FIRST_VERSION_WITH_ARRAY_COMPRESSION)
                {
                    i32 inputBytes; 
                    archive.GetStream().Read(&inputBytes); 
                    i32 bytesInflated = DecompressFromStream(archive.GetStream(), 
                        inputBytes, 
                        (char*) &(m_Data->front()), 
                        sizeof(T) * count); 

                    if(bytesInflated != sizeof(T) * count)
                    {
                        throw Reflect::StreamException( TXT( "Compressed Array size mismatch" ) ); 
                    }
                }
                else
                {
                    archive.GetStream().ReadBuffer(&(m_Data->front()), sizeof(T) * count );
                }
            }
            break;
        }
    }
}

template < class T >
std::ostream& SimpleArraySerializer<T>::operator >> (std::ostream& stream) const
{
    if (!TranslateOutput( stream ))
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
    }
    return stream;
}

template < class T >
std::istream& SimpleArraySerializer<T>::operator << (std::istream& stream)
{
    m_Data->clear();

    if (!TranslateInput( stream ))
    {
        std::string str;
        std::streamsize size = stream.rdbuf()->in_avail();
        str.resize( (size_t) size);
        stream.read(const_cast< char* >( str.c_str() ), size );

        Tokenize( str, m_Data.Ref(), s_ContainerItemDelimiter );
    }
    return stream;
}  

//
// Specializations
//


// keep reading the string until we run out of buffer
template <>
void StringArraySerializer::Serialize(Archive& archive) const
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
#ifdef REFLECT_XML_SUPPORT
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            xml.GetIndent().Push();
            xml.GetIndent().Get(archive.GetStream());

            // start our CDATA section, this prevents XML from parsing its escapes in this cdata section
            archive.GetStream() << "<![CDATA[\n";

            for (size_t i=0; i<m_Data->size(); i++)
            {
                xml.GetIndent().Get(archive.GetStream());

                // output the escape-code free character sequence between double qutoes
                archive.GetStream() << '\"' << m_Data.Get()[i].c_str() << '\"' << s_ContainerItemDelimiter;
            }

            // end our CDATA escape section
            xml.GetIndent().Get(archive.GetStream());
            archive.GetStream() << "]]>\n";

            xml.GetIndent().Pop();
#endif
            break;
        }

    case ArchiveTypes::Binary:
        {
            i32 size = (i32)m_Data->size();
            archive.GetStream().Write(&size); 

            for (size_t i=0; i<m_Data->size(); i++)
            {
                i32 index = static_cast<ArchiveBinary&>(archive).GetStrings().Insert(m_Data.Get()[i]);
                archive.GetStream().Write(&index); 
            }

            break;
        }
    }
}

// must escape strings to account for special "evil" characters... like ", &, `, etc...
template <>
void StringArraySerializer::Deserialize(Archive& archive)
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
#ifdef REFLECT_XML_SUPPORT
            archive.GetStream().SkipWhitespace(); 

            std::string value;

            while (!archive.GetStream().Done())
            {
                std::getline( *archive.GetStream().GetInternal(), value ); 

                size_t start = value.find_first_of('\"');
                size_t end = value.find_last_of('\"');

                // if we found a pair of quotes
                if (start != std::string::npos && end != std::string::npos && start != end)
                {
                    // if all we have are open/close quotes, push a blank string
                    if (start == end-1)
                        m_Data->push_back(std::string ());
                    // else we have some non-null string data
                    else
                        m_Data->push_back(value.substr(start + 1, end - start - 1));
                }
                else
                {
                    start = value.find_first_not_of( TXT( " \t\n" ) );

                    if (start != std::string::npos)
                        m_Data->push_back(value.substr(start));
                }

                archive.GetStream().SkipWhitespace(); 
            }
#endif
            break;
        }

    case ArchiveTypes::Binary:
        {
            i32 size = (i32)m_Data->size();
            archive.GetStream().Read(&size); 

            m_Data->resize(size);
            for (i32 i=0; i<size; i++)
            {
                i32 index;
                archive.GetStream().Read(&index); 
                m_Data.Ref()[i] = static_cast<ArchiveBinary&>(archive).GetStrings().GetString(index);
            }

            break;
        }
    }
}

template SimpleArraySerializer<std::string>;
template SimpleArraySerializer<bool>;
template SimpleArraySerializer<u8>;
template SimpleArraySerializer<i8>;
template SimpleArraySerializer<u16>;
template SimpleArraySerializer<i16>;
template SimpleArraySerializer<u32>;
template SimpleArraySerializer<i32>;
template SimpleArraySerializer<u64>;
template SimpleArraySerializer<i64>;
template SimpleArraySerializer<f32>;
template SimpleArraySerializer<f64>;
template SimpleArraySerializer<Nocturnal::GUID>;
template SimpleArraySerializer<Nocturnal::TUID>;
template SimpleArraySerializer< Nocturnal::Path >;

template SimpleArraySerializer<Math::Vector2>;
template SimpleArraySerializer<Math::Vector3>;
template SimpleArraySerializer<Math::Vector4>;
template SimpleArraySerializer<Math::Matrix3>;
template SimpleArraySerializer<Math::Matrix4>;
template SimpleArraySerializer<Math::Quaternion>;

template SimpleArraySerializer<Math::Color3>;
template SimpleArraySerializer<Math::Color4>;
template SimpleArraySerializer<Math::HDRColor3>;
template SimpleArraySerializer<Math::HDRColor4>;

REFLECT_DEFINE_CLASS(StringArraySerializer);
REFLECT_DEFINE_CLASS(BoolArraySerializer);
REFLECT_DEFINE_CLASS(U8ArraySerializer);
REFLECT_DEFINE_CLASS(I8ArraySerializer);
REFLECT_DEFINE_CLASS(U16ArraySerializer);
REFLECT_DEFINE_CLASS(I16ArraySerializer);
REFLECT_DEFINE_CLASS(U32ArraySerializer);
REFLECT_DEFINE_CLASS(I32ArraySerializer);
REFLECT_DEFINE_CLASS(U64ArraySerializer);
REFLECT_DEFINE_CLASS(I64ArraySerializer);
REFLECT_DEFINE_CLASS(F32ArraySerializer);
REFLECT_DEFINE_CLASS(F64ArraySerializer);
REFLECT_DEFINE_CLASS(GUIDArraySerializer);
REFLECT_DEFINE_CLASS(TUIDArraySerializer);
REFLECT_DEFINE_CLASS( PathArraySerializer );

REFLECT_DEFINE_CLASS(Vector2ArraySerializer);
REFLECT_DEFINE_CLASS(Vector3ArraySerializer);
REFLECT_DEFINE_CLASS(Vector4ArraySerializer);
REFLECT_DEFINE_CLASS(Matrix3ArraySerializer);
REFLECT_DEFINE_CLASS(Matrix4ArraySerializer);
REFLECT_DEFINE_CLASS(QuaternionArraySerializer);

REFLECT_DEFINE_CLASS(Color3ArraySerializer);
REFLECT_DEFINE_CLASS(Color4ArraySerializer);
REFLECT_DEFINE_CLASS(HDRColor3ArraySerializer);
REFLECT_DEFINE_CLASS(HDRColor4ArraySerializer);
