#pragma once

#include "SceneGraph/API.h"
#include "SceneGraph/Transform.h"

namespace Helium
{
    namespace SceneGraph
    {
        class PrimitiveRings;

        class JointTransform : public Transform
        {
        public:
            HELIUM_DECLARE_CLASS( JointTransform, Transform );
            static void PopulateMetaType( Reflect::MetaStruct& comp );

        public:
            JointTransform();
            ~JointTransform();

            virtual void Initialize() HELIUM_OVERRIDE;
            virtual void Render( RenderVisitor* render ) HELIUM_OVERRIDE;
            virtual bool Pick( PickVisitor* pick ) HELIUM_OVERRIDE;

        protected:
            bool m_SegmentScaleCompensate;
        };
    }
}