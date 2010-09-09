#pragma once

#include "Editor/Inspect/Widget.h"
#include "Editor/Inspect/Widgets/ButtonWidget.h"

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
        };

        class FileDialogButtonWidget : public Reflect::ConcreteInheritor< FileDialogButtonWidget, ButtonWidget >
        {
        public:
            FileDialogButtonWidget()
            {

            }

            FileDialogButtonWidget( Inspect::FileDialogButton* control );                

        protected:
            void OnClicked( const Inspect::FileDialogButtonClickedArgs& args );
        };
    }
}