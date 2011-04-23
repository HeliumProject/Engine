#pragma once

#include "Foundation/Reflect/Data/Data.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API EnumerationData : public Data
        {
        public:
            typedef uint32_t DataType;
            DataPointer<DataType> m_Data;
            tstring m_String;

            REFLECT_DECLARE_OBJECT( EnumerationData, Data )

            EnumerationData ();
            ~EnumerationData();

            virtual bool IsCompact() const HELIUM_OVERRIDE { return true; }

            virtual void ConnectData(void* data) HELIUM_OVERRIDE;

            virtual bool Set(Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(Object* object) HELIUM_OVERRIDE;

            virtual void Serialize(ArchiveBinary& archive) HELIUM_OVERRIDE;
            virtual void Deserialize(ArchiveBinary& archive) HELIUM_OVERRIDE;

            virtual void Serialize(ArchiveXML& archive) HELIUM_OVERRIDE;
            virtual void Deserialize(ArchiveXML& archive) HELIUM_OVERRIDE;

            virtual tostream& operator>>(tostream& stream) const HELIUM_OVERRIDE;
            virtual tistream& operator<<(tistream& stream) HELIUM_OVERRIDE;
        };

        typedef Helium::StrongPtr<EnumerationData> EnumerationDataPtr;
    }
}