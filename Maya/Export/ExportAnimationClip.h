#pragma once

#include "ExportAnimationBase.h"
#include "Pipeline/Content/Animation/Animation.h"

namespace MayaContent
{

  struct ExportDeformationWeight
  {
    u32               m_WeightId;
    Nocturnal::V_TUID  m_TargetIds;
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
    std::vector<MObject>  m_RequiredJoints;
    Content::V_Animation  m_Animations;
    MPlug                 m_KeyPosePlug;
    std::vector<MPlug>    m_WrinkleMapRegionPlugs;

    V_ExportBlendShapeDeformer m_ExportBlendShapeDeformers;

  public:
    ExportAnimationClip(const Nocturnal::TUID& skeletonID)
    {
      Content::AnimationClipPtr clip = new Content::AnimationClip();

      clip->m_SkeletonID = skeletonID;
      clip->m_ActorName = TXT("");

      m_ContentObject = clip;
    }

    // Gather the necessary maya data and prepare for SampleOneFrame
    void GatherMayaData( V_ExportBase &newExportObjects );

    // gather data for a single frame
    virtual void SampleOneFrame( const MTime & currentTime, bool extraFrame = false );

    const Content::AnimationClipPtr GetContentAnimationClip() const
    {
      return Reflect::DangerousCast< Content::AnimationClip >( m_ContentObject );
    }

  private:
    void GatherBlendShapeDeformers();
    void SampleOneFramesMorphTargetWeights( const MTime& currentTime, const Content::AnimationClipPtr& animClip );

  };

  typedef Nocturnal::SmartPtr<ExportAnimationClip> ExportAnimationClipPtr;
}
