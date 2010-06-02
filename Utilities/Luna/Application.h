#pragma once

#include "Common/Compiler.h"
#include "Common/InitializerStack.h"

#include "Asset/Tracker.h"

#include <wx/app.h>

namespace Luna
{
    class Application : public wxApp
    {
    public:
        Application()
            : wxApp()
            , m_AssetTracker( NULL )
        {
        }

        Application(const Luna::Application &)
        {
            NOC_BREAK();
            throw Nocturnal::Exception( "Cannot copy the Luna application." );
        }

        virtual ~Application()
        {
            if ( m_AssetTracker )
            {
                delete m_AssetTracker;
            }
        }

        virtual void  OnInitCmdLine( wxCmdLineParser& parser ) NOC_OVERRIDE;
        virtual bool  OnCmdLineParsed( wxCmdLineParser& parser ) NOC_OVERRIDE;
        virtual int   OnRun() NOC_OVERRIDE;
        virtual int   OnExit() NOC_OVERRIDE;

        std::string ShowFileBrowser();

        Asset::Tracker* GetAssetTracker()
        {
            return m_AssetTracker;
        }

    protected:
        Nocturnal::InitializerStack m_InitializerStack;
        Asset::Tracker* m_AssetTracker;
    };

    DECLARE_APP( Application );
}