#pragma once

#include "Platform/Thread.h"

#include "EditorScene/SettingsManager.h"

#include "Editor/MainFrame.h"

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

			virtual bool OnInit() override;
			virtual int OnRun() override;
			virtual int OnExit() override;

			void OnChar( wxKeyEvent& event );
#ifdef IDLE_LOOP
			void OnIdle( wxIdleEvent& event );
#endif

			virtual void OnAssertFailure(const wxChar *file, int line, const wxChar *func, const wxChar *cond, const wxChar *msg) override;
#if wxUSE_EXCEPTIONS
			virtual void OnUnhandledException() override;
			virtual bool OnExceptionInMainLoop() override;
#endif

			const std::string& AppVersion() const { return m_AppVersion; }
			const std::string& AppName() const { return m_AppName; }
			const std::string& AppVerName() const { return m_AppVerName; }

			bool GetEnableAssetTracker() const;
			void SetEnableAssetTracker( bool value );

			void SaveSettings();
			void LoadSettings();

			SettingsManager* GetSettingsManager()
			{
				return m_SettingsManager;
			}

			MainFrame* GetFrame()
			{
				HELIUM_ASSERT( m_Frame );
				return m_Frame;
			}

		protected:
			bool m_Running;

			std::string m_AppVersion;
			std::string m_AppName;
			std::string m_AppVerName;
			bool m_EnableAssetTracker;

			SettingsManagerPtr m_SettingsManager;
			MainFrame* m_Frame;

#ifdef IDLE_LOOP
			DECLARE_EVENT_TABLE();
#endif
		};

		DECLARE_APP( App );
	}
}