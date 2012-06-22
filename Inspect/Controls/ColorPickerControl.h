#pragma once

#include "Math/FpuColor3.h"
#include "Math/FpuColor4.h"
#include "Reflect/Class.h"

#include "Inspect/API.h"
#include "Inspect/Control.h"

namespace Helium
{
    namespace Inspect
    {
        class HELIUM_INSPECT_API ColorPicker : public Control
        {
        public:
            REFLECT_DECLARE_OBJECT( ColorPicker, Control );

            ColorPicker();

        protected:
            virtual void SetDefaultAppearance( bool def ) HELIUM_OVERRIDE;
            void SetToDefault( const ContextMenuEventArgs& event );

        public:
            Attribute< bool >             a_Highlight;

            Attribute< bool >             a_Alpha;
            Attribute< Color3 >     a_Color3;
            Attribute< Color4 >     a_Color4;
        };

        typedef Helium::StrongPtr< ColorPicker > ColorPickerPtr;
    }
}
