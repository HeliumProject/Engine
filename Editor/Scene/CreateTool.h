#pragma once

#include "Editor/API.h"
#include "Tool.h"
#include "HierarchyNode.h"
#include "Scene.h"

#include "Foundation/TimerThread.h"

namespace Helium
{
    namespace Editor
    {
        namespace PlacementStyles
        {
            enum PlacementStyle
            {
                Grid,
                Radial,
            };
        }

        typedef PlacementStyles::PlacementStyle PlacementStyle;

        namespace DistributionStyles
        {
            enum DistributionStyle
            {
                Constant,
                Uniform,
                Linear,
                Normal,
            };
        }

        typedef DistributionStyles::DistributionStyle DistributionStyle;

        struct Vector3Compare
        {
            bool operator()( const Math::Vector3& rhs, const Math::Vector3& lhs ) const
            {
                return ( lhs.Length() < rhs.Length() );
            }
        };

        typedef std::map< Math::Vector3, Math::V_Vector3, Vector3Compare > M_Vector3;

        class CreateTool : public Tool
        {
            //
            // Members
            //

        private:
            // Created flag
            bool m_Created;

            // The selection of the created objects
            OS_SelectableDumbPtr m_Selection;

            // The instance we are creating
            bool m_InstanceUpdateOffsets;
            Editor::TransformPtr m_Instance;
            Math::Vector3 m_InstanceTranslation;
            Math::Vector3 m_InstanceNormal;
            Math::AlignedBox m_InstanceBounds;
            float m_InstanceRadius;
            Math::V_Vector3 m_InstanceOffsets;

            TimerThread m_PaintTimer;

            // UI handles
            Inspect::Container* m_AzimuthMin;
            Inspect::Container* m_AzimuthMax;
            Inspect::Container* m_DirectionMin;
            Inspect::Container* m_DirectionMax;
            Inspect::Container* m_ScaleMin;
            Inspect::Container* m_ScaleMax;
            Inspect::Container* m_PaintPreventAnyOverlap;
            Inspect::Container* m_PaintPlacementStyle;
            Inspect::Container* m_PaintDistributionStyle;
            Inspect::Container* m_PaintRadius;
            Inspect::Container* m_PaintSpeed;
            Inspect::Container* m_PaintDensity;
            Inspect::Container* m_PaintJitter;

        public:
            // Plane snap to use
            static IntersectionPlane s_PlaneSnap;

            // Should we snap to live object only
            static bool s_LiveObjectsOnly;

            // Should we snap to surfaces
            static bool s_SurfaceSnap;

            // Should we snap to objects
            static bool s_ObjectSnap;

            // Should we snap to normal
            static bool s_NormalSnap;

            // Azimuth (up-axis rotation) randomization
            static bool s_RandomizeAzimuth;
            static float s_AzimuthMin;
            static float s_AzimuthMax;

            // Direction (up-axis orientation) randomization
            static bool s_RandomizeDirection;
            static float s_DirectionMin;
            static float s_DirectionMax;

            // Scale randomization
            static bool s_RandomizeScale;
            static float s_ScaleMin;
            static float s_ScaleMax;

            // Distribution
            static bool s_PaintMode;
            static bool s_PaintPreventAnyOverlap;
            static PlacementStyle s_PaintPlacementStyle;
            static DistributionStyle s_PaintDistributionStyle;
            static float s_PaintRadius;
            static int s_PaintSpeed;
            static float s_PaintDensity;
            static float s_PaintJitter;

            //
            // RTTI
            //
        public:

            EDITOR_DECLARE_TYPE(Editor::CreateTool, Tool);
            static void InitializeType();
            static void CleanupType();


            //
            // Implementation
            //

        public:
            CreateTool(Editor::Scene* scene, PropertiesGenerator* generator);

            virtual ~CreateTool();

            virtual Editor::TransformPtr CreateNode() = 0;

            void Place(const Math::Matrix4& position);

            void DetermineTranslationAndNormal( int x, int y, Math::Vector3& t, Math::Vector3& n );

            bool DetermineTranslationAndNormal( PickVisitor& pick, Math::Vector3& t, Math::Vector3& n );

            void GenerateInstanceOffsets( PlacementStyle style, float radius, float instanceRadius, Math::V_Vector3& positions );

            void SelectInstanceOffsets( DistributionStyle style, float radius, Math::V_Vector3& offsets );

            void JitterInstanceOffsets( float instanceRadius, float maxJitter, Math::V_Vector3& offsets );

            void RandomizeInstanceOffsets( Math::V_Vector3& offsets );

            void FinalizeOrientation( Math::Matrix4& position, const Math::Vector3& t, const Math::Vector3& n );

            bool ValidPosition( const Math::AlignedBox& bounds, const Math::Vector3& translation, float minDistance );

            void CalculateInstanceRadiusAndBounds( f32& instanceRadius, Math::AlignedBox& bounds );

        protected:
            void RefreshInstance( void );

            static float GetNormalProbabilityFromPercent( float value );

        private:
            void AddToScene();
            void SceneNodeAdded( const NodeChangeArgs& args );
            void SceneNodeRemoved( const NodeChangeArgs& args );

        public:
            virtual void Draw( DrawArgs* args ) HELIUM_OVERRIDE;

            virtual bool AllowSelection() HELIUM_OVERRIDE;

            virtual bool MouseDown(wxMouseEvent& e) HELIUM_OVERRIDE;

            virtual void MouseMove(wxMouseEvent& e) HELIUM_OVERRIDE;

            virtual void MouseUp(wxMouseEvent& e) HELIUM_OVERRIDE;

            virtual void KeyPress( wxKeyEvent& e ) HELIUM_OVERRIDE;

            virtual void CreateProperties() HELIUM_OVERRIDE;

            void SetupInstanceOffsets( float instanceRadius, Math::V_Vector3& instanceOffsets );

            void CreateSingleObject( const Math::Vector3& translation, const Math::Vector3& normal, bool checkValid = false );
            void CreateMultipleObjects( bool stamp = false );

            void TimerCallback( const TimerTickArgs& args );

            int GetPlaneSnap() const;
            void SetPlaneSnap(int snap);

            bool GetLiveObjectsOnly() const;
            void SetLiveObjectsOnly(bool snap);
            bool GetSurfaceSnap() const;
            void SetSurfaceSnap(bool snap);
            bool GetObjectSnap() const;
            void SetObjectSnap(bool snap);
            bool GetNormalSnap() const;
            void SetNormalSnap(bool snap);

            bool GetRandomizeAzimuth() const;
            void SetRandomizeAzimuth(bool value);
            float GetAzimuthMin() const;
            void SetAzimuthMin(float value);
            float GetAzimuthMax() const;
            void SetAzimuthMax(float value);

            bool GetRandomizeDirection() const;
            void SetRandomizeDirection(bool value);
            float GetDirectionMin() const;
            void SetDirectionMin(float value);
            float GetDirectionMax() const;
            void SetDirectionMax(float value);

            bool GetRandomizeScale() const;
            void SetRandomizeScale(bool value);
            float GetScaleMin() const;
            void SetScaleMin(float value);
            float GetScaleMax() const;
            void SetScaleMax(float value);

            bool GetPaintMode() const;
            void SetPaintMode(bool value);
            bool GetPaintPreventAnyOverlap() const;
            void SetPaintPreventAnyOverlap(bool value);
            bool GetPaintDelay() const;
            void SetPaintDelay(bool value);
            float GetPaintRadius() const;
            void SetPaintRadius(float value);
            int GetPaintSpeed() const;
            void SetPaintSpeed(int value);
            float GetPaintDensity() const;
            void SetPaintDensity(float value);
            float GetPaintJitter() const;
            void SetPaintJitter(float value);
            int GetPaintPlacementStyle() const;
            void SetPaintPlacementStyle( int style );
            int GetPaintDistributionStyle() const;
            void SetPaintDistributionStyle( int style );
        };
    }
}