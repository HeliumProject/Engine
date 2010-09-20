#pragma once

#include "Platform/Compiler.h"

#include "Core/SettingsManager.h"

#include "Editor/RenderWindow.h"

namespace Helium
{
    namespace Reflect
    {
        struct ElementChangeArgs;
    }

    namespace Editor
    {
        class VaultPanel;

        class VaultPreviewWindow : public RenderWindow
        {
        public:
            VaultPreviewWindow( SettingsManager* settingsManager, wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE, const wxString& name = wxT( "Editor::VaultPreviewWindow" ) );
            virtual ~VaultPreviewWindow();

            void SetVaultPanel( VaultPanel* vaultPanel );

            virtual void DisplayReferenceAxis( bool display ) HELIUM_OVERRIDE;

        private:
            void OnSettingsChanged( const Reflect::ElementChangeArgs& args );

        private:
            SettingsManager* m_SettingsManager;
            VaultPanel* m_VaultPanel;

        private:
            DECLARE_EVENT_TABLE();
        };
    }
}