#pragma once

#include <set>

#include "Reflect/Registry.h"
#include "Reflect/Data/SimpleData.h"
#include "Reflect/Data/ContainerData.h"
#include "Reflect/Data/PathData.h"

namespace Helium
{
    namespace Reflect
    {
        class HELIUM_REFLECT_API StlSetData : public ContainerData
        {
        public:
            REFLECT_DECLARE_ABSTRACT( StlSetData, ContainerData );

            virtual const Class* GetItemClass() const = 0;
            virtual void GetItems(std::vector< DataPtr >& items) const = 0;
            virtual void AddItem(Data* value) = 0;
            virtual void RemoveItem(Data* value) = 0;
            virtual bool ContainsItem(Data* value) const = 0;
        };

        template <class DataT, class DataClassT>
        class HELIUM_REFLECT_API SimpleStlSetData : public StlSetData
        {
        public:
            typedef std::set<DataT> DataType;
            DataPointer<DataType> m_Data;

            typedef SimpleStlSetData<DataT, DataClassT> StlSetDataT;
            REFLECT_DECLARE_OBJECT( StlSetDataT, StlSetData )

            SimpleStlSetData();
            ~SimpleStlSetData();

            virtual void ConnectData(void* data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual const Class* GetItemClass() const HELIUM_OVERRIDE;
            virtual void GetItems(std::vector< DataPtr >& items) const HELIUM_OVERRIDE;
            virtual void AddItem(Data* value) HELIUM_OVERRIDE;
            virtual void RemoveItem(Data* value) HELIUM_OVERRIDE;
            virtual bool ContainsItem(Data* value) const HELIUM_OVERRIDE;

            virtual bool Set(Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(Object* object) HELIUM_OVERRIDE;

            virtual void Serialize( ArchiveBinary& archive ) HELIUM_OVERRIDE;
            virtual void Deserialize( ArchiveBinary& archive ) HELIUM_OVERRIDE;

            virtual void Serialize( ArchiveXML& archive ) HELIUM_OVERRIDE;
            virtual void Deserialize( ArchiveXML& archive ) HELIUM_OVERRIDE;

            virtual tostream& operator>>(tostream& stream) const HELIUM_OVERRIDE;
            virtual tistream& operator<<(tistream& stream) HELIUM_OVERRIDE;

		private:
			template< class ArchiveT >
            void Serialize( ArchiveT& archive );
			template< class ArchiveT >
            void Deserialize( ArchiveT& archive );
		};

        typedef SimpleStlSetData<tstring, StlStringData> StlStringStlSetData;
        typedef SimpleStlSetData<uint32_t, UInt32Data> UInt32StlSetData;
        typedef SimpleStlSetData<uint64_t, UInt64Data> UInt64StlSetData;
        typedef SimpleStlSetData<Helium::TUID, TUIDData> TUIDStlSetData;
        typedef SimpleStlSetData<Helium::FilePath, PathData> PathStlSetData;
    }
}