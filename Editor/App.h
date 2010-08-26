#pragma once

#include "Platform/Compiler.h"
#include "Platform/Thread.h"

#include "Application/DocumentManager.h"

#include "Core/SettingsManager.h"

#include "Foundation/InitializerStack.h"

#include "Editor/MainFrame.h"
#include "Editor/Tracker/Tracker.h"
#include "Editor/Vault/VaultSearch.h"

#include <wx/app.h>
#include <wx/xrc/xmlres.h>

namespace Helium
{
    namespace Editor
    {
        class App : public wxApp
        {
        public:
            App();
            ~App();

            virtual bool    OnInit() HELIUM_OVERRIDE;
            virtual int     OnExit() HELIUM_OVERRIDE;
            virtual void    OnAssertFailure(const wxChar *file, int line, const wxChar *func, const wxChar *cond, const wxChar *msg) HELIUM_OVERRIDE;

            void SaveSettings();
            void LoadSettings();

            Core::SettingsManagerPtr& GetSettingsManager()
            {
                return m_SettingsManager;
            }

            MainFrame* GetFrame()
            {
                if ( !m_Frame )
                {
                    m_Frame = new MainFrame( m_SettingsManager );
                }
                return m_Frame;
            }

            VaultSearch& GetVaultSearch()
            {
                if ( !m_VaultSearch )
                {
                    m_VaultSearch = new VaultSearch();
                }
                return *m_VaultSearch;
            }

        protected:
            Helium::InitializerStack m_InitializerStack;
            Tracker m_Tracker;
            Helium::Thread m_TrackerThread;

            Core::SettingsManagerPtr m_SettingsManager;
            VaultSearch* m_VaultSearch;
            MainFrame* m_Frame;
        };

        DECLARE_APP( App );
    }
}