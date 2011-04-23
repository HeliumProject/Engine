#pragma once

#include "EnumerationData.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API BitfieldData : public EnumerationData
        {
        public:
            REFLECT_DECLARE_OBJECT( BitfieldData, EnumerationData )

            BitfieldData ();
            ~BitfieldData();

            virtual void Serialize(ArchiveBinary& archive) HELIUM_OVERRIDE;
            virtual void Deserialize(ArchiveBinary& archive) HELIUM_OVERRIDE;

            virtual void Serialize(ArchiveXML& archive) HELIUM_OVERRIDE;
            virtual void Deserialize(ArchiveXML& archive) HELIUM_OVERRIDE;

            virtual tostream& operator>>(tostream& stream) const;
            virtual tistream& operator<<(tistream& stream);
        };

        typedef Helium::SmartPtr<BitfieldData> BitfieldDataPtr;
    }
}