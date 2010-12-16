#pragma once

#include <set>

#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Data/SimpleData.h"
#include "Foundation/Reflect/Data/ContainerData.h"
#include "Foundation/Reflect/Data/PathData.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API StlSetData : public ContainerData
        {
        public:
            REFLECT_DECLARE_ABSTRACT( StlSetData, ContainerData );

            virtual const Class* GetItemClass() const = 0;
            virtual void GetItems(std::vector< ConstDataPtr >& items) const = 0;
            virtual void AddItem(const Data* value) = 0;
            virtual void RemoveItem(const Data* value) = 0;
            virtual bool ContainsItem(const Data* value) const = 0;
        };

        template <class DataT, class DataClassT>
        class FOUNDATION_API SimpleStlSetData : public StlSetData
        {
        public:
            typedef std::set<DataT> DataType;
            Data::Pointer<DataType> m_Data;

            typedef SimpleStlSetData<DataT, DataClassT> StlSetDataT;
            REFLECT_DECLARE_CLASS( StlSetDataT, StlSetData )

            SimpleStlSetData();
            virtual ~SimpleStlSetData();

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual const Class* GetItemClass() const HELIUM_OVERRIDE;
            virtual void GetItems(std::vector< ConstDataPtr >& items) const HELIUM_OVERRIDE;
            virtual void AddItem(const Data* value) HELIUM_OVERRIDE;
            virtual void RemoveItem(const Data* value) HELIUM_OVERRIDE;
            virtual bool ContainsItem(const Data* value) const HELIUM_OVERRIDE;

            virtual bool Set(const Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Data* s) const HELIUM_OVERRIDE;

            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

            virtual tostream& operator>> (tostream& stream) const HELIUM_OVERRIDE;
            virtual tistream& operator<< (tistream& stream) HELIUM_OVERRIDE;
        };

        typedef SimpleStlSetData<tstring, StringData> StringStlSetData;
        typedef SimpleStlSetData<uint32_t, UInt32Data> UInt32StlSetData;
        typedef SimpleStlSetData<uint64_t, UInt64Data> UInt64StlSetData;
        typedef SimpleStlSetData<Helium::GUID, GUIDData> GUIDStlSetData;
        typedef SimpleStlSetData<Helium::TUID, TUIDData> TUIDStlSetData;
        typedef SimpleStlSetData<Helium::Path, PathData> PathStlSetData;
    }
}