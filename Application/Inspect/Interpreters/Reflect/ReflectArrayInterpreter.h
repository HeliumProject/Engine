#pragma once

#include "ReflectFieldInterpreter.h"

#include "Application/Inspect/Controls/InspectCanvas.h"
#include "Application/Inspect/Controls/InspectContainer.h"
#include "Application/Inspect/InspectData.h"

namespace Helium
{
    namespace Inspect
    {
        // Forwards
        class Button;
        class List;
        typedef Helium::SmartPtr<Button> ButtonPtr;

        class APPLICATION_API ReflectArrayInterpreter : public ReflectFieldInterpreter
        {
        public:
            ReflectArrayInterpreter (Container* container);

            virtual void InterpretField(const Reflect::Field* field, const std::vector<Reflect::Element*>& instances, Container* parent) override;

        protected:
            virtual ButtonPtr AddAddButton( List* list );
            virtual ButtonPtr AddRemoveButton( List* list );
            virtual ButtonPtr AddMoveUpButton( List* list );
            virtual ButtonPtr AddMoveDownButton( List* list );

            // callbacks
            void OnAdd( const ButtonClickedArgs& args );
            void OnRemove( const ButtonClickedArgs& args );
            void OnMoveUp( const ButtonClickedArgs& args );
            void OnMoveDown( const ButtonClickedArgs& args );

            virtual void OnCreateFieldSerializer( Reflect::SerializerPtr & s ){ }
        };

        typedef Helium::SmartPtr<ReflectArrayInterpreter> ReflectArrayInterpreterPtr;
        typedef std::vector< ReflectArrayInterpreterPtr > V_ReflectArrayInterpreterSmartPtr;
    }
}