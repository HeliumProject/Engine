#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/InspectButton.h"

#ifdef INSPECT_REFACTOR

namespace Helium
{
    namespace Inspect
    {
        typedef Helium::Signature< void, Button* > ActionSignature;

        class APPLICATION_API Action : public Reflect::ConcreteInheritor<Action, Button>
        {
        protected:
            // Event for when the button is clicked
            ActionSignature::Event m_ActionEvent;

        public:
            Action();

            virtual bool Write();

            // Button click listeners
        public:
            void AddListener( const ActionSignature::Delegate& listener )
            {
                m_ActionEvent.Add( listener );
            }
            void RemoveListener( const ActionSignature::Delegate& listener )
            {
                m_ActionEvent.Remove( listener );
            }
        };

        typedef Helium::SmartPtr<Action> ActionPtr;
    }
}

#endif