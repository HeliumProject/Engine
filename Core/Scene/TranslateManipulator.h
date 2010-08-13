#pragma once

#include "TransformManipulator.h"

namespace Helium
{
    namespace Core
    {
        class PrimitiveAxes;
        class PrimitiveCone;
        class PrimitiveCircle;

        namespace TranslateSnappingModes
        {
            enum TranslateSnappingMode
            {
                None,
                Surface,
                Object,
                Vertex,
                Offset,
                Grid,
            };
            static void TranslateSnappingModeEnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement(None, TXT( "None" ) );
                info->AddElement(Surface, TXT( "Surface" ) );
                info->AddElement(Object, TXT( "Object" ) );
                info->AddElement(Vertex, TXT( "Vertex" ) );
                info->AddElement(Offset, TXT( "Offset" ) );
                info->AddElement(Grid, TXT( "Grid" ) );
            }
        }
        typedef TranslateSnappingModes::TranslateSnappingMode TranslateSnappingMode;

        class CORE_API TranslateManipulator : public Core::TransformManipulator
        {
        private:
            // Properties
            ManipulatorSpace m_Space;
            TranslateSnappingMode m_SnappingMode;
            float m_Distance;
            bool m_LiveObjectsOnly;

            // UI
            TranslateSnappingMode m_HotSnappingMode;
            bool m_ShowCones;
            float m_Factor;
            Core::PrimitiveAxes* m_Axes;
            Core::PrimitiveCircle* m_Ring;
            Core::PrimitiveCone* m_XCone;
            Math::Vector3 m_XPosition;
            Core::PrimitiveCone* m_YCone;
            Math::Vector3 m_YPosition;
            Core::PrimitiveCone* m_ZCone;
            Math::Vector3 m_ZPosition;

            SCENE_DECLARE_TYPE(Core::TranslateManipulator, Core::TransformManipulator);
            static void InitializeType();
            static void CleanupType();

        public:
            TranslateManipulator(const ManipulatorMode mode, Core::Scene* scene, PropertiesGenerator* generator);
            ~TranslateManipulator();

            TranslateSnappingMode GetSnappingMode() const;

            virtual void ResetSize() HELIUM_OVERRIDE;

            virtual void ScaleTo(float f) HELIUM_OVERRIDE;

            virtual void Evaluate() HELIUM_OVERRIDE;

        protected:
            virtual void SetResult() HELIUM_OVERRIDE;

            void DrawPoints(Math::AxesFlags axis);

        public:
            virtual void Draw( DrawArgs* args ) HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

            virtual bool MouseDown( const MouseButtonInput& e ) HELIUM_OVERRIDE;
            virtual void MouseMove( const MouseMoveInput& e ) HELIUM_OVERRIDE;

            virtual void KeyPress( const KeyboardInput& e ) HELIUM_OVERRIDE;
            virtual void KeyDown( const KeyboardInput& e ) HELIUM_OVERRIDE;
            virtual void KeyUp( const KeyboardInput& e ) HELIUM_OVERRIDE;

            virtual void CreateProperties() HELIUM_OVERRIDE;

            int GetSpace() const;
            void SetSpace(int space);

            bool GetLiveObjectsOnly() const;
            void SetLiveObjectsOnly(bool liveSnap);

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