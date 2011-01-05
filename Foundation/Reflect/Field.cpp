#include "Field.h"
#include "Registry.h"
#include "Foundation/Reflect/Data/DataDeduction.h"
#include "ArchiveBinary.h"

#include "Foundation/Log.h"

using namespace Helium::Reflect;

Field::Field(const Composite* composite)
: m_Composite( composite )
, m_Flags( 0 )
, m_Index( -1 )
, m_Type( NULL )
, m_DataClass( NULL )
, m_Offset( -1 )
, m_Creator( NULL )
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

    if ( m_DataClass != NULL )
    {
        ObjectPtr object = Registry::GetInstance()->CreateInstance( m_DataClass );

        if (object.ReferencesObject())
        {
            ser = AssertCast<Data>(object);
        }
    }

    if ( ser.ReferencesObject() )
    {
        if ( instance )
        {
            ser->ConnectField( instance, this );
        }

        const Class* classType = ReflectionCast< Class >( m_Type );
        if ( classType )
        {
            PointerData* pointerData = ObjectCast<PointerData>( ser );
            if ( pointerData )
            {
                pointerData->m_Type = m_Type;
            }
            else
            {
                ElementContainerData* containerData = ObjectCast<ElementContainerData>( ser );
                if ( containerData )
                {
                    containerData->m_Type = m_Type;
                }
            }
        }
    }

    return ser;
}

bool Field::HasDefaultValue(Element* instance) const
{
#ifdef REFLECT_REFACTOR
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
#endif

    return false;
}

bool Field::SetDefaultValue(Element* instance) const
{
#ifdef REFLECT_REFACTOR
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
#endif

    return false;
}
