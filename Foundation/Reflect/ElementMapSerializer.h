#pragma once

#include <map>

#include "Registry.h"
#include "TypeIDSerializer.h"
#include "SimpleSerializer.h"
#include "ContainerSerializer.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API ElementMapSerializer : public ContainerSerializer
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
        class FOUNDATION_API SimpleElementMapSerializer : public ElementMapSerializer
        {
        public:
            typedef std::map<KeyT, ElementPtr> DataType;
            Serializer::DataPtr<DataType> m_Data;

            typedef SimpleElementMapSerializer<KeyT> ElementMapSerializerT;
            REFLECT_DECLARE_CLASS( ElementMapSerializerT, ElementMapSerializer )

                SimpleElementMapSerializer();
            virtual ~SimpleElementMapSerializer();

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual i32 GetKeyType() const HELIUM_OVERRIDE;
            virtual void GetItems(V_ValueType& items) HELIUM_OVERRIDE;
            virtual void GetItems(V_ConstValueType& items) const HELIUM_OVERRIDE;
            virtual ElementPtr* GetItem(const Serializer* key) HELIUM_OVERRIDE;
            virtual const ElementPtr* GetItem(const Serializer* key) const HELIUM_OVERRIDE;
            virtual void SetItem(const Serializer* key, const Element* value) HELIUM_OVERRIDE;
            virtual void RemoveItem(const Serializer* key) HELIUM_OVERRIDE;

            virtual bool Set(const Serializer* src, u32 flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Serializer* s) const HELIUM_OVERRIDE;

            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

            virtual void Accept(Visitor& visitor) HELIUM_OVERRIDE;
        };

        typedef SimpleElementMapSerializer<TypeID> TypeIDElementMapSerializer;              REFLECT_SPECIALIZE_SERIALIZER(TypeIDElementMapSerializer);
        typedef SimpleElementMapSerializer<std::string> StringElementMapSerializer;         REFLECT_SPECIALIZE_SERIALIZER(StringElementMapSerializer);
        typedef SimpleElementMapSerializer<u32> U32ElementMapSerializer;                    REFLECT_SPECIALIZE_SERIALIZER(U32ElementMapSerializer);
        typedef SimpleElementMapSerializer<i32> I32ElementMapSerializer;                    REFLECT_SPECIALIZE_SERIALIZER(I32ElementMapSerializer);
        typedef SimpleElementMapSerializer<u64> U64ElementMapSerializer;                    REFLECT_SPECIALIZE_SERIALIZER(U64ElementMapSerializer);
        typedef SimpleElementMapSerializer<i64> I64ElementMapSerializer;                    REFLECT_SPECIALIZE_SERIALIZER(I64ElementMapSerializer);
        typedef SimpleElementMapSerializer<Helium::GUID> GUIDElementMapSerializer;  REFLECT_SPECIALIZE_SERIALIZER(GUIDElementMapSerializer);
        typedef SimpleElementMapSerializer<Helium::TUID> TUIDElementMapSerializer;  REFLECT_SPECIALIZE_SERIALIZER(TUIDElementMapSerializer);
    }
}