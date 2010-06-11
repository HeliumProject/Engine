#include "stdafx.h"

#include "Foundation/Boost/Regex.h"
#include "Foundation/Log.h"
#include "ExportAnimationClip.h"
#include "MayaNodes/ExportNode.h"
#include "MayaNodes/ExportNodeSet.h"
#include "MayaContentCmd.h"
#include "MayaUtils/Export.h"
#include "Foundation/Boost/Regex.h"

using namespace Content;
using namespace MayaContent;
using namespace Nocturnal;

// should be the same as s_KeyPoseAttrName in \MayaNodes\ExportNode.cpp
const char* s_KeyPoseAttrName     = "KeyPose";


#define g_kMinTargetWeight  0.01f // min target contribution


void ExportAnimationClip::GatherMayaData( V_ExportBase &newExportObjects )
{  
  EXPORT_SCOPE_TIMER( ("") );

  MStatus status;

  //AnimationClip* animClip = Reflect::DangerousCast< AnimationClip >( m_ContentObject );
  const Content::AnimationClipPtr animClip = GetContentAnimationClip();

  // 
  // Get the skeleton export node
  // 
  Maya::ExportInfo* skeletonInfo = NULL;
  Maya::GetExportInfo( animClip->m_SkeletonID, skeletonInfo );
  NOC_ASSERT( skeletonInfo );

  boost::cmatch results;
  const boost::regex namePattern( "(.*):.*" );
  char partialName[255];
  sprintf( partialName, "%s", skeletonInfo->m_pathStrPartial.c_str() );
  if ( boost::regex_search(partialName, results, namePattern))
  {
    animClip->m_ActorName = ResultAsString(results, 1); 
  }

  //
  // Get the relevant joints
  //
  MObjectArray joints;
  Maya::findNodesOfType( joints, MFn::kJoint, skeletonInfo->m_path.node() );

  //
  // get the KeyPose attribute plug
  //
  MFnDagNode skelDagNodeFn( skeletonInfo->m_path, &status );
  m_KeyPosePlug = skelDagNodeFn.findPlug( s_KeyPoseAttrName, true, &status );

  //
  // Get the WrinkleMap region plugs
  //
  m_WrinkleMapRegionPlugs.clear();
  MPlug useWrinkleMapPlug = skelDagNodeFn.findPlug( ExportNode::s_UseWrinkleMapAttrName, true, &status );
  bool useWrinkleMap = false;
  if ( ( useWrinkleMapPlug.getValue( useWrinkleMap ) == MStatus::kSuccess ) && useWrinkleMap )
  {
    //m_WrinkleMapRegionPlugs.setLength( (u32) Content::MaxCountWrinkleMapRegions );
    m_WrinkleMapRegionPlugs.resize( (u32) Content::MaxCountWrinkleMapRegions, MPlug() );
    for ( int i = 0; i < Content::MaxCountWrinkleMapRegions; ++i )
    {
      m_WrinkleMapRegionPlugs[i] = skelDagNodeFn.findPlug( ExportNode::s_WrinkleMapRegionAttrNames[i], true, &status );
      NOC_ASSERT( status == MStatus::kSuccess )
    }
  }


  //
  //  this is an offset into the non-required joint array
  //
  //u32 looseAttachmentOffset = joints.length();

  //
  // get the relevant loose attachments 
  //

  if (joints.length() == 0)
  {
    MGlobal::displayInfo("No joints to export animation from.");
    return;
  }

  //
  // Construct Animation Objects
  //

  MTime startTime( MAnimControl::minTime() );
  MTime endTime  ( MAnimControl::maxTime() );

  //crappy hack for multiple actors with the same GUID, deletes each from the multimap as it finishes with it
  //after it runs GetExportInfo for the second time here inside GetRequiredJointCount above
  bool result = Maya::RemoveMultiMapDupe( animClip->m_SkeletonID );

  u32 numJoints = joints.length();
  for( u32 i = 0; i < numJoints; ++i )
  {
    MObject object = joints[i];
    MFnIkJoint jointFn (object);

    m_RequiredJoints.push_back(object);
    Content::JointTransformPtr joint = new Content::JointTransform( Maya::GetNodeID( object ) );

    Content::AnimationPtr anim = new Content::Animation();
    anim->m_JointID = joint->m_ID;

    animClip->Associate( joint, anim );
    m_Animations.push_back( anim );
  }

  //
  // Bake Data
  //
  std::vector<MObject>::iterator itor;
  std::vector<MObject>::iterator end = m_RequiredJoints.end();

  //
  // gather blend factors
  //
  itor = m_RequiredJoints.begin();
  for ( u32 i = 0; itor != end; ++itor, ++i )
  {
    MFnIkJoint jointFn (*itor, &status);

#pragma TODO( "What should we do in this case? this shouldn't be happening" )
    if ( !status )
      continue;

    if( jointFn.hasAttribute( "BlendFactor" ) )
    {
      MStatus s;
      MPlug blendFactorPlug = jointFn.findPlug( "BlendFactor", true, &s );

      if ( s )
      {
        blendFactorPlug.getValue( m_Animations[ i ]->m_BlendFactor );
      }
    }
  }

  animClip->m_StartFrame = (f32)startTime.value();
  animClip->m_EndFrame   = (f32)endTime.value();
  animClip->m_Rate       = Rate( MAnimControl::currentTime().unit() );
  animClip->m_DataRate   = Rate( MAnimControl::currentTime().unit() );


  // Gather Morph target info
  GatherBlendShapeDeformers();

  

  __super::GatherMayaData( newExportObjects );
}



///////////////////////////////////////////////////////////////////////////////
// Gather all of the deformers that are in the scene
//
void ExportAnimationClip::GatherBlendShapeDeformers()
{
  MStatus status;
  
  MFnBlendShapeDeformer morpherFn;
  
  // get all of the exported meshes under the Geometry export nodes
  Maya::S_MObject exportBaseObjects;

  MObjectArray geometryNodes;

  // Cinematic scenes can contain more than one actor, so we want to 
  // only store the export nodes for the actor we are currently exporting
  const Content::AnimationClipPtr animClip = GetContentAnimationClip();
  const std::string actorNamespace = animClip->m_ActorName + ":";
  if ( !animClip->m_ActorName.empty() )
  {
    Maya::S_MObject sets;
    Maya::findNodesOfType( sets, ExportNodeSet::s_TypeID, MFn::kSet );

    Maya::S_MObject::const_iterator setItr = sets.begin();
    Maya::S_MObject::const_iterator setEnd = sets.end();
    for ( ; setItr != setEnd; ++setItr )
    {
      MFnDependencyNode nodeFn( *setItr, &status );
      if ( status )
      {
        ExportNodeSet* exportSet = static_cast< ExportNodeSet* >(nodeFn.userNode( &status ) );
        if ( status )
        {
          std::string nodeSetName = nodeFn.name().asChar();
          if ( nodeSetName.find( actorNamespace ) == 0 )
          {
            exportSet->GetExportNodes( geometryNodes, Content::ContentTypes::Geometry );
          }
        }
      }
    }
  }
  else
  {
    ExportNode::FindExportNodes( geometryNodes, Content::ContentTypes::Geometry );
  }

  u32 numObjects = geometryNodes.length();
  for( u32 i = 0; i < numObjects; ++i )
  {
    MObject& object = geometryNodes[i];
    status = Maya::findNodesOfType( exportBaseObjects, MFn::kMesh, object );
  }


  // iterate over all of the blend shapes in the scene 
  // skipping those with base objects that are NOT under a Geometry ExportNode
  MItDependencyNodes blendShapeItr( MFn::kBlendShape );
  for( ; (blendShapeItr.isDone() == false) ; blendShapeItr.next() )
  {
    MObject& blendShapeObj = blendShapeItr.item();

    NOC_ASSERT( blendShapeObj.apiType() == MFn::kBlendShape );

    morpherFn.setObject( blendShapeObj );
    std::string morpherFnName = morpherFn.name().asChar();

    // get the base object
    MObjectArray baseObjects;

    status = morpherFn.getBaseObjects( baseObjects );
    NOC_ASSERT( status );

    if ( baseObjects.length() == 0 )
    {
      continue;
    }

    if ( baseObjects.length() != 1 )
    {
      Log::Warning("Morph target (%s) has more than one base object, this is not supported", morpherFnName.c_str() );
      continue;
    }

    // not an exported base object, skip the target
    if ( exportBaseObjects.find( baseObjects[0] ) == exportBaseObjects.end() )
    {
      continue;
    }

    ExportBlendShapeDeformer exportBlendShapeDeformer;
    exportBlendShapeDeformer.m_BlendShapeObject = blendShapeObj;
    exportBlendShapeDeformer.m_BaseObject = baseObjects[0];

    Log::Debug( "Gathering morph targets for morpher: %s", morpherFnName.c_str() );

    u32 numWeights = morpherFn.numWeights( &status );
    if ( numWeights < 1 )
    {
      continue;
    }

    MIntArray weightIndices;
    status = morpherFn.weightIndexList( weightIndices );
    for( u32 weightIndex = 0; weightIndex < numWeights; ++weightIndex )
    {
      ExportDeformationWeight exportDeformationWeight;
      exportDeformationWeight.m_WeightId = weightIndices[weightIndex];

      // get the targets
      MObjectArray targets;

      status = morpherFn.getTargets( exportBlendShapeDeformer.m_BaseObject, exportDeformationWeight.m_WeightId, targets);
      NOC_ASSERT(status);

      if ( targets.length() < 1 )
      {
        continue;
      }

      for( u32 targetIndex = 0; targetIndex < targets.length(); ++targetIndex )
      {
        exportDeformationWeight.m_TargetIds.push_back( Maya::GetNodeID( targets[targetIndex] ) );
      }

      exportBlendShapeDeformer.m_ExportDeformationWeight.push_back( exportDeformationWeight );
    }

    m_ExportBlendShapeDeformers.push_back( exportBlendShapeDeformer );
  }
}


///////////////////////////////////////////////////////////////////////////////
void ExportAnimationClip::SampleOneFrame( const MTime& currentTime, bool extraFrame )
{
  const Content::AnimationClipPtr animClip = GetContentAnimationClip();

  // Required Joints
  std::vector<MObject>::iterator itr = m_RequiredJoints.begin();
  std::vector<MObject>::iterator end = m_RequiredJoints.end();
  for ( u32 i = 0; itr != end; ++itr, ++i )
  {
    MFnIkJoint jointFn (*itr);
    MFnTransform transformFn( *itr );

    MVector mayaTranslation = jointFn.getTranslation( MSpace::kTransform );
    mayaTranslation *= Math::CentimetersToMeters;

    m_Animations[ i ]->m_Translate.push_back( Math::Vector3( (float)mayaTranslation.x, (float)mayaTranslation.y, (float)mayaTranslation.z ) );

    double s[ 3 ];
    jointFn.getScale( s );
    m_Animations[i]->m_Scale.push_back( Math::Vector3 ( (float) s[ 0 ], (float) s[ 1 ], (float) s[ 2 ] ) );

    MEulerRotation r;
    jointFn.getRotation( r );
    MQuaternion qr = r.asQuaternion();

    MQuaternion jointOrient;
    jointFn.getOrientation( jointOrient );

    MQuaternion jointRotateAxis;
    jointFn.getScaleOrientation( jointRotateAxis );

    qr = jointRotateAxis * qr * jointOrient;

    Math::Quaternion rotation( (float)qr.x, (float)qr.y, (float)qr.z, (float)qr.w );

    m_Animations[ i ]->m_Rotate.push_back( rotation );

    if ( !extraFrame )
    {
      m_Animations[ i ]->m_WindowSamples++;
    }

    m_Animations[ i ]->m_TotalSamples++;
  }


  if ( !extraFrame )
  {
    // Wrinkle Map
    //if ( m_WrinkleMapRegionPlugs.length() > 0 )
    if ( !m_WrinkleMapRegionPlugs.empty() )
    {
      MStatus status;

      Content::FrameWrinkleMapPtr frameWrinkleMap = new FrameWrinkleMap( (f32)currentTime.value() );

      frameWrinkleMap->m_RegionWeights.resize( Content::MaxCountWrinkleMapRegions );
      for ( int i = 0; i < Content::MaxCountWrinkleMapRegions; ++i )
      {
        if ( !m_WrinkleMapRegionPlugs[i].isNull( &status ) && status == MStatus::kSuccess )
        {
          float region = 0.0f;    
          status = m_WrinkleMapRegionPlugs[i].getValue( region );
          if ( status )
          {
            frameWrinkleMap->m_RegionWeights[i] = region;
          }
        }
      }
      animClip->m_FrameWrinkleMaps.push_back( frameWrinkleMap );
    }

    // Morph Targets
    SampleOneFramesMorphTargetWeights( currentTime, animClip );
  }
}


///////////////////////////////////////////////////////////////////////////////
// Iterate over the morph targets and sample the frames morph target weights
//
void ExportAnimationClip::SampleOneFramesMorphTargetWeights( const MTime& currentTime, const Content::AnimationClipPtr& animClip )
{
  if ( m_ExportBlendShapeDeformers.empty() )
  {
    return;
  }

  MStatus status;

  Content::FrameMorphTargetsPtr frameMorphTargetInfo = new FrameMorphTargets( (f32)currentTime.value() );

  MFnBlendShapeDeformer morpherFn;

  u32 numBlendObjects = (u32)m_ExportBlendShapeDeformers.size();

  for( u32 blendObjIndex = 0; blendObjIndex < numBlendObjects; ++blendObjIndex )
  {
    ExportBlendShapeDeformer& exportBlendShapeDeformer = m_ExportBlendShapeDeformers[blendObjIndex];

    MObject& blendShapeObj = exportBlendShapeDeformer.m_BlendShapeObject;
    morpherFn.setObject( blendShapeObj );

    MObject& baseObject = exportBlendShapeDeformer.m_BaseObject;

    u32 numWeights = (u32) exportBlendShapeDeformer.m_ExportDeformationWeight.size();

    for( u32 weightIndex = 0; weightIndex < numWeights; ++weightIndex )
    {
      ExportDeformationWeight& exportDeformationWeight = exportBlendShapeDeformer.m_ExportDeformationWeight[weightIndex];
      u32 weightId = exportDeformationWeight.m_WeightId;

      f32 weight   = morpherFn.weight( weightId, &status );
      NOC_ASSERT(status);

      if ( ABS(weight) <= g_kMinTargetWeight )
      {
        continue;
      }

      u32 numTargets = (u32) exportDeformationWeight.m_TargetIds.size();

      for( u32 targetIndex = 0; targetIndex < numTargets; ++targetIndex )
      {
        UID::TUID& targetId = exportDeformationWeight.m_TargetIds[targetIndex];
        frameMorphTargetInfo->AddTargetWeight( targetId, weight );
      }
    }
  }

  if ( !frameMorphTargetInfo->m_MorphTargetWeights.empty() )
  {
    animClip->m_FrameMorphTargets.push_back( frameMorphTargetInfo );
  }
}
