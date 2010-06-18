#pragma once

#include "Serializer.h"

namespace Reflect
{
  class FOUNDATION_API TypeIDSerializer : public Reflect::Serializer
  {
  public:
    typedef Reflect::TypeID DataType;
    Serializer::DataPtr<DataType> m_Data;

    REFLECT_DECLARE_CLASS( TypeIDSerializer, Reflect::Serializer );

    TypeIDSerializer ();
    virtual ~TypeIDSerializer();

    virtual bool IsCompact() const NOC_OVERRIDE;

    virtual void ConnectData(Nocturnal::HybridPtr<void> data) NOC_OVERRIDE;

    virtual bool Set(const Reflect::Serializer* s, u32 flags = 0) NOC_OVERRIDE;
    virtual bool Equals(const Reflect::Serializer* s) const NOC_OVERRIDE;

    virtual void Serialize (Reflect::Archive& archive) const NOC_OVERRIDE;
    virtual void Deserialize (Reflect::Archive& archive) NOC_OVERRIDE;
  };

  REFLECT_SPECIALIZE_SERIALIZER( TypeIDSerializer );
}