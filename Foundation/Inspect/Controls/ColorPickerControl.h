#pragma once

#include "Foundation/Math/FpuColor3.h"
#include "Foundation/Math/FpuColor4.h"
#include "Foundation/Reflect/Class.h"

#include "Foundation/API.h"
#include "Foundation/Inspect/Control.h"

namespace Helium
{
    namespace Inspect
    {
        class HELIUM_FOUNDATION_API ColorPicker : public Control
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
