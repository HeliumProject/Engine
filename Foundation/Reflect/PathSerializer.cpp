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

void PathSerializer::ConnectData( Helium::HybridPtr< void > data )
{
    __super::ConnectData( data );

    m_Data.Connect( Helium::HybridPtr< Helium::Path >( data.Address(), data.State() ) );
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
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            xml.GetStream() << data;
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
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            tstring buf;
            std::streamsize size = xml.GetStream().ElementsAvailable(); 
            buf.resize( (size_t)size );
            xml.GetStream().ReadBuffer( const_cast<tchar*>( buf.c_str() ), size );
            m_Data.Ref().Set( buf );
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            i32 index = -1;
            binary.GetStream().Read( &index ); 

            if ( index >= 0 )
            {
                const tstring& str ( binary.GetStrings().Get( index ) );

                m_Data.Ref().Set( str );
            }

            break;
        }
    }
}

tostream& PathSerializer::operator>>( tostream& stream ) const
{
    tstring path = m_Data.Get().Get();
    tstring temp;
    bool converted = Platform::ConvertString( path, temp );
    HELIUM_ASSERT( converted );

    stream << temp;
    return stream;
}

tistream& PathSerializer::operator<<( tistream& stream )
{
    tstring str;
    std::streamsize size = stream.rdbuf()->in_avail();
    str.resize( (size_t) size );
    stream.read( const_cast<tchar*>( str.c_str() ), size );

    if ( !str.empty() )
    {
        m_Data.Ref().Set( str );

        if ( m_Instance && m_Field && m_Field->m_Type->GetReflectionType() == ReflectionTypes::Class )
        {
            Element* element = (Element*)m_Instance;
            element->RaiseChanged( m_Field );
        }
    }

    return stream;
}
