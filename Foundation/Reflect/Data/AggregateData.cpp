#include "Foundation/Reflect/Data/AggregateData.h"

using namespace Helium;
using namespace Helium::Reflect;

REFLECT_DEFINE_OBJECT( AggregateData );

AggregateData::AggregateData()
{

}

AggregateData::~AggregateData()
{

}

void AggregateData::ConnectData(void* data)
{

}

bool AggregateData::Set(Data* data, uint32_t flags)
{
    return false;
}

bool AggregateData::Equals(Object* object)
{
    return false;
}

void AggregateData::Accept(Visitor& visitor)
{

}

void AggregateData::Serialize(Archive& archive) const
{

}

void AggregateData::Deserialize(Archive& archive)
{

}