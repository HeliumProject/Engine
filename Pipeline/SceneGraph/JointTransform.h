#pragma once

#include "Pipeline/API.h"
#include "Pipeline/SceneGraph/Transform.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitiveRings;

        class JointTransform : public Transform
        {
        public:
            REFLECT_DECLARE_OBJECT( JointTransform, Transform );
            static void PopulateComposite( Reflect::Composite& comp );
            static void InitializeType();
            static void CleanupType();

        public:
            JointTransform();
            ~JointTransform();

            virtual void Initialize() HELIUM_OVERRIDE;

            virtual int32_t GetImageIndex() const HELIUM_OVERRIDE;
            virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

            virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

        protected:
            bool m_SegmentScaleCompensate;
        };
    }
}