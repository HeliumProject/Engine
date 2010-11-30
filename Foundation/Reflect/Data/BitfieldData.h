#pragma once

#include "EnumerationData.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API BitfieldData : public EnumerationData
        {
        public:
            REFLECT_DECLARE_CLASS( BitfieldData, EnumerationData )

            BitfieldData ();
            ~BitfieldData();

            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

            virtual tostream& operator>> (tostream& stream) const;
            virtual tistream& operator<< (tistream& stream);
        };

        typedef Helium::SmartPtr<BitfieldData> BitfieldDataPtr;
    }
}