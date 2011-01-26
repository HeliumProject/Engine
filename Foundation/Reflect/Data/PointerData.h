#pragma once

#include "Foundation/Reflect/Data/Data.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API PointerData : public Data
        {
        public:
            typedef ObjectPtr DataType;
            DataPointer<DataType> m_Data;

            REFLECT_DECLARE_OBJECT( PointerData, Data );

            PointerData ();
            ~PointerData();

            virtual void ConnectData(void* data) HELIUM_OVERRIDE;

            virtual bool Set(Data* data, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(Object* object) HELIUM_OVERRIDE;
            virtual void Accept(Visitor& visitor) HELIUM_OVERRIDE;

            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;
        };

        typedef Helium::SmartPtr<PointerData> PointerDataPtr;
    }
}