#pragma once

#include "Foundation/Reflect/Data/Data.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API TypeIDData : public Data
        {
        public:
            typedef TypeID DataType;
            Data::Pointer<DataType> m_Data;

            REFLECT_DECLARE_OBJECT( TypeIDData, Data );

            TypeIDData();
            ~TypeIDData();

            virtual bool IsCompact() const HELIUM_OVERRIDE { return true; }

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual bool Set(const Data* s, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Object* object) const HELIUM_OVERRIDE;

            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;
        };
    }
}