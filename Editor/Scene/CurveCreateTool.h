#pragma once

#include "Editor/API.h"

#include "Core/Content/Nodes/Curve.h"
#include "Curve.h"
#include "Editor/Scene/Tool.h"

namespace Helium
{
    namespace Editor
    {
        class CurveCreateTool : public Tool
        {
            //
            //  Members
            //

        private:
            // Created flag
            bool m_Created;

            // The selection of the created objects
            OS_SelectableDumbPtr m_Selection;

            // The instance we are creating
            LCurvePtr m_Instance;

        public:
            // Type of curve interpolation
            static Content::CurveType s_CurveType;

            // Should we snap to surfaces
            static bool s_SurfaceSnap;

            // Should we snap to objects
            static bool s_ObjectSnap;

            //
            // RTTI
            //

            EDITOR_DECLARE_TYPE(Editor::CurveCreateTool, Tool);
            static void InitializeType();
            static void CleanupType();

        public:
            CurveCreateTool( Editor::Scene* scene, PropertiesGenerator* generator );
            virtual ~CurveCreateTool();

            void CreateInstance( const Math::Vector3& position );
            void PickPosition( int x, int y, Math::Vector3& position );

        private:
            void AddToScene();

        public:
            virtual bool AllowSelection() HELIUM_OVERRIDE;

            virtual bool MouseDown( const MouseButtonInput& e ) HELIUM_OVERRIDE;
            virtual void MouseMove( const MouseMoveInput& e ) HELIUM_OVERRIDE;
            virtual void KeyPress( const KeyboardInput& e ) HELIUM_OVERRIDE;

            virtual void CreateProperties() HELIUM_OVERRIDE;

            bool GetSurfaceSnap() const;
            void SetSurfaceSnap( bool snap );

            bool GetObjectSnap() const;
            void SetObjectSnap( bool snap );

            int GetPlaneSnap() const;
            void SetPlaneSnap(int snap);

            int GetCurveType() const;
            void SetCurveType( int selection );

            bool GetClosed() const;
            void SetClosed( bool closed );
        };
    }
}