#pragma once

#include "Serializer.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API TypeIDSerializer : public Reflect::Serializer
        {
        public:
            typedef Reflect::TypeID DataType;
            Serializer::DataPtr<DataType> m_Data;

            REFLECT_DECLARE_CLASS( TypeIDSerializer, Reflect::Serializer );

            TypeIDSerializer();
            ~TypeIDSerializer();

            virtual bool IsCompact() const HELIUM_OVERRIDE { return true; }

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual bool Set(const Reflect::Serializer* s, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Reflect::Serializer* s) const HELIUM_OVERRIDE;

            virtual void Serialize (Reflect::Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize (Reflect::Archive& archive) HELIUM_OVERRIDE;
        };

        REFLECT_SPECIALIZE_SERIALIZER( TypeIDSerializer );
    }
}