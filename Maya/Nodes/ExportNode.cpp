#include "Precompile.h"

#include "ExportNode.h"
#include "ExportNodeSet.h"

#include "Pipeline/Content/Nodes/Descriptor.h"
#include "Pipeline/Content/Animation/AnimationClip.h"
#include "Maya/ErrorHelpers.h"
#include "Maya/Export.h"
#include "Maya/NodeTypes.h"
#include "Maya/Utils.h"
#include "Foundation/Reflect/Registry.h"

#include <algorithm>
#include "Foundation/Boost/Regex.h"
#include <string>


MString ExportNode::s_KeyPoseAttrName              = "KeyPose";
MString ExportNode::s_DefaultDrawAttrName          = "DefaultDraw";
MString ExportNode::s_CheapChunkAttrName           = "CheapChunk";
MString ExportNode::s_GeomSimAttrName              = "GeometrySimulation";

MString ExportNode::s_UseWrinkleMapAttrName        = "UseWrinkleMapAttributes";
MString ExportNode::s_WrinkleMapRegionAttrNames[Content::MaxCountWrinkleMapRegions] = 
{ 
  "WrinkleMap1R",
  "WrinkleMap1G",
  "WrinkleMap1B",
  "WrinkleMap1A",
  "WrinkleMap2R",
  "WrinkleMap2G",
  "WrinkleMap2B",
  "WrinkleMap2A",
};

MObject  ExportNode::s_PrevContentTypeAttr;
MObject  ExportNode::s_ContentTypeAttr;
MObject  ExportNode::s_ContentNumberAttr;

const MTypeId ExportNode::s_TypeID(IGL_EXPORTNODE_ID);
const char* ExportNode::s_TypeName = "igExportNode";

ExportNode::ExportNode()
{
  m_AttributeChangedCBId = 0;
}

ExportNode::~ExportNode()
{
  removeAttributeChangedCB();
}

void * ExportNode::Creator()
{
  return new ExportNode();
}

void ExportNode::removeAttributeChangedCB()
{
  // De-register the guy's callback
  if(m_AttributeChangedCBId != 0)
  {
    MNodeMessage::removeCallback(m_AttributeChangedCBId);
    m_AttributeChangedCBId = 0;
  }
}

void ExportNode::addAttributeChangedCB()
{
  MStatus stat;
  // Add node callback to take action when attributes change
  if(m_AttributeChangedCBId == 0)
  {
    m_AttributeChangedCBId = MNodeMessage::addAttributeChangedCallback( thisMObject(), ExportNode::AttributeChangedCB, NULL, &stat);
    MCheckNoErr(stat, "Unable to add callback cbID1");
  }
}


///////////////////////////////////////////////////////////////////////////////
void AddVisibleBoolAttribute( MObject& object, MString& attributeName, bool defaultValue, bool keyable, MStatus& status )
{
  MFnDependencyNode nodeFn( object );

  MObject attr;
  MFnNumericAttribute numAttr;

  if ( nodeFn.hasAttribute( attributeName, &status ) )
  {
    attr = nodeFn.attribute( attributeName, &status );
  }
  else
  {
    attr = numAttr.create( attributeName, attributeName, MFnNumericData::kBoolean, defaultValue, &status );
    nodeFn.addAttribute( attr );
  }

  if ( status )
  {
    status = numAttr.setObject( attr );
    status = numAttr.setReadable( true );
    status = numAttr.setWritable( true );
    status = numAttr.setHidden( false );
    status = numAttr.setKeyable( keyable );
    status = numAttr.setChannelBox( keyable );

    MPlug attrPlug( object, attr );

    status = attrPlug.setLocked( false );
    status = attrPlug.setValue( defaultValue );
    status = attrPlug.setKeyable( keyable );
    status = attrPlug.setChannelBox( keyable );
  }
}


///////////////////////////////////////////////////////////////////////////////
// Create the enum attribute "contentType"
//
void AddGeometrySimulationAttribute( MObject& object, MStatus& status )
{
  MFnDependencyNode nodeFn( object );

  MObject attr;
  MFnEnumAttribute	enumAttr;

  if ( nodeFn.hasAttribute( ExportNode::s_GeomSimAttrName, &status ) )
  {
    attr = nodeFn.attribute( ExportNode::s_GeomSimAttrName, &status );
  }
  else
  {
    attr = enumAttr.create( ExportNode::s_GeomSimAttrName, ExportNode::s_GeomSimAttrName, Content::GeometrySimulations::Off, &status);
    status = nodeFn.addAttribute( attr );

    for( u32 index = 0; index < (u32)Content::GeometrySimulations::Count && status; ++index )
    {
      status = enumAttr.addField( Content::GeometrySimulationsStrings[index], index );
    }
  }

  if ( status )
  {
    status = enumAttr.setObject( attr );
    status = enumAttr.setReadable( true );
    status = enumAttr.setWritable( true );
    status = enumAttr.setHidden( false );
  }
}


///////////////////////////////////////////////////////////////////////////////
//
void AddRegionAttr( MObject& object, MString& attributeName, MStatus& status )
{
  MFnDependencyNode nodeFn( object );

  MObject attr;
  MFnNumericAttribute numAttr;

  if ( nodeFn.hasAttribute( attributeName, &status ) )
  {
    //attr = nodeFn.attribute( attributeName, &status );
    return;
  }
  else
  {
    attr = numAttr.create( attributeName, attributeName, MFnNumericData::kDouble, 0.0, &status );

    if ( status )
    {
      status = numAttr.setObject( attr );

      status = numAttr.setDefault( 0.0 );
      status = numAttr.setMin( -1.0 );
      status = numAttr.setMax( 1.0 );
      status = numAttr.setReadable( true );
      status = numAttr.setWritable( true );
      status = numAttr.setHidden( false );
      status = numAttr.setKeyable( true );
      //status = numAttr.setChannelBox( true );
      
      status = nodeFn.addAttribute( attr );

      MPlug attrPlug( object, attr );

      status = attrPlug.setLocked( false );
      status = attrPlug.setValue( 0.0 );
      status = attrPlug.setKeyable( true );
      //status = attrPlug.setChannelBox( true );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Adds or removes bangle attributes
//
void ConfigBangleAttributes( MObject& object, MStatus& status, bool add = true )
{
  if ( add )
  {
    AddVisibleBoolAttribute( object, ExportNode::s_DefaultDrawAttrName, true, false, status );
    AddVisibleBoolAttribute( object, ExportNode::s_CheapChunkAttrName, true, false, status );
    AddGeometrySimulationAttribute( object, status );
  }
  else
  {
    Maya::RemoveAttribute( object, ExportNode::s_DefaultDrawAttrName );
    Maya::RemoveAttribute( object, ExportNode::s_CheapChunkAttrName );
    Maya::RemoveAttribute( object, ExportNode::s_GeomSimAttrName );
  }

}


///////////////////////////////////////////////////////////////////////////////
// Adds or removes skeleton attributes
// if the type changed to Skeleton, add the KeyPose
//
void ConfigSkeletonAttributes( MObject& object, MStatus& status, bool add = true )
{
  if ( add )
  {
    AddVisibleBoolAttribute( object, ExportNode::s_KeyPoseAttrName, false, true, status );

    AddVisibleBoolAttribute( object, ExportNode::s_UseWrinkleMapAttrName, false, false, status );
    for ( int i = 0; i < Content::MaxCountWrinkleMapRegions; ++i )
    {
      AddRegionAttr( object, ExportNode::s_WrinkleMapRegionAttrNames[i], status );
    }
  }
  else
  {
    Maya::RemoveAttribute( object, ExportNode::s_KeyPoseAttrName );

    Maya::RemoveAttribute( object, ExportNode::s_UseWrinkleMapAttrName );
    for ( int i = 0; i < Content::MaxCountWrinkleMapRegions; ++i )
    {
      status = Maya::RemoveAttribute( object, ExportNode::s_WrinkleMapRegionAttrNames[i] );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
// Callback handles operations when some base attributes change
//
void ExportNode::AttributeChangedCB(MNodeMessage::AttributeMessage msg, 
                                    MPlug& plug, MPlug& otherPlug, void*) 
{
  MStatus status;

  // If an attribute value changes
  if ( msg & MNodeMessage::kAttributeSet ) 
  {
    MObject nodeThatChanged = plug.node();
    MFnDependencyNode nodeFn( nodeThatChanged );

    if( plug.attribute() == ExportNode::s_ContentTypeAttr )
    {      
      int currentContentType;
      plug.getValue( currentContentType );
      Content::ContentType type = static_cast<Content::ContentType>(currentContentType);

      MPlug prevContentTypePlug = nodeFn.findPlug("prevContentType");    
      int prevContentType;
      prevContentTypePlug.getValue(prevContentType);
      Content::ContentType prevType = static_cast<Content::ContentType>(prevContentType);

      MPlug numPlug( plug.node(), ExportNode::s_ContentNumberAttr );
      numPlug.setLocked( false );

      // add the appropriate attributes depending on the type
      ConfigSkeletonAttributes( nodeThatChanged, status, ( type == Content::ContentTypes::Skeleton ) );
      ConfigBangleAttributes( nodeThatChanged, status, ( type == Content::ContentTypes::Bangle ) );

      // Keep the node type and name synchronized
      tregex  autoNamePattern( TXT("^([a-zA-Z]+)(\\d*)$") );
      tcmatch matches;

      if( boost::regex_match( nodeFn.name().asTChar(), matches, autoNamePattern ) )
      {
        tstring namePortion(matches[1].first, matches[1].second);

        // name is the last content type we had
        if( (prevType == Content::ContentTypes::Null) 
          || (namePortion == Maya::ContentTypesString[prevContentType]) )
        {
          // unselect this node
          MGlobal::unselectByName( nodeFn.name() );
          
          MString updatedName(Maya::ContentTypesString[currentContentType]);
          nodeFn.setName( updatedName );

          // get the new name, may have been updated if already existed ( Geometry -> Geometry1 if Geometry existed )
          updatedName = nodeFn.name();

          // not sure why we need to do this
          // if we dont reselect the item, we cant change its properties
          MGlobal::selectByName(updatedName);
        }
      }

      // next time we come come to this function, this will be the previous node type
      prevContentTypePlug.setValue( currentContentType );
    }
  }
}


///////////////////////////////////////////////////////////////////////////////
// 
void ExportNode::NodeAddedCB( MObject &node, void *clientData )
{
  // only do this if a maya file is not being loaded!
  if( MFileIO::isReadingFile() )
    return;

  // see if there is a valid  ExportNodeSet in the scene
  MObject theSet = ExportNodeSet::GetSceneExportNodeSet( true );
  if( theSet == MObject::kNullObj )
  {
    MGlobal::deleteNode( node );
    return;
  }

  MStatus stat;
  MFnSet setFn( theSet, &stat );

  stat = setFn.addMember( node );
  if( !stat )
  {
    MGlobal::deleteNode( node );
    return;
  }

  MFnDependencyNode nodeFn( node );
  ExportNode* exportNode = static_cast< ExportNode * >( nodeFn.userNode( &stat ) );
  if( !stat )
  {
    MGlobal::deleteNode( node );
    return;
  }
}

void ExportNode::postConstructor()
{
  addAttributeChangedCB();
}


MStatus ExportNode::Initialize()
{
  MStatus stat;

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // Create a short attribute "contentNumber"
  MFnNumericAttribute nAttr;
  s_ContentNumberAttr = nAttr.create("contentNumber", "contentNumber", MFnNumericData::kLong, 0, &stat);
  MCheckErr(stat, "Unable to do: nAttr.create");

  nAttr.setKeyable(true);
  nAttr.setReadable(true);
  nAttr.setWritable(true);

  // Add "refType" to the ref Trans
  stat = addAttribute(s_ContentNumberAttr);
  MCheckErr(stat, "Unable to do: refTransNode.addAttribute");

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // Create an enum attribute "contentType" and "prevContentType
  MFnEnumAttribute	eAttr;
  MFnEnumAttribute  eAttr2;

  s_ContentTypeAttr = eAttr.create("contentType", "contentType", Content::ContentTypes::Default, &stat);
  MCheckErr(stat, "Unable to create attr: contentType");

  s_PrevContentTypeAttr = eAttr2.create("prevContentType", "prevContentType", Content::ContentTypes::Null, &stat);
  MCheckErr(stat, "Unable to create attr: contentType");

  for( u32 i = 0; i < (u32)Content::ContentTypes::NumContentTypes; ++i )
  {
    eAttr.addField( Maya::ContentTypesString[i], i );
    eAttr2.addField( Maya::ContentTypesString[i], i );
  }

  eAttr.setKeyable(true);
  eAttr.setReadable(true);
  eAttr.setWritable(true);

  eAttr2.setKeyable(true);
  eAttr2.setReadable(true);
  eAttr2.setWritable(true);

  stat = addAttribute(s_ContentTypeAttr);
  MCheckErr(stat, "Unable to add attr: contentType");

  stat = addAttribute(s_PrevContentTypeAttr);
  MCheckErr(stat, "Unable to add attr: prevContentType");

  // hide the prevContentType attribute, after it has been added
  eAttr2.setHidden(true);

  return MS::kSuccess;
}

void ExportNode::FindExportNodes( MObjectArray& exportNodes, Content::ContentType exportType, MObject node, int num )
{
  if( node != MObject::kNullObj )
  {
    MItDag dagIter;
    dagIter.reset( node );

    MFnDependencyNode nodeFn;
    while( !dagIter.isDone() )
    {
      nodeFn.setObject( dagIter.currentItem() );
      if( nodeFn.typeId() == ExportNode::s_TypeID )
      {
        if( exportType != Content::ContentTypes::Default )
        {
          MPlug plug( nodeFn.object(), ExportNode::s_ContentTypeAttr );

          int tmp;
          plug.getValue( tmp );
          if( exportType == static_cast<Content::ContentType>(tmp) )
          {
            if( num == -1 )
            {
              exportNodes.append( dagIter.currentItem() );
            }
            else
            {
              plug.setAttribute( ExportNode::s_ContentNumberAttr );
              plug.getValue( tmp );
              if( tmp == num )
                exportNodes.append( dagIter.currentItem() );
            }
          }
        }
        else
        {
          if( num == -1 )
          {
            exportNodes.append( dagIter.currentItem() );
          }
          else
          {
            MPlug plug( nodeFn.object(), ExportNode::s_ContentNumberAttr );
            int tmp;
            plug.getValue( tmp );
            if( tmp == num )
              exportNodes.append( dagIter.currentItem() );
          }
        }

      }
      dagIter.next();
    }
  }
  else
  {
    ExportNodeSet::GetSceneExportNodes( exportNodes, exportType, num );
  }
}

MObject ExportNode::CreateExportNode( Content::ContentType exportType, int num )
{
  MFnDagNode nodeFn;
  MObject exportNode = nodeFn.create( ExportNode::s_TypeID );
  if( exportNode == MObject::kNullObj )
    return MObject::kNullObj;

  MPlug exportPlug = nodeFn.findPlug( "contentType" );
  exportPlug.setValue( (i32)exportType );

  exportPlug = nodeFn.findPlug( "contentNumber" );
  exportPlug.setValue( (i32)num );

  return exportNode;
}

bool ExportNode::HasContentType( MObject& object, Content::ContentType exportType )
{
  MObjectArray exportNodes;
  FindExportNodes( exportNodes, exportType );

  u32 numNodes = exportNodes.length();
  for( u32 i = 0; i < numNodes; ++i )
  {
    MItDag dagIter;
    dagIter.reset( exportNodes[i] );

    MFnDagNode nodeFn;
    while( !dagIter.isDone() )
    {
      if( dagIter.currentItem() == object )
        return true;
      dagIter.next();
    }
  }

  return false;
}