#include "ContainerSerializer.h"
#include "Serializers.h"

using namespace Helium::Reflect;

REFLECT_DEFINE_ABSTRACT( ContainerSerializer )

ContainerSerializer::ContainerSerializer()
{

}

ElementContainerSerializer::ElementContainerSerializer()
: m_TypeID ( ReservedTypes::Invalid )
{

}