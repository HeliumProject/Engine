#include "PathSerializer.h"
#include "ArchiveBinary.h"
#include "ArchiveXML.h"

#include "Foundation/Log.h"

using namespace Reflect;

REFLECT_DEFINE_CLASS( PathSerializer );

PathSerializer::PathSerializer()
{
}

PathSerializer::~PathSerializer()
{
}

bool PathSerializer::IsCompact() const 
{ 
    return true;
}

void PathSerializer::ConnectData( Nocturnal::HybridPtr< void > data )
{
    __super::ConnectData( data );

    m_Data.Connect( Nocturnal::HybridPtr< Nocturnal::Path >( data.Address(), data.State() ) );
}

bool PathSerializer::Set( const Serializer* src, u32 flags )
{
    if ( GetType() != src->GetType() )
    {
        return false;
    }

    const PathSerializer* rhs = static_cast< const PathSerializer* >( src );

    m_Data.Set( rhs->m_Data.Get() );

    return true;
}

bool PathSerializer::Equals( const Serializer* s ) const
{
    if ( GetType() != s->GetType() )
    {
        return false;
    }

    const PathSerializer* rhs = static_cast< const PathSerializer* >( s );

    return rhs->m_Data.Get() == m_Data.Get();
}

void PathSerializer::Serialize( Archive& archive ) const
{
    tstring data = m_Data.Get().Get();

    switch ( archive.GetType() )
    {
    case ArchiveTypes::XML:
        {
#ifdef REFLECT_XML_SUPPORT
            archive.GetStream() << data;
#endif
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            // get string pool index
            i32 index = binary.GetStrings().Insert( data );

            // write that index
            binary.GetStream().Write( &index ); 
            break;
        }
    }
}

void PathSerializer::Deserialize( Archive& archive )
{
    switch ( archive.GetType() )
    {
    case ArchiveTypes::XML:
        {
#ifdef REFLECT_XML_SUPPORT
            std::string buf;
            std::streamsize size = archive.GetStream().BytesAvailable(); 
            buf.resize( (size_t) size);
            archive.GetStream().ReadBuffer( const_cast<char*>( buf.c_str() ), size );
            m_Data.Ref().Set( buf );
#endif
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            i32 index = -1;
            binary.GetStream().Read( &index ); 

            if ( index >= 0 )
            {
                const tstring& str ( binary.GetStrings().GetString( index ) );

                m_Data.Ref().Set( str );
            }

            break;
        }
    }
}

std::ostream& PathSerializer::operator >>( std::ostream& stream ) const
{
    tstring path = m_Data.Get().Get();
    std::string temp;
    bool converted = Platform::ConvertString( path, temp );
    NOC_ASSERT( converted );

    stream << temp;
    return stream;
}

std::istream& PathSerializer::operator <<( std::istream& stream )
{
    std::string buf;
    std::streamsize size = stream.rdbuf()->in_avail();
    buf.resize( (size_t) size);
    stream.read( const_cast<char*>( buf.c_str() ), size );

    if ( !buf.empty() )
    {
        tstring path;
        bool converted = Platform::ConvertString( buf, path );
        NOC_ASSERT( converted );

        m_Data.Ref().Set( path );

        if ( m_Instance && m_Field && m_Field->m_Type->GetReflectionType() == ReflectionTypes::Class )
        {
            Element* element = (Element*)m_Instance;
            element->RaiseChanged( m_Field );
        }
    }

    return stream;
}
