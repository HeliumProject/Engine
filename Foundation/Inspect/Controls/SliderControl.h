#pragma once

#include "Foundation/API.h"
#include "Foundation/Inspect/Control.h"

namespace Helium
{
    namespace Inspect
    {
        const static tchar_t SLIDER_ATTR_MIN[] = TXT( "min" );
        const static tchar_t SLIDER_ATTR_MAX[] = TXT( "max" );

        class FOUNDATION_API Slider : public Control
        {
        public:
            REFLECT_DECLARE_CLASS( Slider, Control );

            Slider();

            virtual bool Process( const tstring& key, const tstring& value );

        public:
            Attribute< float >  a_Min;
            Attribute< float >  a_Max;
            Attribute< bool >   a_AutoAdjustMinMax;
        };

        typedef Helium::SmartPtr<Slider> SliderPtr;
    }
}
