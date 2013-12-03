#pragma once

#include "EditorScene/SettingsManager.h"
#include "EditorScene/TransformManipulator.h"

namespace Helium
{
	namespace Editor
	{
		class PrimitiveAxes;
		class PrimitiveCube;

		class HELIUM_EDITOR_SCENE_API ScaleManipulator : public TransformManipulator
		{
		private:
			// UI
			Editor::PrimitiveAxes* m_Axes;
			Editor::PrimitiveCube* m_Cube;
			Editor::PrimitiveCube* m_XCube;
			Vector3 m_XPosition;
			Editor::PrimitiveCube* m_YCube;
			Vector3 m_YPosition;
			Editor::PrimitiveCube* m_ZCube;
			Vector3 m_ZPosition;

			float32_t m_Size;
			bool m_GridSnap;
			float32_t m_Distance;

			SettingsManager* m_SettingsManager;

			HELIUM_DECLARE_ABSTRACT(Editor::ScaleManipulator, Editor::TransformManipulator);

		public:
			ScaleManipulator( SettingsManager* settingsManager, const ManipulatorMode mode, Editor::Scene* scene, PropertiesGenerator* generator);

			~ScaleManipulator();

			virtual void ResetSize() HELIUM_OVERRIDE;

			virtual void ScaleTo(float factor) HELIUM_OVERRIDE;

			virtual void Evaluate() HELIUM_OVERRIDE;

		protected:
			virtual void SetResult() HELIUM_OVERRIDE;

		public:
			virtual void Draw( BufferedDrawer* pDrawer ) HELIUM_OVERRIDE;
			virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

			virtual bool MouseDown( const MouseButtonInput& e ) HELIUM_OVERRIDE;
			virtual void MouseMove( const MouseMoveInput& e ) HELIUM_OVERRIDE;

			virtual void CreateProperties() HELIUM_OVERRIDE;

			float32_t GetSize() const;
			void SetSize( float32_t size );

			bool GetGridSnap() const;
			void SetGridSnap( bool gridSnap );

			float GetDistance() const;
			void SetDistance( float distance );
		};
	}
}