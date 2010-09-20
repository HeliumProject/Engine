#pragma once

#include "Core/SceneGraph/PropertiesManager.h"
#include "Core/SceneGraph/PropertiesGenerator.h"

#include "Editor/CommandQueue.h"
#include "Editor/EditorGenerated.h"
#include "Editor/Inspect/StripCanvas.h"

#include <wx/tglbtn.h>

namespace Helium
{
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

            StripCanvas& GetCanvas()
            {
                return m_ToolPropertiesCanvas;
            }

            SceneGraph::PropertiesManager& GetPropertiesManager()
            {
                return m_ToolPropertiesManager;
            }

            SceneGraph::PropertiesGenerator& GetPropertiesGenerator()
            {
                return m_ToolPropertiesGenerator;
            }

        private:
            std::vector< wxBitmapToggleButton* > m_ToolsButtons;

            CommandQueue                m_CommandQueue;
            StripCanvas                 m_ToolPropertiesCanvas;
            SceneGraph::PropertiesGenerator   m_ToolPropertiesGenerator; // HEADS UP: do this one first in the constructor!
            SceneGraph::PropertiesManager     m_ToolPropertiesManager;
        };
    }
}