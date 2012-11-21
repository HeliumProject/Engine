#pragma once

#include "Foundation/String.h"
#include "Reflect/Registry.h"
#include "Reflect/Data/SimpleData.h"
#include "Reflect/Data/ContainerData.h"

namespace Helium
{
    namespace Reflect
    {
        class HELIUM_REFLECT_API DynamicArrayData : public ContainerData
        {
        public:
            REFLECT_DECLARE_ABSTRACT( DynamicArrayData, ContainerData );

            virtual void SetSize( size_t size ) = 0;

            virtual const Class* GetItemClass() const = 0;
            virtual DataPtr GetItem( size_t at ) = 0;
            virtual void SetItem( size_t at, Data* value ) = 0;
            virtual void Insert( size_t at, Data* value ) = 0;
            virtual void Remove( size_t at ) = 0;
            virtual void MoveUp( std::set< size_t >& selectedIndices ) = 0;
            virtual void MoveDown( std::set< size_t >& selectedIndices ) = 0;
        };

        template< class T >
        class HELIUM_REFLECT_API SimpleDynamicArrayData : public DynamicArrayData
        {
        public:
            typedef DynamicArray< T > DataType;
            DataPointer< DataType > m_Data;

            typedef SimpleDynamicArrayData< T > DynamicArrayDataT;
            REFLECT_DECLARE_OBJECT( DynamicArrayDataT, DynamicArrayData )

            SimpleDynamicArrayData();
            ~SimpleDynamicArrayData();

            virtual void ConnectData( void* data ) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void SetSize( size_t size ) HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual const Class* GetItemClass() const HELIUM_OVERRIDE;
            virtual DataPtr GetItem( size_t at ) HELIUM_OVERRIDE;
            virtual void SetItem( size_t at, Data* value ) HELIUM_OVERRIDE;
            virtual void Insert( size_t at, Data* value ) HELIUM_OVERRIDE;
            virtual void Remove( size_t at ) HELIUM_OVERRIDE;
            virtual void MoveUp( std::set< size_t >& selectedIndices ) HELIUM_OVERRIDE;
            virtual void MoveDown( std::set< size_t >& selectedIndices ) HELIUM_OVERRIDE;

            virtual bool Set( Data* src, uint32_t flags = 0 ) HELIUM_OVERRIDE;
            virtual bool Equals( Object* object ) HELIUM_OVERRIDE;

            virtual void Serialize( ArchiveBinary& archive ) HELIUM_OVERRIDE;
            virtual void Deserialize( ArchiveBinary& archive ) HELIUM_OVERRIDE;

            virtual void Serialize( ArchiveXML& archive ) HELIUM_OVERRIDE;
            virtual void Deserialize( ArchiveXML& archive ) HELIUM_OVERRIDE;

            virtual tostream& operator>>( tostream& stream ) const HELIUM_OVERRIDE;
            virtual tistream& operator<<( tistream& stream ) HELIUM_OVERRIDE;
        };
        
        typedef SimpleDynamicArrayData< Name > NameDynamicArrayData;
        typedef SimpleDynamicArrayData< String > StringDynamicArrayData;
        typedef SimpleDynamicArrayData< bool > BoolDynamicArrayData;
        typedef SimpleDynamicArrayData< uint8_t > UInt8DynamicArrayData;
        typedef SimpleDynamicArrayData< int8_t > Int8DynamicArrayData;
        typedef SimpleDynamicArrayData< uint16_t > UInt16DynamicArrayData;
        typedef SimpleDynamicArrayData< int16_t > Int16DynamicArrayData;
        typedef SimpleDynamicArrayData< uint32_t > UInt32DynamicArrayData;
        typedef SimpleDynamicArrayData< int32_t > Int32DynamicArrayData;
        typedef SimpleDynamicArrayData< uint64_t > UInt64DynamicArrayData;
        typedef SimpleDynamicArrayData< int64_t > Int64DynamicArrayData;
        typedef SimpleDynamicArrayData< float32_t > Float32DynamicArrayData;
        typedef SimpleDynamicArrayData< float64_t > Float64DynamicArrayData;
        typedef SimpleDynamicArrayData< Helium::TUID > TUIDDynamicArrayData;
        typedef SimpleDynamicArrayData< Helium::FilePath > PathDynamicArrayData;
    }
}
