#pragma once

#include "Serializer.h"

namespace Helium
{
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
            ~EnumerationSerializer();

            virtual bool IsCompact() const HELIUM_OVERRIDE { return true; }

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;
            virtual void ConnectField(Helium::HybridPtr<Element> instance, const Field* field, uintptr offsetInField = 0) HELIUM_OVERRIDE;

            virtual bool Set(const Serializer* src, u32 flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Serializer* s) const HELIUM_OVERRIDE;

            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

            virtual tostream& operator>> (tostream& stream) const HELIUM_OVERRIDE;
            virtual tistream& operator<< (tistream& stream) HELIUM_OVERRIDE;
        };

        typedef Helium::SmartPtr<EnumerationSerializer> EnumerationSerializerPtr;
    }
}