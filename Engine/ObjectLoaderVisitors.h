#pragma once

#include "Engine/Engine.h"

#include "Reflect/Visitor.h"
#include "Reflect/Composite.h"
#include "Foundation/DynamicArray.h"

namespace Helium
{
    // Called it something bad happens during deserialization
    class HELIUM_ENGINE_API ClearLinkIndicesFromObject : public Helium::Reflect::Visitor
    {        
    public:
        ClearLinkIndicesFromObject();
        virtual ~ClearLinkIndicesFromObject();

        virtual bool VisitField(void* instance, const Helium::Reflect::Field* field) HELIUM_OVERRIDE;
    };
}