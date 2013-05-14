#pragma once

#include "ReflectFieldInterpreter.h"

namespace Helium
{
    namespace Inspect
    {
        class HELIUM_INSPECT_API ReflectBitfieldInterpreter : public ReflectFieldInterpreter
        {
        public:
            ReflectBitfieldInterpreter( Container* container );

            virtual void InterpretField( const Reflect::Field* field, const std::vector<Reflect::Object*>& instances, Container* parent ) HELIUM_OVERRIDE;
        };
    }
}