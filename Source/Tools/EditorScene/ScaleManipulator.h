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

			virtual void ResetSize() override;

			virtual void ScaleTo(float factor) override;

			virtual void Evaluate() override;

		protected:
			virtual void SetResult() override;

		public:
			virtual void Draw( BufferedDrawer* pDrawer ) override;
			virtual bool Pick( PickVisitor* pick ) override;

			virtual bool MouseDown( const MouseButtonInputEvent& e ) override;
			virtual void MouseMove( const MouseMoveInputEvent& e ) override;

			virtual void CreateProperties() override;

			float32_t GetSize() const;
			void SetSize( float32_t size );

			bool GetGridSnap() const;
			void SetGridSnap( bool gridSnap );

			float GetDistance() const;
			void SetDistance( float distance );
		};
	}
}
