#pragma once

#include "EditorScene/SettingsManager.h"

#include "Editor/EditorGeneratedWrapper.h"
#include "Editor/ViewCanvas.h"

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
            ViewPanel( SettingsManager* settingsManager, wxWindow* parent = NULL, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );

            ViewCanvas* GetViewCanvas()
            {
                return m_ViewCanvas;
            }

            static wxSize DefaultIconSize;

        private:
            void RefreshButtonStates();

        private:
            void OnChar( wxKeyEvent& event );

            void OnRenderMode( wxCommandEvent& event );
            void OnCamera( wxCommandEvent& event );
            void OnFrameOrigin( wxCommandEvent& event );
            void OnFrameSelected( wxCommandEvent& event );
            void OnToggleHighlightMode( wxCommandEvent& event );
            void OnToggleShowAxes( wxCommandEvent& event );
            void OnToggleShowGrid( wxCommandEvent& event );
            void OnNextView( wxCommandEvent& event );
            void OnPreviousView( wxCommandEvent& event );

        private:
            ViewCanvas* m_ViewCanvas;
        };
    }
}