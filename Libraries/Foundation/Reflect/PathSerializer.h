#pragma once

#include "Serializer.h"

#include "Foundation/File/Path.h"

namespace Reflect
{
    class FOUNDATION_API PathSerializer : public Serializer
    {
    public:
        typedef Nocturnal::Path DataType;
        Serializer::DataPtr< DataType > m_Data;

        REFLECT_DECLARE_CLASS( PathSerializer, Serializer );

        PathSerializer();
        virtual ~PathSerializer();

        virtual bool IsCompact () const  NOC_OVERRIDE;

        virtual void ConnectData( Nocturnal::HybridPtr< void > data ) NOC_OVERRIDE;

        virtual bool Set( const Serializer* src, u32 flags = 0 ) NOC_OVERRIDE;
        virtual bool Equals( const Serializer* s ) const NOC_OVERRIDE;

        virtual void Serialize( Archive& archive ) const NOC_OVERRIDE;
        virtual void Deserialize( Archive& archive ) NOC_OVERRIDE;

        virtual std::ostream& operator >> (std::ostream& stream) const NOC_OVERRIDE;
        virtual std::istream& operator << (std::istream& stream) NOC_OVERRIDE;
    };

    typedef Nocturnal::SmartPtr< PathSerializer > PathSerializerPtr;

    template<> static inline int Serializer::DeduceType< PathSerializer::DataType >()
    {
        return Reflect::GetType< PathSerializer >();
    }
}
