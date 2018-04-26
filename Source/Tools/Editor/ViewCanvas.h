#pragma once

#include "Math/Axes.h"
#include "Math/Vector3.h"
#include "Math/AlignedBox.h"

#include "Reflect/MetaEnum.h"
#include "Application/UndoQueue.h"

#include "EditorScene/Render.h"
#include "EditorScene/Camera.h"
#include "EditorScene/Resource.h"
#include "EditorScene/Viewport.h"
#include "EditorScene/ViewportSettings.h"
#include "EditorScene/SettingsManager.h"
#include "EditorScene/DeviceManager.h"

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

            Editor::Viewport& GetViewport()
            {
                return m_Viewport;
            }

        private:
            bool                    m_Focused;
            Editor::Viewport    m_Viewport;
        };
    }
}