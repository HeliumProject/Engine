#include "Foundation/Reflect/Data/EnumerationData.h"

#include "Foundation/Log.h"
#include "Foundation/Reflect/Enumeration.h"
#include "Foundation/Reflect/ArchiveBinary.h"
#include "Foundation/Reflect/ArchiveXML.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_OBJECT(EnumerationData);

EnumerationData::EnumerationData ()
{

}

EnumerationData::~EnumerationData()
{

}

void EnumerationData::ConnectData(void* data)
{
    m_Data.Connect( data );
}

bool EnumerationData::Set(Data* src, uint32_t flags)
{
    if (GetClass() != src->GetClass())
    {
        return false;
    }

    const EnumerationData* rhs = static_cast<const EnumerationData*>(src);

    *m_Data = *rhs->m_Data;

    return true;
}

bool EnumerationData::Equals(Object* object)
{
    const EnumerationData* rhs = SafeCast< EnumerationData >( object );
    
    if (!rhs)
    {
        return false;
    }

    return *m_Data == *rhs->m_Data;
}

void EnumerationData::Serialize(ArchiveBinary& archive)
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

    tstring label;

    if (enumeration)
    {
        if (!enumeration->GetElementName(*m_Data, label))
        {
            throw Reflect::TypeInformationException( TXT( "Unable to serialize enumeration '%s', value %d" ), enumeration->m_Name, *m_Data );
        }
    }

    archive.GetStream().WriteString( label ); 

    if (enumeration == NULL)
    {
        throw Reflect::TypeInformationException( TXT( "Missing type information" ) );
    }
}

void EnumerationData::Deserialize(ArchiveBinary& archive)
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
    archive.GetStream().ReadString( str );
    if (enumeration && !enumeration->GetElementValue(str, *m_Data))
    {
        Log::Debug( TXT( "Unable to deserialize %s::%s, discarding\n" ), enumeration->m_Name, str.c_str() );
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

void EnumerationData::Serialize(ArchiveXML& archive)
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

    tstring label;
    if (enumeration)
    {
        if (!enumeration->GetElementName(*m_Data, label))
        {
            throw Reflect::TypeInformationException( TXT( "Unable to serialize enumeration '%s', value %d" ), enumeration->m_Name, *m_Data );
        }
    }

    archive.GetStream() << label;

    if (enumeration == NULL)
    {
        throw Reflect::TypeInformationException( TXT( "Missing type information" ) );
    }
}

void EnumerationData::Deserialize(ArchiveXML& archive)
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
    if (!buf.empty())
    {
        if (enumeration && !enumeration->GetElementValue(buf, *m_Data))
        {
            Log::Debug( TXT( "Unable to deserialize %s::%s, discarding\n" ), enumeration->m_Name, buf.c_str() );
        }
        else
        {
            m_String = buf;
        }
    }

    if (enumeration == NULL)
    {
        throw Reflect::TypeInformationException( TXT( "Missing type information" ) );
    }
}

tostream& EnumerationData::operator>>(tostream& stream) const
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

    tstring label;
    if (enumeration && !enumeration->GetElementName(*m_Data, label))
    {
        // something is amiss, we should be guaranteed serialization of enum elements
        HELIUM_BREAK();
    }

    stream << label;

    return stream;
}

tistream& EnumerationData::operator<<(tistream& stream)
{
    tstring buf;
    stream >> buf;

    const Enumeration* enumeration = NULL;
    
    if ( m_Field )
    {
        enumeration = ReflectionCast< Enumeration >( m_Field->m_Type );
    }
    else
    {
        HELIUM_BREAK(); // not really supported yet
    }

    if ( !buf.empty() && enumeration )
    {
        enumeration->GetElementValue(buf, *m_Data);

        if ( m_Instance && m_Field && m_Field->m_Composite->GetReflectionType() == ReflectionTypes::Class )
        {
            Object* object = static_cast< Object* >( m_Instance );
            object->RaiseChanged( m_Field );
        }
    }

    return stream;
}
