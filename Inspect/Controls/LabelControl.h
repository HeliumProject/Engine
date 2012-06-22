#pragma once

#include "Inspect/API.h"
#include "Inspect/Control.h"

namespace Helium
{
    namespace Inspect
    {
        const static tchar_t LABEL_ATTR_TEXT[] = TXT( "text" );

        class HELIUM_INSPECT_API Label : public Control
        {
        public:
            REFLECT_DECLARE_OBJECT( Label, Control );

            Label();

            virtual bool Process(const tstring& key, const tstring& value);

            void BindText( const tstring& text )
            {
                Bind( new StringFormatter<tstring>( new tstring( text ), true ) );
            }

            Attribute<bool> a_Ellipsize;
        };

        typedef Helium::StrongPtr<Label> LabelPtr;
    }
}