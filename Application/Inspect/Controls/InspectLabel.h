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
            virtual void Read();

            void SetText( const tstring& text );
            tstring GetText() const;

            void SetToolTip( const tstring& toolTip );

        private:
            void UpdateUI( const tstring& text );
        };

        typedef Helium::SmartPtr<Label> LabelPtr;
    }
}