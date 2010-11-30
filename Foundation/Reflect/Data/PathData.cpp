#include "Foundation/Reflect/Data/PathData.h"

#include "Foundation/Log.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"

using namespace Helium::Reflect;

REFLECT_DEFINE_CLASS( PathData );

PathData::PathData()
{
}

PathData::~PathData()
{
}

void PathData::ConnectData( Helium::HybridPtr< void > data )
{
    __super::ConnectData( data );

    m_Data.Connect( Helium::HybridPtr< Helium::Path >( data.Address(), data.State() ) );
}

bool PathData::Set( const Data* src, uint32_t flags )
{
    if ( GetType() != src->GetType() )
    {
        return false;
    }

    const PathData* rhs = static_cast< const PathData* >( src );

    m_Data.Set( rhs->m_Data.Get() );

    return true;
}

bool PathData::Equals( const Data* s ) const
{
    if ( GetType() != s->GetType() )
    {
        return false;
    }

    const PathData* rhs = static_cast< const PathData* >( s );

    return rhs->m_Data.Get() == m_Data.Get();
}

void PathData::Serialize( Archive& archive ) const
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
            int32_t index = binary.GetStrings().Insert( data );

            // write that index
            binary.GetStream().Write( &index ); 
            break;
        }
    }
}

void PathData::Deserialize( Archive& archive )
{
    switch ( archive.GetType() )
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            tstring buf;
            std::streamsize size = xml.GetStream().ElementsAvailable(); 
            buf.resize( (size_t)size );
            xml.GetStream().ReadBuffer( const_cast<tchar_t*>( buf.c_str() ), size );
            m_Data.Ref().Set( buf );
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            int32_t index = -1;
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

tostream& PathData::operator>>( tostream& stream ) const
{
    tstring path = m_Data.Get().Get();
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
        m_Data.Ref().Set( str );

        if ( m_Instance && m_Field && m_Field->m_Type->GetReflectionType() == ReflectionTypes::Class )
        {
            Element* element = (Element*)m_Instance;
            element->RaiseChanged( m_Field );
        }
    }

    return stream;
}
