#pragma once

#include "Engine/Engine.h"

#include "Reflect/Data.h"
#include "Reflect/DataReflection.h"

namespace Helium
{
    class HELIUM_ENGINE_API GameObjectPointerData : public Reflect::PointerData
    {
    public:
        virtual void Serialize(Reflect::ArchiveBinary& archive) HELIUM_OVERRIDE;
        virtual void Deserialize(Reflect::ArchiveBinary& archive) HELIUM_OVERRIDE;

        virtual void Serialize(Reflect::ArchiveXML& archive) HELIUM_OVERRIDE;
        virtual void Deserialize(Reflect::ArchiveXML& archive) HELIUM_OVERRIDE;
    };

    typedef Helium::SmartPtr<GameObjectPointerData> GameObjectPointerDataPtr;
}