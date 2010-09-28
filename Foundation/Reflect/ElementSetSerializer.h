#pragma once

#include <set>

#include "Registry.h"
#include "SimpleSerializer.h"
#include "ContainerSerializer.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API ElementSetSerializer : public ContainerSerializer
        {
        public:
            typedef std::set<ElementPtr> DataType;
            Serializer::DataPtr<DataType> m_Data;

            REFLECT_DECLARE_CLASS( ElementSetSerializer, ContainerSerializer )

                ElementSetSerializer();
            virtual ~ElementSetSerializer();

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual bool Set(const Serializer* src, u32 flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Serializer* s) const HELIUM_OVERRIDE;

            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

            virtual void Accept(Visitor& visitor) HELIUM_OVERRIDE;
        };
    }
}