#pragma once

#include "EditorScene/PropertiesManager.h"
#include "EditorScene/PropertiesGenerator.h"

#include "Editor/CommandQueue.h"
#include "Editor/EditorGeneratedWrapper.h"
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

            Editor::PropertiesManager& GetPropertiesManager()
            {
                return m_PropertiesManager;
            }

            Editor::PropertiesGenerator& GetPropertiesGenerator()
            {
                return m_PropertiesGenerator;
            }

            virtual void OnIntersection(wxCommandEvent& event) override;
            virtual void OnUnion(wxCommandEvent& event) override;

        private:
            CommandQueue                m_CommandQueue;
            TreeCanvas                  m_PropertiesCanvas;
            Editor::PropertiesGenerator   m_PropertiesGenerator; // HEADS UP: do this one first in the constructor!
            Editor::PropertiesManager     m_PropertiesManager;
        };
    }
}