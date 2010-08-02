#pragma once

#include "Foundation/Math/Constants.h"
#include "Foundation/Math/Vector3.h"
#include "Foundation/Math/AlignedBox.h"

#include "Editor/API.h"
#include "Render.h"
#include "Resource.h"
#include "Camera.h"
#include "Render/D3DManager.h"
#include "Application/Undo/Queue.h"

#include "Foundation/Reflect/Enumeration.h"

namespace Helium
{
    namespace Editor
    {
        //
        // Enums
        //

        namespace DragModes
        {
            enum DragMode
            {
                None,
                Select,
                Camera,
                Tool,
            };
        }

        typedef DragModes::DragMode DragMode;

        namespace GeometryModes
        {
            enum GeometryMode
            {
                None,
                Render,
                Collision,
                Pathfinding,
                Count
            };

            static void GeometryModeEnumerateEnumeration( Reflect::Enumeration* info )
            {
                info->AddElement(GeometryModes::None, TXT( "GeometryModes::None" ) ); 
                info->AddElement(GeometryModes::Render, TXT( "GeometryModes::Render" ) ); 
                info->AddElement(GeometryModes::Collision, TXT( "GeometryModes::Collision" ) ); 
                info->AddElement(GeometryModes::Pathfinding, TXT( "GeometryModes::Pathfinding" ) ); 
                info->AddElement(GeometryModes::Count, TXT( "GeometryModes::Count" ) ); 
            }
        }

        typedef GeometryModes::GeometryMode GeometryMode;

        namespace CameraModes
        {
            enum CameraMode
            {
                Orbit,
                Front,
                Side,
                Top,

                Count
            };

            static void CameraModeEnumerateEnumeration( Reflect::Enumeration* info )
            {
                info->AddElement(CameraModes::Orbit, TXT( "CameraModes::Orbit" ), TXT( "Orbit" ) ); 
                info->AddElement(CameraModes::Front, TXT( "CameraModes::Front" ), TXT( "Front" ) ); 
                info->AddElement(CameraModes::Side, TXT( "CameraModes::Side" ), TXT( "Side" ) ); 
                info->AddElement(CameraModes::Top, TXT( "CameraModes::Top" ), TXT( "Top" ) ); 
                info->AddElement(CameraModes::Count, TXT( "CameraModes::Count" ) ); 
            }
        }

        typedef CameraModes::CameraMode CameraMode;

        namespace GlobalPrimitives
        {
            enum GlobalPrimitive
            {
                ViewportAxes,
                StandardAxes,
                StandardGrid,
                StandardRings,
                TransformAxes,
                SelectedAxes,
                JointAxes,
                JointRings,

                Count
            };
        }

        typedef GlobalPrimitives::GlobalPrimitive GlobalPrimitive;

        //
        // Events
        //

        typedef Helium::Signature< void, Editor::RenderVisitor* > RenderSignature;

        namespace SelectionModes
        {
            enum SelectionMode
            {
                //
                // Click/Bounds based select modes
                //

                Replace,    // replace selection with targets
                Add,        // add targets to selection
                Remove,     // remove targets from selection
                Toggle,     // toggle targets in selection

                //
                // Context Menu based select modes
                //

                Type,       // enumerate by type
                Manifest,   // manifest target region with items
            };
        }

        typedef SelectionModes::SelectionMode SelectionMode;

        namespace SelectionTargetModes
        {
            enum SelectionTargetMode
            {
                Single,
                Multiple
            };
        }

        typedef SelectionTargetModes::SelectionTargetMode SelectionTargetMode;

        // 
        // Struct to be passed into all select calls in the system.  Contains the pick
        // information, and whether the user wants a manifest of what all the pick encompasses to select one from
        // 

        struct SelectArgs
        {
            PickVisitor* m_Pick;
            SelectionMode m_Mode;
            SelectionTargetMode m_Target;

            SelectArgs( PickVisitor* pick, SelectionMode mode, SelectionTargetMode target )
                : m_Pick (pick)
                , m_Mode (mode)
                , m_Target (target)
            {

            }

            SelectArgs( const SelectArgs& rhs )
                : m_Pick (rhs.m_Pick)
                , m_Mode (rhs.m_Mode)
                , m_Target (rhs.m_Target)
            {
            }
        };

        typedef Helium::Signature< void, const Editor::SelectArgs& > SelectSignature;

        //
        // Highlight
        //

        struct SetHighlightArgs
        {
            PickVisitor* m_Pick;
            SelectionTargetMode m_Target;

            SetHighlightArgs(PickVisitor* pick, SelectionTargetMode target)
                : m_Pick (pick)
                , m_Target (target)
            {

            }
        };

        typedef Helium::Signature< void, const Editor::SetHighlightArgs& > SetHighlightSignature;

        struct ClearHighlightArgs
        {
            bool m_Update;

            ClearHighlightArgs(bool update)
                : m_Update (update)
            {

            }
        };

        typedef Helium::Signature< void, const Editor::ClearHighlightArgs& > ClearHighlightSignature;

        // 
        // Tool 
        // 

        class Tool;

        struct ToolChangeArgs
        {
            Editor::Tool* m_NewTool;

            ToolChangeArgs( Editor::Tool* newTool )
                : m_NewTool( newTool )
            {
            }
        };

        typedef Helium::Signature< void, const Editor::ToolChangeArgs& > ToolChangeSignature;

        struct CameraModeChangeArgs
        {
            CameraMode m_OldMode;
            CameraMode m_NewMode;

            CameraModeChangeArgs( CameraMode oldMode, CameraMode newMode )
                : m_OldMode( oldMode )
                , m_NewMode( newMode )
            {
            }
        };

        typedef Helium::Signature< void, const CameraModeChangeArgs& > CameraModeChangeSignature;

        //
        // Viewport Manager
        //

        class Statistics;
        class Primitive;
        class PrimitiveAxes;
        class PrimitiveGrid;
        class PrimitiveFrame;

        class EDITOR_SCENE_API Viewport : public wxWindow
        {
            //
            // Members
            //

        private:
            // Control/Device
            bool m_Focused;
            Render::D3DManager m_D3DManager;

            // Resources
            ResourceTracker* m_ResourceTracker;

            // Renderer
            RenderVisitor m_RenderVisitor;

            // Tool
            Editor::Tool* m_Tool;

            // Camera
            Editor::Camera m_Cameras[CameraModes::Count];
            Undo::Queue m_CameraHistory[CameraModes::Count];

            CameraMode m_CameraMode;
            GeometryMode m_GeometryMode;

            // Drag
            DragMode m_DragMode;
            Math::Point m_Start;
            Math::Point m_End;

            // Widgets
            bool m_Highlighting;
            bool m_AxesVisible;
            bool m_GridVisible;
            bool m_BoundsVisible;
            bool m_StatisticsVisible;
            Statistics* m_Statistics;
            Editor::PrimitiveFrame* m_SelectionFrame;
            Editor::Primitive* m_GlobalPrimitives[GlobalPrimitives::Count];

        public:
            // Materials
            static D3DMATERIAL9 s_LiveMaterial;
            static D3DMATERIAL9 s_SelectedMaterial;
            static D3DMATERIAL9 s_ReactiveMaterial;
            static D3DMATERIAL9 s_HighlightedMaterial;
            static D3DMATERIAL9 s_UnselectableMaterial;
            static D3DMATERIAL9 s_ComponentMaterial;
            static D3DMATERIAL9 s_SelectedComponentMaterial;

            static D3DMATERIAL9 s_RedMaterial;
            static D3DMATERIAL9 s_YellowMaterial;
            static D3DMATERIAL9 s_GreenMaterial;
            static D3DMATERIAL9 s_BlueMaterial;

            //
            // Setup/Reset
            //

            static void InitializeType();
            static void CleanupType();

            Viewport(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER | wxFULL_REPAINT_ON_RESIZE,
                const wxString& name = wxT( "Editor::Viewport" ) );

            ~Viewport ();

            DECLARE_EVENT_TABLE();

        public:
            inline IDirect3DDevice9* GetDevice() const
            {
                return m_D3DManager.GetD3DDevice();
            }

            ResourceTracker* GetResources() const;
            Statistics* GetStatistics() const;

            Editor::Camera* GetCamera();
            const Editor::Camera* GetCamera() const;

            Editor::Camera* GetCameraForMode(CameraMode mode); 

            CameraMode GetCameraMode() const;
            void SetCameraMode(CameraMode mode);
            void NextCameraMode();

            GeometryMode GetGeometryMode() const;
            void SetGeometryMode(GeometryMode mode);
            void NextGeometryMode();

            Editor::Tool* GetTool();
            void SetTool(Editor::Tool* tool);

            bool IsHighlighting() const;
            void SetHighlighting(bool highlight);

            bool IsAxesVisible() const;
            void SetAxesVisible(bool visible);

            bool IsGridVisible() const;
            void SetGridVisible(bool visible);

            bool IsBoundsVisible() const;
            void SetBoundsVisible(bool visible);

            bool IsStatisticsVisible() const;
            void SetStatisticsVisible(bool visible);

            Editor::Primitive* GetGlobalPrimitive( GlobalPrimitives::GlobalPrimitive which );

        private:
            void InitDevice();
            void InitWidgets();
            void InitCameras();

            void Reset();

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

        public:
            void Draw();

            /// @brief Undo the last translation of the last selected view
            void UndoTransform();

            /// @brief Undo the last translation of the specified camera mode
            void UndoTransform( CameraMode mode );

            /// @brief Redo the last translation of the last selected view
            void RedoTransform();

            /// @brief Redo the last translation of the specified view
            void RedoTransform( CameraMode mode );    

            /// @brief Update the camera history. 
            /// Update the camera history so we can undo/redo previous camera moves. 
            /// This is implemented seperately from 'CameraMoved' since 'CameraMoved' reports all incremental spots during a transition.
            /// We also need to be able to update this from other events in the scene editor, such as when we focus on an object
            /// directly ( shortcut key - f )
            void UpdateCameraHistory( );

        private:
            void PreDraw( DrawArgs* args );
            void PostDraw( DrawArgs* args );
            void OnReleaseResources( const Render::DeviceStateArgs& args );
            void OnAllocateResources( const Render::DeviceStateArgs& args );

        private:
            void CameraMoved( const Editor::CameraMovedArgs& args );
        public:
            void RemoteCameraMoved( const Math::Matrix4& transform );

            // 
            // Listeners
            // 

        protected:
            RenderSignature::Event m_Render;
        public:
            void AddRenderListener( const RenderSignature::Delegate& listener )
            {
                m_Render.Add( listener );
            }
            void RemoveRenderListener( const RenderSignature::Delegate& listener )
            {
                m_Render.Remove( listener );
            }

        protected:
            SelectSignature::Event m_Select;
        public:
            void AddSelectListener( const SelectSignature::Delegate& listener )
            {
                m_Select.Add( listener );
            }
            void RemoveSelectListener( const SelectSignature::Delegate& listener )
            {
                m_Select.Remove( listener );
            }

        protected:
            SetHighlightSignature::Event m_SetHighlight;
        public:
            void AddSetHighlightListener( const SetHighlightSignature::Delegate& listener )
            {
                m_SetHighlight.Add( listener );
            }
            void RemoveSetHighlightListener( const SetHighlightSignature::Delegate& listener )
            {
                m_SetHighlight.Remove( listener );
            }

        protected:
            ClearHighlightSignature::Event m_ClearHighlight;
        public:
            void AddClearHighlightListener( const ClearHighlightSignature::Delegate& listener )
            {
                m_ClearHighlight.Add( listener );
            }
            void RemoveClearHighlightListener( const ClearHighlightSignature::Delegate& listener )
            {
                m_ClearHighlight.Remove( listener );
            }

        protected:
            ToolChangeSignature::Event m_ToolChanged;
        public:
            void AddToolChangedListener( const ToolChangeSignature::Delegate& listener )
            {
                m_ToolChanged.Add( listener );
            }
            void RemoveToolChangedListener( const ToolChangeSignature::Delegate& listener )
            {
                m_ToolChanged.Remove( listener );
            }

        protected:
            CameraMovedSignature::Event m_CameraMoved;
        public:
            void AddCameraMovedListener( const CameraMovedSignature::Delegate& listener )
            {
                m_CameraMoved.Add( listener );
            }
            void RemoveCameraMovedListener( const CameraMovedSignature::Delegate& listener )
            {
                m_CameraMoved.Remove( listener );
            }

        protected:
            CameraModeChangeSignature::Event m_CameraModeChanged;
        public:
            void AddCameraModeChangedListener( const CameraModeChangeSignature::Delegate& listener )
            {
                m_CameraModeChanged.Add( listener );
            }
            void RemoveCameraModeChangedListener( const CameraModeChangeSignature::Delegate& listener )
            {
                m_CameraModeChanged.Remove( listener );
            }

        protected:
            void OnGridPreferencesChanged( const Reflect::ElementChangeArgs& args );
        };
    }
}