#pragma once

#include "TransformManipulator.h"

#include "Core/SettingsManager.h"

namespace Helium
{
    namespace Core
    {
        class PrimitiveAxes;
        class PrimitiveCube;

        class CORE_API ScaleManipulator : public Core::TransformManipulator
        {
        private:
            // UI
            Core::PrimitiveAxes* m_Axes;
            Core::PrimitiveCube* m_Cube;
            Core::PrimitiveCube* m_XCube;
            Math::Vector3 m_XPosition;
            Core::PrimitiveCube* m_YCube;
            Math::Vector3 m_YPosition;
            Core::PrimitiveCube* m_ZCube;
            Math::Vector3 m_ZPosition;

            f32 m_Size;
            bool m_GridSnap;
            f32 m_Distance;

            SettingsManager* m_SettingsManager;

            SCENE_DECLARE_TYPE(Core::ScaleManipulator, Core::TransformManipulator);
            static void InitializeType();
            static void CleanupType();

        public:
            ScaleManipulator( SettingsManager* settingsManager, const ManipulatorMode mode, Core::Scene* scene, PropertiesGenerator* generator);

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