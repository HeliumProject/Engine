#pragma once

#include "TransformManipulator.h"

#include "Core/SettingsManager.h"

namespace Helium
{
    namespace SceneGraph
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

        class CORE_API RotateManipulator : public SceneGraph::TransformManipulator
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
            SceneGraph::PrimitiveCircle* m_Ring;

            REFLECT_DECLARE_ABSTRACT(SceneGraph::RotateManipulator, SceneGraph::TransformManipulator);
            static void InitializeType();
            static void CleanupType();

        public:
            RotateManipulator( SettingsManager* settingsManager, const ManipulatorMode mode, SceneGraph::Scene* scene, PropertiesGenerator* generator);

            ~RotateManipulator();

            virtual void ResetSize() HELIUM_OVERRIDE;

            virtual void ScaleTo(float factor) HELIUM_OVERRIDE;

            virtual void Evaluate() HELIUM_OVERRIDE;

        protected:
            virtual void SetResult() HELIUM_OVERRIDE;

            bool ClosestSphericalIntersection(Line line, Vector3 spherePosition, float sphereRadius, Vector3 cameraPosition, Vector3& intersection);

        public:
            virtual void Draw( DrawArgs* args ) HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;
            virtual AxesFlags PickRing(PickVisitor* pick, float err);

            virtual bool MouseDown( const MouseButtonInput& e ) HELIUM_OVERRIDE;
            virtual void MouseUp( const MouseButtonInput& e ) HELIUM_OVERRIDE;
            virtual void MouseMove( const MouseMoveInput& e ) HELIUM_OVERRIDE;

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