#pragma once

#include "Editor/Inspect/wxWidget.h"
#include "Editor/Inspect/Widgets/ButtonWidget.h"

#include "Application/FileDialog.h"

namespace Helium
{
    namespace Editor
    {
        class FileDialogButtonWidget;

        class FileDialogButtonWindow : public ButtonWindow
        {
        public:
            FileDialogButtonWindow( wxWindow* parent, FileDialogButtonWidget* buttonWidget );
        };

        class FileDialogButtonWidget : public ButtonWidget
        {
        public:
            REFLECT_DECLARE_CLASS( FileDialogButtonWidget, ButtonWidget );

            FileDialogButtonWidget()
            {

            }

            FileDialogButtonWidget( Inspect::FileDialogButton* control );                

        protected:
            void OnClicked( const Inspect::FileDialogButtonClickedArgs& args );
        };
    }
}