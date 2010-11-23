#include "Foundation/Reflect/ContainerSerializer.h"
#include "Foundation/Reflect/SerializerDeduction.h"

using namespace Helium::Reflect;

REFLECT_DEFINE_ABSTRACT( ContainerSerializer )

ContainerSerializer::ContainerSerializer()
{

}

REFLECT_DEFINE_ABSTRACT( ElementContainerSerializer )

ElementContainerSerializer::ElementContainerSerializer()
: m_TypeID ( ReservedTypes::Invalid )
{

}