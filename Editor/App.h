#pragma once

#include "Platform/Thread.h"

#include "Foundation/InitializerStack.h"
#include "Foundation/SettingsManager.h"
#include "Foundation/Document/DocumentManager.h"

#include "Editor/MainFrame.h"
#include "Editor/Tracker/Tracker.h"

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
            virtual int OnExit() HELIUM_OVERRIDE;
            virtual void OnChar( wxKeyEvent& event );
            virtual void OnAssertFailure(const wxChar *file, int line, const wxChar *func, const wxChar *cond, const wxChar *msg) HELIUM_OVERRIDE;
            virtual void OnUnhandledException() HELIUM_OVERRIDE;
            virtual bool OnExceptionInMainLoop() HELIUM_OVERRIDE;

            const tstring& AppVersion() const { return m_AppVersion; }
            const tstring& AppName() const { return m_AppName; }
            const tstring& AppVerName() const { return m_AppVerName; }

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

        protected:
            Helium::InitializerStack m_InitializerStack;
            
            tstring m_AppVersion;
            tstring m_AppName;
            tstring m_AppVerName;           
            
            SettingsManagerPtr m_SettingsManager;
            MainFrame* m_Frame;
            Tracker m_Tracker;
        };

        DECLARE_APP( App );
    }
}