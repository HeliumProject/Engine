#include "ContainerSerializer.h"
#include "Serializers.h"

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