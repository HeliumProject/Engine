#pragma once

#include <set>

#include "Registry.h"
#include "SimpleSerializer.h"
#include "ContainerSerializer.h"
#include "PathSerializer.h"

namespace Reflect
{
  class REFLECT_API SetSerializer : public ContainerSerializer
  {
  public:
    REFLECT_DECLARE_ABSTRACT( SetSerializer, ContainerSerializer );

    virtual i32 GetItemType() const = 0;
    virtual void GetItems(V_ConstSerializer& items) const = 0;
    virtual void AddItem(const Serializer* value) = 0;
    virtual void RemoveItem(const Serializer* value) = 0;
    virtual bool ContainsItem(const Serializer* value) const = 0;
  };

  template <class DataT, class DataSer>
  class REFLECT_API SimpleSetSerializer : public SetSerializer
  {
  public:
    typedef std::set<DataT> DataType;
    Serializer::DataPtr<DataType> m_Data;

    typedef SimpleSetSerializer<DataT, DataSer> SetSerializerT;
    REFLECT_DECLARE_CLASS( SetSerializerT, SetSerializer )

    SimpleSetSerializer();
    virtual ~SimpleSetSerializer();

    virtual void ConnectData(Nocturnal::HybridPtr<void> data) NOC_OVERRIDE;

    virtual size_t GetSize() const NOC_OVERRIDE;
    virtual void Clear() NOC_OVERRIDE;

    virtual i32 GetItemType() const NOC_OVERRIDE;
    virtual void GetItems(V_ConstSerializer& items) const NOC_OVERRIDE;
    virtual void AddItem(const Serializer* value) NOC_OVERRIDE;
    virtual void RemoveItem(const Serializer* value) NOC_OVERRIDE;
    virtual bool ContainsItem(const Serializer* value) const NOC_OVERRIDE;

    virtual bool Set(const Serializer* src, u32 flags = 0) NOC_OVERRIDE;
    virtual bool Equals(const Serializer* s) const NOC_OVERRIDE;

    virtual void Serialize(Archive& archive) const NOC_OVERRIDE;
    virtual void Deserialize(Archive& archive) NOC_OVERRIDE;

    virtual std::ostream& operator >> (std::ostream& stream) const NOC_OVERRIDE;
    virtual std::istream& operator << (std::istream& stream) NOC_OVERRIDE;
  };

  typedef SimpleSetSerializer<std::string, StringSerializer> StringSetSerializer;      REFLECT_SPECIALIZE_SERIALIZER( StringSetSerializer );
  typedef SimpleSetSerializer<u32, U32Serializer> U32SetSerializer;                    REFLECT_SPECIALIZE_SERIALIZER( U32SetSerializer );
  typedef SimpleSetSerializer<u64, U64Serializer> U64SetSerializer;                    REFLECT_SPECIALIZE_SERIALIZER( U64SetSerializer );
  typedef SimpleSetSerializer<f32, F32Serializer> F32SetSerializer;                    REFLECT_SPECIALIZE_SERIALIZER( F32SetSerializer );
  typedef SimpleSetSerializer<Nocturnal::UID::GUID, GUIDSerializer> GUIDSetSerializer; REFLECT_SPECIALIZE_SERIALIZER( GUIDSetSerializer );
  typedef SimpleSetSerializer<Nocturnal::UID::TUID, TUIDSerializer> TUIDSetSerializer; REFLECT_SPECIALIZE_SERIALIZER( TUIDSetSerializer );
  typedef SimpleSetSerializer< Nocturnal::Path, PathSerializer > PathSetSerializer;    REFLECT_SPECIALIZE_SERIALIZER( PathSetSerializer );
}
