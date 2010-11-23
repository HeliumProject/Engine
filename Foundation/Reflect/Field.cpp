#include "Field.h"
#include "Registry.h"
#include "Foundation/Reflect/Data/DataDeduction.h"
#include "ArchiveBinary.h"

#include "Foundation/Log.h"

using namespace Helium::Reflect;

Field::Field(const Composite* type)
: m_Type ( type )
, m_Flags ( 0 )
, m_FieldID ( -1 )
, m_DataID ( ReservedTypes::Invalid )
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

DataPtr Field::CreateData(Element* instance) const
{
    DataPtr ser;

    if (m_DataID != Reflect::ReservedTypes::Invalid)
    {
        ObjectPtr object = Registry::GetInstance()->CreateInstance(m_DataID);

        if (object.ReferencesObject())
        {
            ser = AssertCast<Data>(object);
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
    DataPtr serializer = CreateData();

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
    DataPtr serializer = CreateData();

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

DataPtr ElementField::CreateData(Element* instance) const
{
    DataPtr ser = __super::CreateData(instance);

    if (ser.ReferencesObject())
    {
        PointerData* pointerData = ObjectCast<PointerData>( ser );
        if ( pointerData )
        {
            pointerData->m_TypeID = m_TypeID;
        }
        else
        {
            ElementContainerData* containerData = ObjectCast<ElementContainerData>( ser );
            if ( containerData )
            {
                containerData->m_TypeID = m_TypeID;
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

DataPtr EnumerationField::CreateData(Element* instance) const
{
    EnumerationDataPtr ser = AssertCast<EnumerationData>(__super::CreateData(instance));

    if (ser.ReferencesObject())
    {
        ser->m_Enumeration = m_Enumeration;
    }

    return ser;
}
