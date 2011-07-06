#pragma once

#include "Math/Axes.h"
#include "Math/FpuVector3.h"
#include "Math/FpuAlignedBox.h"
#include "Foundation/Reflect/Enumeration.h"

#include "Foundation/Undo/Queue.h"

#include "Editor/API.h"
#include "Pipeline/SettingsManager.h"
#include "Pipeline/SceneGraph/Render.h"
#include "Pipeline/SceneGraph/Camera.h"
#include "Pipeline/SceneGraph/Resource.h"
#include "Pipeline/SceneGraph/Viewport.h"
#include "Pipeline/SceneGraph/ViewportSettings.h"
#include "Pipeline/SceneGraph/DeviceManager.h"

namespace Helium
{
    namespace Editor
    {
        class ViewCanvas : public wxWindow
        {
        public:
            ViewCanvas( SettingsManager* settingsManager,
                wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE,
                const wxString& name = wxT( "Editor::Viewport" ) );

        public:
            void OnSize(wxSizeEvent& e);
            void OnPaint(wxPaintEvent& e);
            void OnSetFocus(wxFocusEvent& e);
            void OnKillFocus(wxFocusEvent& e);

            void OnKeyDown(wxKeyEvent& e);
            void OnKeyUp(wxKeyEvent& e);
            void OnChar(wxKeyEvent& e);

            void OnMouseDown(wxMouseEvent& e);
            void OnMouseUp(wxMouseEvent& e);
            void OnMouseMove(wxMouseEvent& e);
            void OnMouseScroll(wxMouseEvent& e);
            void OnMouseLeave(wxMouseEvent& e);
            void OnMouseCaptureLost(wxMouseCaptureLostEvent& e);

            SceneGraph::Viewport& GetViewport()
            {
                return m_Viewport;
            }

        private:
            bool                    m_Focused;
            SceneGraph::Viewport    m_Viewport;
        };
    }
}