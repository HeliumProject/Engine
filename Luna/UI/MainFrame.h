#pragma once

#include "LunaGenerated.h"

#include "DirectoryPanel.h"
#include "HelpPanel.h"
#include "LayersPanel.h"
#include "PropertiesPanel.h"
#include "ToolsPanel.h"
#include "TypesPanel.h"
#include "ViewPanel.h"

#include "PropertiesManager.h"

#include "Scene/SceneManager.h"

#ifdef UI_REFACTOR
# include "TreeMonitor.h"
# include "TreeSortTimer.h"
#endif

namespace Luna
{
    class MainFrame : public MainFrameGenerated
    {
    protected:
        enum
        {
            ID_MenuOpened = wxID_HIGHEST + 1,
        };

    public:
        MainFrame( wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 1024,768 ), long style = wxDEFAULT_FRAME_STYLE|wxTAB_TRAVERSAL );
        virtual ~MainFrame();

        virtual void SaveWindowState()
        {
        }

        virtual const tstring& GetPreferencePrefix() const
        {
            return s_PreferencesPrefix;
        }

        void SetHelpText( const tchar* text );

#ifdef UI_REFACTOR
        TreeMonitor& GetTreeMonitor()
        {
            return m_TreeMonitor;
        }
#endif

    private:
        static tstring s_PreferencesPrefix;

        HelpPanel*       m_HelpPanel;
        LayersPanel*     m_LayersPanel;
        TypesPanel*      m_TypesPanel;
        ViewPanel*       m_ViewPanel;
        ToolsPanel*      m_ToolsPanel;
        DirectoryPanel*  m_DirectoryPanel;
        PropertiesPanel* m_PropertiesPanel;

        SceneManager     m_SceneManager;

        // the attributes for the current tool
        EnumeratorPtr m_ToolEnumerator;
        PropertiesManagerPtr m_ToolPropertiesManager;
        Inspect::Canvas m_ToolProperties;

#ifdef UI_REFACTOR
        TreeMonitor m_TreeMonitor;
        TreeSortTimer m_TreeSortTimer;
#endif

    private:
        void OnMenuOpen( wxMenuEvent& event );
        void OnNewScene( wxCommandEvent& event );
        void CurrentSceneChanged( const SceneChangeArgs& args );
        void CurrentSceneChanging( const SceneChangeArgs& args );
        void OnPropertiesCreated( const PropertiesCreatedArgs& args );
        void OnToolSelected(wxCommandEvent& event);
        void ViewToolChanged( const ToolChangeArgs& args );

    };
}