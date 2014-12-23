#pragma once

#include "Platform/Thread.h"

#include "Application/InitializerStack.h"
//#include "Application/DocumentManager.h"

//#include "EditorScene/SettingsManager.h"

//#include "Editor/MainFrame.h"
//#include "Editor/EditorEngine.h"

#include "Pipeline/PipelineTrayIcon.h"

#include <wx/app.h>
#include <wx/xrc/xmlres.h>

#define HELIUM_APP_VERSION  TXT( "99.99.99.99" )
#define HELIUM_APP_NAME     TXT( "Pipeline" )
#define HELIUM_APP_VER_NAME HELIUM_APP_NAME TXT( " v" ) HELIUM_APP_VERSION

namespace Helium
{
	namespace Pipeline
	{
		class App : public wxApp
		{
		public:
			App();
			~App();

			virtual bool OnInit() HELIUM_OVERRIDE;
			virtual int OnRun() HELIUM_OVERRIDE;
			virtual int OnExit() HELIUM_OVERRIDE;

			const std::string& AppVersion() const { return m_AppVersion; }
			const std::string& AppName() const { return m_AppName; }
			const std::string& AppVerName() const { return m_AppVerName; }

		protected:
			Helium::InitializerStack m_InitializerStack;

			std::string m_AppVersion;
			std::string m_AppName;
			std::string m_AppVerName;

			PipelineTrayIcon m_TrayIcon;

#ifdef IDLE_LOOP
			DECLARE_EVENT_TABLE();
#endif
		};

		DECLARE_APP( App );
	}
}