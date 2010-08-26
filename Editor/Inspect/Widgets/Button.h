#pragma once

#include "Editor/Inspect/Widget.h"

namespace Helium
{
    namespace Editor
    {
        class ButtonWidget;

        class ButtonWindow : public wxPanel
        {
        public:
            ButtonWindow( wxWindow* parent, ButtonWidget* buttonWidget );

            void OnClicked( wxCommandEvent& );

            void SetIcon( const tstring& icon );
            void SetLabel( const tstring& icon );

        private:
            wxButton*     m_Button;
            ButtonWidget* m_ButtonWidget;
        };

        class ButtonWidget : public Widget
        {
        public:
            ButtonWidget( Inspect::Button* control );

            void Create( wxWindow* parent );
            void Destroy();

            void SetLabel( const tstring& label );
            void SetIcon( const tstring& icon );

        protected:
            void OnIconChanged( const Attribute< tstring >::ChangeArgs& args );
            void OnLabelChanged( const Attribute< tstring >::ChangeArgs& args );


            Inspect::Button*  m_ButtonControl;
            ButtonWindow*     m_ButtonWindow;
        };
    }
}