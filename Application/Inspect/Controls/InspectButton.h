#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/InspectControl.h"

#ifdef INSPECT_REFACTOR

namespace Helium
{
    namespace Inspect
    {
        const static tchar BUTTON_ATTR_TEXT[] = TXT( "text" );

        ///////////////////////////////////////////////////////////////////////////
        // 
        // 
        class APPLICATION_API Button : public Reflect::ConcreteInheritor<Button, Control>
        {
        protected:
            // Label on the button
            tstring m_Text;

            // Icon for the button
            wxArtID m_Icon;

        public:
            Button();

        protected:
            virtual bool Process( const tstring& key, const tstring& value );

        public:
            virtual void Realize( Container* parent );
            virtual bool Write();

            virtual void SetText( const tstring& text );
            virtual void SetIcon( const wxArtID& icon );
        };

        typedef Helium::SmartPtr<Button> ButtonPtr;
    }
}

#endif