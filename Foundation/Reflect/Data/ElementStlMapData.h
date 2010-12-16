#pragma once

#include <map>

#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Data/TypeIDData.h"
#include "Foundation/Reflect/Data/SimpleData.h"
#include "Foundation/Reflect/Data/ContainerData.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API ElementStlMapData : public ContainerData
        {
        public:
            REFLECT_DECLARE_ABSTRACT( ElementStlMapData, ContainerData );

            typedef std::pair< ConstDataPtr, ElementPtr* > ValueType;
            typedef std::vector< ValueType > V_ValueType;

            typedef std::pair< ConstDataPtr, const ElementPtr* > ConstValueType;
            typedef std::vector< ConstValueType > V_ConstValueType;

            virtual const Class* GetKeyClass() const = 0;
            virtual void GetItems(V_ValueType& items) = 0;
            virtual void GetItems(V_ConstValueType& items) const = 0;
            virtual ElementPtr* GetItem(const Data* key) = 0;
            virtual const ElementPtr* GetItem(const Data* key) const = 0;
            virtual void SetItem(const Data* key, const Element* value) = 0;
            virtual void RemoveItem(const Data* key) = 0;
        };

        template <class KeyT>
        class FOUNDATION_API SimpleElementStlMapData : public ElementStlMapData
        {
        public:
            typedef std::map<KeyT, ElementPtr> DataType;
            Data::Pointer<DataType> m_Data;

            typedef SimpleElementStlMapData<KeyT> ElementStlMapDataT;
            REFLECT_DECLARE_CLASS( ElementStlMapDataT, ElementStlMapData )

                SimpleElementStlMapData();
            virtual ~SimpleElementStlMapData();

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual const Class* GetKeyClass() const HELIUM_OVERRIDE;
            virtual void GetItems(V_ValueType& items) HELIUM_OVERRIDE;
            virtual void GetItems(V_ConstValueType& items) const HELIUM_OVERRIDE;
            virtual ElementPtr* GetItem(const Data* key) HELIUM_OVERRIDE;
            virtual const ElementPtr* GetItem(const Data* key) const HELIUM_OVERRIDE;
            virtual void SetItem(const Data* key, const Element* value) HELIUM_OVERRIDE;
            virtual void RemoveItem(const Data* key) HELIUM_OVERRIDE;

            virtual bool Set(const Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Data* s) const HELIUM_OVERRIDE;

            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

            virtual void Accept(Visitor& visitor) HELIUM_OVERRIDE;
        };

        typedef SimpleElementStlMapData<TypeID> TypeIDElementStlMapData;
        typedef SimpleElementStlMapData<std::string> StringElementStlMapData;
        typedef SimpleElementStlMapData<uint32_t> UInt32ElementStlMapData;
        typedef SimpleElementStlMapData<int32_t> Int32ElementStlMapData;
        typedef SimpleElementStlMapData<uint64_t> UInt64ElementStlMapData;
        typedef SimpleElementStlMapData<int64_t> Int64ElementStlMapData;
        typedef SimpleElementStlMapData<Helium::GUID> GUIDElementStlMapData;
        typedef SimpleElementStlMapData<Helium::TUID> TUIDElementStlMapData;
    }
}