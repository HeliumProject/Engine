#pragma once

#include "EditorGenerated.h"

#include <wx/tglbtn.h>

namespace Editor
{
    class ToolbarPanel : public ToolbarPanelGenerated
    {
    public:
        ToolbarPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );

        void OnToggleToolButton( wxCommandEvent& );

        void ToggleTool( i32 selectedTool );

        wxPanel* GetToolsPanel()
        {
            return m_ToolsPanel;
        }

        wxPanel* GetToolsPropertiesPanel()
        {
            return m_ToolsPropertiesPanel;
        }

        void SetSaveButtonState( bool enabled );
            
    private:
        std::vector< wxBitmapToggleButton* > m_ToolsButtons;

    };
}
