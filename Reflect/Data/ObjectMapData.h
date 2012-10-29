#pragma once

#include "Foundation/String.h"
#include "Foundation/Container/Map.h"
#include "Reflect/Registry.h"
#include "Reflect/Data/TypeIDData.h"
#include "Reflect/Data/SimpleData.h"
#include "Reflect/Data/ContainerData.h"

namespace Helium
{
    namespace Reflect
    {
        class HELIUM_REFLECT_API ObjectMapData : public ContainerData
        {
        public:
            REFLECT_DECLARE_ABSTRACT( ObjectMapData, ContainerData );

            typedef Pair< DataPtr, ObjectPtr* > ValueType;
            typedef DynArray< ValueType > A_ValueType;

            virtual const Class* GetKeyClass() const = 0;
            virtual void GetItems( A_ValueType& items ) = 0;
            virtual ObjectPtr* GetItem( Data* key ) = 0;
            virtual void SetItem( Data* key, Object* value ) = 0;
            virtual void RemoveItem( Data* key ) = 0;
        };

        template< typename KeyT, typename EqualKeyT = Equals< KeyT >, typename AllocatorT = DefaultAllocator >
        class HELIUM_REFLECT_API SimpleObjectMapData : public ObjectMapData
        {
        public:
            typedef Map< KeyT, ObjectPtr, EqualKeyT, AllocatorT > DataType;
            DataPointer< DataType > m_Data;

            typedef SimpleObjectMapData< KeyT, EqualKeyT, AllocatorT > ObjectMapDataT;
            REFLECT_DECLARE_OBJECT( ObjectMapDataT, ObjectMapData )

            SimpleObjectMapData();
            ~SimpleObjectMapData();

            virtual void ConnectData( void* data ) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual const Class* GetKeyClass() const HELIUM_OVERRIDE;
            virtual void GetItems( A_ValueType& items ) HELIUM_OVERRIDE;
            virtual ObjectPtr* GetItem( Data* key ) HELIUM_OVERRIDE;
            virtual void SetItem( Data* key, Object* value ) HELIUM_OVERRIDE;
            virtual void RemoveItem( Data* key ) HELIUM_OVERRIDE;

            virtual bool Set( Data* src, uint32_t flags = 0 ) HELIUM_OVERRIDE;
            virtual bool Equals( Object* object ) HELIUM_OVERRIDE;
            virtual void Accept( Visitor& visitor ) HELIUM_OVERRIDE;

            virtual void Serialize( ArchiveBinary& archive ) HELIUM_OVERRIDE;
            virtual void Deserialize( ArchiveBinary& archive ) HELIUM_OVERRIDE;

            virtual void Serialize( ArchiveXML& archive ) HELIUM_OVERRIDE;
            virtual void Deserialize( ArchiveXML& archive ) HELIUM_OVERRIDE;

		private:
			template< class ArchiveT >
            void Serialize( ArchiveT& archive );
			template< class ArchiveT >
            void Deserialize( ArchiveT& archive );
        };

        typedef SimpleObjectMapData< TypeID > TypeIDObjectMapData;
        typedef SimpleObjectMapData< String > StringObjectMapData;
        typedef SimpleObjectMapData< uint32_t > UInt32ObjectMapData;
        typedef SimpleObjectMapData< int32_t > Int32ObjectMapData;
        typedef SimpleObjectMapData< uint64_t > UInt64ObjectMapData;
        typedef SimpleObjectMapData< int64_t > Int64ObjectMapData;
        typedef SimpleObjectMapData< Helium::GUID > GUIDObjectMapData;
        typedef SimpleObjectMapData< Helium::TUID > TUIDObjectMapData;
    }
}
