#pragma once

#include <set>

#include "Reflect/Registry.h"
#include "Reflect/Data/SimpleData.h"
#include "Reflect/Data/ContainerData.h"

namespace Helium
{
    namespace Reflect
    {
        class HELIUM_REFLECT_API ObjectStlSetData : public ContainerData
        {
        public:
            typedef std::set<ObjectPtr> DataType;
            DataPointer<DataType> m_Data;

            REFLECT_DECLARE_OBJECT( ObjectStlSetData, ContainerData )

            ObjectStlSetData();
            ~ObjectStlSetData();

            virtual void ConnectData(void* data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual bool Set(Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(Object* object) HELIUM_OVERRIDE;
            virtual void Accept(Visitor& visitor) HELIUM_OVERRIDE;

            virtual void Serialize(ArchiveBinary& archive) HELIUM_OVERRIDE;
            virtual void Deserialize(ArchiveBinary& archive) HELIUM_OVERRIDE;

            virtual void Serialize(ArchiveXML& archive) HELIUM_OVERRIDE;
            virtual void Deserialize(ArchiveXML& archive) HELIUM_OVERRIDE;

		private:
			template< class ArchiveT >
            void Serialize(ArchiveT& archive);
			template< class ArchiveT >
            void Deserialize(ArchiveT& archive);
        };
    }
}