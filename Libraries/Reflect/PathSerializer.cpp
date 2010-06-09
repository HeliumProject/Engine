#include "stdafx.h"
#include "PathSerializer.h"
#include "ArchiveBinary.h"
#include "ArchiveXML.h"

#include "Console/Console.h"

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
    std::string data = m_Data.Get().Get();

    switch ( archive.GetType() )
    {
    case ArchiveTypes::XML:
        {
            archive.GetOutput() << data;
            break;
        }

    case ArchiveTypes::Binary:
        {
            // get string pool index
            i32 index = static_cast< ArchiveBinary& >( archive ).GetStrings().AssignIndex( data );
            // write that index
            archive.GetOutput().Write( &index ); 
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
            std::string buf;
            std::streamsize size = archive.GetInput().BytesAvailable(); 
            buf.resize( (size_t) size);
            archive.GetInput().ReadBuffer( const_cast<char*>( buf.c_str() ), size );
            m_Data.Ref().Set( buf );
            break;
        }

    case ArchiveTypes::Binary:
        {
            i32 index = -1;
            archive.GetInput().Read( &index ); 

            if ( index >= 0 )
            {
                const std::string& str (static_cast< ArchiveBinary& >( archive ).GetStrings().GetString( index ) );
                m_Data.Ref().Set( str );
            }

            break;
        }
    }
}

std::ostream& PathSerializer::operator >>( std::ostream& stream ) const
{
    stream << m_Data.Get().Get();
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
        m_Data.Ref().Set( buf );

        if ( m_Instance && m_Field && m_Field->m_Type->GetReflectionType() == ReflectionTypes::Class )
        {
            Element* element = (Element*)m_Instance;
            element->RaiseChanged( m_Field );
        }
    }

    return stream;
}
