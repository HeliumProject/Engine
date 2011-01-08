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
        class FOUNDATION_API ObjectStlMapData : public ContainerData
        {
        public:
            REFLECT_DECLARE_ABSTRACT( ObjectStlMapData, ContainerData );

            typedef std::pair< ConstDataPtr, ObjectPtr* > ValueType;
            typedef std::vector< ValueType > V_ValueType;

            typedef std::pair< ConstDataPtr, const ObjectPtr* > ConstValueType;
            typedef std::vector< ConstValueType > V_ConstValueType;

            virtual const Class* GetKeyClass() const = 0;
            virtual void GetItems(V_ValueType& items) = 0;
            virtual void GetItems(V_ConstValueType& items) const = 0;
            virtual ObjectPtr* GetItem(const Data* key) = 0;
            virtual const ObjectPtr* GetItem(const Data* key) const = 0;
            virtual void SetItem(const Data* key, const Object* value) = 0;
            virtual void RemoveItem(const Data* key) = 0;
        };

        template <class KeyT>
        class FOUNDATION_API SimpleObjectStlMapData : public ObjectStlMapData
        {
        public:
            typedef std::map<KeyT, ObjectPtr> DataType;
            Data::Pointer<DataType> m_Data;

            typedef SimpleObjectStlMapData<KeyT> ObjectStlMapDataT;
            REFLECT_DECLARE_CLASS( ObjectStlMapDataT, ObjectStlMapData )

                SimpleObjectStlMapData();
            virtual ~SimpleObjectStlMapData();

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual const Class* GetKeyClass() const HELIUM_OVERRIDE;
            virtual void GetItems(V_ValueType& items) HELIUM_OVERRIDE;
            virtual void GetItems(V_ConstValueType& items) const HELIUM_OVERRIDE;
            virtual ObjectPtr* GetItem(const Data* key) HELIUM_OVERRIDE;
            virtual const ObjectPtr* GetItem(const Data* key) const HELIUM_OVERRIDE;
            virtual void SetItem(const Data* key, const Object* value) HELIUM_OVERRIDE;
            virtual void RemoveItem(const Data* key) HELIUM_OVERRIDE;

            virtual bool Set(const Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Data* s) const HELIUM_OVERRIDE;

            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

            virtual void Accept(Visitor& visitor) HELIUM_OVERRIDE;
        };

        typedef SimpleObjectStlMapData<TypeID> TypeIDObjectStlMapData;
        typedef SimpleObjectStlMapData<std::string> StlStringObjectStlMapData;
        typedef SimpleObjectStlMapData<uint32_t> UInt32ObjectStlMapData;
        typedef SimpleObjectStlMapData<int32_t> Int32ObjectStlMapData;
        typedef SimpleObjectStlMapData<uint64_t> UInt64ObjectStlMapData;
        typedef SimpleObjectStlMapData<int64_t> Int64ObjectStlMapData;
        typedef SimpleObjectStlMapData<Helium::GUID> GUIDObjectStlMapData;
        typedef SimpleObjectStlMapData<Helium::TUID> TUIDObjectStlMapData;
    }
}