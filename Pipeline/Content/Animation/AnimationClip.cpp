#include "Platform/Windows/Windows.h"
#include "AnimationClip.h"

#include "Pipeline/Content/Scene.h"
#include "Pipeline/Content/ContentExceptions.h"

#include "Foundation/Log.h"

using namespace Reflect;
using namespace Content;

REFLECT_DEFINE_CLASS(AnimationClip)

////////////////////////////////////////////////////////////////////////////////////////////////
AnimationPtr AnimationClip::GetAnimationForJoint( const JointTransformPtr& joint )
{
  return GetAnimationForJoint( joint->m_ID );
}

////////////////////////////////////////////////////////////////////////////////////////////////
AnimationPtr AnimationClip::GetAnimationForJoint( const Nocturnal::TUID& jointID )
{
  M_Animation::iterator findItor = m_JointAnimationMap.find( jointID );
  if( findItor != m_JointAnimationMap.end() )
  {
    return findItor->second;
  }
  return NULL;
}

////////////////////////////////////////////////////////////////////////////////////////////////
CompressedAnimationPtr AnimationClip::GetCompressedAnimationForJoint( const JointTransformPtr& joint )
{
  return GetCompressedAnimationForJoint( joint->m_ID );
}

////////////////////////////////////////////////////////////////////////////////////////////////
CompressedAnimationPtr AnimationClip::GetCompressedAnimationForJoint( const Nocturnal::TUID& jointID )
{
  M_CompressedAnimation::iterator findItor = m_JointCompressedAnimationMap.find( jointID );
  if( findItor != m_JointCompressedAnimationMap.end() )
  {
    return findItor->second;
  }
  return NULL;
}

// note: setting overWrite = true will stomp any existing association of the specified joint to an animation
void AnimationClip::Associate( const Nocturnal::TUID& jointID, const AnimationPtr& animation, bool overWrite )
{
  if ( m_JointAnimationMap.size() && animation->TotalSamples() != m_JointAnimationMap.begin()->second->TotalSamples() )
    throw Nocturnal::Exception( TXT( "Cannot add an animation to an animation clip that has a different number of samples!" ) );

  M_Animation::iterator findItor = m_JointAnimationMap.find( jointID );
  if( findItor != m_JointAnimationMap.end() )
  {
    if( overWrite )
      m_JointAnimationMap.erase( findItor );   
  }
  m_JointAnimationMap.insert( M_Animation::value_type( jointID, animation ) );
}

////////////////////////////////////////////////////////////////////////////////////////////////
// note: setting overWrite = true will stomp any existing association of the specified joint to an animation
void AnimationClip::Associate( const JointTransformPtr& joint, const AnimationPtr& animation, bool overWrite )
{
  Associate( joint->m_ID, animation, overWrite );
}

////////////////////////////////////////////////////////////////////////////////////////////////
void AnimationClip::GetJointIDs( Nocturnal::V_TUID& jointIDs )
{
  M_Animation::iterator itor = m_JointAnimationMap.begin();
  M_Animation::iterator end = m_JointAnimationMap.end();
  for( ; itor != end; ++itor )
  {
    jointIDs.push_back( itor->first );
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void AnimationClip::GetAnimations( V_Animation& anims )
{
  M_Animation::iterator itor = m_JointAnimationMap.begin();
  M_Animation::iterator end = m_JointAnimationMap.end();
  for( ; itor != end; ++itor )
  {
    anims.push_back( itor->second);
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void AnimationClip::ApplyParentTransforms( const Content::Scene& scene, const Nocturnal::TUID& targetJointId, bool zeroParents )
{
  Content::JointTransformPtr targetJoint = scene.Get< Content::JointTransform >( targetJointId );

  if ( !targetJoint.ReferencesObject() )
  {
    tstring targetJointGuid;
    targetJointId.ToString( targetJointGuid );
    throw MissingJointException( targetJointGuid, TXT( "Content Scene" ) );
  }

  Content::AnimationPtr jointAnimation = m_JointAnimationMap[ targetJointId ];

  if ( !jointAnimation.ReferencesObject() )
  {
    tstring targetJointGuid;
    targetJointId.ToString( targetJointGuid );
    throw MissingJointException( targetJointGuid, TXT( "Animation Map" ) );
  }

  Content::JointTransformPtr parentJoint = scene.Get< Content::JointTransform >( targetJoint->m_ParentID );
  while( parentJoint.ReferencesObject() )
  {
    Content::AnimationPtr parentAnimation = m_JointAnimationMap[ parentJoint->m_ID ];
    
    if ( parentAnimation.ReferencesObject() )
    {
      jointAnimation->ApplyParentTransform( parentAnimation );

      // now that it's been applied, zero the parent track
      if ( zeroParents )
      {
        parentAnimation->SetAllTSRSamples();
      }
    }

    parentJoint = scene.Get< Content::JointTransform >( parentJoint->m_ParentID );
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void AnimationClip::ApplyInverseParentTransforms( const Content::Scene& scene, const Nocturnal::TUID& targetJointId )
{
  Content::JointTransformPtr targetJoint = scene.Get< Content::JointTransform >( targetJointId );

  if ( !targetJoint.ReferencesObject() )
  {
    tstring targetJointGuid;
    targetJointId.ToString( targetJointGuid );
    throw MissingJointException( targetJointGuid, TXT( "Content Scene" ) );
  }

  Content::AnimationPtr jointAnimation = m_JointAnimationMap[ targetJointId ];

  if ( !jointAnimation.ReferencesObject() )
  {
    tstring targetJointGuid;
    targetJointId.ToString( targetJointGuid );
    throw MissingJointException( targetJointGuid, TXT( "Animation Map" ) );
  }

  V_Animation parentStack;
  Content::JointTransformPtr parentJoint = scene.Get< Content::JointTransform >( targetJoint->m_ParentID );
  while( parentJoint.ReferencesObject() )
  {
    Content::AnimationPtr parentAnimation = m_JointAnimationMap[ parentJoint->m_ID ];
    parentStack.push_back( parentAnimation );

    parentJoint = scene.Get< Content::JointTransform >( parentJoint->m_ParentID );
  }

  V_Animation::reverse_iterator parentIt = parentStack.rbegin();
  V_Animation::reverse_iterator parentEnd = parentStack.rend();
  for ( ; parentIt != parentEnd; ++parentIt )
  {
    Content::Animation* parentAnimation = *parentIt;

    jointAnimation->ApplyInverseParentTransform( parentAnimation );
  }
}

/////////////////////////////////////////////////////////////////////////////////////////////////
// Convert all the transforms to world space (needed for optimal bsphere computation for the moby)
// should happen at the end of all anim processing so as to not screw them up
///////////////////////////////////////////////////////////////////////////////////////////////////
void AnimationClip::ConvertToWorldSpace(const Content::Scene& scene, JointOrderingPtr& joint_ordering)
{
/*
  //assumes with joint_ordering we will see parent before child
  Nocturnal::V_TUID::iterator jt_tuid = joint_ordering->m_JointOrdering.begin();
  Nocturnal::V_TUID::iterator end_jt_tuid = joint_ordering->m_JointOrdering.end();
  for (; jt_tuid!=end_jt_tuid; ++jt_tuid)
  {
    Content::JointTransformPtr targetJoint = scene.Get< Content::JointTransform >( *jt_tuid );

    if ( !targetJoint.ReferencesObject() )
    {
      tstring targetJointGuid;
      jt_tuid->ToString( targetJointGuid );
      throw MissingJointException( targetJointGuid, "Content Scene" );
    }
    Content::AnimationPtr jointAnimation = m_JointAnimationMap[ *jt_tuid ];
    if ( !jointAnimation.ReferencesObject() )
    {
      tstring targetJointGuid;
      jt_tuid->ToString( targetJointGuid );
      throw MissingJointException( targetJointGuid, "joint animation data.  That means we have no animation data for this joint, which may mean this animation needs to be re-exported." );
    }

    Content::JointTransformPtr parentJoint = scene.Get< Content::JointTransform >( targetJoint->m_ParentID );
    if( parentJoint.ReferencesObject() )
    {
      Content::AnimationPtr parentAnimation = m_JointAnimationMap[ parentJoint->m_ID ];
      //RARAjointAnimation->ApplyParentTransform( parentAnimation, !targetJoint->m_SegmentScaleCompensate );

      Math::Vector4 unit_scale(1.0f, 1.0f, 1.0f, 1.0f);

      u32 numSamples = jointAnimation->NumSamples();

      // 
      // transform by parent
      // 
      for(u32 frameIndex = 0; frameIndex < numSamples; ++frameIndex)
      {
        Math::Vector4     ps      = Math::Vector4(parentAnimation->m_Scale[frameIndex].x, parentAnimation->m_Scale[frameIndex].y, parentAnimation->m_Scale[frameIndex].z, 1.0f);
        Math::Quaternion  pq      = parentAnimation->m_Rotate[frameIndex];
        Math::Vector4     pt      = Math::Vector4(parentAnimation->m_Translate[frameIndex].x, parentAnimation->m_Translate[frameIndex].y, parentAnimation->m_Translate[frameIndex].z, 1.0f);
        Math::Vector4     ps_comp = (targetJoint->m_SegmentScaleCompensate ? unit_scale : ps);

        Math::Vector4     ls      = Math::Vector4(jointAnimation->m_Scale[frameIndex].x, jointAnimation->m_Scale[frameIndex].y, jointAnimation->m_Scale[frameIndex].z, 1.0f);
        Math::Quaternion  lq      = jointAnimation->m_Rotate[frameIndex];
        Math::Vector4     lt      = Math::Vector4(jointAnimation->m_Translate[frameIndex].x, jointAnimation->m_Translate[frameIndex].y, jointAnimation->m_Translate[frameIndex].z, 1.0f);

        Math::Matrix4     pm(pq);               // parent rot-translate
        pm.t.x = pt.x;
        pm.t.y = pt.y;
        pm.t.z = pt.z;

        Math::Vector4     s = (ls * ps_comp);   // scale by compensated parent scale
        Math::Quaternion  q = (pq * lq);        // mul rotation by parent
        Math::Vector4     t = pm * (lt * ps);   // scale translation by parent scale then transform by parent rot-translate

        jointAnimation->m_Scale[frameIndex]     = Math::Vector3(s.x, s.y, s.z);
        jointAnimation->m_Rotate[frameIndex]    = q;
        jointAnimation->m_Translate[frameIndex] = Math::Vector3(t.x, t.y, t.z);
      }
    }
  }



  for (; jt_tuid!=end_jt_tuid; ++jt_tuid)
  {
    Content::JointTransformPtr targetJoint = scene.Get< Content::JointTransform >( *jt_tuid );
    Content::JointTransformPtr parentJoint = scene.Get< Content::JointTransform >( targetJoint->m_ParentID );

    if (parentJoint.ReferencesObject())
    {
      Content::AnimationPtr parentAnimation = m_JointAnimationMap[ parentJoint->m_ID ];
      Content::AnimationPtr jointAnimation  = m_JointAnimationMap[ *jt_tuid ];
      u32                   numSamples      = jointAnimation->NumSamples();

      // 
      // apply scales
      // 
      for(u32 frameIndex = 0; frameIndex < numSamples; ++frameIndex)
      {
        jointAnimation->m_Scale[frameIndex] *= parentAnimation->m_Scale[frameIndex];
      }
    }
  }
*/


  Math::V_Vector3 stashed_scales(joint_ordering->m_JointOrdering.size());

  Nocturnal::TUID rootId = joint_ordering->GetMasterJoint( joint_ordering->m_JointOrdering[ 0 ] );

  Content::AnimationPtr jointAnimation = m_JointAnimationMap[ rootId ];
  if (!jointAnimation.ReferencesObject())
  {
    tstring targetJointGuid;
    rootId.ToString( targetJointGuid );
    throw MissingJointException( targetJointGuid, TXT( "joint animation data.  That means we have no animation data for this joint, which may mean this animation needs to be re-exported." ) );
  }

  u32 numSamples = jointAnimation->TotalSamples();

  for(u32 frameIndex = 0; frameIndex < numSamples; ++frameIndex)
  {
    //note: assumes with joint_ordering we will see parent before child
    Nocturnal::V_TUID::iterator it_tuid_start = joint_ordering->m_JointOrdering.begin();
    Nocturnal::V_TUID::iterator it_tuid_end   = joint_ordering->m_JointOrdering.end();

    // 
    // stash scales
    //
    u32 i = 0;
    for(Nocturnal::V_TUID::iterator it = it_tuid_start; it != it_tuid_end; ++it)
    {
      Content::JointTransformPtr targetJoint = scene.Get< Content::JointTransform >( *it );

      if ( !targetJoint.ReferencesObject() )
      {
        tstring targetJointGuid;
        it->ToString( targetJointGuid );
        throw MissingJointException( targetJointGuid, TXT( "Content Scene" ) );
      }

      Nocturnal::TUID masterId = joint_ordering->GetMasterJoint( *it );

      Content::AnimationPtr jointAnimation = m_JointAnimationMap[ masterId ];
      if ( !jointAnimation.ReferencesObject() )
      {
        tstring targetJointGuid;
        it->ToString( targetJointGuid );
        //throw MissingJointException( targetJointGuid, m_Name.c_str() );

//@@@ bad_bad_bad_bad_bad_bad_bad_bad
stashed_scales[i++] = Math::Vector3(1.0f, 1.0f, 1.0f);
continue;
//@@@ bad_bad_bad_bad_bad_bad_bad_bad
      }

      stashed_scales[i++] = jointAnimation->m_Scale[frameIndex];
    }



    // 
    // transform by parent
    // 
    for(Nocturnal::V_TUID::iterator it = it_tuid_start; it != it_tuid_end; ++it)
    {
      Content::JointTransformPtr targetJoint = scene.Get< Content::JointTransform >( *it );
      Content::JointTransformPtr parentJoint = scene.Get< Content::JointTransform >( targetJoint->m_ParentID );

      if (parentJoint.ReferencesObject() == false)
      {
        continue;
      }

      Nocturnal::TUID masterId = joint_ordering->GetMasterJoint( *it );
      Content::AnimationPtr jointAnimation  = m_JointAnimationMap[ masterId ];

//@@@ bad_bad_bad_bad_bad_bad_bad_bad
if ( !jointAnimation.ReferencesObject() )
{
  continue;
}
//@@@ bad_bad_bad_bad_bad_bad_bad_bad

      Nocturnal::TUID parentMasterId = joint_ordering->GetMasterJoint( parentJoint->m_ID  );
      Content::AnimationPtr parentAnimation = m_JointAnimationMap[ parentMasterId ];

      NOC_ASSERT(jointAnimation->TotalSamples() == numSamples);


      Math::Vector4 unit_scale(1.0f, 1.0f, 1.0f, 1.0f);

      Math::Vector4     ps      = Math::Vector4(parentAnimation->m_Scale[frameIndex].x, parentAnimation->m_Scale[frameIndex].y, parentAnimation->m_Scale[frameIndex].z, 1.0f);
      Math::Quaternion  pq      = parentAnimation->m_Rotate[frameIndex];
      Math::Vector4     pt      = Math::Vector4(parentAnimation->m_Translate[frameIndex].x, parentAnimation->m_Translate[frameIndex].y, parentAnimation->m_Translate[frameIndex].z, 1.0f);
      Math::Vector4     ps_comp = (targetJoint->m_SegmentScaleCompensate ? unit_scale : ps);

      Math::Vector4     ls      = Math::Vector4(jointAnimation->m_Scale[frameIndex].x, jointAnimation->m_Scale[frameIndex].y, jointAnimation->m_Scale[frameIndex].z, 1.0f);
      Math::Quaternion  lq      = jointAnimation->m_Rotate[frameIndex];
      Math::Vector4     lt      = Math::Vector4(jointAnimation->m_Translate[frameIndex].x, jointAnimation->m_Translate[frameIndex].y, jointAnimation->m_Translate[frameIndex].z, 1.0f);

      Math::Matrix4     pm(pq);               // parent rot-translate
      pm.t.x = pt.x;
      pm.t.y = pt.y;
      pm.t.z = pt.z;

      Math::Vector4     s = (ls * ps_comp);   // scale by compensated parent scale
      Math::Quaternion  q = (pq * lq);        // mul rotation by parent
      Math::Vector4     t = pm * (lt * ps);   // scale translation by parent scale then transform by parent rot-translate

      jointAnimation->m_Scale[frameIndex]     = Math::Vector3(s.x, s.y, s.z);
      jointAnimation->m_Rotate[frameIndex]    = q;
      jointAnimation->m_Translate[frameIndex] = Math::Vector3(t.x, t.y, t.z);
    }



    // 
    // apply scales
    // 
    i = 0;
    for(Nocturnal::V_TUID::iterator it = it_tuid_start; it != it_tuid_end; ++it)
    {
      Content::JointTransformPtr  targetJoint     = scene.Get< Content::JointTransform >( *it );

      Nocturnal::TUID masterId = joint_ordering->GetMasterJoint( *it );
      Content::AnimationPtr       jointAnimation  = m_JointAnimationMap[ masterId ];

//@@@ bad_bad_bad_bad_bad_bad_bad_bad
if ( !jointAnimation.ReferencesObject() )
{
  continue;
}
//@@@ bad_bad_bad_bad_bad_bad_bad_bad

      jointAnimation->m_Scale[frameIndex] *= stashed_scales[i++];
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void AnimationClip::RemoveTransform( const Nocturnal::TUID& targetJointId, const Nocturnal::TUID& referenceJointId )
{
  if ( m_JointAnimationMap.find( referenceJointId ) == m_JointAnimationMap.end() )
  {
    tstring referenceJointGuid;
    referenceJointId.ToString( referenceJointGuid );
    throw MissingJointException( referenceJointGuid, TXT( "Animation Map" ) );
  }

  if ( m_JointAnimationMap.find( targetJointId ) == m_JointAnimationMap.end() )
  {
    tstring targetJointGuid;
    targetJointId.ToString( targetJointGuid );
    throw MissingJointException( targetJointGuid, TXT( "Animation Map" ) );
  }

  const Content::AnimationPtr& referenceAnim = m_JointAnimationMap[ referenceJointId ];
  const Content::AnimationPtr& targetAnim = m_JointAnimationMap[ targetJointId ];

  u32 numFrames = referenceAnim->TotalSamples();
  for ( u32 frame = 0; frame < numFrames; ++frame )
  {
    targetAnim->ApplyTranslateAdjustment( -referenceAnim->m_Translate[ frame ], frame );

    Math::Quaternion rotationAdjustment = referenceAnim->m_Rotate[ frame ];
    rotationAdjustment.Invert();

    Math::Quaternion rotation( targetAnim->m_Rotate[ frame ] );
    rotation = rotationAdjustment * rotation;
    targetAnim->m_Rotate[ frame ] = rotation;

    Math::Matrix3 rotationAdjustmentMat( rotationAdjustment );
    Math::Vector3& trans = targetAnim->m_Translate[ frame ];
    targetAnim->m_Translate[ frame ] = rotationAdjustmentMat * trans;
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void AnimationClip::CalculateLinearDistancePerFrame( const Nocturnal::TUID &referenceJointId )
{
  if ( m_JointAnimationMap.find( referenceJointId ) == m_JointAnimationMap.end() )
  {
    tstring referenceJointGuid;
    referenceJointId.ToString( referenceJointGuid );
    throw MissingJointException( referenceJointGuid, TXT( "Content Scene" ) );
  }

  const Content::AnimationPtr& referenceAnim = m_JointAnimationMap[ referenceJointId ];

  m_LinearDistancePerFrame = referenceAnim->GetLinearDistancePerFrame();
}

////////////////////////////////////////////////////////////////////////////////////////////////
void AnimationClip::RemoveFirstFrameTransform( const Nocturnal::TUID &targetJointId )
{
  if ( m_JointAnimationMap.find( targetJointId ) == m_JointAnimationMap.end() )
  {
    tstring targetJointGuid;
    targetJointId.ToString( targetJointGuid );
    throw MissingJointException( targetJointGuid, TXT( "Animation Map" ) );
  }

  const Content::AnimationPtr& targetAnim = m_JointAnimationMap[ targetJointId ];

  RemoveRotTrans( targetJointId, targetAnim->m_Translate[ 0 ], targetAnim->m_Rotate[ 0 ] );
}

void AnimationClip::RemoveRotTrans( const Nocturnal::TUID& targetJointId, Math::Vector3 translation, Math::Quaternion rotation )
{
  if ( m_JointAnimationMap.find( targetJointId ) == m_JointAnimationMap.end() )
  {
    tstring targetJointGuid;
    targetJointId.ToString( targetJointGuid );
    throw MissingJointException( targetJointGuid, TXT( "Animation Map" ) );
  }

  const Content::AnimationPtr& targetAnim = m_JointAnimationMap[ targetJointId ];

  Math::Quaternion q0i( rotation );
  q0i.Invert();

  Math::Matrix3 q0imat( q0i );

  Math::Vector3 v0 = translation;

  u32 numFrames = targetAnim->TotalSamples();
  for ( u32 frame = 0; frame < numFrames; ++frame )
  {
    targetAnim->ApplyTranslateAdjustment( -v0, frame );

    Math::Quaternion rotation( targetAnim->m_Rotate[ frame ] );
    rotation = q0i * rotation;
    targetAnim->m_Rotate[ frame ] = rotation;

    Math::Vector3& trans = targetAnim->m_Translate[ frame ];
    targetAnim->m_Translate[ frame ] = q0imat * trans;
  }
}

void AnimationClip::DumpAnims( const JointOrderingPtr& jointOrdering )
{
  for ( u32 frame = 0; frame < WindowSamples(); ++frame )
  {
    Log::Print( TXT( "Frame %d:\n" ), frame );

    u32 jointIndex = 0;
    Nocturnal::V_TUID::iterator it = jointOrdering->m_JointOrdering.begin();
    Nocturnal::V_TUID::iterator end = jointOrdering->m_JointOrdering.end();
    for ( ; it != end; ++it, ++jointIndex )
    {
      Nocturnal::TUID& id = *it;
      
      Animation* anim = m_JointAnimationMap[ id ];

      Log::Print( TXT( "Joint %d [" )TUID_HEX_FORMAT TXT( "]\n" ), jointIndex, id );
      Log::Print( TXT( "\tq: %f %f %f %f\n" ), anim->m_Rotate[frame][0]
                                       , anim->m_Rotate[frame][1]
                                       , anim->m_Rotate[frame][2]
                                       , anim->m_Rotate[frame][3]);

      Log::Print( TXT( "\ts: %f %f %f\n" ), anim->m_Scale[frame].x
                                       , anim->m_Scale[frame].y
                                       , anim->m_Scale[frame].z );

      Log::Print( TXT( "\tt: %f %f %f\n" ), anim->m_Translate[frame].x
                                       , anim->m_Translate[frame].y
                                       , anim->m_Translate[frame].z );
      

      
    }
  }
}

void AnimationClip::ConvertToAdditiveBlend( const Content::Scene& scene, const JointOrderingPtr& jointOrdering, i32 reference_frame)
{
  M_Animation::iterator itor = m_JointAnimationMap.begin();
  M_Animation::iterator end = m_JointAnimationMap.end();
  for( ; itor != end; ++itor )
  {
    if ( !jointOrdering->IsRequired( (*itor).first ) )
    {
      continue;
    }

    Content::JointTransformPtr joint = scene.Get< Content::JointTransform >( (*itor).first );

    if ( !joint.ReferencesObject() )
    {
      tstring jointGuid;
      (*itor).first.ToString( jointGuid );
      throw Nocturnal::Exception( TXT( "Joint [%s (%s)] not present in bind scene!\n" ), GetName().c_str(), jointGuid.c_str() );
    }

    Content::Animation* anim = itor->second;
    Math::Matrix4& jointTransform( joint->m_ObjectTransform );

    if(reference_frame != -1)
    {
      if(reference_frame <= (int) anim->WindowSamples())
      {
        jointTransform = anim->m_Rotate[reference_frame]; 

        jointTransform.t.x = anim->m_Translate[reference_frame].x; 
        jointTransform.t.y = anim->m_Translate[reference_frame].y; 
        jointTransform.t.z = anim->m_Translate[reference_frame].z; 

        jointTransform[0] *= anim->m_Scale[reference_frame].x; 
        jointTransform[1] *= anim->m_Scale[reference_frame].y; 
        jointTransform[2] *= anim->m_Scale[reference_frame].z; 
      }
      else
      {
        Log::Warning( TXT( "Additive Clip has reference frame out of range: ref %d, max %d\n" ), reference_frame, anim->WindowSamples()); 
      }
    }


    Math::Quaternion jointQuat( (Math::Quaternion)jointTransform );
    jointQuat.Normalize();
    Math::Quaternion jointQuatInv = jointQuat.Inverted();

    for ( u32 frameIndex = 0; frameIndex < anim->WindowSamples(); ++frameIndex )
    {
      anim->m_Translate[frameIndex].x -= jointTransform.t.x;
      anim->m_Translate[frameIndex].y -= jointTransform.t.y;
      anim->m_Translate[frameIndex].z -= jointTransform.t.z;

      f32 scale_x = jointTransform[0].Length();
      f32 scale_y = jointTransform[1].Length();
      f32 scale_z = jointTransform[2].Length();
      anim->m_Scale[frameIndex].x -= scale_x;
      anim->m_Scale[frameIndex].y -= scale_y;
      anim->m_Scale[frameIndex].z -= scale_z;

      Math::Quaternion currentQuat( anim->m_Rotate[frameIndex] );
      currentQuat.Normalize();

      currentQuat = currentQuat * jointQuatInv;
      currentQuat.Normalize();

      anim->m_Rotate[frameIndex] = currentQuat;
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
u32 AnimationClip::CompressAnimations( const Content::Scene& scene, const AnimationCompressionControl& compressionControl, const JointOrderingPtr& jointOrdering, bool looping )
{
  u32 missingJoints = 0;
  Nocturnal::S_TUID handledJoints;

  M_Animation::iterator itor = m_JointAnimationMap.begin();
  M_Animation::iterator end = m_JointAnimationMap.end();
  for( ; itor != end; ++itor )
  {
    if ( !jointOrdering->IsRequired( (*itor).first ) )
    {
      continue;
    }

    Content::JointTransformPtr joint = scene.Get< Content::JointTransform >( (*itor).first );

    if ( !joint.ReferencesObject() )
    {
      missingJoints++;
      tstring jointGuid;
      (*itor).first.ToString( jointGuid );
      Log::Print(Log::Levels::Verbose,  TXT( "Joint [%s (%s)] not present in bind scene!\n" ), GetName().c_str(), jointGuid.c_str() );
      continue;
    }

    CompressedAnimationPtr compressed = new CompressedAnimation( compressionControl, looping );
    compressed->Compress( joint->GetName(), (*itor).second, joint->m_Translate );

    m_JointCompressedAnimationMap.insert( std::make_pair< Nocturnal::TUID, CompressedAnimationPtr >( (*itor).first, compressed ) );
    if ( !handledJoints.insert( (*itor).first ).second )
    {
      tstring jointGuid;
      (*itor).first.ToString( jointGuid );
      throw Nocturnal::Exception( TXT( "Already compressed data for joint [%s]!" ), jointGuid );
    }
  }

  return missingJoints;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void AnimationClip::SmoothRotations()
{
  M_Animation::iterator itor = m_JointAnimationMap.begin();
  M_Animation::iterator end = m_JointAnimationMap.end();
  for( ; itor != end; ++itor )
  {
    (*itor).second->SmoothRotations();
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
void AnimationClip::ExtractCinematicRootOffset( const Content::Scene  & scene,
                                                      Math::V_Vector3 & cinematicTranslationChannel,
                                                const Nocturnal::TUID    & abodyJointId,
                                                const Nocturnal::TUID    & motionJointId)
{
  JointTransformPtr cineRoot = scene.Get< JointTransform >( abodyJointId );
  JointTransformPtr cineMotion = scene.Get< JointTransform >( motionJointId );

  if ( cineRoot.ReferencesObject() )
  {
    Math::Matrix3 parentMat;

    // calculate parent matrix for removing orientation data from cinematic root joint
    Content::JointTransformPtr cineRootParent = scene.Get<Content::JointTransform>( cineRoot->m_ParentID );
    if ( cineMotion.ReferencesObject() )
    {
      this->ApplyParentTransforms( scene, cineMotion->m_ID, false );
    }
    this->ApplyParentTransforms( scene, cineRoot->m_ID, true );
    if ( cineRootParent )
    {
      parentMat = cineRootParent->m_GlobalTransform;
    }

    Log::Print( TXT( "cinematic root joint is named %s\n" ), cineRoot->GetName().c_str() );

    M_Animation::const_iterator animIt = m_JointAnimationMap.find( cineRoot->m_ID );
    if ( animIt == m_JointAnimationMap.end() )
    {
      throw Nocturnal::Exception( TXT( "Could not find animation for joint %s!" ), cineRoot->GetName().c_str() );
    }

    Content::Animation & rootAnimation = *animIt->second;
    
    Math::V_Vector3::iterator sampleItr = rootAnimation.m_Translate.begin();
    Math::V_Vector3::iterator sampleEnd = rootAnimation.m_Translate.end();

    Math::V_Vector3::iterator sampleMotionItr;
    if ( cineMotion.ReferencesObject() )
    {
      M_Animation::const_iterator motionIt = m_JointAnimationMap.find( cineMotion->m_ID );
      Content::Animation & motionAnimation = *motionIt->second;
      sampleMotionItr = motionAnimation.m_Translate.begin();
    }
    else
    {
      //if there is no motion joint we just want to zero out the a_body below, so by setting the 
      //motionjoint iterator to the a_body/root iterator, it should zero itself out below
      sampleMotionItr = rootAnimation.m_Translate.begin();
    }

    // extracting translation from virtual root joint for later processing
    for ( ; sampleItr != sampleEnd; ++sampleItr, ++sampleMotionItr ) //I'm making the assumption that the joints have the same # of samples
    {
      cinematicTranslationChannel.push_back( (*sampleMotionItr) );

      // make whatever is in sampleItr be relative to the cinematicTranslationChannel

      (*sampleItr).Set( (*sampleItr).x - (*sampleMotionItr).x, (*sampleItr).y - (*sampleMotionItr).y, (*sampleItr).z - (*sampleMotionItr).z );
    }
  }
}

////////////////////////////////////////////////////////////////////////////////////////////////
bool AnimationClip::HasMotion() const
{
  M_Animation::const_iterator itor = m_JointAnimationMap.begin();
  M_Animation::const_iterator end = m_JointAnimationMap.end();
  for( ; itor != end; ++itor )
  {
    if ( (*itor).second->HasMotion() )
      return true;
  }

  return false;
}

////////////////////////////////////////////////////////////////////////////////////////////////
void AnimationClip::ConvertToParentLocalSpace(const Content::Scene& scene, const Content::JointTransformPtr& rootJoint, JointOrderingPtr& joint_ordering)
{
  V_HierarchyNode children;
  scene.GetChildren( children, rootJoint );

  V_JointTransform jointChildren;

  // Children must convert before parents
  V_HierarchyNode::iterator childIt = children.begin();
  V_HierarchyNode::iterator childEnd = children.end();
  for ( ; childIt != childEnd; ++childIt )
  {
    JointTransform* joint = ObjectCast<JointTransform>( *childIt );
    if ( !joint )
    {
      continue;
    }

    if ( !joint_ordering->IsRequired( joint->m_ID ) )
    {
      continue;
    }

    jointChildren.push_back( joint );

    ConvertToParentLocalSpace( scene, joint, joint_ordering );
  }

  V_JointTransform::iterator jointChildIt = jointChildren.begin();
  V_JointTransform::iterator jointChildEnd = jointChildren.end();
  for ( ; jointChildIt != jointChildEnd; ++jointChildIt )
  {
    JointTransform* joint = *jointChildIt;
  
    M_Animation::iterator animIt = m_JointAnimationMap.find( joint->m_ID );
    if ( animIt == m_JointAnimationMap.end() )
    {
      tstring jointGuid;
      joint->m_ID.ToString( jointGuid );
      throw Nocturnal::Exception( TXT( "Could not find animation for joint [%s] [%s]!\n" ), GetName().c_str(), jointGuid.c_str() );
    }

    M_Animation::iterator parentAnimIt = m_JointAnimationMap.find( joint->m_ParentID );
    if ( parentAnimIt == m_JointAnimationMap.end() )
    {
      tstring jointGuid;
      joint->m_ParentID.ToString( jointGuid );
      throw Nocturnal::Exception( TXT( "Could not find animation for joint [%s] [%s]!\n" ), GetName().c_str(), jointGuid.c_str() );
    }

    Animation* anim = animIt->second;
    Animation* parentAnim = parentAnimIt->second;

    anim->ApplyInverseParentTransform( parentAnim );
  }
}
