#pragma once

#include <vector>

#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Data/SimpleData.h"
#include "Foundation/Reflect/Data/ContainerData.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API ObjectStlVectorData : public ContainerData
        {
        public:
            typedef std::vector< ObjectPtr > DataType;
            Data::Pointer<DataType> m_Data;

            REFLECT_DECLARE_CLASS( ObjectStlVectorData, ContainerData )

                ObjectStlVectorData();
            virtual ~ObjectStlVectorData();

            virtual void ConnectData(Helium::HybridPtr<void> data) HELIUM_OVERRIDE;

            virtual size_t GetSize() const  HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual bool Set(const Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals(const Data* s) const HELIUM_OVERRIDE;

            virtual void Serialize(Archive& archive) const HELIUM_OVERRIDE;
            virtual void Deserialize(Archive& archive) HELIUM_OVERRIDE;

            virtual void Accept(Visitor& visitor) HELIUM_OVERRIDE;
        };
        typedef Helium::SmartPtr< ObjectStlVectorData > ObjectStlVectorDataPtr;
    }
}