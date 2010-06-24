#include "EnumerationSerializer.h"
#include "ArchiveBinary.h"
#include "ArchiveXML.h"

#include "Foundation/Log.h"

using namespace Reflect;

REFLECT_DEFINE_CLASS(EnumerationSerializer);

EnumerationSerializer::EnumerationSerializer ()
: m_Enumeration( NULL )
{

}

EnumerationSerializer::~EnumerationSerializer()
{

}

bool EnumerationSerializer::IsCompact () const 
{ 
    return true; 
}

void EnumerationSerializer::ConnectData(Nocturnal::HybridPtr<void> data)
{
    __super::ConnectData( data );

    m_Data.Connect( Nocturnal::HybridPtr<DataType> (data.Address(), data.State()) );

    if ( data.Address() == NULL )
    {
        // Disconnect case
        m_Enumeration = NULL;
    }
}

void EnumerationSerializer::ConnectField(Nocturnal::HybridPtr<Element> instance, const Field* field, uintptr offsetInField)
{
    __super::ConnectField(instance, field, offsetInField);

    const EnumerationField* enumField = ReflectionCast<EnumerationField>(field);
    NOC_ASSERT( enumField );

    m_Enumeration = enumField->m_Enumeration;
}

bool EnumerationSerializer::Set(const Serializer* src, u32 flags)
{
    if (GetType() != src->GetType())
    {
        return false;
    }

    const EnumerationSerializer* rhs = static_cast<const EnumerationSerializer*>(src);

    m_Data.Set( rhs->m_Data.Get() );

    return true;
}

bool EnumerationSerializer::Equals(const Serializer* s) const
{
    if (GetType() != s->GetType())
    {
        return false;
    }

    const EnumerationSerializer* rhs = static_cast<const EnumerationSerializer*>(s);

    return rhs->m_Data.Get() == m_Data.Get();
}

void EnumerationSerializer::Serialize(Archive& archive) const
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
#ifdef REFLECT_XML_SUPPORT
            std::string label;

            if (m_Enumeration)
            {
                if (!m_Enumeration->GetElementLabel(m_Data.Get(), label))
                {
                    throw Reflect::TypeInformationException( TXT( "Unable to serialize enumeration '%s', value %d" ), m_Enumeration->m_ShortName.c_str(), m_Data.Get() );
                }
            }

            // store corresponding string
            archive.GetStream() << label;
#endif
            break;
        }

    case ArchiveTypes::Binary:
        {
            std::string label;

            if (m_Enumeration)
            {
                if (!m_Enumeration->GetElementLabel(m_Data.Get(), label))
                {
                    throw Reflect::TypeInformationException( TXT( "Unable to serialize enumeration '%s', value %d" ), m_Enumeration->m_ShortName.c_str(), m_Data.Get() );
                }
            }

            // get string pool index
            i32 index = static_cast<ArchiveBinary&>(archive).GetStrings().Insert(label);

            // write that index
            archive.GetStream().Write(&index); 

            break;
        }
    }

    if (m_Enumeration == NULL)
    {
        throw Reflect::TypeInformationException( TXT( "Missing type information" ) );
    }
}

void EnumerationSerializer::Deserialize(Archive& archive)
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
#ifdef REFLECT_XML_SUPPORT
            std::string buf;
            archive.GetStream() >> buf;

            if (!buf.empty())
            {
                if (m_Enumeration && !m_Enumeration->GetElementValue(buf, m_Data.Ref()))
                {
                    archive.Debug( TXT( "Unable to deserialize %s::%s, discarding\n" ), m_Enumeration->m_ShortName.c_str(), buf.c_str() );
                }
                else
                {
                    m_String = buf;
                }
            }
#endif
            break;
        }

    case ArchiveTypes::Binary:
        {
            i32 index = -1;
            archive.GetStream().Read(&index); 

            if (index >= 0)
            {
                const std::string& str (static_cast<ArchiveBinary&>(archive).GetStrings().GetString(index));

                if (m_Enumeration && !m_Enumeration->GetElementValue(str, m_Data.Ref()))
                {
                    archive.Debug( TXT( "Unable to deserialize %s::%s, discarding\n" ), m_Enumeration->m_ShortName.c_str(), str.c_str() );
                }
                else
                {
                    m_String = str;
                }
            }

            break;
        }
    }

    if (m_Enumeration == NULL)
    {
        throw Reflect::TypeInformationException( TXT( "Missing type information" ) );
    }
}

std::ostream& EnumerationSerializer::operator >> (std::ostream& stream) const
{
    bool set = false;

    std::string label;
    if (!m_Enumeration->GetElementLabel(m_Data.Get(), label))
    {
        // something is amiss, we should be guaranteed serialization of enum elements
        NOC_BREAK();
    }

    stream << label;

    return stream;
}

std::istream& EnumerationSerializer::operator << (std::istream& stream)
{
    std::string buf;
    stream >> buf;

    if (!buf.empty())
    {
        m_Enumeration->GetElementValue(buf, m_Data.Ref());

        if ( m_Instance && m_Field && m_Field->m_Type->GetReflectionType() == ReflectionTypes::Class )
        {
            Element* element = (Element*)m_Instance;
            element->RaiseChanged( m_Field );
        }
    }

    return stream;
}
