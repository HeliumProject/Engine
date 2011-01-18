#pragma once

#include "Foundation/String.h"
#include "Foundation/Container/SortedMap.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Data/TypeIDData.h"
#include "Foundation/Reflect/Data/SimpleData.h"
#include "Foundation/Reflect/Data/ContainerData.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API ObjectSortedMapData : public ContainerData
        {
        public:
            REFLECT_DECLARE_ABSTRACT( ObjectSortedMapData, ContainerData );

            typedef Pair< ConstDataPtr, ObjectPtr* > ValueType;
            typedef DynArray< ValueType > A_ValueType;

            typedef Pair< ConstDataPtr, const ObjectPtr* > ConstValueType;
            typedef DynArray< ConstValueType > A_ConstValueType;

            virtual const Class* GetKeyClass() const = 0;
            virtual void GetItems( A_ValueType& items ) = 0;
            virtual void GetItems( A_ConstValueType& items ) const = 0;
            virtual ObjectPtr* GetItem( const Data* key ) = 0;
            virtual const ObjectPtr* GetItem( const Data* key ) const = 0;
            virtual void SetItem( const Data* key, const Object* value ) = 0;
            virtual void RemoveItem( const Data* key ) = 0;
        };

        template< typename KeyT, typename CompareKeyT = Less< KeyT >, typename AllocatorT = DefaultAllocator >
        class FOUNDATION_API SimpleObjectSortedMapData : public ObjectSortedMapData
        {
        public:
            typedef SortedMap< KeyT, ObjectPtr, CompareKeyT, AllocatorT > DataType;
            Data::Pointer< DataType > m_Data;

            typedef SimpleObjectSortedMapData< KeyT, CompareKeyT, AllocatorT > ObjectSortedMapDataT;
            REFLECT_DECLARE_OBJECT( ObjectSortedMapDataT, ObjectSortedMapData )

            SimpleObjectSortedMapData();
            virtual ~SimpleObjectSortedMapData();

            virtual void ConnectData( Helium::HybridPtr< void > data ) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual const Class* GetKeyClass() const HELIUM_OVERRIDE;
            virtual void GetItems( A_ValueType& items ) HELIUM_OVERRIDE;
            virtual void GetItems( A_ConstValueType& items ) const HELIUM_OVERRIDE;
            virtual ObjectPtr* GetItem( const Data* key ) HELIUM_OVERRIDE;
            virtual const ObjectPtr* GetItem( const Data* key ) const HELIUM_OVERRIDE;
            virtual void SetItem( const Data* key, const Object* value ) HELIUM_OVERRIDE;
            virtual void RemoveItem( const Data* key ) HELIUM_OVERRIDE;

            virtual bool Set( const Data* src, uint32_t flags = 0 ) HELIUM_OVERRIDE;
            virtual bool Equals( const Object* object ) const HELIUM_OVERRIDE;

            virtual void Serialize( Archive& archive ) const HELIUM_OVERRIDE;
            virtual void Deserialize( Archive& archive ) HELIUM_OVERRIDE;

            virtual void Accept( Visitor& visitor ) HELIUM_OVERRIDE;
        };

        typedef SimpleObjectSortedMapData< TypeID > TypeIDObjectSortedMapData;
        typedef SimpleObjectSortedMapData< String > StringObjectSortedMapData;
        typedef SimpleObjectSortedMapData< uint32_t > UInt32ObjectSortedMapData;
        typedef SimpleObjectSortedMapData< int32_t > Int32ObjectSortedMapData;
        typedef SimpleObjectSortedMapData< uint64_t > UInt64ObjectSortedMapData;
        typedef SimpleObjectSortedMapData< int64_t > Int64ObjectSortedMapData;
        typedef SimpleObjectSortedMapData< Helium::GUID > GUIDObjectSortedMapData;
        typedef SimpleObjectSortedMapData< Helium::TUID > TUIDObjectSortedMapData;
    }
}
