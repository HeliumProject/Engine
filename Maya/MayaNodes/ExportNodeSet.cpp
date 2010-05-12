#include "stdafx.h"
#include "ExportNodeSet.h"

#include "MayaUtils/ErrorHelpers.h"
#include "MayaUtils/NodeTypes.h"

using namespace Maya;


// statics
MTypeId ExportNodeSet::s_TypeID( IGL_EXPORTNODESET_ID );
MString ExportNodeSet::s_TypeName( "igExportNodeSet" );
S_MObject ExportNodeSet::ExportNodeSets;
MObject ExportNodeSet::SceneNodeSet;

// attributes 
//MObject ExportNodeSet::assetType;
//MObject ExportNodeSet::classId;


ExportNodeSet::ExportNodeSet(){}

ExportNodeSet::~ExportNodeSet(){}

void* ExportNodeSet::Creator()
{
  return new ExportNodeSet();
}

MStatus ExportNodeSet::Initialize()
{
  MStatus stat;

  MFnEnumAttribute	eAttr;

  // assetType attribute
  /*
  assetType = eAttr.create("assetType", "assetType", BatchSet::kNullSet, &stat);
  MCheckErr(stat, "Unable to create attr: assetType");
  for( unsigned i = 0; i < BatchSet::kNumSetTypes; ++i )
  {
  stat = eAttr.addField( BatchSet::SetTypeString[i], i );
  }
  eAttr.setReadable( true );
  eAttr.setWritable( true );
  eAttr.setKeyable( false );
  eAttr.setHidden( false );
  stat = addAttribute( assetType );
  MCheckErr(stat, "Unable to add attr: assetType");
  */
  // parentName attribute
  /*
  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // Create an int attribute "classId"
  MFnNumericAttribute nAttr;
  classId = nAttr.create("contentNumber", "contentNumber", MFnNumericData::kLong, -1, &stat);
  MCheckErr(stat, "Unable to do: nAttr.create");

  nAttr.setKeyable(true)
  nAttr.setReadable(true);
  nAttr.setWritable(false);

  // Add "refType" to the ref Trans
  stat = addAttribute(contentType);
  MCheckErr(stat, "Unable to do: refTransNode.addAttribute");
  */

  return MS::kSuccess;
}


void ExportNodeSet::GetExportNodes( MObjectArray &exportNodes, Content::ContentType nodeType, int num )
{
  MStatus stat;

  MFnSet setFn( thisMObject() );
  MSelectionList members;

  setFn.getMembers( members, true );

  MObject obj;
  MFnDependencyNode nodeFn;
  u32 len = members.length();
  for( u32 i = 0; i < len; ++i )
  {
    members.getDependNode( i, obj );
    nodeFn.setObject( obj );

    if( nodeFn.typeId() == ExportNode::s_TypeID )
    {
      if( nodeType != Content::ContentTypes::Null )
      {
        int tmp;
        MPlug plug( obj, ExportNode::s_attr_contentType );
        plug.getValue( tmp );
        if( static_cast<Content::ContentType>( tmp ) == nodeType )
        {
          if( num != -1)
          {
            int contentNum;
            plug.setAttribute( ExportNode::s_attr_contentNumber );
            plug.getValue( contentNum );
            if( num == contentNum )
            {
              exportNodes.append( obj );
            }
          }
          else
          {
            exportNodes.append( obj );
          }

        }
      }
      else
      {
        exportNodes.append( obj );
      }

    }
  }   
}

MObject ExportNodeSet::GetSceneExportNodeSet( bool createIfNoExist )
{
  // see if there is a single non-referenced ExportNodeSet in the scene
  // if there isn't, create one, if there is more than one, abort!
  MObject theSet;
  MFnDependencyNode nodeFn;

  Maya::S_MObject sets;
  Maya::findNodesOfType( sets, ExportNodeSet::s_TypeID, MFn::kSet );

  // if there isn't currently an ExportNodeSet in the scene, create one
  if( sets.empty() )
  {
    if( createIfNoExist )
      theSet = nodeFn.create( ExportNodeSet::s_TypeID, "igExportNodes" );
  }
  // otherwise, see if there is more than one, prefer non-referenced sets to referenced ones
  else
  {
    u32 numNonReferenced = 0;
    Maya::S_MObject::iterator itor = sets.begin();
    for( ; itor != sets.end(); ++itor )
    {
      nodeFn.setObject( *itor );
      if( !nodeFn.isFromReferencedFile() )
      {
        ++numNonReferenced;
        theSet = *itor;
      }
    }  

    if ( numNonReferenced == 0 )
    {
      theSet = *sets.begin();
    }

  }

  return theSet;
}

MStatus ExportNodeSet::GetSceneExportNodes( MObjectArray &exportNodes, 
                                           Content::ContentType nodeType /* = Content::ContentTypes::Null  */, 
                                           int num /* = -1  */)
{
  MObject sceneSet = GetSceneExportNodeSet();

  if( sceneSet == MObject::kNullObj )
    MCheckErr( MS::kFailure, "Current scene is unable to create a valid Export Node Set!\n" );

  return GetExportNodes( sceneSet,  exportNodes, nodeType, num );
}

MStatus ExportNodeSet::GetExportNodes( const MObject &sceneSet, 
                                      MObjectArray &exportNodes, 
                                      Content::ContentType nodeType /* = Content::ContentTypes::Null  */, 
                                      int num /* = -1  */)
{
  MStatus stat;

  MFnDependencyNode nodeFn( sceneSet, &stat );
  MCheckErr( stat, "Specified MObject is invalid!\n" );

  ExportNodeSet* exportSet = static_cast< ExportNodeSet* >(nodeFn.userNode( &stat ) );
  MCheckErr( stat, "Specified MObject is not an ExportNodeSet!\n" );

  exportSet->GetExportNodes( exportNodes, nodeType, num );

  return stat;
}

void ExportNodeSet::PreImportCB( void* clientData )
{
  ExportNodeSets.clear();

  // there aren't any ExportNodeSets in the scene to cause clashes with...so just return
  SceneNodeSet = ExportNodeSet::GetSceneExportNodeSet();
  if( SceneNodeSet == MObject::kNullObj ) 
    return;

  // fill out the set of existing ExportNodeSets
  findNodesOfType( ExportNodeSets, ExportNodeSet::s_TypeID, MFn::kSet );        
}

void ExportNodeSet::PostImportCB( void* clientData )
{
  // see if any ExportNodeSets were imported, and if so, which one(s)
  S_MObject tmpSet;
  findNodesOfType( tmpSet, ExportNodeSet::s_TypeID, MFn::kSet );

  MSelectionList list;
  MGlobal::getSelectionListByName( "igExportNodes", list);

  // if there's nothing new, we are done
  if( tmpSet.size() == ExportNodeSets.size() )
    return;

  MObjectArray newSets;

  S_MObject::iterator itor = tmpSet.begin();
  S_MObject::iterator end = tmpSet.end();
  for( ; itor != end; ++itor )
  {
    S_MObject::iterator findItor = ExportNodeSets.find( *itor );
    if(  findItor == ExportNodeSets.end() )
    {
      // if we don't find it in the pre-import set, add it as new
      newSets.append( *itor );
    }
  }

  unsigned numNewSets = newSets.length();
  for ( unsigned i = 0; i < numNewSets; ++i )
  {
    // delete the imported ExportNodeSet
    MGlobal::deleteNode( newSets[i] );
  }

  /*
  // get all the ExportNodes out of the new ExportNodeSets, and put them into the scene's ExportNodeSet
  unsigned numNewSets = newSets.length();
  for ( unsigned i = 0; i < numNewSets; ++i )
  {
  newSetFn.setObject( newSets[i] );

  MSelectionList members;
  newSetFn.getMembers( members, true );

  // remove the new ExportNodes from the imported ExportNodeSet
  newSetFn.removeMembers( members );
  // delete the imported ExportNodeSet
  MGlobal::deleteNode( newSets[i] );
  }
  */
}


