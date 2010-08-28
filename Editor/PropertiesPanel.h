#pragma once

#include "Editor/EditorGenerated.h"
#include "Editor/Inspect/TreeCanvasCtrl.h"

#include "Core/Scene/PropertiesManager.h"

namespace Helium
{
    namespace Editor
    {
        class PropertiesPanel : public PropertiesPanelGenerated
        {
        public:
            PropertiesPanel( Core::PropertiesManager* manager, wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );

            Inspect::TreeCanvasCtrl* GetPropertiesCanvas()
            {
                return m_PropertyCanvas;
            }

        private:
            Core::PropertiesManager* m_PropertiesManager;
            Inspect::TreeCanvasCtrl* m_PropertyCanvas;

            virtual void OnIntersection(wxCommandEvent& event) HELIUM_OVERRIDE;
            virtual void OnUnion(wxCommandEvent& event) HELIUM_OVERRIDE;
        };
    }
}