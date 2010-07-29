#pragma once

#include "Platform/Compiler.h"
#include "Foundation/InitializerStack.h"

#include "UI/MainFrame.h"
#include "Platform/Thread.h"
#include "Tracker/Tracker.h"
#include "Vault/Vault.h"

#include "Preferences.h"
#include "DocumentManager.h"

#include <wx/app.h>
#include <wx/xrc/xmlres.h>

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

        DocumentManager& GetDocumentManager()
        {
            return m_DocumentManager;
        }

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
        DocumentManager m_DocumentManager;
        Tracker m_Tracker;
        Platform::Thread m_TrackerThread;

        PreferencesPtr m_Preferences;
        Vault* m_Vault;
        MainFrame* m_Frame;
    };

    DECLARE_APP( App );
}