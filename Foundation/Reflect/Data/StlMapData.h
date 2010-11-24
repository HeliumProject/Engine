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

            virtual int32_t GetKeyType() const = 0;
            virtual int32_t GetValueType() const = 0;
            virtual void GetItems(V_ValueType& items) = 0;
            virtual void GetItems(V_ConstValueType& items) const = 0;
            virtual DataPtr GetItem(const Data* key) = 0;
            virtual ConstDataPtr GetItem(const Data* key) const = 0;
            virtual void SetItem(const Data* key, const Data* value) = 0;
            virtual void RemoveItem(const Data* key) = 0;
        };

        template <class KeyT, class KeySer, class ValueT, class ValueSer>
        class FOUNDATION_API SimpleStlMapData : public StlMapData
        {
        public:
            typedef std::map<KeyT, ValueT> DataType;
            Data::Pointer<DataType> m_Data;

            typedef SimpleStlMapData< KeyT, KeySer, ValueT, ValueSer > StlMapDataT;
            REFLECT_DECLARE_CLASS( StlMapDataT, StlMapData );

            SimpleStlMapData();
            virtual ~SimpleStlMapData();

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual int32_t GetKeyType() const HELIUM_OVERRIDE;
            virtual int32_t GetValueType() const HELIUM_OVERRIDE;
            virtual void GetItems(V_ValueType& items) HELIUM_OVERRIDE;
            virtual void GetItems(V_ConstValueType& items) const HELIUM_OVERRIDE;
            virtual DataPtr GetItem(const Data* key) HELIUM_OVERRIDE;
            virtual ConstDataPtr GetItem(const Data* key) const HELIUM_OVERRIDE;
            virtual void SetItem(const Data* key, const Data* value) HELIUM_OVERRIDE;
            virtual void RemoveItem(const Data* key) HELIUM_OVERRIDE;

            virtual bool Set(const Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Data* s) const HELIUM_OVERRIDE;

            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

            virtual tostream& operator>> (tostream& stream) const HELIUM_OVERRIDE;
            virtual tistream& operator<< (tistream& stream) HELIUM_OVERRIDE;
        };

        typedef SimpleStlMapData<tstring, StringData, tstring, StringData> StringStringStlMapData;
        typedef SimpleStlMapData<tstring, StringData, bool, BoolData> StringBoolStlMapData;
        typedef SimpleStlMapData<tstring, StringData, uint32_t, U32Data> StringU32StlMapData;
        typedef SimpleStlMapData<tstring, StringData, int32_t, I32Data> StringI32StlMapData;

        typedef SimpleStlMapData<uint32_t, U32Data, tstring, StringData> U32StringStlMapData;
        typedef SimpleStlMapData<uint32_t, U32Data, uint32_t, U32Data> U32U32StlMapData;
        typedef SimpleStlMapData<uint32_t, U32Data, int32_t, I32Data> U32I32StlMapData;
        typedef SimpleStlMapData<uint32_t, U32Data, uint64_t, U64Data> U32U64StlMapData;

        typedef SimpleStlMapData<int32_t, I32Data, tstring, StringData> I32StringStlMapData;
        typedef SimpleStlMapData<int32_t, I32Data, uint32_t, U32Data> I32U32StlMapData;
        typedef SimpleStlMapData<int32_t, I32Data, int32_t, I32Data> I32I32StlMapData;
        typedef SimpleStlMapData<int32_t, I32Data, uint64_t, U64Data> I32U64StlMapData;

        typedef SimpleStlMapData<uint64_t, U64Data, tstring, StringData> U64StringStlMapData;
        typedef SimpleStlMapData<uint64_t, U64Data, uint32_t, U32Data> U64U32StlMapData;
        typedef SimpleStlMapData<uint64_t, U64Data, uint64_t, U64Data> U64U64StlMapData;
        typedef SimpleStlMapData<uint64_t, U64Data, Matrix4, Matrix4Data> U64Matrix4StlMapData;

        typedef SimpleStlMapData<Helium::GUID, GUIDData, uint32_t, U32Data> GUIDU32StlMapData;
        typedef SimpleStlMapData<Helium::GUID, GUIDData, Matrix4, Matrix4Data> GUIDMatrix4StlMapData;
        typedef SimpleStlMapData<Helium::TUID, TUIDData, uint32_t, U32Data> TUIDU32StlMapData;
        typedef SimpleStlMapData<Helium::TUID, TUIDData, Matrix4, Matrix4Data> TUIDMatrix4StlMapData;
    }
}