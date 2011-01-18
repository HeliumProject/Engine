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
            virtual void GetItems( DynArray< ConstDataPtr >& items ) const = 0;
            virtual void AddItem( const Data* value ) = 0;
            virtual void RemoveItem( const Data* value ) = 0;
            virtual bool ContainsItem( const Data* value ) const = 0;
        };

        template< typename KeyT, typename EqualKeyT = Equals< KeyT >, typename AllocatorT = DefaultAllocator >
        class FOUNDATION_API SimpleSetData : public SetData
        {
        public:
            typedef Helium::Set< KeyT, EqualKeyT, AllocatorT > DataType;
            Data::Pointer< DataType > m_Data;

            typedef SimpleSetData< KeyT, EqualKeyT, AllocatorT > SetDataT;
            REFLECT_DECLARE_OBJECT( SetDataT, SetData )

            SimpleSetData();
            virtual ~SimpleSetData();

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

        typedef SimpleSetData< String > StringSetData;
        typedef SimpleSetData< uint32_t > UInt32SetData;
        typedef SimpleSetData< uint64_t > UInt64SetData;
        typedef SimpleSetData< Helium::GUID > GUIDSetData;
        typedef SimpleSetData< Helium::TUID > TUIDSetData;
        typedef SimpleSetData< Helium::Path > PathSetData;
    }
}
