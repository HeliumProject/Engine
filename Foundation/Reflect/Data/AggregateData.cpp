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

void AggregateData::ConnectData(Helium::HybridPtr<void> data)
{

}

bool AggregateData::Set(const Data* s, uint32_t flags)
{
    return false;
}

bool AggregateData::Equals(const Object* object) const
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