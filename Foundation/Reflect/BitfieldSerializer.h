#pragma once

#include "EnumerationSerializer.h"

namespace Reflect
{
    class FOUNDATION_API BitfieldSerializer : public EnumerationSerializer
    {
    public:
        REFLECT_DECLARE_CLASS( BitfieldSerializer, EnumerationSerializer )

        BitfieldSerializer ();
        virtual ~BitfieldSerializer();

        virtual void Serialize(Archive& archive) const NOC_OVERRIDE;
        virtual void Deserialize(Archive& archive) NOC_OVERRIDE;

        virtual tostream& operator>> (tostream& stream) const;
        virtual tistream& operator<< (tistream& stream);
    };

    typedef Nocturnal::SmartPtr<BitfieldSerializer> BitfieldSerializerPtr;
}
