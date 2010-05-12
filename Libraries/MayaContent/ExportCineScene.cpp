#include "stdafx.h"

#include "ExportCineScene.h"
#include "ExportAnimationClip.h"

#include "MayaContentCmd.h"

#include "MayaNodes/CameraController.h"
#include "igCinematicContext/FXEmitterNode.h"
#include "igCinematicContext/EffectSpecPlayerNode.h"

#include <maya/MFnCamera.h>

using namespace MayaContent;
using namespace Content;

#define DEFAULT_FLAGS ( 2 )
#define RADIUS        ( 32.0f )
#define RADIUS_INNER  ( 24.0f )

inline static f32 GetFloatChildPlugValue( const MPlug   & keysPlug,
                                                MObject & childAttribute )
{
  f32 value;
  MPlug childPlug( keysPlug.child( childAttribute ) );
  childPlug.getValue( value );
  return value;
}

void ExportCineScene::GatherMayaData( V_ExportBase & newExportObjects )
{
  EXPORT_SCOPE_TIMER( ("") );

  GatherMayaData_Cameras( newExportObjects );
  GatherMayaData_Lights( newExportObjects );
  GatherMayaData_Effects( newExportObjects );
  GatherMayaData_EffectSpecPlayers( newExportObjects );
  __super::GatherMayaData( newExportObjects );
}

void ExportCineScene::SampleOneFrame( const MTime & currentTime, bool extraFrame )
{
  // Cinematics don't need the extra frame data
  if ( extraFrame )
  {
    return;
  }

  SampleOneFrame_Cameras( currentTime );
  SampleOneFrame_Lights( currentTime );
  SampleOneFrame_Effects( currentTime );
  SampleOneFrame_EffectSpecPlayers( currentTime );
}

inline static void GatherMayaShot_FrameRange( const MPlug & keysPlug,
                                                    f32   & cutStart )
{
  cutStart = GetFloatChildPlugValue( keysPlug, ::CameraController::s_attr_i_keys_cutStart );
}

inline static MObject GatherMayaShot_CameraShape( const MPlug & inputElement )
{
  MStatus status;
  MPlug inputFocalLengthPlug( inputElement.child( ::CameraController::s_attr_i_focalLength ) );
  MPlugArray focalLengthSources;
  if ( inputFocalLengthPlug.connectedTo( focalLengthSources, true, false, &status )
    && status == MS::kSuccess
    && focalLengthSources.length() == 1 )
  {
    return focalLengthSources[0].node();
  }
  return MObject::kNullObj;
}

void ExportCineScene::GatherMayaData_Cameras( V_ExportBase &newExportObjects )
{
  MStatus status;
  // clear out any old data (shouldn't actually be any)
  m_CameraShape.clear();
  // doing a dangerous cast, because there should be no way for m_ContentObject to NOT be a Content::CineScene
  Content::CineScenePtr cineScene = Reflect::DangerousCast< Content::CineScene >( m_ContentObject );

  int len = m_ExportObjects.length();
  for ( int i = 0; i < len; ++i )
  {
    MFnDependencyNode depNodeFn( m_ExportObjects[i], &status );
    if ( status == MS::kSuccess && depNodeFn.typeId().id() == IGL_CAMERA_CONTROLLER_ID )
    {
      cineScene->m_CameraController->m_DefaultName = depNodeFn.name().asChar();
      MPlug inputPlug( depNodeFn.findPlug( ::CameraController::s_attr_input, true, &status ) );

      u32 inputSize = inputPlug.numElements();
      if ( status == MS::kSuccess
        && inputPlug.isArray()
        && inputSize > 0 )
      {
        // get all of the frame ranges
        float cutStart, nextCutStart;

        for ( i32 i=0; i<=(int)inputSize-1; i++ )
        {
          if ( i == inputSize-1 )
          {
            nextCutStart = (float)MAnimControl::animationEndTime().value();
          }
          else
          {
            MPlug inputElement( inputPlug.elementByLogicalIndex( i + 1, &status ) );
            nextCutStart = GetFloatChildPlugValue( inputElement, ::CameraController::s_attr_i_keys_cutStart );
          }
          MPlug inputElement( inputPlug.elementByLogicalIndex( i, &status ) );
          if ( status == MS::kSuccess )
          {
            CameraClipPtr shot = new CameraClip;
            GatherMayaShot_FrameRange( inputElement, cutStart );
            shot->m_StartFrame = cutStart;
            shot->m_CutFrame = cutStart;
            shot->m_EndFrame = nextCutStart - 1;
            shot->m_Rate     = Rate( MAnimControl::currentTime().unit() );
            shot->m_DataRate = Rate( MAnimControl::currentTime().unit() );

            MObject cameraShapeObj = GatherMayaShot_CameraShape( inputElement );
            MFnCamera camera( cameraShapeObj, &status );
            if ( status == MS::kSuccess )
            {
              MFnTransform transform( camera.parent(0), &status );
              if ( status == MS::kSuccess )
              {
                shot->m_DefaultName = transform.partialPathName().asChar();
                cineScene->m_CameraController->m_Shots.push_back( shot );
                
                MDagPath path;
                status = MDagPath::getAPathTo( cameraShapeObj, path );
                m_CameraShape.append( path );
              }
            }
          }
        }
      }
      //break;
    }

    //we're trying to collect our new MonitorCams for off-screen targets on Monitors, they pretty much suck
    else if( status == MS::kSuccess && depNodeFn.typeId().id() == IGL_EXPORTNODE_ID )
    {

      MFnDagNode camDag ( depNodeFn.object() );
      u32 childCount = camDag.childCount();
      for (u32 i=0; i<childCount; i++)
      {
        //this should be the cam transform
        MObject childObj( camDag.child(i) );
        MFnDagNode fnChild(childObj);

        //this should be the cam shape where the data lives
        MObject childObj2( fnChild.child(0) );
        MFnDagNode fnChild2(childObj2);
        MFnCamera camera(childObj2, &status);
        if ( status == MS::kSuccess )
        {
          MFnTransform transform( camera.parent(0), &status );
          if ( status == MS::kSuccess )
          {
            CameraClipPtr shot = new CameraClip;
            shot->m_StartFrame = 100;
            shot->m_CutFrame = 100;
            shot->m_EndFrame = (float)MAnimControl::animationEndTime().value();
            shot->m_Rate     = Rate( MAnimControl::currentTime().unit() );
            shot->m_DataRate = Rate( MAnimControl::currentTime().unit() );
            shot->m_DefaultName = transform.partialPathName().asChar();
            cineScene->m_CameraController->m_Shots.push_back( shot );
                
            MDagPath path;
            status = MDagPath::getAPathTo( childObj2, path );
            m_CameraShape.append( path );
          }
        }
      }
    }
  }
}

void ExportCineScene::GatherMayaData_Lights( V_ExportBase &newExportObjects )
{
  MStatus status;
  // clear out any old data (shouldn't actually be any)
  m_LightShape.clear();
  // doing a dangerous cast, because there should be no way for m_ContentObject to NOT be a Content::CineScene
  Content::CineScenePtr cineScene = Reflect::DangerousCast< Content::CineScene >( m_ContentObject );

  int len = m_ExportObjects.length();
  for ( int i = 0; i < len; ++i )
  {
    MFnSpotLight spotLight( m_ExportObjects[i], &status );
    if ( status==MS::kSuccess && !spotLight.isIntermediateObject() )
    {
      MFnTransform transform( spotLight.parent(0), &status );
      if ( status==MS::kSuccess )
      {
        Content::LightAnimationPtr lightAnimation = new LightAnimation( Maya::GetNodeID( m_ExportObjects[i] ) );
        lightAnimation->m_DefaultName = transform.partialPathName().asChar();
        cineScene->m_LightAnimations.push_back( lightAnimation );

        MDagPath path;
        status = MDagPath::getAPathTo( m_ExportObjects[i], path );
        m_LightShape.append( path );
      }
    }
  }
}


inline static void GatherMayaEffectSpecPlayer_Params(  MFnDagNode  & effectSpecPlayerNode,
                                                  std::string & effectSpecPlayerPackageName,
                                                  u32         & effectSpecPlayerEnum,
                                                  u32         & staticFlags )
{
  MStatus status;

  // init
  effectSpecPlayerPackageName.clear();
  effectSpecPlayerEnum = (u32)(-1);
  staticFlags = 0;

  // effectSpecPlayerPackageName
  MPlug effectSpecPlayerPackageNamePlug = effectSpecPlayerNode.findPlug( EffectSpecPlayerNode::s_attr_effectSpecPlayerPackageName, true, &status );
  if ( status == MS::kSuccess )
  {
    MString stringValue;
    effectSpecPlayerPackageNamePlug.getValue( stringValue );
    effectSpecPlayerPackageName = stringValue.asChar();
  }

  // effectSpecPlayerEnum
  MPlug effectSpecPlayerIdPlug = effectSpecPlayerNode.findPlug( EffectSpecPlayerNode::s_attr_effectSpecPlayerInsomniacId, true, &status );
  if ( status == MS::kSuccess )
  {
    int intValue;
    effectSpecPlayerIdPlug.getValue( intValue );
    effectSpecPlayerEnum = (u32)intValue;
  }

  // staticFlags
  MPlug transformAfterSpawnPlug = effectSpecPlayerNode.findPlug( EffectSpecPlayerNode::s_attr_transformAfterSpawn, true, &status );
  if ( status == MS::kSuccess )
  {
    bool boolValue;
    transformAfterSpawnPlug.getValue( boolValue );
    staticFlags = ((u32)boolValue << 1);
  }
}


inline static void GatherMayaEffect_Params( MFnDagNode  & fxNode,
                                            std::string & fxPackageName,
                                            u32         & fxEnum,
                                            u32         & staticFlags )
{
  MStatus status;

  // init
  fxPackageName.clear();
  fxEnum = (u32)(-1);
  staticFlags = 0;

  // fxPackageName
  MPlug fxPackageNamePlug = fxNode.findPlug( FXEmitterNode::s_attr_fxPackageName, true, &status );
  if ( status == MS::kSuccess )
  {
    MString stringValue;
    fxPackageNamePlug.getValue( stringValue );
    fxPackageName = stringValue.asChar();
  }

  // fxEnum
  MPlug fxIdPlug = fxNode.findPlug( FXEmitterNode::s_attr_fxInsomniacId, true, &status );
  if ( status == MS::kSuccess )
  {
    int intValue;
    fxIdPlug.getValue( intValue );
    fxEnum = (u32)intValue;
  }

  // staticFlags
  MPlug transformAfterSpawnPlug = fxNode.findPlug( FXEmitterNode::s_attr_transformAfterSpawn, true, &status );
  if ( status == MS::kSuccess )
  {
    bool boolValue;
    transformAfterSpawnPlug.getValue( boolValue );
    staticFlags = ((u32)boolValue << 1);
  }
}

void ExportCineScene::GatherMayaData_Effects( V_ExportBase &newExportObjects )
{
  MStatus status;
  // clear out any old data (shouldn't actually be any)
  m_FXNodeShape.clear();
  // doing a dangerous cast, because there should be no way for m_ContentObject to NOT be a Content::CineScene
  Content::CineScenePtr cineScene = Reflect::DangerousCast< Content::CineScene >( m_ContentObject );

  int len = m_ExportObjects.length();
  for ( int i = 0; i < len; ++i )
  {
    MFnDagNode fxNode( m_ExportObjects[i], &status );
    if ( status == MS::kSuccess
      && fxNode.typeId() == FXEmitterNode::typeId
      && !fxNode.isIntermediateObject() )
    {
      MFnTransform transform( fxNode.parent(0), &status );
      if ( status == MS::kSuccess )
      {
        Content::EffectAnimationPtr effectAnimation = new EffectAnimation( Maya::GetNodeID( m_ExportObjects[i] ) );
        effectAnimation->m_DefaultName = transform.partialPathName().asChar();
        GatherMayaEffect_Params( fxNode,
                                 effectAnimation->m_FXPackageName,
                                 effectAnimation->m_FXEnum,
                                 effectAnimation->m_StaticFlags );
        cineScene->m_EffectAnimations.push_back( effectAnimation );

        MDagPath path;
        status = MDagPath::getAPathTo( m_ExportObjects[i], path );
        m_FXNodeShape.append( path );
      }
    }
  }
}

void ExportCineScene::GatherMayaData_EffectSpecPlayers( V_ExportBase &newExportObjects )
{
  MStatus status;
  // clear out any old data (shouldn't actually be any)
  m_EffectSpecPlayerNodeShape.clear();
  // doing a dangerous cast, because there should be no way for m_ContentObject to NOT be a Content::CineScene
  Content::CineScenePtr cineScene = Reflect::DangerousCast< Content::CineScene >( m_ContentObject );

  int len = m_ExportObjects.length();
  for ( int i = 0; i < len; ++i )
  {
    MFnDagNode effectSpecPlayerNode( m_ExportObjects[i], &status );
    if ( status == MS::kSuccess
      && effectSpecPlayerNode.typeId() == EffectSpecPlayerNode::typeId
      && !effectSpecPlayerNode.isIntermediateObject() )
    {
      MFnTransform transform( effectSpecPlayerNode.parent(0), &status );
      if ( status == MS::kSuccess )
      {
        Content::EffectSpecPlayerAnimationPtr effectSpecPlayerAnimation = new EffectSpecPlayerAnimation( Maya::GetNodeID( m_ExportObjects[i] ) );
        effectSpecPlayerAnimation->m_DefaultName = transform.partialPathName().asChar();
        GatherMayaEffectSpecPlayer_Params( effectSpecPlayerNode,
                                 effectSpecPlayerAnimation->m_EffectSpecPlayerPackageName,
                                 effectSpecPlayerAnimation->m_EffectSpecPlayerEnum,
                                 effectSpecPlayerAnimation->m_StaticFlags );
        cineScene->m_EffectSpecPlayerAnimations.push_back( effectSpecPlayerAnimation );

        MDagPath path;
        status = MDagPath::getAPathTo( m_ExportObjects[i], path );
        m_EffectSpecPlayerNodeShape.append( path );
      }
    }
  }
}

inline static void SampleOneFrame_ReadCamera( const MDagPath & cameraShapePath,
                                              Math::Vector3  & translate,
                                              Math::Vector3  & forward,
                                              Math::Vector3  & up,
                                              f32            & horizFOV,
                                              f32            & vertFOV,
                                              f32            & nearPlane,
                                              f32            & farPlane )
{
  MStatus status;
  MObject worldMatrixObject;
  MFnCamera camera( cameraShapePath, &status );
  if ( status == MS::kSuccess )
  {
    MMatrix worldMatrix( cameraShapePath.inclusiveMatrix( &status ) );

    // extract useful information from the world matrix
    MVector mayaForward = camera.viewDirection( MSpace::kWorld, &status );
    MVector mayaUp = camera.upDirection( MSpace::kWorld, &status );
    
    // return the results
    translate.Set( (float)worldMatrix( 3, 0 ) * Math::CentimetersToMeters,
                   (float)worldMatrix( 3, 1 ) * Math::CentimetersToMeters,
                   (float)worldMatrix( 3, 2 ) * Math::CentimetersToMeters );

    forward.Set( (float)mayaForward.x,
                 (float)mayaForward.y,
                 (float)mayaForward.z );

    up.Set( (float)mayaUp.x,
            (float)mayaUp.y,
            (float)mayaUp.z );

    horizFOV = (float)camera.horizontalFieldOfView();
    vertFOV = (float)camera.verticalFieldOfView();
    nearPlane = (float)camera.nearClippingPlane() * Math::CentimetersToMeters;
    farPlane = (float)camera.farClippingPlane() * Math::CentimetersToMeters;
  }
  if ( status != MS::kSuccess )
  {
    translate.Set( 0.0f, 0.0f, 0.0f );
    forward.Set( 0.0f, 0.0f, 1.0f );
    up.Set( 0.0f, 1.0f, 0.0f );
    horizFOV = Math::HalfPi;
    vertFOV = 0.51238946f;  // given pi/2 horiz fov, this is 16:9 vert fov
    nearPlane = 0.01f;
    farPlane = 1000.0f;
  }
}

void ExportCineScene::SampleOneFrame_Cameras( const MTime & currentTime )
{
  Math::Vector3 translate, forward, up;
  f32           horizFOV, vertFOV, nearPlane, farPlane;
  Content::CineScene* cineScene = Reflect::DangerousCast< Content::CineScene >( m_ContentObject );
  V_CameraClip::iterator itr = cineScene->m_CameraController->m_Shots.begin();
  V_CameraClip::iterator end = cineScene->m_CameraController->m_Shots.end();
  for (unsigned int i=0; itr!=end; ++itr, ++i )
  {
    CameraClipPtr clip = (*itr);
    SampleOneFrame_ReadCamera( m_CameraShape[i],
                                 translate, forward, up, horizFOV, vertFOV, nearPlane, farPlane );
    clip->m_Animation->PushBack( translate, forward, up, horizFOV, vertFOV, nearPlane, farPlane );
  }
}

inline static void SampleOneFrame_ReadLight( const MDagPath & lightShapePath,
                                             Math::Vector3  & translate,
                                             Math::Vector3  & forward,
                                             Math::Vector3  & color,
                                             u32            & flags,
                                             f32            & radius,
                                             f32            & radiusInner,
                                             f32            & conePhi,
                                             f32            & coneTheta )
{
  MObject worldMatrixObject;
  MStatus status;
  MFnSpotLight spotLight( lightShapePath, &status );
  if ( status == MS::kSuccess )
  {
    // get the light's world matrix
    MMatrix worldMatrix( lightShapePath.inclusiveMatrix( &status ) );

     // extract useful information from the world matrix
    double *t = worldMatrix[3];

    MVector mayaForward( spotLight.lightDirection( 0, MSpace::kWorld, &status ) );
    MColor  mayaColor( spotLight.color() );

    // return the results
    translate.Set( (float)worldMatrix( 3, 0 ) * Math::CentimetersToMeters,
                   (float)worldMatrix( 3, 1 ) * Math::CentimetersToMeters,
                   (float)worldMatrix( 3, 2 ) * Math::CentimetersToMeters );

    forward.Set( (float)mayaForward.x,
                 (float)mayaForward.y,
                 (float)mayaForward.z );

    color.Set( mayaColor.r, mayaColor.g, mayaColor.b );
    radius = RADIUS;
    radiusInner = RADIUS_INNER;
    conePhi = (float)spotLight.coneAngle();
    coneTheta = (255.0f - (float)spotLight.dropOff()) * (0.5f / 255.0f);
  }
  if ( status != MS::kSuccess )
  {
    translate.Set( 0.0f, 0.0f, 0.0f );
    forward.Set( 0.0f, 0.0f, 1.0f );
    color.Set( 1.0f, 1.0f, 1.0f );
    flags = DEFAULT_FLAGS;
    radius = RADIUS;
    radiusInner = RADIUS_INNER;
    conePhi = Math::HalfPi;
    coneTheta = 1.0f;
  }
}

void ExportCineScene::SampleOneFrame_Lights( const MTime & currentTime )
{
  Math::Vector3 translate, forward, color;
  u32           flags;
  f32           radius, radiusInner, conePhi, coneTheta;
  // doing a dangerous cast, because there should be no way for m_ContentObject to NOT be a Content::LightAnimation
  Content::CineScene* cineScene = Reflect::DangerousCast< Content::CineScene >( m_ContentObject );
  V_LightAnimation::iterator itr = cineScene->m_LightAnimations.begin();
  V_LightAnimation::iterator end = cineScene->m_LightAnimations.end();
  for (unsigned int i=0; itr!=end; ++itr, ++i )
  {
    LightAnimationPtr clip = (*itr);
    SampleOneFrame_ReadLight( m_LightShape[i],
                    translate, forward, color, flags, radius, radiusInner, conePhi, coneTheta );
    clip->PushBack( translate, forward, color, flags, radius, radiusInner, conePhi, coneTheta );
  }
}

inline static void SampleOneFrame_ReadFXEmitter( const MDagPath & fxEmitterPath,
                                                 Math::Vector3  & translate,
                                                 Math::Matrix3  & rotation,
                                                 u32            & dynamicFlags )
{
  MStatus status;
  MFnDagNode fxEmitter( fxEmitterPath, &status );
  if ( status == MS::kSuccess )
  {
    // get the light's world matrix
    MMatrix worldMatrix( fxEmitterPath.inclusiveMatrix( &status ) );
     // extract useful information from the world matrix
    double *t0 = worldMatrix[0];
    double *t1 = worldMatrix[1];
    double *t2 = worldMatrix[2];
    double *t3 = worldMatrix[3];
    // get some other data from maya
    MPlug activePlug( fxEmitter.findPlug( FXEmitterNode::s_attr_active, true, &status ) );

    if ( status == MS::kSuccess )
    {
      int df;
      activePlug.getValue( df );
      // return the results
      translate.Set( (float)t3[0] * Math::CentimetersToMeters,
                     (float)t3[1] * Math::CentimetersToMeters,
                     (float)t3[2] * Math::CentimetersToMeters );
#pragma TODO("which axis is forward on an FXEmitter? I don't remember.")
      rotation = Math::Matrix3( Math::Vector3((float)t0[0],(float)t0[1],(float)t0[2]),Math::Vector3((float)t1[0],(float)t1[1],(float)t1[2]),Math::Vector3((float)t2[0],(float)t2[1],(float)t2[2]));
      dynamicFlags = (u32)df;
    }
  }
  if ( status != MS::kSuccess )
  {
    translate.Set( 0.0f, 0.0f, 0.0f );
    rotation = Math::Matrix3(Math::Vector3 (0, 0, 0), Math::Vector3 (0, 0, 0), Math::Vector3 (0, 0, 0));
    dynamicFlags = 0;
  }
}

inline static void SampleOneFrame_ReadEffectSpecPlayer(  const MDagPath & effectSpecPlayerPath,
                                                           Math::Vector3  & translate,
                                                           Math::Vector3  & direction,
                                                           u32            & dynamicFlags )
{
  MStatus status;
  MFnDagNode effectSpecPlayer( effectSpecPlayerPath, &status );
  if ( status == MS::kSuccess )
  {
    // get the light's world matrix
    MMatrix worldMatrix( effectSpecPlayerPath.inclusiveMatrix( &status ) );
     // extract useful information from the world matrix
    double *t0 = worldMatrix[0];
    double *t3 = worldMatrix[3];
    // get some other data from maya
    MPlug activePlug( effectSpecPlayer.findPlug( EffectSpecPlayerNode::s_attr_active, true, &status ) );
    if ( status == MS::kSuccess )
    {
      int df;
      activePlug.getValue( df );
      // return the results
      translate.Set( (float)t3[0] * Math::CentimetersToMeters,
                     (float)t3[1] * Math::CentimetersToMeters,
                     (float)t3[2] * Math::CentimetersToMeters );
      direction.Set( (float)t0[0], (float)t0[1], (float)t0[2] );
#pragma TODO("which axis is forward on an EffectSpecPlayer? I don't remember.")
      dynamicFlags = (u32)df;
    }
  }
  if ( status != MS::kSuccess )
  {
    translate.Set( 0.0f, 0.0f, 0.0f );
    direction.Set( 0.0f, 0.0f, 1.0f );
    dynamicFlags = 0;
  }
}

void ExportCineScene::SampleOneFrame_Effects( const MTime & currentTime )
{
  Math::Vector3 translate;
  Math::Matrix3 rotation;
  u32           dynamicFlags;
  Content::CineScene* cineScene = Reflect::DangerousCast< Content::CineScene >( m_ContentObject );
  V_EffectAnimation::iterator itr = cineScene->m_EffectAnimations.begin();
  V_EffectAnimation::iterator end = cineScene->m_EffectAnimations.end();
  for (unsigned int i=0; itr!=end; ++itr, ++i )
  {
    EffectAnimationPtr clip = (*itr);
    SampleOneFrame_ReadFXEmitter( m_FXNodeShape[i],
                    translate, rotation, dynamicFlags );
    clip->PushBack( translate, rotation, dynamicFlags );
  }
}

void ExportCineScene::SampleOneFrame_EffectSpecPlayers( const MTime & currentTime )
{
  Math::Vector3 translate, direction;
  u32           dynamicFlags;
  Content::CineScene* cineScene = Reflect::DangerousCast< Content::CineScene >( m_ContentObject );
  V_EffectSpecPlayerAnimation::iterator itr = cineScene->m_EffectSpecPlayerAnimations.begin();
  V_EffectSpecPlayerAnimation::iterator end = cineScene->m_EffectSpecPlayerAnimations.end();
  for (unsigned int i=0; itr!=end; ++itr, ++i )
  {
    EffectSpecPlayerAnimationPtr clip = (*itr);
    SampleOneFrame_ReadEffectSpecPlayer( m_EffectSpecPlayerNodeShape[i],
                    translate, direction, dynamicFlags );
    clip->PushBack( translate, direction, dynamicFlags );
  }
}