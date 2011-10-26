#include "FoundationPch.h"
#include "Foundation/Reflect/Data/PathData.h"

#include "Foundation/Log.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"

REFLECT_DEFINE_OBJECT( Helium::Reflect::PathData );

using namespace Helium::Reflect;

PathData::PathData()
{
}

PathData::~PathData()
{
}

void PathData::ConnectData( void* data )
{
    m_Data.Connect( data );
}

bool PathData::Set( Data* src, uint32_t flags )
{
    if ( GetClass() != src->GetClass() )
    {
        return false;
    }

    const PathData* rhs = static_cast< const PathData* >( src );

    *m_Data = *rhs->m_Data;

    return true;
}

bool PathData::Equals( Object* object )
{
    const PathData* rhs = SafeCast< PathData >( object );

    if ( !rhs )
    {
        return false;
    }

    return *rhs->m_Data == *m_Data;
}

void PathData::Serialize( ArchiveBinary& archive )
{
    const tstring& str = m_Data->Get();
    archive.GetStream().WriteString( str ); 
}

void PathData::Deserialize( ArchiveBinary& archive )
{
    tstring str;
    archive.GetStream().ReadString( str );
    m_Data->Set( str );
}

void PathData::Serialize( ArchiveXML& archive )
{
    const tstring& str = m_Data->Get();
    archive.GetStream() << str;
}

void PathData::Deserialize( ArchiveXML& archive )
{
    tstring buf;
    std::streamsize size = archive.GetStream().ElementsAvailable(); 
    buf.resize( (size_t)size );
    archive.GetStream().ReadBuffer( const_cast<tchar_t*>( buf.c_str() ), size );
    m_Data->Set( buf );
}

tostream& PathData::operator>>( tostream& stream ) const
{
    tstring path = m_Data->Get();
    tstring temp;
    bool converted = Helium::ConvertString( path, temp );
    HELIUM_ASSERT( converted );

    stream << temp;
    return stream;
}

tistream& PathData::operator<<( tistream& stream )
{
    tstring str;
    std::streamsize size = stream.rdbuf()->in_avail();
    str.resize( (size_t) size );
    stream.read( const_cast<tchar_t*>( str.c_str() ), size );

    if ( !str.empty() )
    {
        m_Data->Set( str );

        if ( m_Instance && m_Field && m_Field->m_Composite->GetReflectionType() == ReflectionTypes::Class )
        {
            Object* object = static_cast< Object* >( m_Instance );
            object->RaiseChanged( m_Field );
        }
    }

    return stream;
}
