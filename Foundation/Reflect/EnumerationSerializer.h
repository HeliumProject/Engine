#pragma once

#include "Serializer.h"

namespace Reflect
{
    class FOUNDATION_API EnumerationSerializer : public Serializer
    {
    public:
        typedef u32 DataType;
        Serializer::DataPtr<DataType> m_Data;
        const Enumeration* m_Enumeration;
        tstring m_String;

        REFLECT_DECLARE_CLASS( EnumerationSerializer, Serializer )

        EnumerationSerializer ();
        virtual ~EnumerationSerializer();

        virtual bool IsCompact () const  NOC_OVERRIDE;

        virtual void ConnectData(Nocturnal::HybridPtr<void> data) NOC_OVERRIDE;
        virtual void ConnectField(Nocturnal::HybridPtr<Element> instance, const Field* field, uintptr offsetInField = 0) NOC_OVERRIDE;

        virtual bool Set(const Serializer* src, u32 flags = 0) NOC_OVERRIDE;
        virtual bool Equals(const Serializer* s) const NOC_OVERRIDE;

        virtual void Serialize(Archive& archive) const NOC_OVERRIDE;
        virtual void Deserialize(Archive& archive) NOC_OVERRIDE;

        virtual tostream& operator>> (tostream& stream) const NOC_OVERRIDE;
        virtual tistream& operator<< (tistream& stream) NOC_OVERRIDE;
    };

    typedef Nocturnal::SmartPtr<EnumerationSerializer> EnumerationSerializerPtr;
}
