#pragma once

#include "Editor/API.h"

#include "Pipeline/Content/Nodes/Point.h"

#include "RotateManipulator.h"
#include "NavMesh.h"
#include "Editor/Scene/Tool.h"

#define PUNCH_VOLUME 1

namespace Helium
{
    namespace Editor
    {
        class PrimitiveAxes;
        class PrimitiveCone;
        class PrimitiveCircle;
        class Volume;
        class TranslateManipulator;
        class ScaleManipulator;

        class NavMeshCreateTool : public Tool
        {
            //
            //  Members
            //

        public :
            enum
            {
                EDIT_MODE_DISABLED           = 0,
                EDIT_MODE_ADD                = 1,
                EDIT_MODE_MOVE               = 2,
                EDIT_MODE_MARQUEE_SELECT     = 3,
                EDIT_MODE_ROTATE             = 4,
                EDIT_MODE_CUBE_PUNCH_OUT     = 5,
                EDIT_MODE_SCALE              = 6,
            };

            enum
            {
                RES_MODE_HIGH_RES   = 0,
                RES_MODE_LOW_RES    = 1,
            };

            enum
            {
                MOUSE_HOVER_SELECT_VERT = 0,
                MOUSE_HOVER_SELECT_EDGE,
                MOUSE_HOVER_SELECT_TRI,

                // this should be at the end
                MOUSE_HOVER_SELECT_DISABLED,
            };

#if (PUNCH_VOLUME)
            Editor::Volume* m_PunchOutVolume;
#endif
        private:
            // Created flag
            bool m_Created;

            // The selection of the created objects
            OS_SelectableDumbPtr m_Selection;

            // The instance we are creating
            NavMeshPtr m_Instance;

            // Should we snap to surfaces
            static bool s_SurfaceSnap;

            // Should we snap to objects
            static bool s_ObjectSnap;

            // The currently selected vert
            u32 m_SelectedVert;

            // we can make these flags if we want multiple on at one time ??

            Math::Vector2 m_MarqueMouseCoords[2];

            u32 m_EditMode;
            u32 m_PrevEditMode;

            u32 m_ResMode;

            u32 m_MouseHoverSelectionMode;

            //
            // Punch out manipulators
            //
            u32 m_PunchOutEditMode;
            Editor::TranslateManipulator* m_PunchOutTranslator;
            Editor::RotateManipulator* m_PunchOutRotator;
            Editor::ScaleManipulator* m_PunchOutScaler;

            //
            // translation manipulator members
            //
            static f32 s_Size;
            float m_Factor;
            Editor::PrimitiveAxes* m_Axes;
            Editor::PrimitiveCircle* m_Ring;
            Editor::PrimitiveCone* m_XCone;
            Math::Vector3 m_XPosition;
            Editor::PrimitiveCone* m_YCone;
            Math::Vector3 m_YPosition;
            Editor::PrimitiveCone* m_ZCone;
            Math::Vector3 m_ZPosition;
            D3DMATERIAL9 m_AxisMaterial;
            D3DMATERIAL9 m_SelectedAxisMaterial;
            Math::AxesFlags m_SelectedAxes;
            Math::Matrix4 m_ManipulatorPos;
            int m_StartX;
            int m_StartY;
            Math::Vector3 m_PreviousDrag;

            // Marquee select frame
            Editor::PrimitiveFrame* m_SelectionFrame;

            // start of drag
            Math::Vector3 m_StartValue;

            bool m_PickedManipulator;

            //
            // Rotate manipulator members
            //

            bool m_AxisSnap;
            float m_SnapDegrees;
            Math::Vector3 m_RotationStartValue;

            Math::V_Vector3 m_CachedPositions;

            // Rotation drag mode
            RotationType m_Type;

            // used for copy/paste
            Math::V_Vector3 m_CopiedPositions;

        public:
            //static Content::CurveType s_CurveType;

            //
            // RTTI
            //

            EDITOR_DECLARE_TYPE(Editor::NavMeshCreateTool, Tool);
            static void InitializeType();
            static void CleanupType();

        public:
            NavMeshCreateTool( Editor::Scene* scene, PropertiesGenerator* generator );
            virtual ~NavMeshCreateTool();

            void CreateInstance( const Math::Vector3& position );
            void PickPosition( int x, int y, Math::Vector3& position );
            bool PickManipulator( PickVisitor* pick );
            bool PickRotator( PickVisitor* pick );
            Math::AxesFlags PickRing(PickVisitor* pick, float err);

        private:
            void AddToScene();

            bool SetAxisMaterial(Math::AxesFlags axes);
            void ResetManipulatorSize();
            void MoveManipulator( wxMouseEvent& e );
            void MoveRotator( wxMouseEvent& e );

            Math::Vector3 GetAxesNormal(Math::AxesFlags axes);

            bool ClosestSphericalIntersection(Math::Line line, Math::Vector3 spherePosition, float sphereRadius, Math::Vector3 cameraPosition, Math::Vector3& intersection);

        public:
            virtual bool AllowSelection() HELIUM_OVERRIDE;

            virtual void MouseUp( wxMouseEvent& e ) HELIUM_OVERRIDE;
            virtual bool MouseDown( wxMouseEvent& e ) HELIUM_OVERRIDE;
            virtual void MouseMove( wxMouseEvent& e ) HELIUM_OVERRIDE;
            virtual void KeyPress( wxKeyEvent& e ) HELIUM_OVERRIDE;

            virtual void CreateProperties() HELIUM_OVERRIDE;
            virtual void Evaluate() HELIUM_OVERRIDE;
            virtual void Draw( DrawArgs* args ) HELIUM_OVERRIDE;

            bool GetSurfaceSnap() const;
            void SetSurfaceSnap( bool snap );

            bool GetObjectSnap() const;
            void SetObjectSnap( bool snap );

            int GetPlaneSnap() const;
            void SetPlaneSnap(int snap);

            void SetEditMode(u32 mode);
            void SetPunchOutMode( u32 mode );
            void SetHoverSelectMode( u32 mode );

            void SetResolutionMode(u32 res_mode);

            const Math::Matrix4& GetManipulatorPos() { return m_ManipulatorPos; }
            void SetManipulatorPos( const Math::Matrix4& pos ) { m_ManipulatorPos = pos; }

            const Math::Vector3& GetRotationStart() { return m_RotationStartValue; }
            void SetRotationStart( const Math::Vector3& val ) { m_RotationStartValue = val; }

            void CopySelected();
            void Paste();
        };
    }
}