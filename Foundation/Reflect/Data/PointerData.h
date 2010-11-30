#pragma once

#include "Foundation/Reflect/Data/Data.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API PointerData : public Reflect::Data
        {
        public:
            typedef Reflect::ElementPtr DataType;
            Data::Pointer<DataType> m_Data;
            int32_t m_TypeID;

            REFLECT_DECLARE_CLASS( PointerData, Reflect::Data );

            PointerData ();
            ~PointerData();

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual bool Set(const Reflect::Data* s, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Reflect::Data* s) const HELIUM_OVERRIDE;
            virtual void Accept(Reflect::Visitor& visitor) HELIUM_OVERRIDE;

            virtual void Serialize (Reflect::Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize (Reflect::Archive& archive) HELIUM_OVERRIDE;
        };

        typedef Helium::SmartPtr<PointerData> PointerDataPtr;
    }
}