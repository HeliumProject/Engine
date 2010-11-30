#pragma once

#include "Foundation/Reflect/Data/Data.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API TypeIDData : public Reflect::Data
        {
        public:
            typedef Reflect::TypeID DataType;
            Data::Pointer<DataType> m_Data;

            REFLECT_DECLARE_CLASS( TypeIDData, Reflect::Data );

            TypeIDData();
            ~TypeIDData();

            virtual bool IsCompact() const HELIUM_OVERRIDE { return true; }

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual bool Set(const Reflect::Data* s, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Reflect::Data* s) const HELIUM_OVERRIDE;

            virtual void Serialize (Reflect::Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize (Reflect::Archive& archive) HELIUM_OVERRIDE;
        };
    }
}