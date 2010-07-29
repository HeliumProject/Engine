#pragma once

#include "Serializer.h"

#include "Foundation/File/Path.h"

namespace Reflect
{
    class FOUNDATION_API PathSerializer : public Serializer
    {
    public:
        typedef Helium::Path DataType;
        Serializer::DataPtr< DataType > m_Data;

        REFLECT_DECLARE_CLASS( PathSerializer, Serializer );

        PathSerializer();
        virtual ~PathSerializer();

        virtual bool IsCompact () const  HELIUM_OVERRIDE;

        virtual void ConnectData( Helium::HybridPtr< void > data ) HELIUM_OVERRIDE;

        virtual bool Set( const Serializer* src, u32 flags = 0 ) HELIUM_OVERRIDE;
        virtual bool Equals( const Serializer* s ) const HELIUM_OVERRIDE;

        virtual void Serialize( Archive& archive ) const HELIUM_OVERRIDE;
        virtual void Deserialize( Archive& archive ) HELIUM_OVERRIDE;

        virtual tostream& operator>> (tostream& stream) const HELIUM_OVERRIDE;
        virtual tistream& operator<< (tistream& stream) HELIUM_OVERRIDE;
    };

    typedef Helium::SmartPtr< PathSerializer > PathSerializerPtr;

    template<> static inline int Reflect::GetType< PathSerializer::DataType >()
    {
        return Reflect::GetType< PathSerializer >();
    }
}
