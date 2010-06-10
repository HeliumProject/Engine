#pragma once

#include <map>

#include "Registry.h"
#include "TypeIDSerializer.h"
#include "SimpleSerializer.h"
#include "ContainerSerializer.h"

namespace Reflect
{
  class REFLECT_API ElementMapSerializer : public ContainerSerializer
  {
  public:
    REFLECT_DECLARE_ABSTRACT( ElementMapSerializer, ContainerSerializer );

    typedef std::pair< ConstSerializerPtr, ElementPtr* > ValueType;
    typedef std::vector< ValueType > V_ValueType;

    typedef std::pair< ConstSerializerPtr, const ElementPtr* > ConstValueType;
    typedef std::vector< ConstValueType > V_ConstValueType;

    virtual i32 GetKeyType() const = 0;
    virtual void GetItems(V_ValueType& items) = 0;
    virtual void GetItems(V_ConstValueType& items) const = 0;
    virtual ElementPtr* GetItem(const Serializer* key) = 0;
    virtual const ElementPtr* GetItem(const Serializer* key) const = 0;
    virtual void SetItem(const Serializer* key, const Element* value) = 0;
    virtual void RemoveItem(const Serializer* key) = 0;
  };

  template <class KeyT>
  class REFLECT_API SimpleElementMapSerializer : public ElementMapSerializer
  {
  public:
    typedef std::map<KeyT, ElementPtr> DataType;
    Serializer::DataPtr<DataType> m_Data;

    typedef SimpleElementMapSerializer<KeyT> ElementMapSerializerT;
    REFLECT_DECLARE_CLASS( ElementMapSerializerT, ElementMapSerializer )

    SimpleElementMapSerializer();
    virtual ~SimpleElementMapSerializer();

    virtual void ConnectData(Nocturnal::HybridPtr<void> data) NOC_OVERRIDE;

    virtual size_t GetSize() const NOC_OVERRIDE;
    virtual void Clear() NOC_OVERRIDE;

    virtual i32 GetKeyType() const NOC_OVERRIDE;
    virtual void GetItems(V_ValueType& items) NOC_OVERRIDE;
    virtual void GetItems(V_ConstValueType& items) const NOC_OVERRIDE;
    virtual ElementPtr* GetItem(const Serializer* key) NOC_OVERRIDE;
    virtual const ElementPtr* GetItem(const Serializer* key) const NOC_OVERRIDE;
    virtual void SetItem(const Serializer* key, const Element* value) NOC_OVERRIDE;
    virtual void RemoveItem(const Serializer* key) NOC_OVERRIDE;

    virtual bool Set(const Serializer* src, u32 flags = 0) NOC_OVERRIDE;
    virtual bool Equals(const Serializer* s) const NOC_OVERRIDE;

    virtual void Serialize(Archive& archive) const NOC_OVERRIDE;
    virtual void Deserialize(Archive& archive) NOC_OVERRIDE;

    virtual void Host(Visitor& visitor) NOC_OVERRIDE;
  };

  typedef SimpleElementMapSerializer<TypeID> TypeIDElementMapSerializer;              REFLECT_SPECIALIZE_SERIALIZER(TypeIDElementMapSerializer);
  typedef SimpleElementMapSerializer<std::string> StringElementMapSerializer;         REFLECT_SPECIALIZE_SERIALIZER(StringElementMapSerializer);
  typedef SimpleElementMapSerializer<u32> U32ElementMapSerializer;                    REFLECT_SPECIALIZE_SERIALIZER(U32ElementMapSerializer);
  typedef SimpleElementMapSerializer<i32> I32ElementMapSerializer;                    REFLECT_SPECIALIZE_SERIALIZER(I32ElementMapSerializer);
  typedef SimpleElementMapSerializer<u64> U64ElementMapSerializer;                    REFLECT_SPECIALIZE_SERIALIZER(U64ElementMapSerializer);
  typedef SimpleElementMapSerializer<i64> I64ElementMapSerializer;                    REFLECT_SPECIALIZE_SERIALIZER(I64ElementMapSerializer);
  typedef SimpleElementMapSerializer<Nocturnal::UID::GUID> GUIDElementMapSerializer;  REFLECT_SPECIALIZE_SERIALIZER(GUIDElementMapSerializer);
  typedef SimpleElementMapSerializer<Nocturnal::UID::TUID> TUIDElementMapSerializer;  REFLECT_SPECIALIZE_SERIALIZER(TUIDElementMapSerializer);
}
