#pragma once

#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Data/SimpleData.h"
#include "Foundation/Reflect/Data/ContainerData.h"
#include "Foundation/Reflect/Data/DynArrayData.h"

namespace Helium
{
    namespace Reflect
    {

        class HELIUM_FOUNDATION_API StructureDynArrayData : public DynArrayData
        {
        public:
            typedef void DataType;
            VoidDataPointer m_Data;

            REFLECT_DECLARE_OBJECT( StructureDynArrayData, DynArrayData )

            StructureDynArrayData();
            ~StructureDynArrayData();

            //DynArrayData Interface
            
            virtual void SetSize( size_t size );

            virtual const Class* GetItemClass() const;
            virtual DataPtr GetItem( size_t at );
            virtual void SetItem( size_t at, Data* value );
            virtual void Insert( size_t at, Data* value );
            virtual void Remove( size_t at );
            virtual void MoveUp( std::set< size_t >& selectedIndices );
            virtual void MoveDown( std::set< size_t >& selectedIndices );

            virtual void ConnectData( void* data ) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual bool Set( Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
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

            const Structure* GetInternalStructure();

            void *GetInternalPtr(const Structure* structure);
        };
        typedef Helium::SmartPtr< StructureDynArrayData > StructureDynArrayDataPtr;
    }
}
