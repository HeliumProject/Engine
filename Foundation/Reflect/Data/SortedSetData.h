#pragma once

#include "Foundation/Container/SortedSet.h"
#include "Foundation/String.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Data/SimpleData.h"
#include "Foundation/Reflect/Data/ContainerData.h"
#include "Foundation/Reflect/Data/PathData.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API SortedSetData : public ContainerData
        {
        public:
            REFLECT_DECLARE_ABSTRACT( SortedSetData, ContainerData );

            virtual const Class* GetItemClass() const = 0;
            virtual void GetItems( DynArray< ConstDataPtr >& items ) const = 0;
            virtual void AddItem( const Data* value ) = 0;
            virtual void RemoveItem( const Data* value ) = 0;
            virtual bool ContainsItem( const Data* value ) const = 0;
        };

        template< typename KeyT, typename CompareKeyT = Less< KeyT >, typename AllocatorT = DefaultAllocator >
        class FOUNDATION_API SimpleSortedSetData : public SortedSetData
        {
        public:
            typedef SortedSet< KeyT, CompareKeyT, AllocatorT > DataType;
            Data::Pointer< DataType > m_Data;

            typedef SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT > SortedSetDataT;
            REFLECT_DECLARE_OBJECT( SortedSetDataT, SortedSetData )

            SimpleSortedSetData();
            virtual ~SimpleSortedSetData();

            virtual void ConnectData( Helium::HybridPtr< void > data ) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual const Class* GetItemClass() const HELIUM_OVERRIDE;
            virtual void GetItems( DynArray< ConstDataPtr >& items ) const HELIUM_OVERRIDE;
            virtual void AddItem( const Data* value ) HELIUM_OVERRIDE;
            virtual void RemoveItem( const Data* value ) HELIUM_OVERRIDE;
            virtual bool ContainsItem( const Data* value ) const HELIUM_OVERRIDE;

            virtual bool Set( const Data* src, uint32_t flags = 0 ) HELIUM_OVERRIDE;
            virtual bool Equals( const Object* object ) const HELIUM_OVERRIDE;

            virtual void Serialize( Archive& archive ) const HELIUM_OVERRIDE;
            virtual void Deserialize( Archive& archive ) HELIUM_OVERRIDE;

            virtual tostream& operator>>( tostream& stream ) const HELIUM_OVERRIDE;
            virtual tistream& operator<<( tistream& stream ) HELIUM_OVERRIDE;
        };

        typedef SimpleSortedSetData< String > StringSortedSetData;
        typedef SimpleSortedSetData< uint32_t > UInt32SortedSetData;
        typedef SimpleSortedSetData< uint64_t > UInt64SortedSetData;
        typedef SimpleSortedSetData< Helium::GUID > GUIDSortedSetData;
        typedef SimpleSortedSetData< Helium::TUID > TUIDSortedSetData;
        typedef SimpleSortedSetData< Helium::Path > PathSortedSetData;
    }
}
