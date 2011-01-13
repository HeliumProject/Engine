#pragma once

#include "Foundation/API.h"
#include "Foundation/Inspect/Control.h"

namespace Helium
{
    namespace Inspect
    {
        class FOUNDATION_API CheckBox : public Control
        {
        public:
            REFLECT_DECLARE_OBJECT( CheckBox, Control );

            CheckBox();

        protected:
            virtual void SetDefaultAppearance( bool def ) HELIUM_OVERRIDE;
            void SetToDefault( const ContextMenuEventArgs& event );

        public:
            Attribute< bool >   a_Highlight;
        };

        typedef Helium::StrongPtr< CheckBox > CheckBoxPtr;
    }
}