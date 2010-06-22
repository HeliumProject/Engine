#include "ContainerSerializer.h"
#include "Serializers.h"

using namespace Math;
using namespace Reflect;

REFLECT_DEFINE_ABSTRACT( ContainerSerializer )

ContainerSerializer::ContainerSerializer()
{

}

ElementContainerSerializer::ElementContainerSerializer()
: m_TypeID ( ReservedTypes::Invalid )
{

}