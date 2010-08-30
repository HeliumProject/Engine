#pragma once

#include "Foundation/API.h"
#include "Foundation/Inspect/Controls/InspectControl.h"

namespace Helium
{
    namespace Inspect
    {
        const static tchar LABEL_ATTR_TEXT[] = TXT( "text" );

        class FOUNDATION_API Label : public Reflect::ConcreteInheritor<Label, Control>
        {
        public:
            Label();

            virtual bool Process(const tstring& key, const tstring& value);

            void BindText( const tstring& text )
            {
                Bind( new StringFormatter<tstring>( new tstring( text ), true ) );
            }
        };

        typedef Helium::SmartPtr<Label> LabelPtr;
    }
}