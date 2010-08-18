#pragma once

#include "Foundation/Math/Constants.h"
#include "Foundation/Math/Vector3.h"
#include "Foundation/Math/AlignedBox.h"
#include "Foundation/Reflect/Enumeration.h"

#include "Foundation/Undo/Queue.h"

#include "Editor/API.h"
#include "Core/Scene/Render.h"
#include "Core/Scene/Camera.h"
#include "Core/Scene/Resource.h"
#include "Core/Scene/Viewport.h"
#include "Core/Scene/ViewportSettings.h"
#include "Core/Render/DeviceManager.h"

namespace Helium
{
    namespace Editor
    {
        class ViewCanvas : public wxWindow
        {
        public:
            ViewCanvas(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE,
                const wxString& name = wxT( "Editor::Viewport" ) );

            DECLARE_EVENT_TABLE();

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

            Core::Viewport& GetViewport()
            {
                return m_Viewport;
            }

        private:
            bool            m_Focused;
            Core::Viewport  m_Viewport;
        };
    }
}