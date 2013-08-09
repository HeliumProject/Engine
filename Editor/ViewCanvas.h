#pragma once

#include "Math/Axes.h"
#include "Math/Vector3.h"
#include "Math/AlignedBox.h"

#include "Reflect/MetaEnum.h"
#include "Application/UndoQueue.h"

#include "SceneGraph/Render.h"
#include "SceneGraph/Camera.h"
#include "SceneGraph/Resource.h"
#include "SceneGraph/Viewport.h"
#include "SceneGraph/ViewportSettings.h"
#include "SceneGraph/SettingsManager.h"
#include "SceneGraph/DeviceManager.h"

#include "Editor/API.h"

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