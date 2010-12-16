#include "Class.h"
#include "Element.h"
#include "Registry.h"
#include "Foundation/Reflect/Data/DataDeduction.h"
#include "ArchiveBinary.h"

#include "Foundation/Log.h"

using namespace Helium;
using namespace Helium::Reflect;

template<>
Class* Class::Create< Object >( Name name, Name baseName, CreateObjectFunc creator )
{
    Class* info = Class::Create();

    info->m_Size = sizeof( Object );
    info->m_Name = name;
    info->m_UIName = *info->m_Name;

    // object should have no creator
    HELIUM_ASSERT( creator == NULL );

    // object should have no base class
    HELIUM_ASSERT( baseName.IsEmpty() );

    return info;
}

Class::Class()
: m_Creator (NULL)
{

}

Class::~Class()
{

}

Class* Class::Create()
{
    return new Class();
}

ElementPtr Class::Clone(Element* element)
{
    if (!element)
    {
        return NULL;
    }

    ElementPtr clone = AssertCast<Element>( Registry::GetInstance()->CreateInstance( element->GetClass()) );

    element->PreSerialize();

    clone->PreDeserialize();

    Class::Copy( element, clone );

    clone->PostDeserialize();

    element->PostSerialize();

    return clone;
}