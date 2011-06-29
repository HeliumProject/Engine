#pragma once

#include "Foundation/Container/Set.h"
#include "Foundation/String.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Data/SimpleData.h"
#include "Foundation/Reflect/Data/ContainerData.h"
#include "Foundation/Reflect/Data/PathData.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API SetData : public ContainerData
        {
        public:
            REFLECT_DECLARE_ABSTRACT( SetData, ContainerData );

            virtual const Class* GetItemClass() const = 0;
            virtual void GetItems( DynArray< DataPtr >& items ) const = 0;
            virtual void AddItem( Data* value ) = 0;
            virtual void RemoveItem( Data* value ) = 0;
            virtual bool ContainsItem( Data* value ) const = 0;
        };

        template< typename KeyT, typename EqualKeyT = Equals< KeyT >, typename AllocatorT = DefaultAllocator >
        class FOUNDATION_API SimpleSetData : public SetData
        {
        public:
            typedef Helium::Set< KeyT, EqualKeyT, AllocatorT > DataType;
            DataPointer< DataType > m_Data;

            typedef SimpleSetData< KeyT, EqualKeyT, AllocatorT > SetDataT;
            REFLECT_DECLARE_OBJECT( SetDataT, SetData )

            SimpleSetData();
            ~SimpleSetData();

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

        typedef SimpleSetData< String > StringSetData;
        typedef SimpleSetData< uint32_t > UInt32SetData;
        typedef SimpleSetData< uint64_t > UInt64SetData;
        typedef SimpleSetData< Helium::GUID > GUIDSetData;
        typedef SimpleSetData< Helium::TUID > TUIDSetData;
        typedef SimpleSetData< Helium::Path > PathSetData;
    }
}
