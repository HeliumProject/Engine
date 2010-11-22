#include "Field.h"
#include "Registry.h"
#include "Serializers.h"
#include "ArchiveBinary.h"

#include "Foundation/Log.h"

using namespace Helium::Reflect;

Field::Field(const Composite* type)
: m_Type ( type )
, m_Flags ( 0 )
, m_FieldID ( -1 )
, m_SerializerID ( ReservedTypes::Invalid )
, m_Offset ( -1 )
, m_Creator ( NULL )
{

}

Field::~Field()
{

}

Field* Field::Create(const Composite* type)
{
    return new Field( type );
}

SerializerPtr Field::CreateSerializer(Element* instance) const
{
    SerializerPtr ser;

    if (m_SerializerID != Reflect::ReservedTypes::Invalid)
    {
        ObjectPtr object = Registry::GetInstance()->CreateInstance(m_SerializerID);

        if (object.ReferencesObject())
        {
            ser = AssertCast<Serializer>(object);
        }
    }

    if (ser.ReferencesObject() && instance)
    {
        ser->ConnectField( instance, this );
    }

    return ser;
}

bool Field::HasDefaultValue(Element* instance) const
{
    // if we don't have a default value, we can never be at the default value
    if (!m_Default.ReferencesObject())
    {
        return false;
    }

    // get a serializer
    SerializerPtr serializer = CreateSerializer();

    if (serializer.ReferencesObject())
    {
        // set data pointer
        serializer->ConnectField(instance, this);

        // return equality
        bool result = m_Default->Equals(serializer);

        // disconnect
        serializer->Disconnect();

        // result
        return result;
    }

    return false;
}

bool Field::SetDefaultValue(Element* instance) const
{
    // if we don't have a default value, we can never be at the default value
    if (!m_Default.ReferencesObject())
    {
        return false;
    }

    // get a serializer
    SerializerPtr serializer = CreateSerializer();

    if (serializer.ReferencesObject())
    {
        // set data pointer
        serializer->ConnectField(instance, this);

        // copy the data
        serializer->Set(m_Default);

        // disconnect
        serializer->Disconnect();

        return true;
    }

    return false;
}

void Field::SetName(const tstring& name)
{
    m_Name = name;

    if ( m_UIName.empty() )
    {
        if (m_Name.substr(0, 2) == TXT("m_") )
        {
            m_UIName = m_Name.substr(2);
        }
        else
        {
            m_UIName = m_Name;
        }
    }
}

ElementField::ElementField(const Composite* type)
: Field ( type )
, m_TypeID ( Reflect::GetType<Reflect::Element>() )
{

}

ElementField::~ElementField()
{

}

ElementField* ElementField::Create(const Composite* type)
{
    return new ElementField( type );
}

SerializerPtr ElementField::CreateSerializer(Element* instance) const
{
    SerializerPtr ser = __super::CreateSerializer(instance);

    if (ser.ReferencesObject())
    {
        PointerSerializer* pointerSerializer = ObjectCast<PointerSerializer>( ser );
        if ( pointerSerializer )
        {
            pointerSerializer->m_TypeID = m_TypeID;
        }
        else
        {
            ElementContainerSerializer* containerSerializer = ObjectCast<ElementContainerSerializer>( ser );
            if ( containerSerializer )
            {
                containerSerializer->m_TypeID = m_TypeID;
            }
        }
    }

    return ser;
}

EnumerationField::EnumerationField(const Composite* type, const Enumeration* enumeration)
: Field ( type )
, m_Enumeration ( enumeration )
{

}

EnumerationField::~EnumerationField()
{

}

EnumerationField* EnumerationField::Create(const Composite* type, const Enumeration* enumeration)
{
    return new EnumerationField( type, enumeration );
}

SerializerPtr EnumerationField::CreateSerializer(Element* instance) const
{
    EnumerationSerializerPtr ser = AssertCast<EnumerationSerializer>(__super::CreateSerializer(instance));

    if (ser.ReferencesObject())
    {
        ser->m_Enumeration = m_Enumeration;
    }

    return ser;
}
