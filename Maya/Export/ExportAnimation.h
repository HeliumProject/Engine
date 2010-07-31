#pragma once

#include "ExportAnimationBase.h"
#include "Pipeline/Content/Animation/Animation.h"

namespace Helium
{
    namespace MayaContent
    {

        struct ExportDeformationWeight
        {
            u32               m_WeightId;
            Helium::V_TUID  m_TargetIds;
        };
        typedef std::vector<ExportDeformationWeight> V_ExportDeformationWeight;


        struct ExportBlendShapeDeformer
        {
            MObject   m_BlendShapeObject;
            MObject   m_BaseObject;
            V_ExportDeformationWeight m_ExportDeformationWeight;
        };
        typedef std::vector<ExportBlendShapeDeformer> V_ExportBlendShapeDeformer;


        class MAYA_API ExportAnimationClip : public ExportAnimationBase
        {
        protected:
            Content::V_JointAnimation  m_JointAnimations;
            std::vector<MObject>  m_RequiredJoints;

            V_ExportBlendShapeDeformer m_ExportBlendShapeDeformers;

        public:
            ExportAnimationClip()
            {
                m_ContentObject = new Content::Animation();
            }

            // Gather the necessary maya data and prepare for SampleOneFrame
            void GatherMayaData( V_ExportBase &newExportObjects );

            // gather data for a single frame
            virtual void SampleOneFrame( const MTime & currentTime, bool extraFrame = false );

            const Content::AnimationPtr GetContentAnimation() const
            {
                return Reflect::DangerousCast< Content::Animation >( m_ContentObject );
            }

        private:
            void GatherBlendShapeDeformers();
            void SampleOneFramesMorphTargetWeights( const MTime& currentTime, const Content::AnimationPtr& animClip );
        };

        typedef Helium::SmartPtr<ExportAnimationClip> ExportAnimationClipPtr;
    }
}