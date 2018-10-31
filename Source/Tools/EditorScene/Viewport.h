#pragma once

#include "Math/Axes.h"
#include "Math/Vector3.h"
#include "Math/AlignedBox.h"

#include "Reflect/MetaEnum.h"
#include "Application/UndoQueue.h"

#include "Math/Color.h"

#include "EditorScene/API.h"
#include "EditorScene/Render.h"
#include "EditorScene/Camera.h"
#include "EditorScene/SettingsManager.h"
#include "EditorScene/ViewportSettings.h"

#include "Framework/Slice.h"
#include "Framework/World.h"

namespace Helium
{
	class GraphicsScene;

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

		typedef Helium::Signature< Editor::RenderVisitor* > RenderSignature;

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

		typedef Helium::Signature< const Editor::SelectArgs& > SelectSignature;

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

		typedef Helium::Signature< const Editor::SetHighlightArgs& > SetHighlightSignature;

		struct ClearHighlightArgs
		{
			bool m_Update;

			ClearHighlightArgs(bool update)
				: m_Update (update)
			{

			}
		};

		typedef Helium::Signature< const Editor::ClearHighlightArgs& > ClearHighlightSignature;

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

		typedef Helium::Signature< const Editor::ToolChangeArgs& > ToolChangeSignature;

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

		class Primitive;
		class PrimitiveAxes;
		class PrimitiveGrid;
		class PrimitiveFrame;

		class HELIUM_EDITOR_SCENE_API Viewport
		{
		public:
			static const Helium::Color s_LiveMaterial;
			static const Helium::Color s_SelectedMaterial;
			static const Helium::Color s_ReactiveMaterial;
			static const Helium::Color s_HighlightedMaterial;
			static const Helium::Color s_UnselectableMaterial;
			static const Helium::Color s_ComponentMaterial;
			static const Helium::Color s_SelectedComponentMaterial;

			static const Helium::Color s_RedMaterial;
			static const Helium::Color s_YellowMaterial;
			static const Helium::Color s_GreenMaterial;
			static const Helium::Color s_BlueMaterial;

			Viewport( void* wnd, SettingsManager* settingsManager );
			~Viewport();

			void BindToWorld( World* newWorld );
			void UnbindFromWorld();

			void Reset();

			void LoadSettings(ViewportSettings* prefs);
			void SaveSettings(ViewportSettings* prefs);

			void SetSize( Point size )
			{
				m_Size = size;
			}

			SettingsManager* GetSettingsManager() const
			{
				return m_SettingsManager;
			}

			Editor::Camera* GetCamera()
			{
				return &m_Cameras[m_CameraMode];
			}

			const Editor::Camera* GetCamera() const
			{
				return &m_Cameras[m_CameraMode];
			}

			Editor::Camera* GetCameraForMode(CameraMode mode)
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

			Editor::Tool* GetTool()
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

			Editor::Primitive* GetGlobalPrimitive( GlobalPrimitives::GlobalPrimitive which );

		private:
			void InitWidgets();
			void InitCameras();

			void OnResize();

		public:
			void SetSize(uint32_t x, uint32_t y);
			void SetFocused(bool focused);

			void KeyDown( const Helium::KeyboardInputEvent& input );
			void KeyUp( const Helium::KeyboardInputEvent& input );
			void KeyPress( const Helium::KeyboardInputEvent& input );

			void MouseDown( const Helium::MouseButtonInputEvent& input );
			void MouseUp( const Helium::MouseButtonInputEvent& input );
			void MouseMove( const Helium::MouseMoveInputEvent& input );
			void MouseScroll( const Helium::MouseScrollInputEvent& input );

			void Draw();

			void UndoTransform();
			void UndoTransform( CameraMode mode );
			void RedoTransform();
			void RedoTransform( CameraMode mode );    
			void UpdateCameraHistory();

		private:
			// callbacks
			void CameraMoved( const CameraMovedArgs& args );

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
			void OnGridSettingsChanged( const Reflect::ObjectChangeArgs& args );

			GraphicsScene *GetGraphicsScene();

		private:
			void*                   m_Window;
			Point                   m_Size;
			bool                    m_Focused;

			SettingsManager*        m_SettingsManager;

			RenderVisitor           m_RenderVisitor;

			WorldPtr                m_World;
			uint32_t                m_SceneViewId;

			Tool*                   m_Tool;
			Camera                  m_Cameras[CameraMode::Count];
			UndoQueue               m_CameraHistory[CameraMode::Count];

			CameraMode              m_CameraMode;
			GeometryMode            m_GeometryMode;

			DragMode                m_DragMode;
			Point                   m_Start;
			Point                   m_End;

			bool                    m_Highlighting;
			bool                    m_AxesVisible;
			bool                    m_GridVisible;
			bool                    m_BoundsVisible;
			PrimitiveFrame*         m_SelectionFrame;
			Primitive*              m_GlobalPrimitives[GlobalPrimitives::Count];
		};
	}
}