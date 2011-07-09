#pragma once

#include "Foundation/API.h"
#include "Foundation/Inspect/Control.h"

namespace Helium
{
    namespace Inspect
    {
        const static tchar_t TEXTBOX_ATTR_REQUIRED[] = TXT( "required" );
        const static tchar_t TEXTBOX_ATTR_JUSTIFY[] = TXT( "justify" );
        const static tchar_t TEXTBOX_ATTR_JUSTIFY_LEFT[] = TXT( "left" );
        const static tchar_t TEXTBOX_ATTR_JUSTIFY_RIGHT[] = TXT( "right" );

        namespace Justifications
        {
            enum Justification
            {
                Left,
                Right,
            };
        };
        typedef Justifications::Justification Justification;

        class HELIUM_FOUNDATION_API Value : public Control
        {
        public:
            REFLECT_DECLARE_OBJECT( Value, Control );

            Value();

        protected:
            virtual bool Process(const tstring& key, const tstring& value) HELIUM_OVERRIDE;
            virtual void SetDefaultAppearance(bool def) HELIUM_OVERRIDE;
            void SetToDefault(const ContextMenuEventArgs& event);

        public:
            Attribute< Justification >  a_Justification;
            Attribute< bool >           a_Highlight;
        };

        typedef Helium::StrongPtr<Value> ValuePtr;
    }
}