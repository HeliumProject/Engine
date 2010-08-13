#pragma once

#include "Core/API.h"

#include "Core/Content/Nodes/ContentSceneNode.h"

namespace Helium
{
    namespace Content
    {
        class CORE_API JointOrdering : public SceneNode
        {
        public:

            JointOrdering() : SceneNode() {}
            JointOrdering( u32 numRequiredJoints );

            Helium::V_TUID m_JointOrdering;
            Helium::HM_TUID m_MasterToLocalMap;
            Helium::HM_TUID m_LocalToMasterMap;

            REFLECT_DECLARE_CLASS( JointOrdering, SceneNode );

            static void EnumerateClass( Reflect::Compositor<JointOrdering>& comp );

            void SetRequiredJointCount( u32 requiredJointCount );
            u32  GetRequiredJointCount();

            void AddJoint( const Helium::TUID& jointId );
            void Clear();

            Helium::TUID GetMasterJoint( const Helium::TUID& localJoint );
            Helium::TUID GetLocalJoint( const Helium::TUID& masterJoint );

            bool IsRequired( const Helium::TUID& jointId );
        };

        typedef Helium::SmartPtr< JointOrdering > JointOrderingPtr;
        typedef std::vector< JointOrderingPtr > V_JointOrdering;
    }
}