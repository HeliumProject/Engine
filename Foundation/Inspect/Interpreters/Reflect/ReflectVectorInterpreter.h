#pragma once

#include "ReflectFieldInterpreter.h"

#include "Foundation/Inspect/Controls/InspectCanvas.h"
#include "Foundation/Inspect/Controls/InspectContainer.h"

namespace Helium
{
    namespace Inspect
    {
        class FOUNDATION_API ReflectVectorInterpreter : public ReflectFieldInterpreter
        {
        public:
            ReflectVectorInterpreter( Container* container );

            virtual void InterpretField( const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent ) HELIUM_OVERRIDE;
        };

        typedef Helium::SmartPtr<ReflectVectorInterpreter> ReflectVectorInterpreterPtr;
        typedef std::vector< ReflectVectorInterpreterPtr > V_ReflectVectorInterpreterSmartPtr;
    }
}