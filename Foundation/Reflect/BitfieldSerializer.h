#pragma once

#include "EnumerationSerializer.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API BitfieldSerializer : public EnumerationSerializer
        {
        public:
            REFLECT_DECLARE_CLASS( BitfieldSerializer, EnumerationSerializer )

                BitfieldSerializer ();
            virtual ~BitfieldSerializer();

            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

            virtual tostream& operator>> (tostream& stream) const;
            virtual tistream& operator<< (tistream& stream);
        };

        typedef Helium::SmartPtr<BitfieldSerializer> BitfieldSerializerPtr;
    }
}