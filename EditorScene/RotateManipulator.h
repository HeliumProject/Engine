#pragma once

#include "EditorScene/SettingsManager.h"
#include "EditorScene/TransformManipulator.h"

namespace Helium
{
	namespace Editor
	{
		class PrimitiveCircle;

		namespace RotationTypes
		{
			enum RotationType
			{
				None,
				Normal,
				ArcBall,
				CameraPlane,
			};
		}

		typedef RotationTypes::RotationType RotationType;

		class HELIUM_EDITOR_SCENE_API RotateManipulator : public TransformManipulator
		{
		private:
			SettingsManager* m_SettingsManager;

			float32_t m_Size;
			ManipulatorSpace m_Space;
			bool m_AxisSnap;
			float m_SnapDegrees;

			// Rotation drag mode
			RotationType m_Type;

			// UI
			Editor::PrimitiveCircle* m_Ring;

			HELIUM_DECLARE_ABSTRACT(Editor::RotateManipulator, Editor::TransformManipulator);

		public:
			RotateManipulator( SettingsManager* settingsManager, const ManipulatorMode mode, Editor::Scene* scene, PropertiesGenerator* generator);

			~RotateManipulator();

			virtual void ResetSize() HELIUM_OVERRIDE;

			virtual void ScaleTo(float factor) HELIUM_OVERRIDE;

			virtual void Evaluate() HELIUM_OVERRIDE;

		protected:
			virtual void SetResult() HELIUM_OVERRIDE;

			bool ClosestSphericalIntersection(Line line, Vector3 spherePosition, float sphereRadius, Vector3 cameraPosition, Vector3& intersection);

		public:
			virtual void Draw( BufferedDrawer* pDrawer ) HELIUM_OVERRIDE;
			virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;
			virtual AxesFlags PickRing(PickVisitor* pick, float err);

			virtual bool MouseDown( const MouseButtonInputEvent& e ) HELIUM_OVERRIDE;
			virtual void MouseUp( const MouseButtonInputEvent& e ) HELIUM_OVERRIDE;
			virtual void MouseMove( const MouseMoveInputEvent& e ) HELIUM_OVERRIDE;

			virtual void CreateProperties() HELIUM_OVERRIDE;

			float32_t GetSize() const;
			void SetSize( float32_t size );

			int GetSpace() const;
			void SetSpace(int space);

			bool GetAxisSnap() const;
			void SetAxisSnap(bool axisSnap);

			float32_t GetSnapDegrees() const;
			void SetSnapDegrees(float snapDegrees);
		};
	}
}