#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/InspectControl.h"

namespace Helium
{
    namespace Inspect
    {
        const static tchar LABEL_ATTR_TEXT[] = TXT( "text" );

        class APPLICATION_API Label : public Reflect::ConcreteInheritor<Label, Control>
        {
        public:
            Label();

            virtual bool Process(const tstring& key, const tstring& value);
        };

        typedef Helium::SmartPtr<Label> LabelPtr;
    }
}