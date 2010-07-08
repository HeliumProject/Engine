#pragma once

#include <wx/panel.h>
#include <wx/tglbtn.h>

#include "Platform/Types.h"

namespace Luna
{
    class SceneEditor;

    class ToolsPanel : public wxPanel 
    {
    public:
        ToolsPanel( SceneEditor* sceneEditor, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 140, 300 ), long style = wxTAB_TRAVERSAL );
        virtual ~ToolsPanel();

        void OnToggleButton( wxCommandEvent& );

        void ToggleTool( i32 selectedTool );

    private:
        SceneEditor* m_SceneEditor;
        std::vector<wxBitmapToggleButton*> m_Buttons;
        wxStaticLine* m_Divider;
        wxScrolledWindow* m_ScrollWindow;
        wxStaticText* m_StaticText;  	
    };
}
