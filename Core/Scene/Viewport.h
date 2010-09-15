#pragma once

#include "Foundation/Math/Constants.h"
#include "Foundation/Math/Vector3.h"
#include "Foundation/Math/AlignedBox.h"
#include "Foundation/Reflect/Enumeration.h"

#include "Foundation/Undo/Queue.h"

#include "Core/API.h"
#include "Core/SettingsManager.h"
#include "Core/Scene/Render.h"
#include "Core/Scene/Camera.h"
#include "Core/Scene/Resource.h"
#include "Core/Scene/ViewportSettings.h"
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

        typedef Helium::Signature< Core::RenderVisitor* > RenderSignature;

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

        typedef Helium::Signature< const Core::SelectArgs& > SelectSignature;

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

        typedef Helium::Signature< const Core::SetHighlightArgs& > SetHighlightSignature;

        struct ClearHighlightArgs
        {
            bool m_Update;

            ClearHighlightArgs(bool update)
                : m_Update (update)
            {

            }
        };

        typedef Helium::Signature< const Core::ClearHighlightArgs& > ClearHighlightSignature;

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

        typedef Helium::Signature< const Core::ToolChangeArgs& > ToolChangeSignature;

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

        typedef Helium::Signature< const CameraModeChangeArgs& > CameraModeChangeSignature;

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
        public:
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

            static void InitializeType();
            static void CleanupType();

            Viewport( HWND wnd, SettingsManager* settingsManager );
            ~Viewport();

            void Reset();

            void LoadSettings(ViewportSettings* prefs);
            void SaveSettings(ViewportSettings* prefs);

            void SetSize( Math::Point size )
            {
                m_Size = size;
            }

            inline IDirect3DDevice9* GetDevice() const
            {
                return m_DeviceManager.GetD3DDevice();
            }

            SettingsManager* GetSettingsManager() const
            {
                return m_SettingsManager;
            }

            ResourceTracker* GetResources() const
            {
                return m_ResourceTracker;
            }

            Statistics* GetStatistics() const
            {
                return m_Statistics;
            }

            Core::Camera* GetCamera()
            {
                return &m_Cameras[m_CameraMode];
            }

            const Core::Camera* GetCamera() const
            {
                return &m_Cameras[m_CameraMode];
            }

            Core::Camera* GetCameraForMode(CameraMode mode)
            {
                return &m_Cameras[mode]; 
            }

            CameraMode GetCameraMode() const
            {
                return m_CameraMode;
            }

            void SetCameraMode(CameraMode mode);
            void NextCameraMode();
            void PreviousCameraMode();

            GeometryMode GetGeometryMode() const
            {
                return m_GeometryMode;
            }

            void SetGeometryMode(GeometryMode mode)
            {
                m_GeometryMode = mode;
            }

            void NextGeometryMode();

            Core::Tool* GetTool()
            {
                return m_Tool;
            }

            void SetTool(Tool* tool);

            bool IsHighlighting() const;
            void SetHighlighting(bool highlight);

            bool IsAxesVisible() const
            {
                return m_AxesVisible;
            }

            void SetAxesVisible(bool visible)
            {
                m_AxesVisible = visible;
            }

            bool IsGridVisible() const
            {
                return m_GridVisible;
            }

            void SetGridVisible(bool visible)
            {
                m_GridVisible = visible;
            }

            bool IsBoundsVisible() const
            {
                return m_BoundsVisible;
            }

            void SetBoundsVisible(bool visible)
            {
                m_BoundsVisible = visible;
            }

            bool IsStatisticsVisible() const
            {
                return m_StatisticsVisible;
            }

            void SetStatisticsVisible(bool visible)
            {
                m_StatisticsVisible = visible;
            }

            Core::Primitive* GetGlobalPrimitive( GlobalPrimitives::GlobalPrimitive which );

        private:
            void InitDevice( HWND wnd );
            void InitWidgets();
            void InitCameras();

        public:
            void SetSize(u32 x, u32 y);
            void SetFocused(bool focused);

            void KeyDown( const Helium::KeyboardInput& input );
            void KeyUp( const Helium::KeyboardInput& input );
            void KeyPress( const Helium::KeyboardInput& input );

            void MouseDown( const Helium::MouseButtonInput& input );
            void MouseUp( const Helium::MouseButtonInput& input );
            void MouseMove( const Helium::MouseMoveInput& input );
            void MouseScroll( const Helium::MouseScrollInput& input );

            void Draw();

            void UndoTransform();
            void UndoTransform( CameraMode mode );
            void RedoTransform();
            void RedoTransform( CameraMode mode );    
            void UpdateCameraHistory();

        private:
            // callbacks
            void ReleaseResources( const Render::DeviceStateArgs& args );
            void AllocateResources( const Render::DeviceStateArgs& args );
            void CameraMoved( const Core::CameraMovedArgs& args );

            // 
            // Events
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
            void OnGridSettingsChanged( const Reflect::ElementChangeArgs& args );

        private:
            HWND                    m_Window;
            Math::Point             m_Size;
            bool                    m_Focused;

            SettingsManager*        m_SettingsManager;

            Render::DeviceManager   m_DeviceManager;
            ResourceTracker*        m_ResourceTracker;
            RenderVisitor           m_RenderVisitor;

            Tool*                   m_Tool;
            Camera                  m_Cameras[CameraModes::Count];
            Undo::Queue             m_CameraHistory[CameraModes::Count];

            CameraMode              m_CameraMode;
            GeometryMode            m_GeometryMode;

            DragMode                m_DragMode;
            Math::Point             m_Start;
            Math::Point             m_End;

            bool                    m_Highlighting;
            bool                    m_AxesVisible;
            bool                    m_GridVisible;
            bool                    m_BoundsVisible;
            bool                    m_StatisticsVisible;
            Statistics*             m_Statistics;
            PrimitiveFrame*         m_SelectionFrame;
            Primitive*              m_GlobalPrimitives[GlobalPrimitives::Count];
        };
    }
}