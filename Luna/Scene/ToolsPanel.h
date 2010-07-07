#pragma once

namespace Luna
{
    class SceneEditor;

    class ToolsPanel : public wxPanel 
    {
    public:
        ToolsPanel( SceneEditor* sceneEditor, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 140, 300 ), long style = wxTAB_TRAVERSAL );
        virtual ~ToolsPanel();

    private:
        SceneEditor* m_SceneEditor;
        wxStaticLine* m_Divider;
        wxScrolledWindow* m_ScrollWindow;
        wxStaticText* m_StaticText;  	
    };
}
