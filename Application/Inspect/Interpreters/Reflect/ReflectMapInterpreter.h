#pragma once

#include "ReflectFieldInterpreter.h"

#include "Application/Inspect/Controls/Canvas.h"
#include "Application/Inspect/Controls/Container.h"

namespace Helium
{
    namespace Inspect
    {
        class Button;
        class Container;

        class APPLICATION_API ReflectMapInterpreter : public ReflectFieldInterpreter
        {
        public:
            ReflectMapInterpreter( Container* container );
            virtual void InterpretField( const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent ) HELIUM_OVERRIDE;

        protected:

            void OnAdd( Button* button );
            void OnRemove( Button* button );
            void OnEdit( Button* button );
        };

        typedef Helium::SmartPtr<ReflectMapInterpreter> ReflectMapInterpreterPtr;
        typedef std::vector< ReflectMapInterpreterPtr > V_ReflectMapInterpreterSmartPtr;
    }
}