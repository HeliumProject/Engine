#pragma once

#include <set>

#include "Registry.h"
#include "SimpleSerializer.h"
#include "ContainerSerializer.h"

namespace Reflect
{
  class REFLECT_API ElementSetSerializer : public ContainerSerializer
  {
  public:
    typedef std::set<ElementPtr> DataType;
    Serializer::DataPtr<DataType> m_Data;

    REFLECT_DECLARE_CLASS( ElementSetSerializer, ContainerSerializer )

    ElementSetSerializer();
    virtual ~ElementSetSerializer();

    virtual void ConnectData(Nocturnal::HybridPtr<void> data) NOC_OVERRIDE;

    virtual size_t GetSize() const NOC_OVERRIDE;
    virtual void Clear() NOC_OVERRIDE;

    virtual bool Set(const Serializer* src, u32 flags = 0) NOC_OVERRIDE;
    virtual bool Equals(const Serializer* s) const NOC_OVERRIDE;

    virtual void Serialize(Archive& archive) const NOC_OVERRIDE;
    virtual void Deserialize(Archive& archive) NOC_OVERRIDE;

    virtual void Host(Visitor& visitor) NOC_OVERRIDE;
  };
}
