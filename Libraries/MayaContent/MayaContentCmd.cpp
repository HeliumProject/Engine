#include "stdafx.h"

#include "MayaContentCmd.h"

#include "Common/Container/BitArray.h"

#include "RCS/RCS.h"
#include "Debug/Exception.h"
#include "Finder/Finder.h"
#include "Finder/ContentSpecs.h"
#include "FileSystem/FileSystem.h"

#include "Content/ContentVersion.h"
#include "Content/Curve.h"
#include "Content/ContentTypes.h"

#include "MayaUtils/Export.h"
#include <maya/MSyntax.h>
#include <maya/MArgDataBase.h>

#include "MayaNodes/CameraController.h"
#include "MayaNodes/ExportNodeSet.h"
#include "MayaNodes/EntityNode.h"

#include "MayaUtils/Utils.h"

#include "ExportAnimationClip.h"
#include "ExportCollision.h"
#include "ExportGameplay.h"
#include "ExportCurve.h"
#include "ExportDescriptor.h"
#include "ExportEffector.h"
#include "ExportJoint.h"
#include "ExportMesh.h"

#include <sstream>

using namespace Reflect;
using namespace Content;
using namespace Symbol;
using namespace Finder;
using namespace MayaContent;

static const char* ExportSelectionFlag = "-es";
static const char* ExportSelectionFlagLong  = "-exportSelection";

static const char* s_ProxyRigSuffix = "_proxy";
static const char* s_MasterRigSuffix = "_master";
M_string MayaContentCmd::m_UnloadedProxyFileRefNodes;

// #define DEBUG_MAYACONTENTCMD

//---------------------------------------------------------------------------
// MayaContentCmd constructor
//---------------------------------------------------------------------------
MayaContentCmd::MayaContentCmd()
{
  m_Abort = false;
}

//-----------------------------------------------------------------------------
// MayaContentCmd::newSyntax
// definition of the syntax for command
//-----------------------------------------------------------------------------
MSyntax MayaContentCmd::newSyntax()
{
  MSyntax syntax;

  syntax.setObjectType(MSyntax::kSelectionList, 0); // will function without error, but do nothing with no selection
  syntax.useSelectionAsDefault( true );

  syntax.addFlag( ExportSelectionFlag, ExportSelectionFlagLong );

  return syntax;
}

//---------------------------------------------------------------------------
// MayaContentCmd::doIt
//---------------------------------------------------------------------------
MStatus MayaContentCmd::doIt(const MArgList &args)
{
  MStatus status = MS::kSuccess;

  Debug::EnableTranslator<Debug::TranslateException> translator;
  try
  {
    // parse the command line arguments using the declared syntax
    MArgDatabase argParser( syntax(), args, &status );

    if( argParser.isFlagSet( ExportSelectionFlag ) )
    {
      MSelectionList selList;
      MGlobal::getActiveSelectionList( selList );

      MObject object;
      u32 len = selList.length();
      for( u32 i = 0; i < len; ++i )
      {
        selList.getDependNode( i, object );
        m_ObjectsToExport[kSelected].append( object );      
      }
      m_Data = kSelected;

      status = doIt();

      if (status == MS::kSuccess)
      {
        MGlobal::displayInfo("MayaContentCmd completed successfully");
      }
      else
      {
        MGlobal::displayError("MayaContentCmd encountered errors, see script editor");
      }
    }
  }
  catch ( Debug::StructuredException& ex )
  {
    Debug::ProcessException( ex );

    if( MGlobal::mayaState() == MGlobal::kInteractive )
    {
      ::MessageBoxA( M3dView::applicationShell(), ex.what(), "Critical Error", MB_OK );
    }
    else
    {
      std::cerr << ex.what() << std::endl;
    }
  }

  return status;
}

//---------------------------------------------------------------------------
// MayaContentCmd::doIt
//---------------------------------------------------------------------------
MStatus MayaContentCmd::doIt()
{
  EXPORT_SCOPE_TIMER( ("") );

  MStatus status = MS::kSuccess;

  Debug::EnableTranslator<Debug::TranslateException> translator;
  try
  {
    // gather the definitions for the export scene
    if ( DefineExportScene() )
    {
      // go ahead with the export
      ExportSceneData();
      if ( WriteExportedData() )
      {
        // export finished, cleanup the export scene
        m_ExportScene.Reset();
      }
      else
      {
        // failure while writing
        status = MS::kFailure;
      }
    }
    else
    {
      // export scene did not export successfully
      status = MS::kFailure;
    }

    m_QueuedNodes.clear();
  }
  catch ( Debug::StructuredException& ex )
  {
    Debug::ProcessException( ex );

    if( MGlobal::mayaState() == MGlobal::kInteractive )
    {
      ::MessageBoxA( M3dView::applicationShell(), ex.what(), "Critical Error", MB_OK );
    }
    else
    {
      std::cerr << ex.what() << std::endl;
    }
  }

  return status;
}

//---------------------------------------------------------------------------
// MayaContentCmd::DefineExportScene
//---------------------------------------------------------------------------
bool MayaContentCmd::DefineExportScene()
{
  EXPORT_SCOPE_TIMER( ("") );

  bool         success = true;
  MObjectArray joints;

  if (m_Data == MayaContentCmd::kAnimation)
  {
    //
    // Export Animations
    //  loop over the various skeletons that are being exported and export an animation clip for each one
    //

    Maya::V_ExportInfo exportNodes;
    Maya::GetExportInfo( exportNodes, MObject::kNullObj, -1, Content::ContentTypes::Skeleton );

    for each ( const Maya::ExportInfo& exportInfo in exportNodes )
    {
      if ( exportInfo.m_ID == UniqueID::TUID::Null )
        continue;

      ExportAnimationClipPtr exportAnimClip = new ExportAnimationClip( exportInfo.m_ID );
      m_ExportScene.Add( exportAnimClip );
    }

    exportNodes.clear();
  }
  else
  {
    //
    // Setup export list
    //
    MItDag dagIter;
    MObjectArray objects;

    if( m_Data < kCommandDataCount )
    {
      u32 numObjects = m_ObjectsToExport[m_Data].length();
      for( u32 i = 0; i < numObjects; ++i )
      {
        MObject& object = m_ObjectsToExport[m_Data][i];
        dagIter.reset(object);
        Queue( dagIter, objects );
      }
    }

    success = ExportArray(objects);
  }

  return success;
}

//---------------------------------------------------------------------------
// MayaContentCmd::ExportSceneData
//---------------------------------------------------------------------------
void MayaContentCmd::ExportSceneData()
{
  EXPORT_SCOPE_TIMER( ("") );

  m_ExportScene.GatherMayaData();
  ExportAnimationBase::SampleMayaAnimationData();
  m_ExportScene.ProcessMayaData();
  m_ExportScene.ExportData();
}

//---------------------------------------------------------------------------
// MayaContentCmd::WriteExportedData
//---------------------------------------------------------------------------
bool MayaContentCmd::WriteExportedData()
{
  EXPORT_SCOPE_TIMER( ("") );

  bool success = true;

  if( m_Data == kSelected )
  {
    std::string xml;

    bool success;
    try
    {
      Reflect::Archive::ToXML(m_ExportScene.m_Spool, xml);
      success = true;
    }
    catch ( Nocturnal::Exception& ex )
    {
      Console::Error( "%s\n", ex.what() );
      success = false;
    }

    if ( success )
    {
      // null term
      size_t size = xml.size()+1;

      // alloc zeroed global moveable mem
      HGLOBAL mem = GlobalAlloc(GHND, size);
      success = ( mem != NULL );

      if ( success )
      {
        // copy
        char* buf = (char*)GlobalLock(mem);
        {
          strcpy(buf, xml.c_str());
        }
        GlobalUnlock(mem);

        // set cb
        OpenClipboard(M3dView::applicationShell());
        EmptyClipboard();
        SetClipboardData(CF_TEXT, buf);
        CloseClipboard();
      }
    }
  }
  else
  {
    ContentVersionPtr v = new ContentVersion ("Maya (synccmd)", MGlobal::mayaVersion().asChar());

    RCS::File rcsFile( m_ContentFileName );
    rcsFile.GetInfo();

    if ( rcsFile.IsCheckedOutBySomeoneElse() )
    {
      throw Nocturnal::Exception( "One (or more) of this asset's export files is checked out and locked by another user. %s", rcsFile.m_LocalPath.c_str() );
    }

    rcsFile.Open( RCS::OpenFlags::Exclusive );

    try
    {
      Reflect::Archive::ToFile(m_ExportScene.m_Spool, m_ContentFileName, v);
    }
    catch ( Nocturnal::Exception& ex )
    {
      MString str ("Unable to save content file to: ");
      str += m_ContentFileName.c_str();
      str += ": ";
      str += ex.what();
      MGlobal::displayError( str );
    }
  }

  return success;
}

//---------------------------------------------------------------------------
// MayaContentCmd::ExportArray
//---------------------------------------------------------------------------
bool MayaContentCmd::ExportArray(MObjectArray objects)
{
  bool success = true;


  //
  // Export queued nodes
  //

  unsigned int numObjects = objects.length();
  for ( unsigned int i = 0; i < numObjects; ++i )
  {
    int percent = (int)((float)i/(float)objects.length()*100.0f);

    if ( !ExportObject( objects[i], percent ) )
    {
      success = false;
    }
  }

  return success;
}

//---------------------------------------------------------------------------
// MayaContentCmd::ExportObject
//---------------------------------------------------------------------------
bool MayaContentCmd::ExportObject(MObject object, int percent)
{
  ElementPtr e = NULL;
  MFnDependencyNode nodeFn (object);

  //
  // Check for exclusion
  //

  MStatus status;
  MDagPath path = MDagPath::getAPathTo( object, &status );
  if ( status != MS::kSuccess )
  {
    return true;
  }

  if ( Maya::IsExcluded( path.node() ) )
  {
    return true;
  }

  // filter out objects not under export nodes
  // but only if we're not in selected mode (so exporting things like selected curves works)
  if( m_Data != kSelected )
  {
    if ( nodeFn.typeId().id() != IGL_EXPORTNODE_ID && !Maya::DescendantOf( object, MTypeId( IGL_EXPORTNODE_ID ), true ) )
    {
      return true;
    }
  }

  //
  // API Types
  //

  MFn::Type objType = object.apiType();

  switch ( objType )
  {
  case MFn::kPluginTransformNode:
    {
      switch( nodeFn.typeId().id() )
      {
      case IGL_EXPORTNODE_ID:
        {
#ifdef DEBUG_MAYACONTENTCMD
          std::ostringstream str;
          str << "Found export node " << MFnDagNode( object ).fullPathName().asChar();
          MGlobal::displayInfo( str.str().c_str() );
#endif

          m_ExportScene.Add( new ExportDescriptor( object, Maya::GetNodeID( object ) ) );
          return true;
        }

      case IGL_ENTITYNODE_ID:
        {
          // this is a special case, because EntityNodes already have a valid Content object. grab it and add it directly to the content scene
          EntityNode* entityNode = (EntityNode*)(nodeFn.userNode());

          if ( entityNode )
          {
            Asset::Entity* backingEntity = entityNode->GetBackingEntity();
            if ( backingEntity )
            {
              Asset::EntityPtr entity = Reflect::ObjectCast<Asset::Entity>( backingEntity->Clone() );

              entity->PreSerialize();

              m_ExportScene.m_ContentScene.Add( entity );
            }
          }
          return true;
        }
      }
      break;
    }

  case MFn::kTransform:
    {
      m_ExportScene.Add( new ExportPivotTransform( object, Maya::GetNodeID( object ) ) );
      return true;
    }

  case MFn::kJoint:
    {
      MFnIkJoint jointFn ( object );

      m_ExportScene.Add( new ExportJoint( object, Maya::GetNodeID( object ) ) );

      return true;
    }

  case MFn::kMesh:
    {
      ExportMeshPtr exportMesh = new ExportMesh( object, Maya::GetNodeID( object ) );

      m_ExportScene.Add( exportMesh );

      if (m_Data == MayaContentCmd::kRiggedMesh || m_Data == MayaContentCmd::kCollision )
      {
        exportMesh->m_ContentSkin = new Content::Skin();

        m_ExportScene.Add( new ExportBase( exportMesh->m_ContentSkin ) );
      }

      return true;
    }

  case MFn::kNurbsCurve:
    {
      m_ExportScene.Add( new ExportCurve ( object, Maya::GetNodeID( object ) ) );

      return true;
    }

  case MFn::kPluginLocatorNode:
    {
      switch( nodeFn.typeId().id() )
      {
      case IGL_GP_SPHERE:                 
      case IGL_GP_CAPSULE:                
      case IGL_GP_CYLINDER:               
      case IGL_GP_CUBOID:
        m_ExportScene.Add( new ExportGameplay( object, Maya::GetNodeID( object ) ) );
        return true;

      case IGL_COLL_SPHERE:                 
      case IGL_COLL_CAPSULE:                
      case IGL_COLL_CAPSULE_CHILD:   
      case IGL_COLL_CYLINDER:               
      case IGL_COLL_CYLINDER_CHILD:
      case IGL_COLL_CUBOID:
      case IGL_LOOSE_ATTACHMENT_SPHERE:
      case IGL_LOOSE_ATTACHMENT_CAPSULE: 
      case IGL_LOOSE_ATTACHMENT_CAPSULE_CHILD: 
        m_ExportScene.Add( new ExportCollision( object, Maya::GetNodeID( object ) ) );
        return true;

      case IGL_JOINT_EFFECTOR:
        m_ExportScene.Add( new ExportEffector( object, Maya::GetNodeID( object ) ) );
        return true;

      }
      break;
    }
  }


  return true;
}

//---------------------------------------------------------------------------
// MayaContentCmd::Queue
//---------------------------------------------------------------------------
void MayaContentCmd::Queue(MItDag& dagIter, MObjectArray& objects)
{
  //
  // Queue
  //

#ifdef DEBUG_MAYACONTENTCMD
  std::ostringstream str;
  str << std::endl << "Queuing relatives of " << dagIter.fullPathName().asChar();
  MGlobal::displayInfo( str.str().c_str() );
#endif

  for (; !dagIter.isDone(); dagIter.next())
  {
    MFnDagNode nodeFn (dagIter.item());

    if (nodeFn.isIntermediateObject())
    {
      continue;
    }

    QueueNode(dagIter.item(), objects);
  }
}

//---------------------------------------------------------------------------
// MayaContentCmd::QueueNode
//---------------------------------------------------------------------------
void MayaContentCmd::QueueNode(const MObject node, MObjectArray& objects)
{
  MFnDagNode nodeFn (node);


  //
  // Check ID
  //


  if (Maya::GetNodeID( node ) == UniqueID::TUID::Null)
  {
    MGlobal::displayError("Unable to set UniqueID::TUID attribute on maya node!");
    return;
  }


  //
  // Early out if we are not to be exported or we are already queued
  //

  if ( IsQueued( node ) )
  {
    return;
  }


  //
  // Queue
  //

#ifdef DEBUG_MAYACONTENTCMD
  std::ostringstream str;
  str << " Queuing " << nodeFn.fullPathName().asChar();
  MGlobal::displayInfo( str.str().c_str() );
#endif

  objects.append(node);
  SetQueued(node);


  //
  // Queue Parents
  //

  MFn::Type objType = node.apiType();

  switch (objType)
  {
  case MFn::kPluginLocatorNode:
    {
      // instances export thier parent transform as themself, so we can skip it
      if (!QueueParents(nodeFn.parent(0), objects))
      {
        return;
      }

      break;
    }

  default:
    {
      // export our parent transform nodes (group nodes)
      if (!QueueParents(node, objects))
      {
        return;
      }

      break;
    }
  }
}

//---------------------------------------------------------------------------
// MayaContentCmd::QueueParents
//---------------------------------------------------------------------------
bool MayaContentCmd::QueueParents(const MObject node, MObjectArray& objects)
{
  //
  // Queue all nodes up the hierarchy
  //

  MObject parent (MFnDagNode(node).parent(0));

  while (parent != MObject::kNullObj)
  {
    MFnDagNode parentFn (parent);

    //
    // Check for ID
    //


    if ( Maya::GetNodeID( parent ) == UniqueID::TUID::Null)
    {
      MGlobal::displayError("Unable to set UniqueID::TUID attribute on maya node!");
      return false;
    }


    //
    // Queue
    //

    MDagPath path;
    parentFn.getPath(path);

    if (!IsQueued(parent) && path.fullPathName().length() > 0)
    {
      objects.append(parent);
      SetQueued(parent);
    }

    parent = parentFn.parent(0);
  }

  return true;
}

//---------------------------------------------------------------------------
// MayaContentCmd::IsQueued
//---------------------------------------------------------------------------
bool MayaContentCmd::IsQueued(const MObject node)
{
  // insert it into the map of exported ids
  Maya::S_MObject::iterator i = m_QueuedNodes.find(node);

  return i != m_QueuedNodes.end();
}

//---------------------------------------------------------------------------
// MayaContentCmd::SetQueued
//---------------------------------------------------------------------------
void MayaContentCmd::SetQueued(const MObject node)
{
  // insert it into the map of exported ids
  bool result = m_QueuedNodes.insert(node).second;

  NOC_ASSERT(result);
}

// assumes a 1-to-1 mapping
static const DecorationSpec& ExportTypeToFileSpec( MayaContentCmd::CommandData type )
{
  switch( type )
  {
  case MayaContentCmd::kAnimation:
    return FinderSpecs::Content::ANIMATION_DECORATION;
  case MayaContentCmd::kRiggedMesh:
    return FinderSpecs::Content::RIGGED_DECORATION;
  case MayaContentCmd::kStaticMesh:
    return FinderSpecs::Content::STATIC_DECORATION;
  case MayaContentCmd::kCollision:
    return FinderSpecs::Content::COLLISION_DECORATION;
  case MayaContentCmd::kGlue:
    return FinderSpecs::Content::DESTRUCTION_GLUE_DECORATION;
  case MayaContentCmd::kPathfinding:
    return FinderSpecs::Content::PATHFINDING_DECORATION;
  case MayaContentCmd::kCineScene:
    return FinderSpecs::Content::CINESCENE_DECORATION;
  default:
    {
      NOC_BREAK();
      return FinderSpecs::Content::STATIC_DECORATION;
    }
  }
}

void MayaContentCmd::DetermineExportedTypes( BitArray& types )
{
  EXPORT_SCOPE_TIMER( ("") );
  // check for anims
#if 0
  int result = 0;
  MGlobal::executeCommand( "hasTimeAnimCurves", result, false, false );

  if ( result )
  {
    types[MayaContentCmd::kAnimation] = true;

    // also consider cinescene stuff
    MObjectArray cameraControllers;
    Maya::findNodesOfType( cameraControllers, IGL_CAMERA_CONTROLLER_ID );
    if( cameraControllers.length() )
      types[MayaContentCmd::kCineScene] = true;
  }

  if( !types[MayaContentCmd::kAnimation] )
#else
  types[MayaContentCmd::kAnimation] = true;

  // also consider cinescene stuff

  Maya::findNodesOfType( m_ObjectsToExport[kCineScene], IGL_CAMERA_CONTROLLER_ID, MFn::kInvalid, m_Root );

  if( m_ObjectsToExport[kCineScene].length() )
  {
    ExportNode::FindExportNodes( m_ObjectsToExport[kCineScene], Content::ContentTypes::MonitorCam, m_Root, 0 );
    Maya::findNodesOfType( m_ObjectsToExport[kCineScene], MFn::kSpotLight, m_Root );

    types[kCineScene] = true;
  }

  // find the other types and junk if it doesn't have animation
#endif
  {

    MFnDagNode nodeFn;

    //
    //Geometry
    //

    ExportNode::FindExportNodes( m_ObjectsToExport[kStaticMesh], Content::ContentTypes::Geometry, m_Root );
    ExportNode::FindExportNodes( m_ObjectsToExport[kStaticMesh], Content::ContentTypes::Bangle, m_Root );
    ExportNode::FindExportNodes( m_ObjectsToExport[kStaticMesh], Content::ContentTypes::Water, m_Root );
    ExportNode::FindExportNodes( m_ObjectsToExport[kStaticMesh], Content::ContentTypes::RisingWater, m_Root );
    Maya::findNodesOfType( m_ObjectsToExport[kStaticMesh], IGL_WATER_PLANE, MFn::kDagNode, m_Root );

    u32 numObjects = m_ObjectsToExport[kStaticMesh].length();
    for( u32 i = 0; i < numObjects; ++i )
    {
      nodeFn.setObject( m_ObjectsToExport[kStaticMesh][i] );
      if( nodeFn.childCount() )
      {
        types[kStaticMesh] = true;
        break;
      }     
    }

    //
    // Skeleton
    //

    ExportNode::FindExportNodes( m_ObjectsToExport[kRiggedMesh], Content::ContentTypes::Skeleton, m_Root );

    numObjects = m_ObjectsToExport[kRiggedMesh].length();
    for( u32 i = 0; i < numObjects; ++i )
    {
      nodeFn.setObject( m_ObjectsToExport[kRiggedMesh][i] );
      if( nodeFn.childCount() )
      {
        Maya::appendObjectArray(m_ObjectsToExport[kRiggedMesh], m_ObjectsToExport[kStaticMesh] );
        types[kRiggedMesh] = true;
        break;
      }     
    }


    //
    // Collision
    //

    MObjectArray resColl;
    ExportNode::FindExportNodes( resColl, Content::ContentTypes::HighResCollision, m_Root );
    ExportNode::FindExportNodes( resColl, Content::ContentTypes::LowResCollision, m_Root );

    numObjects = resColl.length();
    if( resColl.length() != 0 )
    {
      for( u32 objIndex = 0; objIndex < numObjects; ++objIndex )
      {
        nodeFn.setObject( resColl[objIndex] );
        if( nodeFn.childCount() )
        {
          types[kCollision] = true;
          Maya::appendObjectArray(m_ObjectsToExport[kCollision], resColl );
          break;
        }
      }     
    }

    MObjectArray collPrims;
    Maya::findNodesOfType( collPrims, IGL_GP_CAPSULE, MFn::kDagNode, m_Root );
    Maya::findNodesOfType( collPrims, IGL_GP_SPHERE, MFn::kDagNode, m_Root );
    Maya::findNodesOfType( collPrims, IGL_GP_CUBOID, MFn::kDagNode, m_Root );
    Maya::findNodesOfType( collPrims, IGL_GP_CYLINDER, MFn::kDagNode, m_Root);
    Maya::findNodesOfType( collPrims, IGL_COLL_CAPSULE, MFn::kDagNode, m_Root );
    Maya::findNodesOfType( collPrims, IGL_COLL_SPHERE, MFn::kDagNode, m_Root );
    Maya::findNodesOfType( collPrims, IGL_COLL_CAPSULE_CHILD, MFn::kDagNode, m_Root );
    Maya::findNodesOfType( collPrims, IGL_COLL_CUBOID, MFn::kDagNode, m_Root );
    Maya::findNodesOfType( collPrims, IGL_COLL_CYLINDER, MFn::kDagNode, m_Root);
    Maya::findNodesOfType( collPrims, IGL_COLL_CYLINDER_CHILD, MFn::kDagNode, m_Root);
    Maya::findNodesOfType( collPrims, IGL_LOOSE_ATTACHMENT_SPHERE, MFn::kDagNode, m_Root);
    Maya::findNodesOfType( collPrims, IGL_LOOSE_ATTACHMENT_CAPSULE, MFn::kDagNode, m_Root);
    Maya::findNodesOfType( collPrims, IGL_LOOSE_ATTACHMENT_CAPSULE_CHILD, MFn::kDagNode, m_Root);

    if( collPrims.length() != 0 )
    {
      types[kCollision] = true;
      Maya::appendObjectArray( m_ObjectsToExport[kCollision], collPrims );
    }

    // Destruction Glue
    MObjectArray resGlue;
    ExportNode::FindExportNodes( resGlue, Content::ContentTypes::Glue, m_Root);
    
    numObjects = resGlue.length();

    if(resGlue.length() != 0)
    {
      for( u32 objIndex = 0; objIndex < numObjects; ++objIndex )
      {
        nodeFn.setObject( resGlue[objIndex] );
        if(nodeFn.childCount())
        {
          types[kGlue] = true;
          Maya::appendObjectArray(m_ObjectsToExport[kGlue], resGlue);
          break;
        }
      }
    }

    MObjectArray glueNodes;
    Maya::findNodesOfType( glueNodes, IGL_DESTRUCTION_GLUE, MFn::kDagNode, m_Root );
    if(glueNodes.length() != 0)
    {
      types[kGlue] = true;
      Maya::appendObjectArray( m_ObjectsToExport[kGlue], glueNodes );
    }


    //
    //Pathfinding
    //

    ExportNode::FindExportNodes( m_ObjectsToExport[kPathfinding], Content::ContentTypes::Pathfinding, m_Root );
    ExportNode::FindExportNodes( m_ObjectsToExport[kPathfinding], Content::ContentTypes::LowResPathfinding, m_Root );

    numObjects = m_ObjectsToExport[kPathfinding].length();
    for( u32 i = 0; i < numObjects; ++i )
    {
      nodeFn.setObject( m_ObjectsToExport[kPathfinding][i] );
      if( nodeFn.childCount() )
      {
        types[kPathfinding] = true;
        break;
      }     
    }

    //
    // NavEffectors
    //

    MObjectArray navEffectors;
    Maya::findNodesOfType( navEffectors, IGT_NAV_SPHERE_ID, MFn::kDagNode, m_Root );
    Maya::findNodesOfType( navEffectors, IGT_NAV_CYLINDER_ID, MFn::kDagNode, m_Root );
    Maya::findNodesOfType( navEffectors, IGT_NAV_CUBOID_ID, MFn::kDagNode, m_Root );
    if( navEffectors.length() != 0 )
    {
      types[kRiggedMesh] = true;
      Maya::appendObjectArray( m_ObjectsToExport[kRiggedMesh], navEffectors );
    }


    //
    // NavClues
    //

    MObjectArray navClues;
    Maya::findNodesOfType( navClues, IGL_NAV_CLUE_CUBOID_ID, MFn::kDagNode, m_Root );
    if( navClues.length() != 0 )
    {
      types[kRiggedMesh] = true;
      Maya::appendObjectArray( m_ObjectsToExport[kRiggedMesh], navClues );
    }
  }
}

void MayaContentCmd::ExportCurrentScene( MObject root, std::string& currentFile, std::string& fragmentName )
{
  EXPORT_SCOPE_TIMER( ("") );

  UnloadProxyFileReferences();

  MGlobal::MSelectionMode selectionMode = MGlobal::selectionMode();

  MObjectArray fragmentNodes;
  ExportNode::FindExportNodes( fragmentNodes, Content::ContentTypes::FragmentGroup, root );
  u32 numFragments = fragmentNodes.length();
  if( numFragments && fragmentNodes[0] != root )
  {
    MFnDependencyNode nodeFn;
    for( u32 i = 0; i < numFragments; ++i )
    {
      nodeFn.setObject( fragmentNodes[i] );
      ExportCurrentScene( fragmentNodes[i], currentFile, std::string( nodeFn.name().asChar() ) );
    }
  }

  BitArray usedTypes( MayaContentCmd::kCommandDataCount );
  MayaContentCmd cmd;
  cmd.m_Root = root;

  cmd.DetermineExportedTypes( usedTypes );

  if( currentFile.empty() )
  {
    currentFile = MFileIO::currentFile().asChar();
  }
  // clean path returned by maya 
  FileSystem::CleanName( currentFile );

  std::string contentFileDir = ExportTypeToFileSpec( MayaContentCmd::kStaticMesh ).GetExportFile( currentFile, fragmentName );
  FileSystem::StripLeaf( contentFileDir );

  FileSystem::MakePath( contentFileDir );
  contentFileDir += "...";

  try
  {
    RCS::File rcsContentFile( contentFileDir );
    rcsContentFile.GetInfo();
    if ( rcsContentFile.ExistsInDepot() && !rcsContentFile.IsCheckedOutByMe() && !rcsContentFile.IsUpToDate() )
    {
      rcsContentFile.m_LocalPath += "\\...";
      rcsContentFile.Sync();
    }
  }
  catch( const Nocturnal::Exception& )
  {
    // this is ok, the directory may not exist in perforce yet
  }

  // remove all objects that are under a fragment export node 
  // (except if the fragment export node is the root node)
  for( u32 i = 0; i < MayaContentCmd::kCommandDataCount; ++i )
  {
    //horrible hack
    if( i == kSelected ) continue;

    u32 numObjects = cmd.m_ObjectsToExport[i].length();
    for( u32 k = 0; k < numFragments; ++k )
    {
      if ( fragmentNodes[k] == root ) continue;

      MFnDagNode fragmentFn( fragmentNodes[k] );
      u32 numObjects = cmd.m_ObjectsToExport[i].length();
      for( u32 j = 0; j < numObjects; )
      { 
        if( cmd.m_ObjectsToExport[i][j].hasFn( MFn::kDagNode ) )
        {
          if( fragmentFn.isParentOf( cmd.m_ObjectsToExport[i][j] ) )
          {
            cmd.m_ObjectsToExport[i].remove( j );
            --numObjects;
          }
          else
            ++j;
        }
      }
    }

    CommandData data = static_cast< CommandData >( i );
    std::string contentFile = ExportTypeToFileSpec( data ).GetExportFile( currentFile, fragmentName );

    if( usedTypes[i] )
    {
      cmd.m_Data = data;
      cmd.m_SourceFileName = currentFile;
      cmd.m_ContentFileName = contentFile;
      cmd.m_FragmentName = fragmentName;
      cmd.doIt();
    }
    else if( FileSystem::Exists( contentFile ) )        
    {
      RCS::File rcsFile( contentFile );

      try
      {
        rcsFile.Revert();
      }
      catch ( RCS::Exception& e )
      {
        Console::Warning( "Could not revert '%s': %s\n", contentFile.c_str(), e.what() );
      }

      try
      {
        rcsFile.Delete();             // delete from perforce deletes locally
      }
      catch ( RCS::Exception& e )
      {
        Console::Warning( "Could not delete '%s': %s\n", contentFile.c_str(), e.what() );
      }

      // if file was never checked into perforce (+add), revert/delete
      // will leave the file on disk locally
      if( FileSystem::Exists( contentFile ) )
      {
        FileSystem::Delete( contentFile );
      }
    }
  }

  ReloadProxyFileReferences();

  MGlobal::setSelectionMode( selectionMode );
}

bool MayaContentCmd::ExportSelectionToClipboard()
{
  MayaContentCmd cmd;

  MSelectionList selList;
  MGlobal::getActiveSelectionList( selList );

  MObject object;
  u32 len = selList.length();
  for( u32 i = 0; i < len; ++i )
  {
    selList.getDependNode( i, object );
    cmd.m_ObjectsToExport[kSelected].append( object );      
  }
  cmd.m_Data = kSelected;
  return cmd.doIt() == MS::kSuccess;
}

MStatus MayaContentCmd::UnloadProxyFileReferences()
{
  MStatus status( MStatus::kSuccess );

  m_UnloadedProxyFileRefNodes.clear();

  Maya::S_MObject refNodes;
  Maya::findNodesOfType( refNodes, MFn::kReference );

  Maya::S_MObject::iterator itor = refNodes.begin();
  Maya::S_MObject::iterator end  = refNodes.end();
  for( ; itor != end; ++itor )
  {
    MObject &object = *itor;   
    MFnDependencyNode nodeFn( object );

    // skip sharedReferenceNode
    std::string nodeName( nodeFn.name().asChar() );
    if ( nodeName.find( "sharedReferenceNode" ) != std::string::npos )
    {
      continue;
    }

    // get the current fileName attribute
    MString command( "referenceQuery -filename " );
    command += nodeFn.name();

    MString refFileName;
    status = MGlobal::executeCommand( command, refFileName );
    if ( !status )
    {
      continue;
    }

    // clean path returned by maya 
    std::string curFilePath = refFileName.asChar();
    FileSystem::CleanName( curFilePath );

    std::string::size_type findProxyPos = curFilePath.rfind( s_ProxyRigSuffix );
    if ( findProxyPos != std::string::npos )
    {
      std::string masterFilePath = curFilePath;
      masterFilePath.erase( findProxyPos, strlen( s_ProxyRigSuffix ) );
      masterFilePath.insert( findProxyPos, s_MasterRigSuffix );

      if ( FileSystem::Exists( masterFilePath ) )
      {
        //file -loadReference "yourReferenceNodeHere" -type "mayaBinary" -options "v=0" "pathToNewReferenceFileHere";
        std::stringstream command;
        command << "file -loadReference \"" << nodeFn.name().asChar() << "\"";
        command << " -type \"mayaBinary\" -options \"v=0\"";
        command << " \"" << masterFilePath.c_str() << "\"";

        status = MGlobal::executeCommand( command.str().c_str(), false, false );

        if ( status )
        {
          // add the node to the list to be reset later
          m_UnloadedProxyFileRefNodes.insert( M_string::value_type( nodeFn.name().asChar(), curFilePath ) );
        }
        else
        {
          MGlobal::displayError( MString ( "Unable to unload proxy file for reference node: " ) + nodeFn.name() );
        }
      }
      else
      {
        // warn the user that they are exporting a proxy file that doesn't have a master
      }
    }
  }

  return status;
}

MStatus MayaContentCmd::ReloadProxyFileReferences()
{
  MStatus status( MStatus::kSuccess );

  for each ( const M_string::value_type& proxyNodePair in m_UnloadedProxyFileRefNodes )
  {
    //file -loadReference "yourReferenceNodeHere" -type "mayaBinary" -options "v=0" "pathToNewReferenceFileHere";
    std::stringstream command;
    command << "file -loadReference \"" << proxyNodePair.first << "\"";
    command << " -type \"mayaBinary\" -options \"v=0\"";
    command << " \"" << proxyNodePair.second << "\"";

    status = MGlobal::executeCommand( command.str().c_str(), false, false );

    if ( status == MS::kFailure )
    {
      MGlobal::displayError( MString ( "Unable to reload proxy file for reference node: " ) + proxyNodePair.first.c_str() );
    }
  }

  return status;
}

void MayaContentCmd::ConvertMatrix(const MMatrix& matrix, Math::Matrix4& outMatrix)
{
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
      outMatrix[i][j] = (float)matrix[i][j];
}

void MayaContentCmd::ConvertMatrix(const Math::Matrix4& matrix, MMatrix& outMatrix)
{
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
      outMatrix[i][j] = matrix[i][j];
}

void MayaContentCmd::GetDynamicAttributes( const MObject &node, const Attribute::ComponentCollectionPtr& element )
{
  MFnDependencyNode nodeFn (node);

  for (unsigned int i=0; i<nodeFn.attributeCount(); i++)
  {
    MObject attr (nodeFn.attribute(i));
    MFnAttribute attrFn (attr);

    if( attrFn.isDynamic() )
    {
      ConvertAttributeToComponent( nodeFn, attr, element );
    }
  }
}

void MayaContentCmd::ConvertAttributeToComponent( const MFnDependencyNode &nodeFn, const MFnAttribute &attrFn, const Attribute::ComponentCollectionPtr &element )
{
  MStatus status;

  MObject attr = attrFn.object();
  switch (attr.apiType())
  {
  case MFn::kNumericAttribute:
    {
      MFnNumericAttribute numericAttr (attr);

      switch (numericAttr.unitType())
      {
      case MFnNumericData::kBoolean:
        {
          bool value;
          MPlug plug (nodeFn.findPlug(attr, &status));

          if (status == MS::kSuccess)
          {
            plug.getValue(value);
            element->SetComponent(numericAttr.name().asChar(), Reflect::Serializer::Create<bool>( value ) );
          }

          break;
        }

      case MFnNumericData::kShort:
        {
          short value;
          MPlug plug (nodeFn.findPlug(attr, &status));

          if (status == MS::kSuccess)
          {
            plug.getValue(value);
            element->SetComponent(numericAttr.name().asChar(), Reflect::Serializer::Create<i16>( value ) );
          }

          break;
        }

      case MFnNumericData::kInt:
        {
          int value;
          MPlug plug (nodeFn.findPlug(attr, &status));

          if (status == MS::kSuccess)
          {
            plug.getValue(value);
            element->SetComponent(numericAttr.name().asChar(), Reflect::Serializer::Create<i32>((i32)value));
          }

          break;
        }

      case MFnNumericData::kFloat:
        {
          float value;
          MPlug plug (nodeFn.findPlug(attr, &status));

          if (status == MS::kSuccess)
          {
            plug.getValue(value);
            element->SetComponent(numericAttr.name().asChar(), Reflect::Serializer::Create<f32>((f32)value));
          }

          break;
        }

      case MFnNumericData::kDouble:
        {
          double value;
          MPlug plug (nodeFn.findPlug(attr, &status));

          if (status == MS::kSuccess)
          {
            plug.getValue(value);
            element->SetComponent(numericAttr.name().asChar(), Reflect::Serializer::Create<f32>((f32)value));
          }

          break;
        }
      }

      break;
    }

  case MFn::kTypedAttribute:
    {
      MFnTypedAttribute typedAttr (attr);

      switch (typedAttr.attrType())
      {

      case MFnData::kIntArray:
        {
          V_i32 values;

          MObject plugValues;
          MPlug plug (nodeFn.findPlug(attr, &status));
          plug.getValue( plugValues );

          if ( !plugValues.hasFn( MFn::kIntArrayData ) )
            break;

          MFnIntArrayData valueList( plugValues );

          unsigned int count = valueList.length();

          for ( unsigned int j=0; j < count; ++j )
            values.push_back( valueList[ j ] );

          element->SetComponent(typedAttr.name().asChar(), Reflect::Serializer::Create<V_i32>(values));
          values.clear();

          break;
        }

      case MFnData::kString:
        {
          if (!attrFn.isArray())
          {
            MString value;
            MPlug plug (nodeFn.findPlug(attr, &status));

            if (status == MS::kSuccess)
            {
              plug.getValue(value);
              element->SetComponent(typedAttr.name().asChar(), Reflect::Serializer::Create(std::string (value.asChar())));
            }

            break;
          }
        }

      case MFnData::kStringArray:
        {
          V_string values;

          MString value;
          MPlug plug (nodeFn.findPlug(attr, &status));

          unsigned int count = plug.numElements();

          for (unsigned int j=0; j<count; j++)
          {
            // by PHYSICAL index
            plug.elementByPhysicalIndex(j).getValue(value);
            values.push_back(value.asChar());
          }

          element->SetComponent(typedAttr.name().asChar(), Reflect::Serializer::Create<V_string>(values));
          values.clear();

          break;
        }
      }

      break;
    }

  case MFn::kUnitAttribute:
    {
      // we should catch all of these below, I think UnitAttribute is just a utility class FnSet
      NOC_ASSERT(false);
      break;
    }

  case MFn::kFloatLinearAttribute:
  case MFn::kDoubleLinearAttribute:
    {
      MFnUnitAttribute unitAttr (attr);

      float value;
      MPlug plug (nodeFn.findPlug(attr, &status));

      if (status != MS::kSuccess)
        break;

      plug.getValue(value);

      // CM -> M
      value *= Math::CentimetersToMeters;

      element->SetComponent(unitAttr.name().asChar(), Reflect::Serializer::Create<f32>((f32)value));

      break;
    }

  case MFn::kFloatAngleAttribute:
  case MFn::kDoubleAngleAttribute:
    {
      MFnUnitAttribute unitAttr (attr);

      float value;
      MPlug plug (nodeFn.findPlug(attr, &status));

      if (status != MS::kSuccess)
        break;

      plug.getValue(value);

      // RADIANS -> DEGREES
      value *= Math::RadToDeg;

      element->SetComponent(unitAttr.name().asChar(), Reflect::Serializer::Create<f32>((f32)value));

      break;
    }
  }
}
