#pragma once

#include "Platform/Compiler.h"
#include "Foundation/InitializerStack.h"

#include "Editor/UI/MainFrame.h"
#include "Platform/Thread.h"
#include "Tracker/Tracker.h"
#include "Editor/Vault/Vault.h"

#include "Preferences.h"
#include "Editor/DocumentManager.h"

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

            void SavePreferences();
            void LoadPreferences();

            Preferences* GetPreferences()
            {
                return m_Preferences;
            }

            MainFrame* GetFrame()
            {
                if ( !m_Frame )
                {
                    m_Frame = new MainFrame();
                }
                return m_Frame;
            }

            Vault& GetVault()
            {
                if ( !m_Vault )
                {
                    m_Vault = new Vault();
                }
                return *m_Vault;
            }

        protected:
            Helium::InitializerStack m_InitializerStack;
            Tracker m_Tracker;
            Helium::Thread m_TrackerThread;

            PreferencesPtr m_Preferences;
            Vault* m_Vault;
            MainFrame* m_Frame;
        };

        DECLARE_APP( App );
    }
}