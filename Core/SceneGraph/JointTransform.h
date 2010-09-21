#pragma once

#include "Core/API.h"
#include "Core/SceneGraph/Transform.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitiveRings;

        class JointTransform : public Transform
        {
        public:
            REFLECT_DECLARE_CLASS( JointTransform, Transform );
            static void EnumerateClass( Reflect::Compositor<JointTransform>& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            JointTransform();
            ~JointTransform();

            virtual void Initialize() HELIUM_OVERRIDE;

            virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

            virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;
            static void DrawNormal( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
            static void DrawSelected( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

        protected:
            bool m_SegmentScaleCompensate;
        };
    }
}