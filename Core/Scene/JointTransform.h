#pragma once

#include "Core/API.h"
#include "Core/Scene/Transform.h"

namespace Helium
{
    namespace Core
    {
        class PrimitiveRings;

        class JointTransform : public Transform
        {
        public:
            REFLECT_DECLARE_ABSTRACT( JointTransform, Transform );
            static void EnumerateClass( Reflect::Compositor<JointTransform>& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            JointTransform();
            ~JointTransform();

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