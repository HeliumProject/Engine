#pragma once

#include <map>

#include "Math/FpuMatrix4.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Data/SimpleData.h"
#include "Foundation/Reflect/Data/ContainerData.h"

namespace Helium
{
    namespace Reflect
    {
        class HELIUM_FOUNDATION_API StlMapData : public ContainerData
        {
        public:
            REFLECT_DECLARE_ABSTRACT( StlMapData, ContainerData );

            typedef std::pair< DataPtr, DataPtr > ValueType;
            typedef std::vector< ValueType > V_ValueType;

            virtual const Class* GetKeyClass() const = 0;
            virtual const Class* GetValueClass() const = 0;
            virtual void GetItems(V_ValueType& items) = 0;
            virtual DataPtr GetItem(Data* key) = 0;
            virtual void SetItem(Data* key, Data* value) = 0;
            virtual void RemoveItem(Data* key) = 0;
        };

        template <class KeyT, class KeyClassT, class ValueT, class ValueClassT>
        class HELIUM_FOUNDATION_API SimpleStlMapData : public StlMapData
        {
        public:
            typedef std::map<KeyT, ValueT> DataType;
            DataPointer<DataType> m_Data;

            typedef SimpleStlMapData< KeyT, KeyClassT, ValueT, ValueClassT > StlMapDataT;
            REFLECT_DECLARE_OBJECT( StlMapDataT, StlMapData );

            SimpleStlMapData();
            ~SimpleStlMapData();

            virtual void ConnectData(void* data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual const Class* GetKeyClass() const HELIUM_OVERRIDE;
            virtual const Class* GetValueClass() const HELIUM_OVERRIDE;
            virtual void GetItems(V_ValueType& items) HELIUM_OVERRIDE;
            virtual DataPtr GetItem(Data* key) HELIUM_OVERRIDE;
            virtual void SetItem(Data* key, Data* value) HELIUM_OVERRIDE;
            virtual void RemoveItem(Data* key) HELIUM_OVERRIDE;

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

        typedef SimpleStlMapData<tstring, StlStringData, tstring, StlStringData> StlStringStlStringStlMapData;
        typedef SimpleStlMapData<tstring, StlStringData, bool, BoolData> StlStringBoolStlMapData;
        typedef SimpleStlMapData<tstring, StlStringData, uint32_t, UInt32Data> StlStringUInt32StlMapData;
        typedef SimpleStlMapData<tstring, StlStringData, int32_t, Int32Data> StlStringInt32StlMapData;

        typedef SimpleStlMapData<uint32_t, UInt32Data, tstring, StlStringData> UInt32StringStlMapData;
        typedef SimpleStlMapData<uint32_t, UInt32Data, uint32_t, UInt32Data> UInt32UInt32StlMapData;
        typedef SimpleStlMapData<uint32_t, UInt32Data, int32_t, Int32Data> UInt32Int32StlMapData;
        typedef SimpleStlMapData<uint32_t, UInt32Data, uint64_t, UInt64Data> UInt32UInt64StlMapData;

        typedef SimpleStlMapData<int32_t, Int32Data, tstring, StlStringData> Int32StringStlMapData;
        typedef SimpleStlMapData<int32_t, Int32Data, uint32_t, UInt32Data> Int32UInt32StlMapData;
        typedef SimpleStlMapData<int32_t, Int32Data, int32_t, Int32Data> Int32Int32StlMapData;
        typedef SimpleStlMapData<int32_t, Int32Data, uint64_t, UInt64Data> Int32UInt64StlMapData;

        typedef SimpleStlMapData<uint64_t, UInt64Data, tstring, StlStringData> UInt64StringStlMapData;
        typedef SimpleStlMapData<uint64_t, UInt64Data, uint32_t, UInt32Data> UInt64UInt32StlMapData;
        typedef SimpleStlMapData<uint64_t, UInt64Data, uint64_t, UInt64Data> UInt64UInt64StlMapData;
        typedef SimpleStlMapData<uint64_t, UInt64Data, Matrix4, Matrix4Data> UInt64Matrix4StlMapData;

        typedef SimpleStlMapData<Helium::GUID, GUIDData, uint32_t, UInt32Data> GUIDUInt32StlMapData;
        typedef SimpleStlMapData<Helium::GUID, GUIDData, Matrix4, Matrix4Data> GUIDMatrix4StlMapData;
        typedef SimpleStlMapData<Helium::TUID, TUIDData, uint32_t, UInt32Data> TUIDUInt32StlMapData;
        typedef SimpleStlMapData<Helium::TUID, TUIDData, Matrix4, Matrix4Data> TUIDMatrix4StlMapData;
    }
}