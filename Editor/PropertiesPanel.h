#pragma once

#include "Core/Scene/PropertiesManager.h"
#include "Core/Scene/PropertiesGenerator.h"

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

            Core::PropertiesManager& GetPropertiesManager()
            {
                return m_PropertiesManager;
            }

            virtual void OnIntersection(wxCommandEvent& event) HELIUM_OVERRIDE;
            virtual void OnUnion(wxCommandEvent& event) HELIUM_OVERRIDE;

        private:
            TreeCanvas                  m_PropertiesCanvas;
            Core::PropertiesGenerator   m_PropertiesGenerator; // HEADS UP: do this one first in the constructor!
            Core::PropertiesManager     m_PropertiesManager;
        };
    }
}