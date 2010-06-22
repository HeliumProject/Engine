#pragma once

#include "Serializer.h"

namespace Reflect
{
    class FOUNDATION_API PointerSerializer : public Reflect::Serializer
    {
    public:
        typedef Reflect::ElementPtr DataType;
        Serializer::DataPtr<DataType> m_Data;
        i32 m_TypeID;

        REFLECT_DECLARE_CLASS( PointerSerializer, Reflect::Serializer );

        PointerSerializer ();
        virtual ~PointerSerializer();

        virtual void ConnectData(Nocturnal::HybridPtr<void> data) NOC_OVERRIDE;

        virtual bool Set(const Reflect::Serializer* s, u32 flags = 0) NOC_OVERRIDE;
        virtual bool Equals(const Reflect::Serializer* s) const NOC_OVERRIDE;
        virtual void Host(Reflect::Visitor& visitor) NOC_OVERRIDE;

        virtual void Serialize (Reflect::Archive& archive) const NOC_OVERRIDE;
        virtual void Deserialize (Reflect::Archive& archive) NOC_OVERRIDE;
    };

    typedef Nocturnal::SmartPtr<PointerSerializer> PointerSerializerPtr;
}