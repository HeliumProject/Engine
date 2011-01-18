#pragma once

#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Data/SimpleData.h"
#include "Foundation/Reflect/Data/ContainerData.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API ObjectDynArrayData : public ContainerData
        {
        public:
            typedef DynArray< ObjectPtr > DataType;
            Data::Pointer< DataType > m_Data;

            REFLECT_DECLARE_OBJECT( ObjectDynArrayData, ContainerData )

            ObjectDynArrayData();
            virtual ~ObjectDynArrayData();

            virtual void ConnectData( Helium::HybridPtr< void > data ) HELIUM_OVERRIDE;

            virtual size_t GetSize() const HELIUM_OVERRIDE;
            virtual void Clear() HELIUM_OVERRIDE;

            virtual bool Set( const Data* src, uint32_t flags = 0) HELIUM_OVERRIDE;
            virtual bool Equals( const Object* object ) const HELIUM_OVERRIDE;

            virtual void Serialize( Archive& archive ) const HELIUM_OVERRIDE;
            virtual void Deserialize( Archive& archive ) HELIUM_OVERRIDE;

            virtual void Accept( Visitor& visitor ) HELIUM_OVERRIDE;
        };
        typedef Helium::SmartPtr< ObjectDynArrayData > ObjectDynArrayDataPtr;
    }
}
