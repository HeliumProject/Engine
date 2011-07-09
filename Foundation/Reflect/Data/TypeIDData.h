#pragma once

#include "Foundation/Reflect/Data/Data.h"

namespace Helium
{
    namespace Reflect
    {
        class HELIUM_FOUNDATION_API TypeIDData : public Data
        {
        public:
            typedef TypeID DataType;
            DataPointer<DataType> m_Data;

            REFLECT_DECLARE_OBJECT( TypeIDData, Data );

            TypeIDData();
            ~TypeIDData();

            virtual bool IsCompact() const HELIUM_OVERRIDE { return true; }

            virtual void ConnectData(void* data) HELIUM_OVERRIDE;

            virtual bool Set(Data* data, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(Object* object) HELIUM_OVERRIDE;

            virtual void Serialize(ArchiveBinary& archive) HELIUM_OVERRIDE;
            virtual void Deserialize(ArchiveBinary& archive) HELIUM_OVERRIDE;

            virtual void Serialize(ArchiveXML& archive) HELIUM_OVERRIDE;
            virtual void Deserialize(ArchiveXML& archive) HELIUM_OVERRIDE;
        };
    }
}