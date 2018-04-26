#pragma once

#include "EditorScene/API.h"
#include "EditorScene/Curve.h"
#include "EditorScene/Tool.h"
#include "EditorScene/SettingsManager.h"

namespace Helium
{
	namespace Editor
	{
		class PickVisitor;
		class TranslateManipulator;

		namespace CurveEditModes
		{
			enum CurveEditMode
			{
				None,
				Modify,
				Insert,
				Remove,
			};
		}
		typedef CurveEditModes::CurveEditMode CurveEditMode;

		class HELIUM_EDITOR_SCENE_API CurveEditTool : public Tool
		{
		private:
			SettingsManager* m_SettingsManager;

			static CurveEditMode s_EditMode;
			static bool s_CurrentSelection;

			CurveEditMode m_HotEditMode;
			Editor::TranslateManipulator* m_ControlPointManipulator;

		public:
			HELIUM_DECLARE_ABSTRACT(Editor::CurveEditTool, Tool);

		public:
			CurveEditTool( SettingsManager* settingsManager, Editor::Scene* scene, PropertiesGenerator* generator );
			virtual ~CurveEditTool();

			CurveEditMode GetEditMode() const;

			virtual bool MouseDown( const MouseButtonInputEvent& e ) override;
			virtual void MouseUp( const MouseButtonInputEvent& e ) override;
			virtual void MouseMove( const MouseMoveInputEvent& e ) override;

			virtual void KeyPress( const KeyboardInputEvent& e ) override;
			virtual void KeyDown( const KeyboardInputEvent& e ) override;
			virtual void KeyUp( const KeyboardInputEvent& e ) override;

			virtual bool ValidateSelection( OS_ObjectDumbPtr& items ) override;

			virtual void Evaluate() override;
			virtual void Draw( BufferedDrawer* pDrawer ) override;

			virtual void CreateProperties() override;

			int GetCurveEditMode() const;
			void SetCurveEditMode( int mode );

			bool GetSelectionMode() const;
			void SetSelectionMode( bool mode );

			void StoreSelectedCurves();

		protected:
			OS_SceneNodeDumbPtr m_SelectedCurves;
		};
	}
}
