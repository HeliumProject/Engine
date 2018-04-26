#pragma once

#include "EditorScene/API.h"
#include "EditorScene/Curve.h"
#include "EditorScene/Tool.h"

namespace Helium
{
	namespace Editor
	{
		class HELIUM_EDITOR_SCENE_API CurveCreateTool : public Tool
		{
		private:
			// Created flag
			bool m_Created;

			// The selection of the created objects
			OS_SceneNodeDumbPtr m_Selection;

			// The instance we are creating
			CurvePtr m_Instance;

		public:
			// Type of curve interpolation
			static CurveType s_CurveType;

			// Should we snap to surfaces
			static bool s_SurfaceSnap;

			// Should we snap to objects
			static bool s_ObjectSnap;

		public:
			HELIUM_DECLARE_ABSTRACT(Editor::CurveCreateTool, Tool);

		public:
			CurveCreateTool( Editor::Scene* scene, PropertiesGenerator* generator );
			virtual ~CurveCreateTool();

			void CreateInstance( const Vector3& position );
			void PickPosition( int x, int y, Vector3& position );

		private:
			void AddToScene();

		public:
			virtual bool AllowSelection() override;

			virtual bool MouseDown( const MouseButtonInputEvent& e ) override;
			virtual void MouseMove( const MouseMoveInputEvent& e ) override;
			virtual void KeyPress( const KeyboardInputEvent& e ) override;

			virtual void CreateProperties() override;

			bool GetSurfaceSnap() const;
			void SetSurfaceSnap( bool snap );

			bool GetObjectSnap() const;
			void SetObjectSnap( bool snap );

			int GetPlaneSnap() const;
			void SetPlaneSnap(int snap);

			int GetCurveType() const;
			void SetCurveType( int selection );

			bool GetClosed() const;
			void SetClosed( bool closed );
		};
	}
}
