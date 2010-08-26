#pragma once

#include "Foundation/Reflect/Class.h"

#include "Application/API.h"
#include "Application/Inspect/Controls/InspectControl.h"

namespace Helium
{
    namespace Inspect
    {
        const static tchar BUTTON_ATTR_TEXT[] = TXT( "text" );
        const static tchar BUTTON_ATTR_ICON[] = TXT( "icon" );

        class Button;

        struct ButtonClickedArgs
        {
            ButtonClickedArgs( Button* control )
                : m_Control( control )
            {
            }

            Button* m_Control;
        };
        typedef Helium::Signature< void, const ButtonClickedArgs& > ButtonClickedSignature;

        class APPLICATION_API Button : public Reflect::ConcreteInheritor< Button, Control >
        {
        public:
            Button();

            ButtonClickedSignature::Event& ButtonClickedEvent()
            {
                return m_ButtonClickedEvent;
            }

            virtual bool Write() HELIUM_OVERRIDE
            {
                m_ButtonClickedEvent.Raise( ButtonClickedArgs( this ) );
                return true;
            }

        protected:
            virtual bool Process( const tstring& key, const tstring& value );

        public:
            // Label on the button
            Attribute< tstring > a_Label;

            // Icon for the button
            Attribute< tstring > a_Icon;

        protected:
            ButtonClickedSignature::Event m_ButtonClickedEvent;

        };

        typedef Helium::SmartPtr< Button > ButtonPtr;
    }
}
