#pragma once

#include <map>

#include "Registry.h"
#include "SimpleSerializer.h"
#include "ContainerSerializer.h"

#include "Foundation/Math/Matrix4.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API MapSerializer : public ContainerSerializer
        {
        public:
            REFLECT_DECLARE_ABSTRACT( MapSerializer, ContainerSerializer );

            typedef std::pair< ConstSerializerPtr, SerializerPtr > ValueType;
            typedef std::vector< ValueType > V_ValueType;

            typedef std::pair< ConstSerializerPtr, ConstSerializerPtr > ConstValueType;
            typedef std::vector< ConstValueType > V_ConstValueType;

            virtual int32_t GetKeyType() const = 0;
            virtual int32_t GetValueType() const = 0;
            virtual void GetItems(V_ValueType& items) = 0;
            virtual void GetItems(V_ConstValueType& items) const = 0;
            virtual SerializerPtr GetItem(const Serializer* key) = 0;
            virtual ConstSerializerPtr GetItem(const Serializer* key) const = 0;
            virtual void SetItem(const Serializer* key, const Serializer* value) = 0;
            virtual void RemoveItem(const Serializer* key) = 0;
        };

        template <class KeyT, class KeySer, class ValueT, class ValueSer>
        class FOUNDATION_API SimpleMapSerializer : public MapSerializer
        {
        public:
            typedef std::map<KeyT, ValueT> DataType;
            Serializer::DataPtr<DataType> m_Data;

            typedef SimpleMapSerializer< KeyT, KeySer, ValueT, ValueSer > MapSerializerT;
            REFLECT_DECLARE_CLASS( MapSerializerT, MapSerializer );

            SimpleMapSerializer();
            virtual ~SimpleMapSerializer();

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual int32_t GetKeyType() const HELIUM_OVERRIDE;
            virtual int32_t GetValueType() const HELIUM_OVERRIDE;
            virtual void GetItems(V_ValueType& items) HELIUM_OVERRIDE;
            virtual void GetItems(V_ConstValueType& items) const HELIUM_OVERRIDE;
            virtual SerializerPtr GetItem(const Serializer* key) HELIUM_OVERRIDE;
            virtual ConstSerializerPtr GetItem(const Serializer* key) const HELIUM_OVERRIDE;
            virtual void SetItem(const Serializer* key, const Serializer* value) HELIUM_OVERRIDE;
            virtual void RemoveItem(const Serializer* key) HELIUM_OVERRIDE;

            virtual bool Set(const Serializer* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Serializer* s) const HELIUM_OVERRIDE;

            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

            virtual tostream& operator>> (tostream& stream) const HELIUM_OVERRIDE;
            virtual tistream& operator<< (tistream& stream) HELIUM_OVERRIDE;
        };

        typedef SimpleMapSerializer<tstring, StringSerializer, tstring, StringSerializer> StringStringMapSerializer;                REFLECT_SPECIALIZE_SERIALIZER(StringStringMapSerializer);
        typedef SimpleMapSerializer<tstring, StringSerializer, bool, BoolSerializer> StringBoolMapSerializer;                       REFLECT_SPECIALIZE_SERIALIZER(StringBoolMapSerializer);
        typedef SimpleMapSerializer<tstring, StringSerializer, uint32_t, U32Serializer> StringU32MapSerializer;                          REFLECT_SPECIALIZE_SERIALIZER(StringU32MapSerializer);
        typedef SimpleMapSerializer<tstring, StringSerializer, int32_t, I32Serializer> StringI32MapSerializer;                          REFLECT_SPECIALIZE_SERIALIZER(StringI32MapSerializer);

        typedef SimpleMapSerializer<uint32_t, U32Serializer, tstring, StringSerializer> U32StringMapSerializer;                          REFLECT_SPECIALIZE_SERIALIZER(U32StringMapSerializer);
        typedef SimpleMapSerializer<uint32_t, U32Serializer, uint32_t, U32Serializer> U32U32MapSerializer;                                    REFLECT_SPECIALIZE_SERIALIZER(U32U32MapSerializer);
        typedef SimpleMapSerializer<uint32_t, U32Serializer, int32_t, I32Serializer> U32I32MapSerializer;                                    REFLECT_SPECIALIZE_SERIALIZER(U32I32MapSerializer);
        typedef SimpleMapSerializer<uint32_t, U32Serializer, uint64_t, U64Serializer> U32U64MapSerializer;                                    REFLECT_SPECIALIZE_SERIALIZER(U32U64MapSerializer);

        typedef SimpleMapSerializer<int32_t, I32Serializer, tstring, StringSerializer> I32StringMapSerializer;                          REFLECT_SPECIALIZE_SERIALIZER(I32StringMapSerializer);
        typedef SimpleMapSerializer<int32_t, I32Serializer, uint32_t, U32Serializer> I32U32MapSerializer;                                    REFLECT_SPECIALIZE_SERIALIZER(I32U32MapSerializer);
        typedef SimpleMapSerializer<int32_t, I32Serializer, int32_t, I32Serializer> I32I32MapSerializer;                                    REFLECT_SPECIALIZE_SERIALIZER(I32I32MapSerializer);
        typedef SimpleMapSerializer<int32_t, I32Serializer, uint64_t, U64Serializer> I32U64MapSerializer;                                    REFLECT_SPECIALIZE_SERIALIZER(I32U64MapSerializer);

        typedef SimpleMapSerializer<uint64_t, U64Serializer, tstring, StringSerializer> U64StringMapSerializer;                          REFLECT_SPECIALIZE_SERIALIZER(U64StringMapSerializer);
        typedef SimpleMapSerializer<uint64_t, U64Serializer, uint32_t, U32Serializer> U64U32MapSerializer;                                    REFLECT_SPECIALIZE_SERIALIZER(U64U32MapSerializer);
        typedef SimpleMapSerializer<uint64_t, U64Serializer, uint64_t, U64Serializer> U64U64MapSerializer;                                    REFLECT_SPECIALIZE_SERIALIZER(U64U64MapSerializer);
        typedef SimpleMapSerializer<uint64_t, U64Serializer, Math::Matrix4, Matrix4Serializer> U64Matrix4MapSerializer;                  REFLECT_SPECIALIZE_SERIALIZER(U64Matrix4MapSerializer);

        typedef SimpleMapSerializer<Helium::GUID, GUIDSerializer, uint32_t, U32Serializer> GUIDU32MapSerializer;                      REFLECT_SPECIALIZE_SERIALIZER(GUIDU32MapSerializer);
        typedef SimpleMapSerializer<Helium::GUID, GUIDSerializer, Math::Matrix4, Matrix4Serializer> GUIDMatrix4MapSerializer;    REFLECT_SPECIALIZE_SERIALIZER(GUIDMatrix4MapSerializer);
        typedef SimpleMapSerializer<Helium::TUID, TUIDSerializer, uint32_t, U32Serializer> TUIDU32MapSerializer;                      REFLECT_SPECIALIZE_SERIALIZER(TUIDU32MapSerializer);
        typedef SimpleMapSerializer<Helium::TUID, TUIDSerializer, Math::Matrix4, Matrix4Serializer> TUIDMatrix4MapSerializer;    REFLECT_SPECIALIZE_SERIALIZER(TUIDMatrix4MapSerializer);
    }
}