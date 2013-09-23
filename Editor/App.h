#pragma once

#include "Platform/Thread.h"

#include "Application/InitializerStack.h"
#include "Application/DocumentManager.h"

#include "SceneGraph/SettingsManager.h"

#include "Editor/MainFrame.h"
#include "Editor/Tracker.h"
#include "Editor/EditorEngine.h"

#include <wx/app.h>
#include <wx/xrc/xmlres.h>

#define HELIUM_APP_VERSION  TXT( "99.99.99.99" )
#define HELIUM_APP_NAME     TXT( "Helium" )
#define HELIUM_APP_VER_NAME HELIUM_APP_NAME TXT( " v" ) HELIUM_APP_VERSION

namespace Helium
{
    namespace Editor
    {
        class App : public wxApp
        {
        public:
            App();
            ~App();

            virtual bool OnInit() HELIUM_OVERRIDE;
            virtual int OnRun() HELIUM_OVERRIDE;
            virtual int OnExit() HELIUM_OVERRIDE;

            void OnChar( wxKeyEvent& event );
#ifdef IDLE_LOOP
            void OnIdle( wxIdleEvent& event );
#endif
            
            virtual void OnAssertFailure(const wxChar *file, int line, const wxChar *func, const wxChar *cond, const wxChar *msg) HELIUM_OVERRIDE;
#if wxUSE_EXCEPTIONS
            virtual void OnUnhandledException() HELIUM_OVERRIDE;
            virtual bool OnExceptionInMainLoop() HELIUM_OVERRIDE;
#endif

            const std::string& AppVersion() const { return m_AppVersion; }
            const std::string& AppName() const { return m_AppName; }
            const std::string& AppVerName() const { return m_AppVerName; }

            void SaveSettings();
            void LoadSettings();

            SettingsManagerPtr& GetSettingsManager()
            {
                return m_SettingsManager;
            }

            MainFrame* GetFrame()
            {
                HELIUM_ASSERT( m_Frame );
                return m_Frame;
            }

            Tracker* GetTracker()
            {
                return &m_Tracker;
            }

            EditorEngine* GetEngine()
            {
                return &m_Engine;
            }

        protected:
            Helium::InitializerStack m_InitializerStack;
            bool m_Running;

            std::string m_AppVersion;
            std::string m_AppName;
            std::string m_AppVerName;

            SettingsManagerPtr m_SettingsManager;
            MainFrame* m_Frame;
            Tracker m_Tracker;
            EditorEngine m_Engine;

#ifdef IDLE_LOOP
            DECLARE_EVENT_TABLE();
#endif
        };

        DECLARE_APP( App );
    }
}