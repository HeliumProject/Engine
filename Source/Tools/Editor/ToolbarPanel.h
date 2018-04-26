#pragma once

#include "EditorScene/PropertiesManager.h"
#include "EditorScene/PropertiesGenerator.h"

#include "Editor/CommandQueue.h"
#include "Editor/EditorGeneratedWrapper.h"
#include "Editor/Inspect/StripCanvas.h"

#include "Editor/Controls/EditorButton.h"
#include "Editor/Controls/DynamicBitmap.h"
#include "Editor/Controls/Drawer/DrawerManager.h"


namespace Helium
{
    namespace Editor
    {
        class ToolbarPanel : public ToolbarPanelGenerated
        {
        public:
            ToolbarPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );

            void ToggleTool( int32_t selectedTool );
            void EnableTools( const bool enable = true );

            wxPanel* GetToolsPropertiesPanel()
            {
                return (wxPanel*)m_ToolsPropertiesPanel;
            }

            StripCanvas& GetCanvas()
            {
                return m_ToolPropertiesCanvas;
            }

            Editor::PropertiesManager& GetPropertiesManager()
            {
                return m_ToolPropertiesManager;
            }

            Editor::PropertiesGenerator& GetPropertiesGenerator()
            {
                return m_ToolPropertiesGenerator;
            }

        private:
            std::vector< EditorButton* > m_ToolsButtons;

            CommandQueue m_CommandQueue;
            StripCanvas m_ToolPropertiesCanvas;
            Editor::PropertiesGenerator m_ToolPropertiesGenerator; // HEADS UP: do this one first in the constructor!
            Editor::PropertiesManager m_ToolPropertiesManager;

            DrawerManager m_DrawerManager;
        };
    }
}