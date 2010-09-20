#pragma once

#include "Foundation/Math/Constants.h"
#include "Foundation/Math/Vector3.h"
#include "Foundation/Math/AlignedBox.h"
#include "Foundation/Reflect/Enumeration.h"

#include "Foundation/Undo/Queue.h"

#include "Editor/API.h"
#include "Core/SettingsManager.h"
#include "Core/SceneGraph/Render.h"
#include "Core/SceneGraph/Camera.h"
#include "Core/SceneGraph/Resource.h"
#include "Core/SceneGraph/Viewport.h"
#include "Core/SceneGraph/ViewportSettings.h"
#include "Core/Render/DeviceManager.h"

namespace Helium
{
    namespace Editor
    {
        class ViewCanvas : public wxWindow
        {
        public:
            ViewCanvas( Core::SettingsManager* settingsManager,
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