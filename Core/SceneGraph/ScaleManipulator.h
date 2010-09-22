#pragma once

#include "TransformManipulator.h"

#include "Core/SettingsManager.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitiveAxes;
        class PrimitiveCube;

        class CORE_API ScaleManipulator : public SceneGraph::TransformManipulator
        {
        private:
            // UI
            SceneGraph::PrimitiveAxes* m_Axes;
            SceneGraph::PrimitiveCube* m_Cube;
            SceneGraph::PrimitiveCube* m_XCube;
            Math::Vector3 m_XPosition;
            SceneGraph::PrimitiveCube* m_YCube;
            Math::Vector3 m_YPosition;
            SceneGraph::PrimitiveCube* m_ZCube;
            Math::Vector3 m_ZPosition;

            f32 m_Size;
            bool m_GridSnap;
            f32 m_Distance;

            SettingsManager* m_SettingsManager;

            REFLECT_DECLARE_ABSTRACT(SceneGraph::ScaleManipulator, SceneGraph::TransformManipulator);
            static void InitializeType();
            static void CleanupType();

        public:
            ScaleManipulator( SettingsManager* settingsManager, const ManipulatorMode mode, SceneGraph::Scene* scene, PropertiesGenerator* generator);

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

            f32 GetSize() const;
            void SetSize( f32 size );

            bool GetGridSnap() const;
            void SetGridSnap( bool gridSnap );

            float GetDistance() const;
            void SetDistance( float distance );
        };
    }
}