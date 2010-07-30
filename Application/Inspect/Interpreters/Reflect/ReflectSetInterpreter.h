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

        class APPLICATION_API ReflectSetInterpreter : public ReflectFieldInterpreter
        {
        public:
            ReflectSetInterpreter( Container* container );

            virtual void InterpretField( const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent ) HELIUM_OVERRIDE;

        private:
            // callbacks
            void OnAdd( Button* button );
            void OnRemove( Button* button );
        };

        typedef Helium::SmartPtr<ReflectSetInterpreter> ReflectSetInterpreterPtr;
        typedef std::vector< ReflectSetInterpreterPtr > V_ReflectSetInterpreterSmartPtr;
    }
}