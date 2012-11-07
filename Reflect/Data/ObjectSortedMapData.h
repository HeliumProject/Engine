#pragma once

#include "Foundation/String.h"
#include "Foundation/SortedMap.h"
#include "Reflect/Registry.h"
#include "Reflect/Data/TypeIDData.h"
#include "Reflect/Data/SimpleData.h"
#include "Reflect/Data/ContainerData.h"

namespace Helium
{
    namespace Reflect
    {
        class HELIUM_REFLECT_API ObjectSortedMapData : public ContainerData
        {
        public:
            REFLECT_DECLARE_ABSTRACT( ObjectSortedMapData, ContainerData );

            typedef Pair< DataPtr, ObjectPtr* > ValueType;
            typedef DynArray< ValueType > A_ValueType;

            virtual const Class* GetKeyClass() const = 0;
            virtual void GetItems( A_ValueType& items ) = 0;
            virtual ObjectPtr* GetItem( Data* key ) = 0;
            virtual void SetItem( Data* key, Object* value ) = 0;
            virtual void RemoveItem( Data* key ) = 0;
        };

        template< typename KeyT, typename CompareKeyT = Less< KeyT >, typename AllocatorT = DefaultAllocator >
        class HELIUM_REFLECT_API SimpleObjectSortedMapData : public ObjectSortedMapData
        {
        public:
            typedef SortedMap< KeyT, ObjectPtr, CompareKeyT, AllocatorT > DataType;
            DataPointer< DataType > m_Data;

            typedef SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT > ObjectSortedMapDataT;
            REFLECT_DECLARE_OBJECT( ObjectSortedMapDataT, ObjectSortedMapData )

            SimpleObjectSortedMapData();
            ~SimpleObjectSortedMapData();

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

        typedef SimpleObjectSortedMapData< TypeID > TypeIDObjectSortedMapData;
        typedef SimpleObjectSortedMapData< String > StringObjectSortedMapData;
        typedef SimpleObjectSortedMapData< uint32_t > UInt32ObjectSortedMapData;
        typedef SimpleObjectSortedMapData< int32_t > Int32ObjectSortedMapData;
        typedef SimpleObjectSortedMapData< uint64_t > UInt64ObjectSortedMapData;
        typedef SimpleObjectSortedMapData< int64_t > Int64ObjectSortedMapData;
        typedef SimpleObjectSortedMapData< Helium::TUID > TUIDObjectSortedMapData;
    }
}
