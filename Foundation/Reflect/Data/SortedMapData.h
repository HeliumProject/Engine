#pragma once

#include "Foundation/Container/SortedMap.h"
#include "Foundation/String.h"
#include "Foundation/Math/Matrix4.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Data/SimpleData.h"
#include "Foundation/Reflect/Data/ContainerData.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API SortedMapData : public ContainerData
        {
        public:
            REFLECT_DECLARE_ABSTRACT( SortedMapData, ContainerData );

            typedef Pair< DataPtr, DataPtr > ValueType;
            typedef DynArray< ValueType > A_ValueType;

            virtual const Class* GetKeyClass() const = 0;
            virtual const Class* GetValueClass() const = 0;
            virtual void GetItems( A_ValueType& items ) = 0;
            virtual DataPtr GetItem( Data* key ) = 0;
            virtual void SetItem( Data* key, Data* value ) = 0;
            virtual void RemoveItem( Data* key ) = 0;
        };

        template< typename KeyT, typename ValueT, typename CompareKeyT = Less< KeyT >, typename AllocatorT = DefaultAllocator >
        class FOUNDATION_API SimpleSortedMapData : public SortedMapData
        {
        public:
            typedef SortedMap< KeyT, ValueT, CompareKeyT, AllocatorT > DataType;
            DataPointer< DataType > m_Data;

            typedef SimpleSortedMapData< KeyT, ValueT, CompareKeyT, AllocatorT > SortedMapDataT;
            REFLECT_DECLARE_OBJECT( SortedMapDataT, SortedMapData );

            SimpleSortedMapData();
            virtual ~SimpleSortedMapData();

            virtual void ConnectData( void* data ) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual const Class* GetKeyClass() const HELIUM_OVERRIDE;
            virtual const Class* GetValueClass() const HELIUM_OVERRIDE;
            virtual void GetItems( A_ValueType& items ) HELIUM_OVERRIDE;
            virtual DataPtr GetItem( Data* key ) HELIUM_OVERRIDE;
            virtual void SetItem( Data* key, Data* value ) HELIUM_OVERRIDE;
            virtual void RemoveItem( Data* key ) HELIUM_OVERRIDE;

            virtual bool Set( Data* src, uint32_t flags = 0 ) HELIUM_OVERRIDE;
            virtual bool Equals( Object* object ) HELIUM_OVERRIDE;

            virtual void Serialize( Archive& archive ) const HELIUM_OVERRIDE;
            virtual void Deserialize( Archive& archive ) HELIUM_OVERRIDE;

            virtual tostream& operator>>( tostream& stream ) const HELIUM_OVERRIDE;
            virtual tistream& operator<<( tistream& stream ) HELIUM_OVERRIDE;
        };

        typedef SimpleSortedMapData< String, String > StringStringSortedMapData;
        typedef SimpleSortedMapData< String, bool > StringBoolSortedMapData;
        typedef SimpleSortedMapData< String, uint32_t > StringUInt32SortedMapData;
        typedef SimpleSortedMapData< String, int32_t > StringInt32SortedMapData;

        typedef SimpleSortedMapData< uint32_t, String > UInt32StringSortedMapData;
        typedef SimpleSortedMapData< uint32_t, uint32_t > UInt32UInt32SortedMapData;
        typedef SimpleSortedMapData< uint32_t, int32_t > UInt32Int32SortedMapData;
        typedef SimpleSortedMapData< uint32_t, uint64_t > UInt32UInt64SortedMapData;

        typedef SimpleSortedMapData< int32_t, String > Int32StringSortedMapData;
        typedef SimpleSortedMapData< int32_t, uint32_t > Int32UInt32SortedMapData;
        typedef SimpleSortedMapData< int32_t, int32_t > Int32Int32SortedMapData;
        typedef SimpleSortedMapData< int32_t, uint64_t > Int32UInt64SortedMapData;

        typedef SimpleSortedMapData< uint64_t, String > UInt64StringSortedMapData;
        typedef SimpleSortedMapData< uint64_t, uint32_t > UInt64UInt32SortedMapData;
        typedef SimpleSortedMapData< uint64_t, uint64_t > UInt64UInt64SortedMapData;
        typedef SimpleSortedMapData< uint64_t, Matrix4 > UInt64Matrix4SortedMapData;

        typedef SimpleSortedMapData< Helium::GUID, uint32_t > GUIDUInt32SortedMapData;
        typedef SimpleSortedMapData< Helium::GUID, Matrix4 > GUIDMatrix4SortedMapData;
        typedef SimpleSortedMapData< Helium::TUID, uint32_t > TUIDUInt32SortedMapData;
        typedef SimpleSortedMapData< Helium::TUID, Matrix4 > TUIDMatrix4SortedMapData;
    }
}
