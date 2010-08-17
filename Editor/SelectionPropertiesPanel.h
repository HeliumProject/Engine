#pragma once

#include "Application/Inspect/Controls/InspectCanvas.h"

#include "Core/Scene/PropertiesManager.h"

#include "Editor/API.h"

namespace Helium
{
    namespace Editor
    {
        class SelectionPropertiesPanel : public wxPanel 
        {
        public:
            wxRadioButton* m_Intersection;
            wxRadioButton* m_Union;

            wxButton* m_ExpandAll;
            wxButton* m_CollapseAll;

            Core::PropertiesManager* m_PropertyManager;
            Inspect::TreeCanvasCtrl* m_PropertyCanvas;

            SelectionPropertiesPanel( Core::PropertiesManager* propertyManager, wxWindow* parent, int id = wxID_ANY, wxPoint pos = wxDefaultPosition, wxSize size = wxSize( 243,335 ), int style = wxTAB_TRAVERSAL );

            void OnIntersection(wxCommandEvent& event);
            void OnUnion(wxCommandEvent& event);
            void OnExpandAll(wxCommandEvent& event);
            void OnCollapseAll(wxCommandEvent& event);
        };
    }
}