#include "stdafx.h"
#include "EntityNode.h"
#include "EntityGroupNode.h"
#include "EntityAssetNode.h"
#include "ConstructionTool.h"

#include "Common/Types.h"
#include "Common/Version.h"

#include "MayaUtils/NodeTypes.h"
#include "MayaUtils/ErrorHelpers.h"

#include "UniqueID/TUID.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/ArtFileAttribute.h"
#include "MayaUtils/Duplicate.h"
#include "MayaUtils/Utils.h"
#include "Math/EulerAngles.h"

#include <maya/MPointArray.h>
#include <maya/MModelMessage.h>
#include <maya/MFnStringData.h>
#include <maya/MObjectHandle.h>

using namespace RPC;
using namespace Asset;
using namespace Construction;
using namespace Attribute;

// enable this to watch all dag changes, just for debugging
//#define WATCH_ALL_DAG_CHANGES

// callbacks
MIntArray EntityNode::s_CallbackIDs; 

// attributes 
MObject EntityNode::s_ArtFilePath;

// statics
MPointArray       EntityNode::s_PointerPoints;
MColor            EntityNode::s_DrawColor(0,.2f,1);
bool              EntityNode::s_PointerYUp;
const MTypeId     EntityNode::s_TypeID(IGL_ENTITYNODE_ID);
const char*       EntityNode::s_TypeName("EntityNode");
Math::Matrix4     EntityNode::s_RelativeTransform;
V_EntityNodePtr   EntityNode::s_ShowNodes;
V_EntityNodePtr   EntityNode::s_DupeNodes;
MObject           EntityNode::s_EntityNodeGroup;
bool              EntityNode::s_ReplaceSelection = false;

MStatus EntityNode::AddCallbacks() 
{
  MStatus stat;

  s_CallbackIDs.append((int)MDGMessage::addNodeAddedCallback( EntityNode::NodeAddedCallback, EntityNode::s_TypeName, NULL, &stat ) );
  if (!stat)
    MGlobal::displayError("Unable to add user callback EntityNode::NodeAddedCallback for EntityAssetNode.\n");

  s_CallbackIDs.append((int)MDGMessage::addNodeRemovedCallback( EntityNode::NodeRemovedCallback, EntityNode::s_TypeName, NULL, &stat ) );
  if (!stat)
    MGlobal::displayError("Unable to add user callback EntityNode::NodeRemovedCallback for EntityAssetNode.\n");

  s_CallbackIDs.append((int)MModelMessage::addAfterDuplicateCallback( EntityNode::AfterDuplicateCallback, NULL, &stat ) );
  if (!stat)
    MGlobal::displayError("Unable to add user callback EntityNode::AfterDuplicateCallback for EntityAssetNode.\n");

  s_CallbackIDs.append((int)MModelMessage::addCallback( MModelMessage::kActiveListModified, EntityNode::SelectionChangedCallback, NULL, &stat ) );
  if (!stat)
    MGlobal::displayError("Unable to add user callback EntityNode::SelectionChangedCallback for EntityAssetNode.\n");

#ifdef WATCH_ALL_DAG_CHANGES

  s_CallbackIDs.append( MDagMessage::addAllDagChangesCallback( EntityNode::DagChangesCallback, NULL, &stat ) );
  if (!stat)
    MGlobal::displayError("Unable to add user callback EntityNode::DagChangesCallback for EntityAssetNode.\n");

#endif

  return MS::kSuccess; 
}

MStatus EntityNode::RemoveCallbacks()
{
  MStatus stat;

  // Remove all the callbacks
  stat = MMessage::removeCallbacks(s_CallbackIDs);
  if (!stat)
  {
    MGlobal::displayError("Unable to delete callbacks");
  }

  return MS::kSuccess;
}

EntityNode::EntityNode() 
: m_UID( UniqueID::TUID::Null )
, m_AttributeChangedCB( -1 )
, m_ChildAddedCB( -1 )
{

}

EntityNode::~EntityNode()
{
  MNodeMessage::removeCallback( m_AttributeChangedCB );
  m_Entity = NULL;
}

void* EntityNode::Creator()
{
  return new EntityNode();
}

MStatus EntityNode::Initialize()
{
  MAYA_START_EXCEPTION_HANDLING();

  MStatus stat;

  // wtf is this - rachel
  MFnStringData dataFn;
  MObject stringData = dataFn.create( "" );

  // fileName attribute
  MFnTypedAttribute tAttr;
  s_ArtFilePath = tAttr.create("ArtFilePath", "fn", MFnData::kString, &stat);
  tAttr.setDefault( stringData );
  MCheckErr(stat, "Unable to create attr: ArtFilePath");

  tAttr.setReadable(true);
  tAttr.setWritable(false);

  stat = addAttribute(s_ArtFilePath);
  MCheckErr(stat, "Unable to add attr: ArtFilePath");

  MAYA_FINISH_EXCEPTION_HANDLING();

  return MS::kSuccess;
}

void EntityNode::postConstructor() 
{
  MPlug plug ( thisMObject(), s_ArtFilePath );

  m_Plug = plug;

  s_ReplaceSelection = true;
}

void EntityNode::copyInternalData( MPxNode* node )
{
  MAYA_START_EXCEPTION_HANDLING();

  EntityNode* source = (EntityNode*)(node);
  source->Hide();
  m_Entity = Reflect::ObjectCast< Asset::Entity >( source->m_Entity->Clone() );
  UniqueID::TUID::Generate( m_Entity->m_ID );
  m_UID = m_Entity->m_ID;

  EntityAssetNode& instanceClassNode = EntityAssetNode::Get( m_Entity->GetEntityAssetID() );
  if( instanceClassNode == EntityAssetNode::Null )
  {
    return;
  }

  instanceClassNode.m_Instances.insert( std::pair< UniqueID::TUID, EntityNode*>( m_UID, this ) );

  s_ShowNodes.push_back( source );
  s_DupeNodes.push_back( this ); 

  MAYA_FINISH_EXCEPTION_HANDLING();
}

bool EntityNode::isBounded() const 
{ 
  return false;
}

bool EntityNode::isAbstractClass() const 
{
  return false;
}

bool EntityNode::excludeAsLocator() const 
{
  return false;
}

MStatus EntityNode::compute(const MPlug& plug, MDataBlock& data) 
{ 
  return MS::kUnknownParameter;
}

void EntityNode::draw( M3dView & view, const MDagPath & path, M3dView::DisplayStyle style, M3dView::DisplayStatus status ) 
{ 
  MStatus stat;

  // Check whether need to redo points
  if (s_PointerPoints.length())
  {
    if (s_PointerYUp != MGlobal::isYAxisUp())
    {
      // Store the up axis
      s_PointerYUp = MGlobal::isYAxisUp();

      // Points are for Y-UP
      float pointerVerts[][4] =
      {
        {-200, -100,   0, 1},
        {-200,    0, 100, 1},
        {-200,    0, 100, 1},
        {-200,  100,   0, 1},
        {-200,  100,   0, 1},
        {   0,    0,   0, 1},
        {   0,    0,   0, 1},
        {-200, -100,   0, 1},
        {-200,    0, 100, 1},
        {   0,    0,   0, 1},
        {-200, -100,   0, 1},
        {-200,  100,   0, 1}
      };

      s_PointerPoints = MPointArray(pointerVerts, 12); 

      // Flip the points if Y-Up
      if (s_PointerYUp) 
      {
        for(unsigned i = 0; i < s_PointerPoints.length(); ++i) 
        {
          MPoint p = s_PointerPoints[i];
          s_PointerPoints[i].x = p.y;
          s_PointerPoints[i].y = p.z;
          s_PointerPoints[i].z = p.x;
        }
      }
    }
  }

  // Go ahead and draw pointer selected if this instance is selected
  switch (status)
  {
  case M3dView::kDormant:
    {
      stat = view.setDrawColor(s_DrawColor);
      MErr(stat, "Unable to do: view.setDrawColor");
      break;
    }

  case M3dView::kTemplate:
    {
      MColor templateColor = view.templateColor(&stat);
      MErr(stat, "Unable to do: view.templateColor");
      stat = view.setDrawColor(templateColor);
      MErr(stat, "Unable to do: view.setDrawColor");
      break;
    }

  default:
    {
      stat = view.setDrawColor(15, M3dView::kActiveColors);
      MErr(stat, "Unable to do: view.setDrawColor"); 
      break;
    }
  }

  glPushMatrix();
  {
    glBegin( GL_LINES );
    {
      u32 len = s_PointerPoints.length();
      for( unsigned i = 0; i < len; i += 2 ) 
      {
        glVertex3f((GLfloat)s_PointerPoints[i].x,   (GLfloat)s_PointerPoints[i].y,   (GLfloat)s_PointerPoints[i].z);
        glVertex3f((GLfloat)s_PointerPoints[i+1].x, (GLfloat)s_PointerPoints[i+1].y, (GLfloat)s_PointerPoints[i+1].z);
      }
    }
    glEnd();
  }
  glPopMatrix();
}

void EntityNode::SetBackingEntity( const Asset::EntityPtr& entity )
{
  m_Entity = entity;
  m_UID = m_Entity->m_ID;

  AttributeViewer< ArtFileAttribute > artFile( m_Entity );

  m_Plug.setAttribute( s_ArtFilePath );
  m_Plug.setValue( artFile->GetFilePath().c_str() );

  Math::Matrix4 gm = m_Entity->m_GlobalTransform * s_RelativeTransform.Inverted();
  MMatrix mat;

  gm.t.x *= 100.0f;
  gm.t.y *= 100.0f;
  gm.t.z *= 100.0f;

  for (int i=0; i<4; i++)
  {
    for (int j=0; j<4; j++)
    {
      mat[i][j] = gm[i][j];
    }
  }

  MFnTransform transFn( thisMObject() );
  MTransformationMatrix transMat (mat);
  transFn.set(transMat);

  AddAttributeChangedCallback();
}

void EntityNode::UpdateBackingEntity()
{
  MFnTransform transFn( thisMObject() );
  MMatrix mat = transFn.transformationMatrix();

  mat[3][0] *= 0.01;
  mat[3][1] *= 0.01;
  mat[3][2] *= 0.01;

  for (int i=0; i<4; i++)
  {
    for (int j=0; j<4; j++)
    {
      m_Entity->m_GlobalTransform[i][j] = (float)mat[i][j];
    }
  }
  m_Entity->m_GlobalTransform *= s_RelativeTransform;

  // there can be no hierarchy in the Scratch Pad scene, so this should be fine
  Math::Scale scale;
  Math::Vector3 translate;
  Math::EulerAngles rotate;
  m_Entity->m_GlobalTransform.Decompose( scale, rotate, translate );

  m_Entity->m_Scale.x = scale.x;
  m_Entity->m_Scale.y = scale.y;
  m_Entity->m_Scale.z = scale.z;

  m_Entity->m_Rotate.x = rotate.angles.x;
  m_Entity->m_Rotate.y = rotate.angles.y;
  m_Entity->m_Rotate.z = rotate.angles.z;

  m_Entity->m_Translate = translate;

  m_Entity->m_ObjectTransform = m_Entity->m_GlobalTransform;
}

void EntityNode::GetArtFilePath( MString& artFilePath )
{
  m_Plug.setAttribute( s_ArtFilePath );
  m_Plug.getValue( artFilePath );
}

void EntityNode::Show( const EntityAssetNode& instanceClassNode )
{
  if( instanceClassNode == EntityAssetNode::Null )
    return;

  MFnDagNode instanceFn( thisMObject() );
  MDagPath path;
  instanceFn.getPath( path );

  MFnDagNode nodeFn( instanceClassNode.thisMObject() );
  u32 len = nodeFn.childCount();

  for( u32 i = 0; i < len; ++i )
  {
    MFnDagNode nodeFn( nodeFn.child( i ) );

    MDagPath child;
    nodeFn.getPath( child );
    MDagPath result;
    Maya::duplicate( child, path, result, true, true );
  }
}

void EntityNode::Show()
{
  EntityAssetNode& instanceClassNode = EntityAssetNode::Get( m_Entity->GetEntityAssetID() );

  Show( instanceClassNode );
}

void EntityNode::Hide()
{
  MFnTransform transformFn( thisMObject() );

  u32 len = transformFn.childCount();

  MFnDagNode nodeFn;
  for( u32 i = 0; i < len; ++i )
  {
    nodeFn.setObject( transformFn.child( 0 ) );
    MDagPath path;
    nodeFn.getPath( path );        
    MGlobal::deleteNode( path.node() );
  }
}

void EntityNode::Flatten()
{
  EntityAssetNode& instanceClassNode = EntityAssetNode::Get( m_Entity->GetEntityAssetID() );

  if( instanceClassNode == EntityAssetNode::Null )
    return;

  MFnDagNode instanceFn( thisMObject() );

  MFnDagNode nodeFn( instanceClassNode.thisMObject() );
  u32 len = nodeFn.childCount();

  MDagPath path;
  MFnTransform newTransformFn;
  MObject newTransform = newTransformFn.create();

  MTransformationMatrix matrix = instanceFn.transformationMatrix();
  newTransformFn.set( matrix );
  newTransformFn.getPath( path );

  for( u32 i = 0; i < len; ++i )
  {
    MFnDagNode nodeFn( nodeFn.child( i ) );

    MDagPath child;
    nodeFn.getPath( child );
    MDagPath result;

    Maya::duplicate( child, path, result, false, false );
  }

  Maya::LockHierarchy( thisMObject(), false );
}

void EntityNode::Unselect( MSelectionList& list )
{
  Maya::RemoveHierarchy( thisMObject(), list );
}

void EntityNode::NodeAddedCallback( MObject& node, void* clientData )
{
  MAYA_START_EXCEPTION_HANDLING();

  if( MFileIO::isReadingFile() )
  {
    return;
  }

  //
  // This is because for some reason, if you do this in the postContructor, it doesn't work!
  //
  MFnDagNode nodeFn( s_EntityNodeGroup );

  //I swapped this check to see if the s_EntityNodeGroup is still valid by its handle, instead
  //of just checking if the object is null because if the object gets deleted, it won't be Null
  //but it will be fucked and crash Maya.
  if( !MObjectHandle( s_EntityNodeGroup ).isValid() )
  {
    s_EntityNodeGroup = nodeFn.create( EntityGroupNode::s_TypeID, "EntityNodeGroup" );
    nodeFn.setDoNotWrite( true );
  }

  nodeFn.addChild( node );

  MAYA_FINISH_EXCEPTION_HANDLING();
}

void EntityNode::NodeRemovedCallback( MObject& node, void* clientData )
{
  MAYA_START_EXCEPTION_HANDLING();

  if( !EntityAssetNode::s_DoRemoveNodeCallback )
  {
    return;
  }

  MFnDependencyNode nodeFn( node );

  EntityAssetNode::RemoveInstance( (EntityNode*)(nodeFn.userNode()) );

  MAYA_FINISH_EXCEPTION_HANDLING();
}

void EntityNode::AddAttributeChangedCallback()
{
  if( m_AttributeChangedCB == -1 )
  {
    m_AttributeChangedCB = MNodeMessage::addAttributeChangedCallback( thisMObject(), AttributeChangedCallback );
  } 
}

void EntityNode::RemoveAttributeChangedCallback()
{
  if( m_AttributeChangedCB != -1 )
  {
    MNodeMessage::removeCallback( m_AttributeChangedCB );
    m_AttributeChangedCB = -1;
  }
}

void EntityNode::AttributeChangedCallback( MNodeMessage::AttributeMessage msg, MPlug& plug, MPlug& otherPlug, void* clientData )
{
  MAYA_START_EXCEPTION_HANDLING();

  // if we don't have a valid ConstructionHost interface, don't bother
  if( !g_ConstructionHost )
  {
    return;
  }

  if( !( msg & MNodeMessage::kAttributeSet ) )
  {
    return;
  }

  MObject attribute = plug.attribute();

  if ( attribute == MPxTransform::translate  || attribute == MPxTransform::translateX   || attribute == MPxTransform::translateY  || attribute == MPxTransform::translateZ ||
    attribute == MPxTransform::scale      || attribute == MPxTransform::scaleX       || attribute == MPxTransform::scaleY      || attribute == MPxTransform::scaleZ ||
    attribute == MPxTransform::rotate     || attribute == MPxTransform::rotateX      || attribute == MPxTransform::rotateY     || attribute == MPxTransform::rotateZ )
  {
    MFnDependencyNode nodeFn( plug.node() );
    EntityNode* entityNode = static_cast<EntityNode*>( nodeFn.userNode() );
    entityNode->UpdateBackingEntity();

    RPC::TransformInstanceParam param;
    param.m_ID = entityNode->GetBackingEntity()->m_ID;
    param.m_Transform = *reinterpret_cast< RPC::Matrix4* >( &entityNode->GetBackingEntity()->m_GlobalTransform );

    g_ConstructionHost->TransformInstance( &param );
  }

  MAYA_FINISH_EXCEPTION_HANDLING();
}

void EntityNode::AfterDuplicateCallback( void* clientData )
{
  MAYA_START_EXCEPTION_HANDLING();

  V_EntityNodePtr::iterator itor = s_ShowNodes.begin();
  V_EntityNodePtr::iterator end  = s_ShowNodes.end();
  for( ; itor != end; ++itor )
  {
    (*itor)->Show();
  }

  itor = s_DupeNodes.begin();
  end  = s_DupeNodes.end();
  for( ; itor != end; ++itor )
  {
    (*itor)->Show();
    (*itor)->AddAttributeChangedCallback();

    if( Construction::Connected() )
    {
      (*itor)->UpdateBackingEntity();

      CreateInstanceParam param;
      {
        MFnDependencyNode nodeFn( (*itor)->thisMObject() );
        tuid id = (*itor)->m_Entity->GetEntityAssetID();
        param.m_ID = (*itor)->m_Entity->m_ID;
        param.m_EntityAsset = id;
        strncpy( param.m_Name.Characters, nodeFn.name().asChar(), RPC_STRING_MAX);
        param.m_Name.Characters[ RPC_STRING_MAX-1] = 0; 
        memcpy( &param.m_Transform, &(*itor)->m_Entity->m_GlobalTransform, sizeof(param.m_Transform) );      
      }      
      Construction::g_ConstructionHost->CreateInstance( &param );
    }
  }

  s_ShowNodes.clear();
  s_DupeNodes.clear();

  MAYA_FINISH_EXCEPTION_HANDLING();
}

static MObject EntityNodeParent( MDagPath& path )
{
  if( path.hasFn( MFn::kDagNode ) )
  {
    MDagPath parentPath;
    MFnDependencyNode nodeFn;

    while( path.pop( 1 ) != MS::kInvalidParameter )
    {        
      nodeFn.setObject( path.node() );
      if( nodeFn.typeId() == EntityNode::s_TypeID )    
      {
        return nodeFn.object();
      }
    }
  }

  return MObject::kNullObj;
}

void EntityNode::SelectionChangedCallback( void* clientData )
{
  MAYA_START_EXCEPTION_HANDLING();

  if( !s_ReplaceSelection )
  {
    return;
  }

  static bool inFunc = false;

  if( inFunc )
    return;
  else
    inFunc = true;

  MSelectionList list;
  MGlobal::getActiveSelectionList( list );

  MStatus stat;

  MSelectionList addList;
  bool added = false;
  u32 len = list.length();
  for( u32 i = 0; i < len; )
  {
    MDagPath path;
    list.getDagPath( i, path );

    MObject entityNode = EntityNodeParent( path );
    if( entityNode != MObject::kNullObj )
    {
      added = true;
      MFnDagNode nodeFn( entityNode );
      MDagPath instancePath;
      nodeFn.getPath( instancePath );
      addList.add( instancePath );
      list.remove( i );
    }
    else
    {
      ++i;
    }
  }

  if( added )
  {
    MGlobal::setActiveSelectionList( list, MGlobal::kReplaceList );
    MGlobal::setActiveSelectionList( addList, MGlobal::kAddToList );
  }

  inFunc = false;

  MAYA_FINISH_EXCEPTION_HANDLING();
}

void EntityNode::AllDagChangesCallback( MDagMessage::DagMessage dagMsg, MDagPath& child, MDagPath &parent, void* clientData )
{
  MAYA_START_EXCEPTION_HANDLING();

  MString message;
  switch( dagMsg )
  {
  case MDagMessage::kParentAdded:
    message += "kParentAdded --";
    break;
  case MDagMessage::kParentRemoved:
    message += "kParentRemoved --";
    break;
  case MDagMessage::kChildAdded:
    message += "kChildAdded --";
    break;
  case MDagMessage::kChildRemoved:
    message += "kChildRemoved --";
    break;
  case MDagMessage::kChildReordered:
    message += "kChildReordered --";
    break;
  case MDagMessage::kInstanceRemoved:
    message += "kInstanceRemoved --";
    break;
  default:
    message += "Some crap --";
  }

  message += " parent: ";
  message += parent.fullPathName();
  message += " child: ";
  message += child.fullPathName();
  MGlobal::displayInfo( message );

  MAYA_FINISH_EXCEPTION_HANDLING();
}
