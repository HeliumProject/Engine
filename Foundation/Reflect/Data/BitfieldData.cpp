#include "FoundationPch.h"
#include "Foundation/Reflect/Data/BitfieldData.h"

#include "Foundation/Log.h"
#include "Foundation/Reflect/Enumeration.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"

REFLECT_DEFINE_OBJECT( Helium::Reflect::BitfieldData );

using namespace Helium;
using namespace Helium::Reflect;

BitfieldData::BitfieldData ()
{

}

BitfieldData::~BitfieldData()
{

}

void BitfieldData::Serialize(ArchiveBinary& archive)
{
    const Enumeration* enumeration = NULL;
    
    if ( m_Field )
    {
        enumeration = ReflectionCast< Enumeration >( m_Field->m_Type );
    }
    else
    {
        HELIUM_BREAK(); // not really supported yet
    }

    if (enumeration)
    {
        std::vector< tstring > strs;
        if (!enumeration->GetBitfieldStrings( *m_Data, strs ))
        {
            throw Reflect::TypeInformationException( TXT( "Unable to serialize bitfield '%s', value %d" ), enumeration->m_Name, *m_Data );
        }

        uint32_t count = (uint32_t)strs.size();
        archive.GetStream().Write(&count); 

        std::vector< tstring >::const_iterator itr = strs.begin();
        std::vector< tstring >::const_iterator end = strs.end();
        for ( ; itr != end; ++itr )
        {
            archive.GetStream().WriteString( *itr );
        }
    }

    if (enumeration == NULL)
    {
        throw Reflect::TypeInformationException( TXT( "Missing type information" ) );
    }
}

void BitfieldData::Deserialize(ArchiveBinary& archive)
{
    const Enumeration* enumeration = NULL;
    
    if ( m_Field )
    {
        enumeration = ReflectionCast< Enumeration >( m_Field->m_Type );
    }
    else
    {
        HELIUM_BREAK(); // not really supported yet
    }

    uint32_t count = 0;
    archive.GetStream().Read(&count); 

    std::vector< tstring > strs;
    strs.reserve( count );
    while ( count-- > 0 )
    {
        tstring str;
        archive.GetStream().ReadString( str );
        strs.push_back( str );
    }

    tstring str;
    std::vector< tstring >::const_iterator itr = strs.begin();
    std::vector< tstring >::const_iterator end = strs.end();
    for ( ; itr != end; ++itr )
    {
        if (itr != strs.begin())
        {
            str += TXT("|");
        }

        str += *itr;
    }

    if (enumeration && !enumeration->GetBitfieldValue(strs, *m_Data))
    {
        Log::Debug( TXT( "Unable to deserialize bitfield %s values '%s'\n" ), enumeration->m_Name, str.c_str() );
    }
    else
    {
        m_String = str;
    }

    if (enumeration == NULL)
    {
        throw Reflect::TypeInformationException( TXT( "Missing type information" ) );
    }
}

void BitfieldData::Serialize(ArchiveXML& archive)
{
    const Enumeration* enumeration = NULL;
    
    if ( m_Field )
    {
        enumeration = ReflectionCast< Enumeration >( m_Field->m_Type );
    }
    else
    {
        HELIUM_BREAK(); // not really supported yet
    }

    tstring str;
    if (enumeration)
    {
        if (!enumeration->GetBitfieldString( *m_Data, str ))
        {
            throw Reflect::TypeInformationException( TXT( "Unable to serialize bitfield '%s', value %d" ), enumeration->m_Name, *m_Data );
        }
    }

    archive.GetStream() << str;

    if (enumeration == NULL)
    {
        throw Reflect::TypeInformationException( TXT( "Missing type information" ) );
    }
}

void BitfieldData::Deserialize(ArchiveXML& archive)
{
    const Enumeration* enumeration = NULL;
    
    if ( m_Field )
    {
        enumeration = ReflectionCast< Enumeration >( m_Field->m_Type );
    }
    else
    {
        HELIUM_BREAK(); // not really supported yet
    }

    tstring buf;
    archive.GetStream() >> buf;

    if (enumeration && !enumeration->GetBitfieldValue(buf, *m_Data))
    {
        Log::Debug( TXT( "Unable to deserialize bitfield %s values '%s'\n" ), enumeration->m_Name, buf );
    }
    else
    {
        m_String = buf;
    }

    if (enumeration == NULL)
    {
        throw Reflect::TypeInformationException( TXT( "Missing type information" ) );
    }
}

tostream& BitfieldData::operator>>(tostream& stream) const
{
    const Enumeration* enumeration = NULL;
    
    if ( m_Field )
    {
        enumeration = ReflectionCast< Enumeration >( m_Field->m_Type );
    }
    else
    {
        HELIUM_BREAK(); // not really supported yet
    }

    tstring str;
    if ( enumeration && !enumeration->GetBitfieldString( *m_Data, str ) )
    {
        // something is amiss, we should be guaranteed serialization of enum elements
        HELIUM_BREAK();
    }

    stream << str;

    return stream;
}

tistream& BitfieldData::operator<<(tistream& stream)
{
    const Enumeration* enumeration = NULL;
    
    if ( m_Field )
    {
        enumeration = ReflectionCast< Enumeration >( m_Field->m_Type );
    }
    else
    {
        HELIUM_BREAK(); // not really supported yet
    }

    tstring buf;
    stream >> buf;

    if ( enumeration )
    {
        enumeration->GetBitfieldValue( buf, *m_Data );
    }

    return stream;
}
