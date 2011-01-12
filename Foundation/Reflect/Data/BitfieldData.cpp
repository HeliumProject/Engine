#include "Foundation/Reflect/Data/BitfieldData.h"

#include "Foundation/Log.h"
#include "Foundation/Reflect/Enumeration.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"

using namespace Helium::Reflect;

REFLECT_DEFINE_CLASS(BitfieldData);

BitfieldData::BitfieldData ()
{

}

BitfieldData::~BitfieldData()
{

}

void BitfieldData::Serialize(Archive& archive) const
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

    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            tstring str;
            if (enumeration)
            {
                if (!enumeration->GetBitfieldString(m_Data.Get(), str))
                {
                    throw Reflect::TypeInformationException( TXT( "Unable to serialize bitfield '%s', value %d" ), enumeration->m_Name, m_Data.Get() );
                }
            }

            xml.GetStream() << str;
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            if (enumeration)
            {
                std::vector< tstring > strs;
                if (!enumeration->GetBitfieldStrings(m_Data.Get(), strs))
                {
                    throw Reflect::TypeInformationException( TXT( "Unable to serialize bitfield '%s', value %d" ), enumeration->m_Name, m_Data.Get() );
                }

                uint32_t count = (uint32_t)strs.size();
                binary.GetStream().Write(&count); 

                std::vector< tstring >::const_iterator itr = strs.begin();
                std::vector< tstring >::const_iterator end = strs.end();
                for ( ; itr != end; ++itr )
                {
                    binary.GetStream().WriteString( *itr );
                }
            }

            break;
        }
    }

    if (enumeration == NULL)
    {
        throw Reflect::TypeInformationException( TXT( "Missing type information" ) );
    }
}

void BitfieldData::Deserialize(Archive& archive)
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

    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            tstring buf;
            xml.GetStream() >> buf;
            if (enumeration && !enumeration->GetBitfieldValue(buf, m_Data.Ref()))
            {
                Log::Debug( TXT( "Unable to deserialize bitfield %s values '%s'\n" ), enumeration->m_Name, buf );
            }
            else
            {
                m_String = buf;
            }
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            uint32_t count = 0;
            binary.GetStream().Read(&count); 

            std::vector< tstring > strs;
            strs.reserve( count );
            while ( count-- > 0 )
            {
                tstring str;
                binary.GetStream().ReadString( str );
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

            if (enumeration && !enumeration->GetBitfieldValue(strs, m_Data.Ref()))
            {
                Log::Debug( TXT( "Unable to deserialize bitfield %s values '%s'\n" ), enumeration->m_Name, str.c_str() );
            }
            else
            {
                m_String = str;
            }

            break;
        }
    }

    if (enumeration == NULL)
    {
        throw Reflect::TypeInformationException( TXT( "Missing type information" ) );
    }
}

tostream& BitfieldData::operator>> (tostream& stream) const
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
    if ( enumeration && !enumeration->GetBitfieldString(m_Data.Get(), str) )
    {
        // something is amiss, we should be guaranteed serialization of enum elements
        HELIUM_BREAK();
    }

    stream << str;

    return stream;
}

tistream& BitfieldData::operator<< (tistream& stream)
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
        enumeration->GetBitfieldValue(buf, m_Data.Ref());
    }

    return stream;
}
