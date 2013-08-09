#pragma once

#include "Editor/Inspect/wxWidget.h"

namespace Helium
{
    namespace Editor
    {
        class SliderWidget;

        class SliderWindow : public wxSlider
        {
        public:
            // Constructor
            SliderWindow( wxWindow* parent, SliderWidget* sliderWidget );

            void SetOverride( bool override )
            {
                m_Override = override;
            }

            // Callback every time a value changes on the slider.
            void OnScroll( wxScrollEvent& e);

            // Callback when a mouse click occurs on the slider.
            void OnMouseDown( wxMouseEvent& e );

            // Callback when a scroll operation has completed (such as
            // when the user stops dragging the thumb).
            void OnScrollChanged( wxScrollEvent& e );

            DECLARE_EVENT_TABLE();

        private:
            SliderWidget*   m_SliderWidget;
            bool            m_Override;
        };

        class SliderWidget : public Widget
        {
        public:
            REFLECT_DECLARE_CLASS( SliderWidget, Widget );

            SliderWidget()
                : m_SliderControl( NULL )
                , m_SliderWindow( NULL )
            {

            }

            SliderWidget( Inspect::Slider* slider );

            virtual void CreateWindow( wxWindow* parent ) HELIUM_OVERRIDE;
            virtual void DestroyWindow() HELIUM_OVERRIDE;
 
            virtual void Read() HELIUM_OVERRIDE;
            virtual bool Write() HELIUM_OVERRIDE;

            void MinChanged( const Attribute<float>::ChangeArgs& args );
            void MaxChanged( const Attribute<float>::ChangeArgs& args );
            void AutoAdjustMinMaxChanged( const Attribute<bool>::ChangeArgs& args );

            void Start();
            void End();

            float GetValue() const;
            void SetValue( float value );

            float GetUIValue() const;
            void SetUIValue( float value );

        private:
            Inspect::Slider*    m_SliderControl;
            SliderWindow*       m_SliderWindow;
            float               m_StartDragValue;
            bool                m_Tracking;
       };
    }
}