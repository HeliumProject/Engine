#pragma once

#include "Engine/Engine.h"

#include "Reflect/Data.h"
#include "Reflect/DataReflection.h"

namespace Helium
{
    class HELIUM_ENGINE_API AssetPointerData : public Reflect::PointerData
    {
    public:
        //typedef Helium::StrongPtr<Asset> DataType;
        //DataPointer<DataType> m_Data;

        REFLECT_DECLARE_OBJECT( AssetPointerData, PointerData );

        AssetPointerData ();
        ~AssetPointerData();

        //virtual void ConnectData(void* data) HELIUM_OVERRIDE;

        //virtual bool Set(Data* data, uint32_t flags = 0) HELIUM_OVERRIDE;
        //virtual bool Equals(Object* object) HELIUM_OVERRIDE;
        //virtual void Accept(Visitor& visitor) HELIUM_OVERRIDE;

        virtual void Serialize(Reflect::ArchiveBinary& archive) HELIUM_OVERRIDE;
        virtual void Deserialize(Reflect::ArchiveBinary& archive) HELIUM_OVERRIDE;

        virtual void Serialize(Reflect::ArchiveXML& archive) HELIUM_OVERRIDE;
        virtual void Deserialize(Reflect::ArchiveXML& archive) HELIUM_OVERRIDE;
    };

    typedef Helium::SmartPtr<AssetPointerData> AssetPointerDataPtr;
}