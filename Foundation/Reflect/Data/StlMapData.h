#pragma once

#include <map>

#include "Foundation/Math/Matrix4.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Data/SimpleData.h"
#include "Foundation/Reflect/Data/ContainerData.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API StlMapData : public ContainerData
        {
        public:
            REFLECT_DECLARE_ABSTRACT( StlMapData, ContainerData );

            typedef std::pair< ConstDataPtr, DataPtr > ValueType;
            typedef std::vector< ValueType > V_ValueType;

            typedef std::pair< ConstDataPtr, ConstDataPtr > ConstValueType;
            typedef std::vector< ConstValueType > V_ConstValueType;

            virtual const Class* GetKeyClass() const = 0;
            virtual const Class* GetValueClass() const = 0;
            virtual void GetItems(V_ValueType& items) = 0;
            virtual void GetItems(V_ConstValueType& items) const = 0;
            virtual DataPtr GetItem(const Data* key) = 0;
            virtual ConstDataPtr GetItem(const Data* key) const = 0;
            virtual void SetItem(const Data* key, const Data* value) = 0;
            virtual void RemoveItem(const Data* key) = 0;
        };

        template <class KeyT, class KeyClassT, class ValueT, class ValueClassT>
        class FOUNDATION_API SimpleStlMapData : public StlMapData
        {
        public:
            typedef std::map<KeyT, ValueT> DataType;
            Data::Pointer<DataType> m_Data;

            typedef SimpleStlMapData< KeyT, KeyClassT, ValueT, ValueClassT > StlMapDataT;
            REFLECT_DECLARE_CLASS( StlMapDataT, StlMapData );

            SimpleStlMapData();
            virtual ~SimpleStlMapData();

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual const Class* GetKeyClass() const HELIUM_OVERRIDE;
            virtual const Class* GetValueClass() const HELIUM_OVERRIDE;
            virtual void GetItems(V_ValueType& items) HELIUM_OVERRIDE;
            virtual void GetItems(V_ConstValueType& items) const HELIUM_OVERRIDE;
            virtual DataPtr GetItem(const Data* key) HELIUM_OVERRIDE;
            virtual ConstDataPtr GetItem(const Data* key) const HELIUM_OVERRIDE;
            virtual void SetItem(const Data* key, const Data* value) HELIUM_OVERRIDE;
            virtual void RemoveItem(const Data* key) HELIUM_OVERRIDE;

            virtual bool Set(const Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Object* object) const HELIUM_OVERRIDE;

            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

            virtual tostream& operator>> (tostream& stream) const HELIUM_OVERRIDE;
            virtual tistream& operator<< (tistream& stream) HELIUM_OVERRIDE;
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