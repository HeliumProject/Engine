#pragma once

#include "TransformManipulator.h"

#include "Core/SettingsManager.h"

namespace Helium
{
    namespace Core
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

        class CORE_API RotateManipulator : public Core::TransformManipulator
        {
        private:
            SettingsManager* m_SettingsManager;

            f32 m_Size;
            ManipulatorSpace m_Space;
            bool m_AxisSnap;
            float m_SnapDegrees;

            // Rotation drag mode
            RotationType m_Type;

            // UI
            Core::PrimitiveCircle* m_Ring;

            REFLECT_DECLARE_ABSTRACT(Core::RotateManipulator, Core::TransformManipulator);
            static void InitializeType();
            static void CleanupType();

        public:
            RotateManipulator( SettingsManager* settingsManager, const ManipulatorMode mode, Core::Scene* scene, PropertiesGenerator* generator);

            ~RotateManipulator();

            virtual void ResetSize() HELIUM_OVERRIDE;

            virtual void ScaleTo(float factor) HELIUM_OVERRIDE;

            virtual void Evaluate() HELIUM_OVERRIDE;

        protected:
            virtual void SetResult() HELIUM_OVERRIDE;

            bool ClosestSphericalIntersection(Math::Line line, Math::Vector3 spherePosition, float sphereRadius, Math::Vector3 cameraPosition, Math::Vector3& intersection);

        public:
            virtual void Draw( DrawArgs* args ) HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;
            virtual Math::AxesFlags PickRing(PickVisitor* pick, float err);

            virtual bool MouseDown( const MouseButtonInput& e ) HELIUM_OVERRIDE;
            virtual void MouseUp( const MouseButtonInput& e ) HELIUM_OVERRIDE;
            virtual void MouseMove( const MouseMoveInput& e ) HELIUM_OVERRIDE;

            virtual void CreateProperties() HELIUM_OVERRIDE;

            f32 GetSize() const;
            void SetSize( f32 size );

            int GetSpace() const;
            void SetSpace(int space);

            bool GetAxisSnap() const;
            void SetAxisSnap(bool axisSnap);

            f32 GetSnapDegrees() const;
            void SetSnapDegrees(float snapDegrees);
        };
    }
}