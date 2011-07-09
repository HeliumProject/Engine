#pragma once

#include "ReflectFieldInterpreter.h"

#include "Foundation/Inspect/Canvas.h"
#include "Foundation/Inspect/Container.h"

namespace Helium
{
    namespace Inspect
    {
        class HELIUM_FOUNDATION_API ReflectVectorInterpreter : public ReflectFieldInterpreter
        {
        public:
            ReflectVectorInterpreter( Container* container );

            virtual void InterpretField( const Reflect::Field* field, const std::vector<Reflect::Object*>& instances, Container* parent ) HELIUM_OVERRIDE;
        };

        typedef Helium::SmartPtr<ReflectVectorInterpreter> ReflectVectorInterpreterPtr;
        typedef std::vector< ReflectVectorInterpreterPtr > V_ReflectVectorInterpreterSmartPtr;
    }
}