#pragma once

#include "Pipeline/API.h"
#include "Foundation/TUID.h"
#include "Foundation/Reflect/Serializers.h"
#include "Pipeline/Content/Nodes/SceneNode.h"

#include "Pipeline/Content/Constants.h"

#include "JointAnimation.h"
#include "CompressedJointAnimation.h"

#include "Pipeline/Content/Nodes/Transform/JointTransform.h"
#include "JointOrdering.h"


namespace AssetBuilder
{
  struct AnimationInfo;
}


namespace Content
{
  class Scene; // forward declare
  class Processor; // forward declare

  /////////////////////////////////////////////////////////////////////////////
  class PIPELINE_API MorphTargetWeight : public Reflect::Element
  {
  public: 
    Nocturnal::TUID  m_TargetId;
    f32           m_Weight;

    MorphTargetWeight()
      : m_TargetId( 0 )
      , m_Weight( 0.0f )
    {
    }

    MorphTargetWeight( const Nocturnal::TUID& targetId, const f32 weight )
      : m_TargetId( targetId )
      , m_Weight( weight )
    {
    }

    REFLECT_DECLARE_CLASS( MorphTargetWeight, Reflect::Element );

    static void EnumerateClass( Reflect::Compositor<MorphTargetWeight>& comp )
    {
      Reflect::Field* fieldTargetId = comp.AddField( &MorphTargetWeight::m_TargetId, "m_TargetId" );
      Reflect::Field* fieldWeight = comp.AddField( &MorphTargetWeight::m_Weight, "m_Weight" );
    }
  };
  typedef Nocturnal::SmartPtr< MorphTargetWeight > MorphTargetWeightPtr;
  typedef std::vector< MorphTargetWeightPtr > V_MorphTargetWeight;


  /////////////////////////////////////////////////////////////////////////////
  class PIPELINE_API FrameMorphTargets : public Reflect::Element
  {
  public: 
    f32                  m_FrameTime;
    V_MorphTargetWeight  m_MorphTargetWeights;

    FrameMorphTargets()
      : m_FrameTime( 0.0f )
    {
    }

    FrameMorphTargets( const f32 frameTime )
      : m_FrameTime( frameTime )
    {
    }

    void AddTargetWeight( const Nocturnal::TUID& targetId, const f32 weight )
    {
      m_MorphTargetWeights.push_back( new MorphTargetWeight( targetId, weight ) );
    }

    REFLECT_DECLARE_CLASS( FrameMorphTargets, Reflect::Element );

    static void EnumerateClass( Reflect::Compositor<FrameMorphTargets>& comp )
    {
      Reflect::Field* fieldFrameTime = comp.AddField( &FrameMorphTargets::m_FrameTime, "m_FrameTime" );
      Reflect::Field* fieldMorphTargetWeights = comp.AddField( &FrameMorphTargets::m_MorphTargetWeights, "m_MorphTargetWeights" );
    }
  };
  typedef Nocturnal::SmartPtr<FrameMorphTargets> FrameMorphTargetsPtr;
  typedef std::vector< FrameMorphTargetsPtr > V_FrameMorphTargets;


  /////////////////////////////////////////////////////////////////////////////
  //
  // Wrinkle Map
  //
  static const int MaxCountWrinkleMapRegions = 8;

  class PIPELINE_API FrameWrinkleMap : public Reflect::Element
  {
  public:
    f32                  m_FrameTime;
    std::vector< f32 >                m_RegionWeights;

    FrameWrinkleMap()
      : m_FrameTime( 0.0f )
    {
    }

    FrameWrinkleMap( const f32 frameTime )
      : m_FrameTime( frameTime )
    {
    }

    void AddRegionWeight( const f32 weight )
    {
      NOC_ASSERT( m_RegionWeights.size() < MaxCountWrinkleMapRegions );
      m_RegionWeights.push_back( weight );
    }

    REFLECT_DECLARE_CLASS( FrameWrinkleMap, Reflect::Element );

    static void EnumerateClass( Reflect::Compositor<FrameWrinkleMap>& comp )
    {
      Reflect::Field* fieldFrameTime = comp.AddField( &FrameWrinkleMap::m_FrameTime, "m_FrameTime" );
      Reflect::Field* fieldRegionWeights = comp.AddField( &FrameWrinkleMap::m_RegionWeights, "m_RegionWeights" );
    }
  };
  typedef Nocturnal::SmartPtr< FrameWrinkleMap > FrameWrinkleMapPtr;
  typedef std::vector< FrameWrinkleMapPtr > V_FrameWrinkleMaps;

  /////////////////////////////////////////////////////////////////////////////
  // An animation is series of floating point values over a fixed time interval
  //
  class PIPELINE_API Animation : public SceneNode
  {
  public:

    Nocturnal::TUID       m_SkeletonID;
    tstring               m_ActorName;
    M_JointAnimation      m_JointAnimationMap;
    M_CompressedJointAnimation m_CompressedJointAnimationMap;

    V_FrameMorphTargets   m_FrameMorphTargets;

    V_FrameWrinkleMaps    m_FrameWrinkleMaps;

    // The start time for playback
    f32                   m_StartFrame;

    // The end time for playback
    f32                   m_EndFrame;

    // The sampling rate of all animations
    unsigned              m_Rate;

    // The sampling rate of the values
    unsigned              m_DataRate;

    // The linear motion this clip should exhibit
    Math::Axis            m_LinearMotionAxis;
    f32                   m_LinearDistancePerFrame;

    Animation()
      : m_ActorName( TXT( "" ) )
      , m_StartFrame( 0 )
      , m_EndFrame( 0 )
      , m_Rate( 30 )
      , m_DataRate( 30 )
      , m_LinearMotionAxis( Math::SingleAxes::Z )
      , m_LinearDistancePerFrame( 0.0f )
    {
    }
    REFLECT_DECLARE_CLASS(Animation, SceneNode);

    static void EnumerateClass( Reflect::Compositor<Animation>& comp )
    {
      Reflect::Field* fieldSkeletonID = comp.AddField( &Animation::m_SkeletonID, "m_SkeletonID" );
      Reflect::Field* fieldActorName = comp.AddField( &Animation::m_ActorName, "m_ActorName" );
      Reflect::Field* fieldJointAnimationMap = comp.AddField( &Animation::m_JointAnimationMap, "m_JointAnimationMap" );
      Reflect::Field* fieldJointCompressedAnimationMap = comp.AddField( &Animation::m_CompressedJointAnimationMap, "m_CompressedJointAnimationMap" );

      Reflect::Field* fieldFrameMorphTargets = comp.AddField( &Animation::m_FrameMorphTargets, "m_FrameMorphTargets" );
      Reflect::Field* fieldFrameWrinkleMaps = comp.AddField( &Animation::m_FrameWrinkleMaps, "m_FrameWrinkleMaps" );

      Reflect::Field* fieldStartFrame = comp.AddField( &Animation::m_StartFrame, "m_StartFrame" );
      Reflect::Field* fieldEndFrame = comp.AddField( &Animation::m_EndFrame, "m_EndFrame" );
      Reflect::Field* fieldRate = comp.AddField( &Animation::m_Rate, "m_Rate" );
      Reflect::Field* fieldDataRate = comp.AddField( &Animation::m_DataRate, "m_DataRate" );
    }

    JointAnimationPtr GetAnimationForJoint( const JointTransformPtr& joint );
    JointAnimationPtr GetAnimationForJoint( const Nocturnal::TUID& jointID );

    CompressedJointAnimationPtr GetCompressedAnimationForJoint( const JointTransformPtr& joint );
    CompressedJointAnimationPtr GetCompressedAnimationForJoint( const Nocturnal::TUID& jointID );

    // associate an animation with a joint
    // note: setting overWrite = true will stomp any existing association of the specified joint to an animation
    void Associate( const JointTransformPtr& joint, const JointAnimationPtr& animation, bool overWrite = false );

    void Associate( const Nocturnal::TUID& jointID, const JointAnimationPtr& animation, bool overWrite = false );

    void GetJointIDs( Nocturnal::V_TUID& jointIDs );

    u32 NumAnimations()
    {
      return (u32)m_JointAnimationMap.size();
    }

    u32 WindowSamples() const
    {
      return m_JointAnimationMap.size() > 0 ? m_JointAnimationMap.begin()->second->WindowSamples() : 0;
    }

    u32 TotalSamples() const
    {
      return m_JointAnimationMap.size() > 0 ? m_JointAnimationMap.begin()->second->TotalSamples() : 0;
    }


    bool HasMotion() const;

    void GetJointAnimations( V_JointAnimation& animations );

    void ConvertToWorldSpace(const Content::Scene& scene, JointOrderingPtr& joint_ordering);
    void ConvertToParentLocalSpace(const Content::Scene& scene, const Content::JointTransformPtr& rootJoint, JointOrderingPtr& joint_ordering);
    void ApplyParentTransforms( const Content::Scene& scene, const Nocturnal::TUID& targetJointId, bool zeroParents );
    void ApplyInverseParentTransforms( const Content::Scene& scene, const Nocturnal::TUID& targetJointId );
    void DumpAnims( const JointOrderingPtr& jointOrdering );
    void RemoveTransform( const Nocturnal::TUID& targetJointId, const Nocturnal::TUID& referenceJointId );
    void RemoveFirstFrameTransform( const Nocturnal::TUID& targetJointId );
    void RemoveRotTrans( const Nocturnal::TUID& targetJointId, Math::Vector3 translation, Math::Quaternion rotation );
    void CalculateLinearDistancePerFrame( const Nocturnal::TUID& referenceJointId );
    void ConvertToAdditiveBlend( const Content::Scene& scene, const JointOrderingPtr& jointOrdering, i32 reference_frame );

    u32  CompressAnimations( const Content::Scene& scene,
      const AnimationCompressionControl& compressionControl,
      const JointOrderingPtr& jointOrdering,
      bool looping = false );

    void SmoothRotations();

    void ExtractCinematicRootOffset( const Content::Scene  & scene,
      Math::V_Vector3 & cinematicTranslationChannel,
      const Nocturnal::TUID    & abodyJointId,
      const Nocturnal::TUID    & motionJointId);
  };

  typedef Nocturnal::SmartPtr<Animation> AnimationPtr;
  typedef std::vector<AnimationPtr> V_Animation;
}