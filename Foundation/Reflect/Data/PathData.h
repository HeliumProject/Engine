#pragma once

#include "Foundation/Reflect/Data/Data.h"

#include "Foundation/File/Path.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API PathData : public Data
        {
        public:
            typedef Helium::Path DataType;
            Data::Pointer< DataType > m_Data;

            REFLECT_DECLARE_CLASS( PathData, Data );

            PathData();
            ~PathData();

            virtual bool IsCompact() const HELIUM_OVERRIDE { return true; }

            virtual void ConnectData( Helium::HybridPtr< void > data ) HELIUM_OVERRIDE;

            virtual bool Set( const Data* src, uint32_t flags = 0 ) HELIUM_OVERRIDE;
            virtual bool Equals( const Object* object ) const HELIUM_OVERRIDE;

            virtual void Serialize( Archive& archive ) const HELIUM_OVERRIDE;
            virtual void Deserialize( Archive& archive ) HELIUM_OVERRIDE;

            virtual tostream& operator>> (tostream& stream) const HELIUM_OVERRIDE;
            virtual tistream& operator<< (tistream& stream) HELIUM_OVERRIDE;
        };

        typedef Helium::SmartPtr< PathData > PathDataPtr;
    }
}