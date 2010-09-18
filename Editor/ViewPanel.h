#pragma once

#include "Editor/EditorGenerated.h"
#include "Editor/ViewCanvas.h"

#include "Core/SettingsManager.h"

namespace Helium
{
    namespace Editor
    {
        namespace ViewPanelEvents
        {
            enum ViewPanelEvent
            {
                Parent,
                Unparent,
                Group,
                Ungroup,
                Center,
                InvertSelection,
                SelectAll,
                Duplicate,
                SmartDuplicate,
                CopyTransform,
                PasteTransform,
                SnapToCamera,
                SnapCameraTo,

                WalkUp,
                WalkDown,
                WalkForward,
                WalkBackward,

                ToggleAxes,
                ToggleGrid,
                ToggleBounds,
                ToggleStatistics,

                OrbitCamera,
                FrontCamera,
                SideCamera,
                TopCamera,

                WireframeOnMesh,
                WireframeOnShaded,
                Wireframe,
                Material, 
                Texture,

                ToggleFrustumCulling, 
                ToggleBackfaceCulling,

                ShowAll,
                ShowAllGeometry,
                ShowSelected,
                ShowSelectedGeometry,
                ShowUnrelated,
                ShowLastHidden,

                HideAll,
                HideAllGeometry,
                HideSelected,
                HideSelectedGeometry,
                HideUnrelated,

                FrameOrigin,
                FrameSelected,

                ToggleHighlightMode,

                PreviousView,
                NextView,
            };
        }
        typedef ViewPanelEvents::ViewPanelEvent ViewPanelEvent;

        class ViewPanel : public ViewPanelGenerated
        {
        public:
            ViewPanel( Core::SettingsManager* settingsManager, wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );

            ViewCanvas* GetViewCanvas()
            {
                return m_ViewCanvas;
            }

        private:
            void RefreshButtonStates();

        private:
            void OnChar( wxKeyEvent& event );

            void OnRenderMode( wxCommandEvent& event );
            void OnCamera( wxCommandEvent& event );
            void OnFrameOrigin( wxCommandEvent& event ) HELIUM_OVERRIDE;
            void OnFrameSelected( wxCommandEvent& event ) HELIUM_OVERRIDE;
            void OnToggleHighlightMode( wxCommandEvent& event ) HELIUM_OVERRIDE;
            void OnNextView( wxCommandEvent& event ) HELIUM_OVERRIDE;
            void OnPreviousView( wxCommandEvent& event ) HELIUM_OVERRIDE;

        private:
            ViewCanvas* m_ViewCanvas;
        };
    }
}