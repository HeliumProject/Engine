#pragma once

#include "ReflectFieldInterpreter.h"

#include "Application/Inspect/Controls/InspectCanvas.h"
#include "Application/Inspect/Controls/InspectContainer.h"

namespace Helium
{
    namespace Inspect
    {
        class APPLICATION_API ReflectColorInterpreter : public ReflectFieldInterpreter
        {
        public:
            ReflectColorInterpreter( Container* container );

            virtual void InterpretField( const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent ) HELIUM_OVERRIDE;
        };

        typedef Helium::SmartPtr<ReflectColorInterpreter> ReflectColorInterpreterPtr;
        typedef std::vector< ReflectColorInterpreterPtr > V_ReflectColorInterpreterSmartPtr;
    }
}