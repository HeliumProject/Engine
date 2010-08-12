#pragma once

#include "Foundation/Math/Constants.h"
#include "Foundation/Math/Vector3.h"
#include "Foundation/Math/AlignedBox.h"
#include "Foundation/Reflect/Enumeration.h"

#include "Application/Undo/Queue.h"

#include "Core/API.h"
#include "Core/Scene/Render.h"
#include "Core/Scene/Camera.h"
#include "Core/Scene/Resource.h"
#include "Core/Scene/ViewportPreferences.h"
#include "Core/Render/DeviceManager.h"

namespace Helium
{
    namespace Core
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

        typedef Helium::Signature< void, Core::RenderVisitor* > RenderSignature;

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

        typedef Helium::Signature< void, const Core::SelectArgs& > SelectSignature;

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

        typedef Helium::Signature< void, const Core::SetHighlightArgs& > SetHighlightSignature;

        struct ClearHighlightArgs
        {
            bool m_Update;

            ClearHighlightArgs(bool update)
                : m_Update (update)
            {

            }
        };

        typedef Helium::Signature< void, const Core::ClearHighlightArgs& > ClearHighlightSignature;

        // 
        // Tool 
        // 

        class Tool;

        struct ToolChangeArgs
        {
            Core::Tool* m_NewTool;

            ToolChangeArgs( Core::Tool* newTool )
                : m_NewTool( newTool )
            {
            }
        };

        typedef Helium::Signature< void, const Core::ToolChangeArgs& > ToolChangeSignature;

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

        class CORE_API Viewport
        {
            //
            // Members
            //

        private:
            Math::Point m_Size;

            HWND m_Window;

            // Control/Device
            Render::DeviceManager m_DeviceManager;

            // Resources
            ResourceTracker* m_ResourceTracker;

            // Renderer
            RenderVisitor m_RenderVisitor;

            // Tool
            Tool* m_Tool;

            // Camera
            Camera m_Cameras[CameraModes::Count];
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
            PrimitiveFrame* m_SelectionFrame;
            Primitive* m_GlobalPrimitives[GlobalPrimitives::Count];

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

            Viewport( HWND wnd );
            ~Viewport();

        public:
            void LoadPreferences(ViewportPreferences* prefs);
            void SavePreferences(ViewportPreferences* prefs);

            void SetSize( Math::Point size )
            {
                m_Size = size;
            }

            inline IDirect3DDevice9* GetDevice() const
            {
                return m_DeviceManager.GetD3DDevice();
            }

            ResourceTracker* GetResources() const;
            Statistics* GetStatistics() const;

            Camera* GetCamera();
            const Camera* GetCamera() const;

            Camera* GetCameraForMode(CameraMode mode); 

            CameraMode GetCameraMode() const;
            void SetCameraMode(CameraMode mode);
            void NextCameraMode();

            GeometryMode GetGeometryMode() const;
            void SetGeometryMode(GeometryMode mode);
            void NextGeometryMode();

            Tool* GetTool();
            void SetTool(Tool* tool);

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

            Core::Primitive* GetGlobalPrimitive( GlobalPrimitives::GlobalPrimitive which );

        private:
            void InitDevice( HWND wnd );
            void InitWidgets();
            void InitCameras();

            void Reset();

            void Resize(u32 x, u32 y);

            void KeyDown( const Helium::KeyboardInput& input );
            void KeyUp( const Helium::KeyboardInput& input );
            void KeyPress( const Helium::KeyboardInput& input );

            void OnMouseDown( const Helium::MouseButtonInput& input );
            void OnMouseUp( const Helium::MouseButtonInput& input );
            void OnMouseMove( const Helium::MouseMoveInput& input );
            void OnMouseScroll( const Helium::MouseScrollInput& input );

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
            void CameraMoved( const Core::CameraMovedArgs& args );
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
            void RaiseClearHighlight( const ClearHighlightArgs& args )
            {
                m_ClearHighlight.Raise( args );
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