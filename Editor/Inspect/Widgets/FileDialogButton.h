#pragma once

#include "Editor/Inspect/Widget.h"
#include "Editor/Inspect/Widgets/Button.h"

#include "Foundation/FileDialog.h"

namespace Helium
{
    namespace Editor
    {
        class FileDialogButtonWidget;

        class FileDialogButtonWindow : public ButtonWindow
        {
        public:
            FileDialogButtonWindow( wxWindow* parent, FileDialogButtonWidget* buttonWidget );

            void OnClicked( wxCommandEvent& ) HELIUM_OVERRIDE;
        };

        class FileDialogButtonWidget : public ButtonWidget
        {
        public:
            FileDialogButtonWidget( Inspect::Button* control );                

        protected:
            Path OnClicked( const Inspect::FileDialogButtonClickedArgs& args );
        };
    }
}