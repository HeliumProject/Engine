#pragma once

#include "Core/API.h"
#include "Core/SceneGraph/Curve.h"
#include "Core/SceneGraph/Tool.h"

namespace Helium
{
    namespace SceneGraph
    {
        class CORE_API CurveCreateTool : public Tool
        {
        private:
            // Created flag
            bool m_Created;

            // The selection of the created objects
            OS_SceneNodeDumbPtr m_Selection;

            // The instance we are creating
            CurvePtr m_Instance;

        public:
            // Type of curve interpolation
            static CurveType s_CurveType;

            // Should we snap to surfaces
            static bool s_SurfaceSnap;

            // Should we snap to objects
            static bool s_ObjectSnap;

        public:
            REFLECT_DECLARE_ABSTRACT(SceneGraph::CurveCreateTool, Tool);
            static void InitializeType();
            static void CleanupType();

        public:
            CurveCreateTool( SceneGraph::Scene* scene, PropertiesGenerator* generator );
            virtual ~CurveCreateTool();

            void CreateInstance( const Vector3& position );
            void PickPosition( int x, int y, Vector3& position );

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