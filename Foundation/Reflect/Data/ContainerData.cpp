#include "Foundation/Reflect/Data/ContainerData.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium::Reflect;

REFLECT_DEFINE_ABSTRACT( ContainerData )

ContainerData::ContainerData()
{

}

REFLECT_DEFINE_ABSTRACT( ElementContainerData )

ElementContainerData::ElementContainerData()
: m_Type ( NULL )
{

}