#pragma once

#include "Foundation/Reflect/Data/Data.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API StructureData : public Data
        {
        public:
            typedef void DataType;
            VoidDataPointer m_Data;

            REFLECT_DECLARE_OBJECT( StructureData, Data );

            StructureData();
            ~StructureData();

            virtual void ConnectData(void* data) HELIUM_OVERRIDE;

            virtual bool Set(Data* data, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(Object* object) HELIUM_OVERRIDE;
            virtual void Accept(Visitor& visitor) HELIUM_OVERRIDE;

            virtual void Serialize(Archive& archive) HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;
        };

        typedef Helium::SmartPtr<StructureData> AggregateDataPtr;
    }
}