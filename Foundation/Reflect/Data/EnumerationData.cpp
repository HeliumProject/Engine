#include "Foundation/Reflect/Data/EnumerationData.h"

#include "Foundation/Log.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"

using namespace Helium::Reflect;

REFLECT_DEFINE_CLASS(EnumerationData);

EnumerationData::EnumerationData ()
: m_Enumeration( NULL )
{

}

EnumerationData::~EnumerationData()
{

}

void EnumerationData::ConnectData(Helium::HybridPtr<void> data)
{
    __super::ConnectData( data );

    m_Data.Connect( Helium::HybridPtr<DataType> (data.Address(), data.State()) );

    if ( data.Address() == NULL )
    {
        // Disconnect case
        m_Enumeration = NULL;
    }
}

void EnumerationData::ConnectField(Helium::HybridPtr<Element> instance, const Field* field, uintptr_t offsetInField)
{
    __super::ConnectField(instance, field, offsetInField);

    const EnumerationField* enumField = ReflectionCast<EnumerationField>(field);
    HELIUM_ASSERT( enumField );

    m_Enumeration = enumField->m_Enumeration;
}

bool EnumerationData::Set(const Data* src, uint32_t flags)
{
    if (GetType() != src->GetType())
    {
        return false;
    }

    const EnumerationData* rhs = static_cast<const EnumerationData*>(src);

    m_Data.Set( rhs->m_Data.Get() );

    return true;
}

bool EnumerationData::Equals(const Data* s) const
{
    if (GetType() != s->GetType())
    {
        return false;
    }

    const EnumerationData* rhs = static_cast<const EnumerationData*>(s);

    return rhs->m_Data.Get() == m_Data.Get();
}

void EnumerationData::Serialize(Archive& archive) const
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            tstring label;
            if (m_Enumeration)
            {
                if (!m_Enumeration->GetElementLabel(m_Data.Get(), label))
                {
                    throw Reflect::TypeInformationException( TXT( "Unable to serialize enumeration '%s', value %d" ), m_Enumeration->m_Name.c_str(), m_Data.Get() );
                }
            }

            // store corresponding string
            xml.GetStream() << label;
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            tstring label;

            if (m_Enumeration)
            {
                if (!m_Enumeration->GetElementLabel(m_Data.Get(), label))
                {
                    throw Reflect::TypeInformationException( TXT( "Unable to serialize enumeration '%s', value %d" ), m_Enumeration->m_Name.c_str(), m_Data.Get() );
                }
            }

            // get string pool index
            int32_t index = binary.GetStrings().Insert(label);

            // write that index
            binary.GetStream().Write(&index); 

            break;
        }
    }

    if (m_Enumeration == NULL)
    {
        throw Reflect::TypeInformationException( TXT( "Missing type information" ) );
    }
}

void EnumerationData::Deserialize(Archive& archive)
{
    switch (archive.GetType())
    {
    case ArchiveTypes::XML:
        {
            ArchiveXML& xml (static_cast<ArchiveXML&>(archive));

            tstring buf;
            xml.GetStream() >> buf;
            if (!buf.empty())
            {
                if (m_Enumeration && !m_Enumeration->GetElementValue(buf, m_Data.Ref()))
                {
                    Log::Debug( TXT( "Unable to deserialize %s::%s, discarding\n" ), m_Enumeration->m_Name.c_str(), buf.c_str() );
                }
                else
                {
                    m_String = buf;
                }
            }
            break;
        }

    case ArchiveTypes::Binary:
        {
            ArchiveBinary& binary (static_cast<ArchiveBinary&>(archive));

            int32_t index = -1;
            binary.GetStream().Read(&index); 

            if (index >= 0)
            {
                const tstring& str (binary.GetStrings().Get(index));

                if (m_Enumeration && !m_Enumeration->GetElementValue(str, m_Data.Ref()))
                {
                    Log::Debug( TXT( "Unable to deserialize %s::%s, discarding\n" ), m_Enumeration->m_Name.c_str(), str.c_str() );
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

tostream& EnumerationData::operator>> (tostream& stream) const
{
    bool set = false;

    tstring label;
    if (!m_Enumeration->GetElementLabel(m_Data.Get(), label))
    {
        // something is amiss, we should be guaranteed serialization of enum elements
        HELIUM_BREAK();
    }

    stream << label;

    return stream;
}

tistream& EnumerationData::operator<< (tistream& stream)
{
    tstring buf;
    stream >> buf;

    if (!buf.empty())
    {
        m_Enumeration->GetElementValue(buf, m_Data.Ref());

        if ( m_Instance && m_Field && m_Field->m_Composite->GetReflectionType() == ReflectionTypes::Class )
        {
            Element* element = (Element*)m_Instance;
            element->RaiseChanged( m_Field );
        }
    }

    return stream;
}
