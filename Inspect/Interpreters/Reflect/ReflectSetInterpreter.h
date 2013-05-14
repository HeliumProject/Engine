#pragma once

#include "ReflectFieldInterpreter.h"

#include "Inspect/Canvas.h"
#include "Inspect/Container.h"

namespace Helium
{
    namespace Inspect
    {
        class Button;
        class Container;

        class HELIUM_INSPECT_API ReflectStlSetInterpreter : public ReflectFieldInterpreter
        {
        public:
            ReflectStlSetInterpreter( Container* container );

            virtual void InterpretField( const Reflect::Field* field, const std::vector<Reflect::Object*>& instances, Container* parent ) HELIUM_OVERRIDE;

        private:
            // callbacks
            void OnAdd( const ButtonClickedArgs& args );
            void OnRemove( const ButtonClickedArgs& args );
        };
    }
}