#pragma once

#include "Pipeline/Content/Nodes/Transform.h"

namespace Helium
{
    namespace Content
    {
        class PIPELINE_API JointTransform : public Transform
        {
        public:
            bool m_SegmentScaleCompensate;

            JointTransform ()
                : m_SegmentScaleCompensate( false )
            {

            }

            JointTransform (const Helium::TUID& id)
                : Transform (id)
                , m_SegmentScaleCompensate( false )
            {

            }

            REFLECT_DECLARE_CLASS(JointTransform, Transform);

            static void EnumerateClass( Reflect::Compositor<JointTransform>& comp );

            virtual void ResetTransform() HELIUM_OVERRIDE;
        };

        typedef Helium::SmartPtr<JointTransform> JointTransformPtr;
        typedef std::vector<JointTransformPtr> V_JointTransform;
        typedef std::vector<V_JointTransform> VV_JointTransform;
        typedef std::set<JointTransformPtr> S_JointTransform;
    }
}