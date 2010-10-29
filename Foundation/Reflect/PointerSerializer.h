#pragma once

#include "Serializer.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API PointerSerializer : public Reflect::Serializer
        {
        public:
            typedef Reflect::ElementPtr DataType;
            Serializer::DataPtr<DataType> m_Data;
            int32_t m_TypeID;

            REFLECT_DECLARE_CLASS( PointerSerializer, Reflect::Serializer );

            PointerSerializer ();
            ~PointerSerializer();

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual bool Set(const Reflect::Serializer* s, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Reflect::Serializer* s) const HELIUM_OVERRIDE;
            virtual void Accept(Reflect::Visitor& visitor) HELIUM_OVERRIDE;

            virtual void Serialize (Reflect::Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize (Reflect::Archive& archive) HELIUM_OVERRIDE;
        };

        typedef Helium::SmartPtr<PointerSerializer> PointerSerializerPtr;
    }
}