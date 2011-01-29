#include "Foundation/Reflect/Data/ContainerData.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_ABSTRACT( ContainerData )

ContainerData::ContainerData()
{

}

bool ContainerData::ShouldSerialize()
{
    return Base::ShouldSerialize() && GetSize() > 0;
}
