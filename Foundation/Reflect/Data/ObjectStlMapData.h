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

            typedef std::pair< DataPtr, ObjectPtr* > ValueType;
            typedef std::vector< ValueType > V_ValueType;

            virtual const Class* GetKeyClass() const = 0;
            virtual void GetItems(V_ValueType& items) = 0;
            virtual ObjectPtr* GetItem(Data* key) = 0;
            virtual void SetItem(Data* key, Object* value) = 0;
            virtual void RemoveItem(Data* key) = 0;
        };

        template <class KeyT>
        class FOUNDATION_API SimpleObjectStlMapData : public ObjectStlMapData
        {
        public:
            typedef std::map<KeyT, ObjectPtr> DataType;
            DataPointer<DataType> m_Data;

            typedef SimpleObjectStlMapData<KeyT> ObjectStlMapDataT;
            REFLECT_DECLARE_OBJECT( ObjectStlMapDataT, ObjectStlMapData )

            SimpleObjectStlMapData();
            ~SimpleObjectStlMapData();

            virtual void ConnectData(void* data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual const Class* GetKeyClass() const HELIUM_OVERRIDE;
            virtual void GetItems(V_ValueType& items) HELIUM_OVERRIDE;
            virtual ObjectPtr* GetItem(Data* key) HELIUM_OVERRIDE;
            virtual void SetItem(Data* key, Object* value) HELIUM_OVERRIDE;
            virtual void RemoveItem(Data* key) HELIUM_OVERRIDE;

            virtual bool Set(Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(Object* object) HELIUM_OVERRIDE;
            virtual void Accept(Visitor& visitor) HELIUM_OVERRIDE;

            virtual void Serialize(ArchiveBinary& archive) HELIUM_OVERRIDE;
            virtual void Deserialize(ArchiveBinary& archive) HELIUM_OVERRIDE;

            virtual void Serialize(ArchiveXML& archive) HELIUM_OVERRIDE;
            virtual void Deserialize(ArchiveXML& archive) HELIUM_OVERRIDE;

            void Serialize(Archive& archive);
            void Deserialize(Archive& archive);
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