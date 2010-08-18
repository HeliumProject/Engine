#pragma once

#include "Platform/Compiler.h"

#include "Editor/RenderWindow.h"

namespace Helium
{
    namespace Reflect
    {
        struct ElementChangeArgs;
    }

    namespace Editor
    {
        class VaultFrame;

        class VaultPreviewWindow : public RenderWindow
        {
        public:
            VaultPreviewWindow( wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE, const wxString& name = wxT( "Editor::VaultPreviewWindow" ) );
            virtual ~VaultPreviewWindow();

            void SetVaultFrame( VaultFrame* browserFrame );

            virtual void DisplayReferenceAxis( bool display ) HELIUM_OVERRIDE;

        private:
            void OnSettingsChanged( const Reflect::ElementChangeArgs& args );

        private:
            VaultFrame* m_VaultFrame;

        private:
            DECLARE_EVENT_TABLE();
        };
    }
}