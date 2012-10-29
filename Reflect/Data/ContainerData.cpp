#include "ReflectPch.h"
#include "Reflect/Data/ContainerData.h"

#include "Reflect/Data/DataDeduction.h"

REFLECT_DEFINE_ABSTRACT( Helium::Reflect::ContainerData )

using namespace Helium;
using namespace Helium::Reflect;

ContainerData::ContainerData()
{

}

bool ContainerData::ShouldSerialize()
{
    return Base::ShouldSerialize() && GetSize() > 0;
}
