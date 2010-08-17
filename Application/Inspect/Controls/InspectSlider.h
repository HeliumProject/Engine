#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/InspectControl.h"

#include "Foundation/Reflect/Class.h"

namespace Helium
{
    namespace Inspect
    {
        const static tchar SLIDER_ATTR_MIN[] = TXT( "min" );
        const static tchar SLIDER_ATTR_MAX[] = TXT( "max" );

        class APPLICATION_API Slider : public Reflect::ConcreteInheritor<Slider, Control>
        {
        private:
            float m_Min;
            float m_Max;
            float m_CurrentValue;
            float m_StartDragValue;
            bool m_Tracking;
            bool m_AutoAdjustMinMax;

        public:
            Slider();

            virtual void Realize( Container* parent );
            virtual void Read();
            virtual bool Write();
            virtual void Start();
            virtual void End();
            virtual float GetValue();
            virtual void SetValue( float value );
            virtual void SetRangeMin( float min, bool clamp = true );
            virtual void SetRangeMax( float max, bool clamp = true );
            virtual void SetAutoAdjustMinMax( bool autoAdjust );

        protected:
            virtual bool Process( const tstring& key, const tstring& value );
            void SetUIValue( float value );
            float GetUIValue() const;
        };

        typedef Helium::SmartPtr<Slider> SliderPtr;
    }
}