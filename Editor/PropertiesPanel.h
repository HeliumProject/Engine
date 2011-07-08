#pragma once

#include "SceneGraph/PropertiesManager.h"
#include "SceneGraph/PropertiesGenerator.h"

#include "Editor/CommandQueue.h"
#include "Editor/EditorGenerated.h"
#include "Editor/Inspect/TreeCanvas.h"

namespace Helium
{
    namespace Editor
    {
        class PropertiesPanel : public PropertiesPanelGenerated
        {
        public:
            PropertiesPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );

            TreeCanvas& GetCanvas()
            {
                return m_PropertiesCanvas;
            }

            SceneGraph::PropertiesManager& GetPropertiesManager()
            {
                return m_PropertiesManager;
            }

            SceneGraph::PropertiesGenerator& GetPropertiesGenerator()
            {
                return m_PropertiesGenerator;
            }

            virtual void OnIntersection(wxCommandEvent& event) HELIUM_OVERRIDE;
            virtual void OnUnion(wxCommandEvent& event) HELIUM_OVERRIDE;

        private:
            CommandQueue                m_CommandQueue;
            TreeCanvas                  m_PropertiesCanvas;
            SceneGraph::PropertiesGenerator   m_PropertiesGenerator; // HEADS UP: do this one first in the constructor!
            SceneGraph::PropertiesManager     m_PropertiesManager;
        };
    }
}