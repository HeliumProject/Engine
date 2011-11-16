#include "FoundationPch.h"
#include "Foundation/Reflect/Data/ContainerData.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

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
