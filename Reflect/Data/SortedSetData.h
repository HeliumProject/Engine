#pragma once

#include "Foundation/SortedSet.h"
#include "Foundation/String.h"
#include "Reflect/Registry.h"
#include "Reflect/Data/SimpleData.h"
#include "Reflect/Data/ContainerData.h"
#include "Reflect/Data/PathData.h"

namespace Helium
{
    namespace Reflect
    {
        class HELIUM_REFLECT_API SortedSetData : public ContainerData
        {
        public:
            REFLECT_DECLARE_ABSTRACT( SortedSetData, ContainerData );

            virtual const Class* GetItemClass() const = 0;
            virtual void GetItems( DynArray< DataPtr >& items ) const = 0;
            virtual void AddItem( Data* value ) = 0;
            virtual void RemoveItem( Data* value ) = 0;
            virtual bool ContainsItem( Data* value ) const = 0;
        };

        template< typename KeyT, typename CompareKeyT = Less< KeyT >, typename AllocatorT = DefaultAllocator >
        class HELIUM_REFLECT_API SimpleSortedSetData : public SortedSetData
        {
        public:
            typedef SortedSet< KeyT, CompareKeyT, AllocatorT > DataType;
            DataPointer< DataType > m_Data;

            typedef SimpleSortedSetData< KeyT, CompareKeyT, AllocatorT > SortedSetDataT;
            REFLECT_DECLARE_OBJECT( SortedSetDataT, SortedSetData )

            SimpleSortedSetData();
            ~SimpleSortedSetData();

            virtual void ConnectData( void* data ) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual const Class* GetItemClass() const HELIUM_OVERRIDE;
            virtual void GetItems( DynArray< DataPtr >& items ) const HELIUM_OVERRIDE;
            virtual void AddItem( Data* value ) HELIUM_OVERRIDE;
            virtual void RemoveItem( Data* value ) HELIUM_OVERRIDE;
            virtual bool ContainsItem( Data* value ) const HELIUM_OVERRIDE;

            virtual bool Set( Data* src, uint32_t flags = 0 ) HELIUM_OVERRIDE;
            virtual bool Equals( Object* object ) HELIUM_OVERRIDE;

            virtual void Serialize( ArchiveBinary& archive ) HELIUM_OVERRIDE;
            virtual void Deserialize( ArchiveBinary& archive ) HELIUM_OVERRIDE;
            
            virtual void Serialize( ArchiveXML& archive ) HELIUM_OVERRIDE;
            virtual void Deserialize( ArchiveXML& archive ) HELIUM_OVERRIDE;

            virtual tostream& operator>>( tostream& stream ) const HELIUM_OVERRIDE;
            virtual tistream& operator<<( tistream& stream ) HELIUM_OVERRIDE;

		private:
			template< class ArchiveT >
            void Serialize( ArchiveT& archive );
			template< class ArchiveT >
            void Deserialize( ArchiveT& archive );
		};

        typedef SimpleSortedSetData< String > StringSortedSetData;
        typedef SimpleSortedSetData< uint32_t > UInt32SortedSetData;
        typedef SimpleSortedSetData< uint64_t > UInt64SortedSetData;
        typedef SimpleSortedSetData< Helium::TUID > TUIDSortedSetData;
        typedef SimpleSortedSetData< Helium::FilePath > PathSortedSetData;
    }
}
