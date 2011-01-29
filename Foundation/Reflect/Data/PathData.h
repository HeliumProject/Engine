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
            DataPointer< DataType > m_Data;

            REFLECT_DECLARE_OBJECT( PathData, Data );

            PathData();
            ~PathData();

            virtual bool IsCompact() const HELIUM_OVERRIDE { return true; }

            virtual void ConnectData( void* data ) HELIUM_OVERRIDE;

            virtual bool Set( Data* src, uint32_t flags = 0 ) HELIUM_OVERRIDE;
            virtual bool Equals( Object* object ) HELIUM_OVERRIDE;

            virtual void Serialize( ArchiveBinary& archive ) HELIUM_OVERRIDE;
            virtual void Deserialize( ArchiveBinary& archive ) HELIUM_OVERRIDE;

            virtual void Serialize( ArchiveXML& archive ) HELIUM_OVERRIDE;
            virtual void Deserialize( ArchiveXML& archive ) HELIUM_OVERRIDE;

            virtual tostream& operator>>(tostream& stream) const HELIUM_OVERRIDE;
            virtual tistream& operator<<(tistream& stream) HELIUM_OVERRIDE;
        };

        typedef Helium::SmartPtr< PathData > PathDataPtr;
    }
}