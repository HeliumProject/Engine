#pragma once

#include <set>

#include "Registry.h"
#include "SimpleSerializer.h"
#include "ContainerSerializer.h"
#include "PathSerializer.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API SetSerializer : public ContainerSerializer
        {
        public:
            REFLECT_DECLARE_ABSTRACT( SetSerializer, ContainerSerializer );

            virtual int32_t GetItemType() const = 0;
            virtual void GetItems(V_ConstSerializer& items) const = 0;
            virtual void AddItem(const Serializer* value) = 0;
            virtual void RemoveItem(const Serializer* value) = 0;
            virtual bool ContainsItem(const Serializer* value) const = 0;
        };

        template <class DataT, class DataSer>
        class FOUNDATION_API SimpleSetSerializer : public SetSerializer
        {
        public:
            typedef std::set<DataT> DataType;
            Serializer::DataPtr<DataType> m_Data;

            typedef SimpleSetSerializer<DataT, DataSer> SetSerializerT;
            REFLECT_DECLARE_CLASS( SetSerializerT, SetSerializer )

                SimpleSetSerializer();
            virtual ~SimpleSetSerializer();

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual int32_t GetItemType() const HELIUM_OVERRIDE;
            virtual void GetItems(V_ConstSerializer& items) const HELIUM_OVERRIDE;
            virtual void AddItem(const Serializer* value) HELIUM_OVERRIDE;
            virtual void RemoveItem(const Serializer* value) HELIUM_OVERRIDE;
            virtual bool ContainsItem(const Serializer* value) const HELIUM_OVERRIDE;

            virtual bool Set(const Serializer* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Serializer* s) const HELIUM_OVERRIDE;

            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

            virtual tostream& operator>> (tostream& stream) const HELIUM_OVERRIDE;
            virtual tistream& operator<< (tistream& stream) HELIUM_OVERRIDE;
        };

        typedef SimpleSetSerializer<tstring, StringSerializer> StringSetSerializer;         REFLECT_SPECIALIZE_SERIALIZER( StringSetSerializer );
        typedef SimpleSetSerializer<uint32_t, U32Serializer> U32SetSerializer;                   REFLECT_SPECIALIZE_SERIALIZER( U32SetSerializer );
        typedef SimpleSetSerializer<uint64_t, U64Serializer> U64SetSerializer;                   REFLECT_SPECIALIZE_SERIALIZER( U64SetSerializer );
        typedef SimpleSetSerializer<float32_t, F32Serializer> F32SetSerializer;                   REFLECT_SPECIALIZE_SERIALIZER( F32SetSerializer );
        typedef SimpleSetSerializer<Helium::GUID, GUIDSerializer> GUIDSetSerializer;     REFLECT_SPECIALIZE_SERIALIZER( GUIDSetSerializer );
        typedef SimpleSetSerializer<Helium::TUID, TUIDSerializer> TUIDSetSerializer;     REFLECT_SPECIALIZE_SERIALIZER( TUIDSetSerializer );
        typedef SimpleSetSerializer<Helium::Path, PathSerializer> PathSetSerializer;     REFLECT_SPECIALIZE_SERIALIZER( PathSetSerializer );
    }
}