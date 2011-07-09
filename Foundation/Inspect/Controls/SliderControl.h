#pragma once

#include "Foundation/API.h"
#include "Foundation/Inspect/Control.h"

namespace Helium
{
    namespace Inspect
    {
        const static tchar_t SLIDER_ATTR_MIN[] = TXT( "min" );
        const static tchar_t SLIDER_ATTR_MAX[] = TXT( "max" );

        class HELIUM_FOUNDATION_API Slider : public Control
        {
        public:
            REFLECT_DECLARE_OBJECT( Slider, Control );

            Slider();

        protected:
            virtual bool Process( const tstring& key, const tstring& value );
            void SetToDefault(const ContextMenuEventArgs& event);

        public:
            Attribute< float >  a_Min;
            Attribute< float >  a_Max;
            Attribute< bool >   a_AutoAdjustMinMax;
        };

        typedef Helium::StrongPtr<Slider> SliderPtr;
    }
}
