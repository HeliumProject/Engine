#pragma once

#include "Editor/API.h"

#include "Pipeline/Content/Nodes/Curve.h"
#include "Curve.h"
#include "Editor/Scene/Tool.h"

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

        class CurveEditTool: public Tool
        {
        private:
            static CurveEditMode s_EditMode;
            static bool s_CurrentSelection;

            CurveEditMode m_HotEditMode;
            Editor::TranslateManipulator* m_ControlPointManipulator;

            //
            // RTTI
            //

            EDITOR_DECLARE_TYPE(Editor::CurveEditTool, Tool);
            static void InitializeType();
            static void CleanupType();

        public:
            CurveEditTool( Editor::Scene* scene, PropertiesGenerator* generator );
            virtual ~CurveEditTool();

            CurveEditMode GetEditMode() const;

            virtual bool MouseDown( const MouseButtonInput& e ) HELIUM_OVERRIDE;
            virtual void MouseUp( const MouseButtonInput& e ) HELIUM_OVERRIDE;
            virtual void MouseMove( const MouseMoveInput& e ) HELIUM_OVERRIDE;

            virtual void KeyPress( const KeyboardInput& e ) HELIUM_OVERRIDE;
            virtual void KeyDown( const KeyboardInput& e ) HELIUM_OVERRIDE;
            virtual void KeyUp( const KeyboardInput& e ) HELIUM_OVERRIDE;

            virtual bool ValidateSelection( OS_SelectableDumbPtr& items ) HELIUM_OVERRIDE;

            virtual void Evaluate() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args ) HELIUM_OVERRIDE;

            virtual void CreateProperties() HELIUM_OVERRIDE;

            int GetCurveEditMode() const;
            void SetCurveEditMode( int mode );

            bool GetSelectionMode() const;
            void SetSelectionMode( bool mode );

            void StoreSelectedCurves();

        protected:
            OS_SelectableDumbPtr m_SelectedCurves;
        };
    }
}