#include "BitfieldSerializer.h"
#include "ArchiveBinary.h"
#include "ArchiveXML.h"

#include "Foundation/Log.h"

using namespace Reflect;

REFLECT_DEFINE_CLASS(BitfieldSerializer);

BitfieldSerializer::BitfieldSerializer ()
{

}

BitfieldSerializer::~BitfieldSerializer()
{

}

void BitfieldSerializer::Serialize(Archive& archive) const
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            tstring str;
            if (m_Enumeration)
            {
                if (!m_Enumeration->GetBitfieldString(m_Data.Get(), str))
                {
                    throw Reflect::TypeInformationException( TXT( "Unable to serialize bitfield '%s', value %d" ), m_Enumeration->m_ShortName.c_str(), m_Data.Get() );
                }
            }

            xml.GetStream() << str;
            break;
        }

    case ArchiveTypes::Binary:
        {
            i32 index = -1;
            std::vector< tstring > strs;
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            if (m_Enumeration)
            {
                if (!m_Enumeration->GetBitfieldStrings(m_Data.Get(), strs))
                {
                    throw Reflect::TypeInformationException( TXT( "Unable to serialize bitfield '%s', value %d" ), m_Enumeration->m_ShortName.c_str(), m_Data.Get() );
                }

                // search the map
                std::vector< tstring >::const_iterator itr = strs.begin();
                std::vector< tstring >::const_iterator end = strs.end();
                for ( ; itr != end; ++itr )
                {
                    index = binary.GetStrings().Insert(*itr);
                    binary.GetStream().Write(&index); 
                }
            }

            // term
            index = -1;
            binary.GetStream().Write(&index); 

            break;
        }
    }

    if (m_Enumeration == NULL)
    {
        throw Reflect::TypeInformationException( TXT( "Missing type information" ) );
    }
}

void BitfieldSerializer::Deserialize(Archive& archive)
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            tstring buf;
            xml.GetStream() >> buf;
            if (m_Enumeration && !m_Enumeration->GetBitfieldValue(buf, m_Data.Ref()))
            {
                xml.Debug( TXT( "Unable to deserialize bitfield %s values '%s'\n" ), m_Enumeration->m_ShortName.c_str(), buf );
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

            i32 index = -1;
            binary.GetStream().Read(&index); 

            std::vector< tstring > strs;
            while (index >= 0)
            {
                strs.push_back(binary.GetStrings().Get(index));

                // read next index
                binary.GetStream().Read(&index); 
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

            if (m_Enumeration && !m_Enumeration->GetBitfieldValue(strs, m_Data.Ref()))
            {
                archive.Debug( TXT( "Unable to deserialize bitfield %s values '%s'\n" ), m_Enumeration->m_ShortName.c_str(), str.c_str() );
            }
            else
            {
                m_String = str;
            }

            break;
        }
    }

    if (m_Enumeration == NULL)
    {
        throw Reflect::TypeInformationException( TXT( "Missing type information" ) );
    }
}

tostream& BitfieldSerializer::operator>> (tostream& stream) const
{
    tstring str;
    if (!m_Enumeration->GetBitfieldString(m_Data.Get(), str))
    {
        // something is amiss, we should be guaranteed serialization of enum elements
        NOC_BREAK();
    }

    stream << str;

    return stream;
}

tistream& BitfieldSerializer::operator<< (tistream& stream)
{
    tstring buf;
    stream >> buf;
    m_Enumeration->GetBitfieldValue(buf, m_Data.Ref());

    return stream;
}
