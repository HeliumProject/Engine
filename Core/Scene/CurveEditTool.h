#pragma once

#include "Core/API.h"
#include "Core/SettingsManager.h"
#include "Core/Content/Nodes/ContentCurve.h"
#include "Core/Scene/Curve.h"
#include "Core/Scene/Tool.h"

namespace Helium
{
    namespace Core
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

        class CORE_API CurveEditTool : public Tool
        {
        private:
            SettingsManager* m_SettingsManager;

            static CurveEditMode s_EditMode;
            static bool s_CurrentSelection;

            CurveEditMode m_HotEditMode;
            Core::TranslateManipulator* m_ControlPointManipulator;

        public:
            REFLECT_DECLARE_ABSTRACT(Core::CurveEditTool, Tool);
            static void InitializeType();
            static void CleanupType();

        public:
            CurveEditTool( SettingsManager* settingsManager, Core::Scene* scene, PropertiesGenerator* generator );
            virtual ~CurveEditTool();

            CurveEditMode GetEditMode() const;

            virtual bool MouseDown( const MouseButtonInput& e ) HELIUM_OVERRIDE;
            virtual void MouseUp( const MouseButtonInput& e ) HELIUM_OVERRIDE;
            virtual void MouseMove( const MouseMoveInput& e ) HELIUM_OVERRIDE;

            virtual void KeyPress( const KeyboardInput& e ) HELIUM_OVERRIDE;
            virtual void KeyDown( const KeyboardInput& e ) HELIUM_OVERRIDE;
            virtual void KeyUp( const KeyboardInput& e ) HELIUM_OVERRIDE;

            virtual bool ValidateSelection( OS_SceneNodeDumbPtr& items ) HELIUM_OVERRIDE;

            virtual void Evaluate() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args ) HELIUM_OVERRIDE;

            virtual void CreateProperties() HELIUM_OVERRIDE;

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