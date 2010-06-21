#include "stdafx.h"
#include "Class.h"
#include "Element.h"
#include "Registry.h"
#include "Serializers.h"
#include "ArchiveBinary.h"

#include "Foundation/Log.h"

using namespace Reflect;

Class::Class()
: m_Create (NULL)
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

    ElementPtr clone = AssertCast<Element>( Registry::GetInstance()->CreateInstance(element->GetType()) );

    element->PreSerialize();

    clone->PreDeserialize();

    Class::Copy( element, clone );

    clone->PostDeserialize();

    element->PostSerialize();

    return clone;
}