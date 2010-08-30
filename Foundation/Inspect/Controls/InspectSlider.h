#pragma once

#include "Foundation/API.h"
#include "Foundation/Inspect/Controls/InspectControl.h"

namespace Helium
{
    namespace Inspect
    {
        const static tchar SLIDER_ATTR_MIN[] = TXT( "min" );
        const static tchar SLIDER_ATTR_MAX[] = TXT( "max" );

        class FOUNDATION_API Slider : public Reflect::ConcreteInheritor<Slider, Control>
        {
        public:
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
