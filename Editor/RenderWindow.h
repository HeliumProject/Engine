#pragma once

#include "Platform/Types.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Pipeline/Render/Renderer.h"
#include "Pipeline/SceneGraph/Camera.h"

namespace Helium
{
    namespace Editor
    {
        struct D3DEventArgs;

        typedef std::vector< Render::RenderScene* > V_Scene;

        class RenderWindow : public wxWindow
        {
        public:
            RenderWindow( wxWindow *parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE, const wxString& name = wxT( "Editor::RenderWindow" ) );
            virtual ~RenderWindow();

            bool LoadScene( const tstring& path );
            void ClearScene();

            virtual bool SaveScreenShotAs( const tstring& path );

            virtual void DisplayReferenceAxis( bool display );

            virtual void Frame();

        protected:
            virtual void SetupLighting( Render::RenderScene* scene );
            virtual void Draw();
            virtual void Resize( const wxSize& size );
            virtual void ShowContextMenu( const wxPoint& pos );

            bool DrawScene();

        private:
            void OnSize( wxSizeEvent& args );
            void OnPaint( wxPaintEvent& args );

            void OnMouseCaptureLost( wxMouseCaptureLostEvent& args );

            void OnMouseDown( wxMouseEvent& args );
            void OnMouseUp( wxMouseEvent& args );
            void OnMouseMove( wxMouseEvent& args );
            void OnMouseWheel( wxMouseEvent& args );

            void OnScreenShotToFile( wxCommandEvent& args );
            void OnFrame( wxCommandEvent& args );
            void OnChangeAxisDisplay( wxCommandEvent& args );

        protected:
            Render::Renderer m_Render;
            Render::RenderScene* m_Scene;
            SceneGraph::Camera m_Camera;

            bool m_IsDeviceLost;
            bool m_DisplayAxis;

            uint32_t m_MeshHandle;

            wxMenu m_ContextMenu;
            wxMenu* m_AxisSubMenu;
            int32_t m_AxisOnMenuID;
            int32_t m_AxisOffMenuID;

        private:
            DECLARE_EVENT_TABLE();
        };
    }
}