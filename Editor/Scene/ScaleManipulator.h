#pragma once

#include "TransformManipulator.h"

namespace Helium
{
    namespace Editor
    {
        class PrimitiveAxes;
        class PrimitiveCube;

        class ScaleManipulator : public Editor::TransformManipulator
        {
        private:
            // UI
            Editor::PrimitiveAxes* m_Axes;
            Editor::PrimitiveCube* m_Cube;
            Editor::PrimitiveCube* m_XCube;
            Math::Vector3 m_XPosition;
            Editor::PrimitiveCube* m_YCube;
            Math::Vector3 m_YPosition;
            Editor::PrimitiveCube* m_ZCube;
            Math::Vector3 m_ZPosition;

            bool m_GridSnap;
            float m_Distance;

            EDITOR_DECLARE_TYPE(Editor::ScaleManipulator, Editor::TransformManipulator);
            static void InitializeType();
            static void CleanupType();

        public:
            ScaleManipulator(const ManipulatorMode mode, Editor::Scene* scene, PropertiesGenerator* generator);

            ~ScaleManipulator();

            virtual void ResetSize() HELIUM_OVERRIDE;

            virtual void ScaleTo(float factor) HELIUM_OVERRIDE;

            virtual void Evaluate() HELIUM_OVERRIDE;

        protected:
            virtual void SetResult() HELIUM_OVERRIDE;

        public:
            virtual void Draw( DrawArgs* args ) HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

            virtual bool MouseDown( const MouseButtonInput& e ) HELIUM_OVERRIDE;
            virtual void MouseMove( const MouseMoveInput& e ) HELIUM_OVERRIDE;

            virtual void CreateProperties() HELIUM_OVERRIDE;

            bool GetGridSnap() const;
            void SetGridSnap( bool gridSnap );

            float GetDistance() const;
            void SetDistance( float distance );
        };
    }
}