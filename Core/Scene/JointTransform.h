#pragma once

#include "Core/API.h"
#include "Core/Scene/Transform.h"

namespace Helium
{
    // Forwards
    namespace Content
    {
        class JointTransform;
    }

    namespace Core
    {
        // Forwards
        class PrimitiveRings;

        class JointTransform : public Core::Transform
        {
            //
            // Runtime Type Info
            //

        public:
            REFLECT_DECLARE_ABSTRACT( Core::JointTransform, Core::Transform );
            static void InitializeType();
            static void CleanupType();


            //
            // Member functions
            //

        public:
            JointTransform( Core::Scene* scene, Content::JointTransform* joint );
            virtual ~JointTransform();

            virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

            virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;
            static void DrawNormal( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );
            static void DrawSelected( IDirect3DDevice9* device, DrawArgs* args, const SceneNode* object );

            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;
        };
    }
}