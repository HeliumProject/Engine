#pragma once

#include "EditorScene/SettingsManager.h"
#include "EditorScene/TransformManipulator.h"

namespace Helium
{
	namespace Editor
	{
		class PrimitiveAxes;
		class PrimitiveCone;
		class PrimitiveCircle;

		class TranslateSnappingMode
		{
		public:
			enum Enum
			{
				None,
				Surface,
				Object,
				Vertex,
				Offset,
				Grid,
			};

			HELIUM_DECLARE_ENUM( TranslateSnappingMode );

			static void PopulateMetaType( Reflect::MetaEnum& info )
			{
				info.AddElement(None,       "None" );
				info.AddElement(Surface,    "Surface" );
				info.AddElement(Object,     "Object" );
				info.AddElement(Vertex,     "Vertex" );
				info.AddElement(Offset,     "Offset" );
				info.AddElement(Grid,       "Grid" );
			}
		};

		class HELIUM_EDITOR_SCENE_API TranslateManipulator : public TransformManipulator
		{
		private:
			SettingsManager* m_SettingsManager;

			// Properties
			float32_t m_Size;
			ManipulatorSpace m_Space;
			TranslateSnappingMode m_SnappingMode;
			float m_Distance;
			bool m_LiveObjectsOnly;

			// UI
			TranslateSnappingMode m_HotSnappingMode;
			bool m_ShowCones;
			float m_Factor;
			Editor::PrimitiveAxes* m_Axes;
			Editor::PrimitiveCircle* m_Ring;
			Editor::PrimitiveCone* m_XCone;
			Vector3 m_XPosition;
			Editor::PrimitiveCone* m_YCone;
			Vector3 m_YPosition;
			Editor::PrimitiveCone* m_ZCone;
			Vector3 m_ZPosition;

			HELIUM_DECLARE_ABSTRACT(Editor::TranslateManipulator, Editor::TransformManipulator);

		public:
			TranslateManipulator( SettingsManager* settingsManager, const ManipulatorMode mode, Editor::Scene* scene, PropertiesGenerator* generator);
			~TranslateManipulator();

			virtual void ResetSize() override;

			virtual void ScaleTo(float f) override;

			virtual void Evaluate() override;

		protected:
			virtual void SetResult() override;

			void DrawPoints(AxesFlags axis);

		public:
			virtual void Draw( BufferedDrawer* pDrawer ) override;
			virtual bool Pick( PickVisitor* pick ) override;

			virtual bool MouseDown( const MouseButtonInputEvent& e ) override;
			virtual void MouseMove( const MouseMoveInputEvent& e ) override;

			virtual void KeyPress( const KeyboardInputEvent& e ) override;
			virtual void KeyDown( const KeyboardInputEvent& e ) override;
			virtual void KeyUp( const KeyboardInputEvent& e ) override;

			virtual void CreateProperties() override;

			float32_t GetSize() const;
			void SetSize( float32_t size );

			int GetSpace() const;
			void SetSpace(int space);

			bool GetLiveObjectsOnly() const;
			void SetLiveObjectsOnly(bool liveSnap);

			TranslateSnappingMode GetSnappingMode() const;
			void UpdateSnappingMode();

			bool GetSurfaceSnap() const;
			void SetSurfaceSnap(bool polygonSnap);

			bool GetObjectSnap() const;
			void SetObjectSnap(bool polygonSnap);

			bool GetVertexSnap() const;
			void SetVertexSnap(bool vertexSnap);

			bool GetOffsetSnap() const;
			void SetOffsetSnap(bool offsetSnap);

			bool GetGridSnap() const;
			void SetGridSnap(bool gridSnap);

			float GetDistance() const;
			void SetDistance(float distance);
		};
	}
}
