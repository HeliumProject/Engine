#pragma once

#include "TransformManipulator.h"

#include "Core/SettingsManager.h"

namespace Helium
{
    namespace SceneGraph
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

        class CORE_API TranslateManipulator : public SceneGraph::TransformManipulator
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
            SceneGraph::PrimitiveAxes* m_Axes;
            SceneGraph::PrimitiveCircle* m_Ring;
            SceneGraph::PrimitiveCone* m_XCone;
            Vector3 m_XPosition;
            SceneGraph::PrimitiveCone* m_YCone;
            Vector3 m_YPosition;
            SceneGraph::PrimitiveCone* m_ZCone;
            Vector3 m_ZPosition;

            REFLECT_DECLARE_ABSTRACT(SceneGraph::TranslateManipulator, SceneGraph::TransformManipulator);
            static void InitializeType();
            static void CleanupType();

        public:
            TranslateManipulator( SettingsManager* settingsManager, const ManipulatorMode mode, SceneGraph::Scene* scene, PropertiesGenerator* generator);
            ~TranslateManipulator();

            virtual void ResetSize() HELIUM_OVERRIDE;

            virtual void ScaleTo(float f) HELIUM_OVERRIDE;

            virtual void Evaluate() HELIUM_OVERRIDE;

        protected:
            virtual void SetResult() HELIUM_OVERRIDE;

            void DrawPoints(AxesFlags axis);

        public:
            virtual void Draw( DrawArgs* args ) HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

            virtual bool MouseDown( const MouseButtonInput& e ) HELIUM_OVERRIDE;
            virtual void MouseMove( const MouseMoveInput& e ) HELIUM_OVERRIDE;

            virtual void KeyPress( const KeyboardInput& e ) HELIUM_OVERRIDE;
            virtual void KeyDown( const KeyboardInput& e ) HELIUM_OVERRIDE;
            virtual void KeyUp( const KeyboardInput& e ) HELIUM_OVERRIDE;

            virtual void CreateProperties() HELIUM_OVERRIDE;

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