#pragma once

#include "LunaGenerated.h"

#include <wx/tglbtn.h>

namespace Luna
{
    class ToolsPanel : public ToolsPanelGenerated
    {
    public:
   	    ToolsPanel( wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );

        void Create( wxScrolledWindow* propertiesScrolledWindow );

        void OnToggleButton( wxCommandEvent& );

        void ToggleTool( i32 selectedTool );

        wxPanel* GetToolsPropertiesPanel()
        {
            return m_PropertiesPanel;
        }

    private:
        std::vector< wxBitmapToggleButton* > m_Buttons;
        wxStaticLine* m_Divider;
        wxScrolledWindow* m_PropertiesScrolledWindow;
    };
}