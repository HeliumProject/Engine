#include "stdafx.h"

#include "ExportNode.h"
#include "ExportNodeSet.h"

#include "Content/Descriptor.h"
#include "Content/AnimationClip.h"
#include "MayaUtils/ErrorHelpers.h"
#include "MayaUtils/Export.h"
#include "MayaUtils/NodeTypes.h"
#include "MayaUtils/Utils.h"
#include "Reflect/Registry.h"

#include "Symbol/Enum.h"
#include "Symbol/Symbols.h"
#include "Symbol/SymbolBuilder.h"

#include <algorithm>
#include <boost/regex.hpp>
#include <string>


MString ExportNode::s_KeyPoseAttrName              = "KeyPose";
MString ExportNode::s_DefaultDrawAttrName          = "DefaultDraw";
MString ExportNode::s_CheapChunkAttrName           = "CheapChunk";
MString ExportNode::s_GeomSimAttrName              = "GeometrySimulation";

static const char* s_RisingWaterPackageIDName      = "RisingWaterPackageID";
MString ExportNode::s_RisingWaterEnumAttrName      = "RisingWaterEnum";
MString ExportNode::s_RisingWaterNameAttrName      = "RisingWaterName";
MString ExportNode::s_RisingWaterEnumIDAttrName    = "RisingWaterPackageID";

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

MObject  ExportNode::s_attr_prev_contentType;
MObject  ExportNode::s_attr_contentType;
MObject  ExportNode::s_attr_contentNumber;

const MTypeId ExportNode::s_TypeID(IGL_EXPORTNODE_ID);
const char* ExportNode::s_TypeName = "igExportNode";

Symbol::EnumPtr g_RisingWaterPackageID;

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
// Create the enum attribute "contentType"
//
void AddRisingWaterDynamicEnumAttrbs( MObject& object, MStatus& status )
{
  MFnDependencyNode nodeFn( object, &status );

  ////////////////////////////////////////////
  // check to see if we are a locked node
  bool nodeWasLocked = nodeFn.isLocked();
  if ( nodeWasLocked )
  {
    // turn off any node locking so an attribute can be added
    nodeFn.setLocked( false );
  }

  ////////////////////////////////////////////
  // Enum drop down menu attribute

  // set up an enum attribute and populate with packages returned SymbolBuilder
  g_RisingWaterPackageID = Symbol::SymbolBuilder::GetInstance()->FindEnum( s_RisingWaterPackageIDName );
  if ( !g_RisingWaterPackageID.ReferencesObject() )
  {
    status = MS::kFailure;
    return;
  }
  V_string packageNames;
  g_RisingWaterPackageID->GetElemNames( packageNames );
  std::sort( packageNames.begin(), packageNames.end());
  // iterate over the different package names, adding them to the enum
  // with an _arbitrary index_.  Why use an enum attribute if we're going
  // to have the indices mean nothing?  Because maya provides a nicer
  // UI for enum attributes than anything else that might work here.
  V_string::const_iterator itr = packageNames.begin();
  V_string::const_iterator end = packageNames.end();

  MObject attr;
  MFnEnumAttribute	enumAttr;

  if ( nodeFn.hasAttribute( ExportNode::s_RisingWaterEnumAttrName, &status ) )
  {
    attr = nodeFn.attribute( ExportNode::s_RisingWaterEnumAttrName, &status );
  }
  else
  {
    attr = enumAttr.create( ExportNode::s_RisingWaterEnumAttrName, ExportNode::s_RisingWaterEnumAttrName, 0, &status);
    status = nodeFn.addAttribute( attr );

    short index = 0;
    for( ; itr != end && status; ++itr, ++index )
    {
      status = enumAttr.addField( (*itr).c_str(), index );
    }
  }

  if ( status )
  {
    status = enumAttr.setObject( attr );
    status = enumAttr.setReadable( true );
    status = enumAttr.setWritable( true );
    status = enumAttr.setHidden( false );
  }


  ////////////////////////////////////////////
  // Enum package string name and int id attributes

  // create a typed string attribute in which we'll store the _name_ of
  // the chosen fx package when the user selects one from the dropdown.
  // This allows us to link everything back up right when the file is loaded
  // the next time, even if the list of attributes (and hence their indices)
  // have changed underneath us.  (See AttributeUpdatedCallback below for the
  // callback associated with changing the enum attribute, which will set
  // this attribute.)
  Maya::SetStringAttribute( object, ExportNode::s_RisingWaterNameAttrName, "" );

  // create an output value that holds the insomniac enum value for the fx package
  MObject nAttrObj;
  MFnNumericAttribute	nAttr;

  if ( nodeFn.hasAttribute( ExportNode::s_RisingWaterEnumIDAttrName, &status ) )
  {
    nAttrObj = nodeFn.attribute( ExportNode::s_RisingWaterEnumIDAttrName, &status );
  }
  else
  {
    nAttrObj = nAttr.create( ExportNode::s_RisingWaterEnumIDAttrName, ExportNode::s_RisingWaterEnumIDAttrName, MFnNumericData::kInt, 0.0, &status );
    status = nodeFn.addAttribute( nAttrObj );
  }

  if ( status )
  {
    status = nAttr.setObject( nAttrObj );
    status = nAttr.setDefault( -1 );
    status = nAttr.setReadable( true );
    status = nAttr.setWritable( false );
    status = nAttr.setKeyable( false );
    status = nAttr.setStorable( false );
  }


  ////////////////////////////////////////////
  // reset to the prior state of wasLocked
  if ( nodeWasLocked )
  {
    nodeFn.setLocked( nodeWasLocked );
  }
}


void UpdateRisingWaterDynamicEnumAttrbs( MObject& object, MPlug& plug, MStatus& status )
{
  MFnDependencyNode nodeFn( object );

  ////////////////////////////////////////////
  // check to see if we are a locked node
  bool nodeWasLocked = nodeFn.isLocked();
  if ( nodeWasLocked )
  {
    // turn off any node locking so an attribute can be added
    nodeFn.setLocked( false );
  }

  MFnEnumAttribute enumAttr( plug.attribute() );
  MObject nodeObj = plug.node();
  MFnDagNode dagNodeFn( nodeObj, &status );

  NOC_ASSERT( status == MStatus::kSuccess );

  int currentPackage = -1;
  plug.getValue( currentPackage );

  if( currentPackage > 0 )
  {
    // update the name attribute
    MString packageName = enumAttr.fieldName( currentPackage );
    MPlug packageNamePlug = dagNodeFn.findPlug( ExportNode::s_RisingWaterNameAttrName, &status );
    if ( status )
    {
      packageNamePlug.setLocked( false );
      packageNamePlug.setValue( packageName );
      packageNamePlug.setLocked( true );
    }

    if ( g_RisingWaterPackageID.ReferencesObject() )
    {
      // update the enum ID attribute
      i32 enumID = -1;
      if ( !g_RisingWaterPackageID->GetElemValue( packageName.asChar(), enumID ) )
      {
        enumID = -1;
      }

      MPlug enumIDPlug = dagNodeFn.findPlug( ExportNode::s_RisingWaterEnumIDAttrName, &status );
      NOC_ASSERT( status == MStatus::kSuccess );

      enumIDPlug.setLocked( false );
      enumIDPlug.setValue( enumID );
      enumIDPlug.setLocked( true );
    }
  }

  ////////////////////////////////////////////
  // reset to the prior state of wasLocked
  if ( nodeWasLocked )
  {
    nodeFn.setLocked( nodeWasLocked );
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
// Adds or removes rising water attributes
//
void ConfigRisingWatterAttributes( MObject& object, MStatus& status, bool add = true )
{
  if ( add )
  {
    AddRisingWaterDynamicEnumAttrbs( object, status );
  }
  else
  {
    Maya::RemoveAttribute( object, ExportNode::s_RisingWaterEnumAttrName );
    Maya::RemoveAttribute( object, ExportNode::s_RisingWaterNameAttrName );
    Maya::RemoveAttribute( object, ExportNode::s_RisingWaterEnumIDAttrName );
  }
}


///////////////////////////////////////////////////////////////////////////////
// Adds or removes wrinkle map attributes
//
//void ConfigWrinkleMapAttributes( MObject& object, MStatus& status, bool add = true )
//{
//  if ( add )
//  {
//    for ( int i = 0; i < Content::MaxCountWrinkleMapRegions; ++i )
//    {
//      AddRegionAttr( object, ExportNode::s_WrinkleMapRegionAttrNames[i], status );
//    }
//  }
//  else
//  {
//    for ( int i = 0; i < Content::MaxCountWrinkleMapRegions; ++i )
//    {
//      status = Maya::RemoveAttribute( object, ExportNode::s_WrinkleMapRegionAttrNames[i] );
//    }
//  }
//}

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

    if( plug.attribute() == ExportNode::s_attr_contentType )
    {      
      int currentContentType;
      plug.getValue( currentContentType );
      Content::ContentType type = static_cast<Content::ContentType>(currentContentType);

      MPlug prevContentTypePlug = nodeFn.findPlug("prevContentType");
     
      int prevContentType;
      prevContentTypePlug.getValue(prevContentType);

      Content::ContentType prevType = static_cast<Content::ContentType>(prevContentType);

      MPlug numPlug( plug.node(), ExportNode::s_attr_contentNumber );
      numPlug.setLocked( false );

      // add the appropriate attributes depending on the type
      ConfigSkeletonAttributes( nodeThatChanged, status, ( type == Content::ContentTypes::Skeleton ) );
      ConfigBangleAttributes( nodeThatChanged, status, ( type == Content::ContentTypes::Bangle ) );
      ConfigRisingWatterAttributes( nodeThatChanged, status, ( type == Content::ContentTypes::RisingWater ) );
      //ConfigWrinkleMapAttributes( nodeThatChanged, status, ( type == Content::ContentTypes::WrinkleMap ) );

      // Keep the node type and name synchronized
      boost::regex  autoNamePattern( "^([a-zA-Z]+)(\\d*)$" );
      boost::cmatch matches;

      if( boost::regex_match( nodeFn.name().asChar(), matches, autoNamePattern ) )
      {
        std::string namePortion(matches[1].first, matches[1].second);

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
    else
    {
      MPlug attrPlug = nodeFn.findPlug( ExportNode::s_RisingWaterEnumAttrName, &status );
      if ( status && plug.attribute() == attrPlug )
      {
        UpdateRisingWaterDynamicEnumAttrbs( nodeThatChanged, plug, status );
      }
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
  s_attr_contentNumber = nAttr.create("contentNumber", "contentNumber", MFnNumericData::kLong, 0, &stat);
  MCheckErr(stat, "Unable to do: nAttr.create");

  nAttr.setKeyable(true);
  nAttr.setReadable(true);
  nAttr.setWritable(true);

  // Add "refType" to the ref Trans
  stat = addAttribute(s_attr_contentNumber);
  MCheckErr(stat, "Unable to do: refTransNode.addAttribute");

  ////////////////////////////////////////////////////////////////////////////////////////////////////
  // Create an enum attribute "contentType" and "prevContentType
  MFnEnumAttribute	eAttr;
  MFnEnumAttribute  eAttr2;

  s_attr_contentType = eAttr.create("contentType", "contentType", Content::ContentTypes::Default, &stat);
  MCheckErr(stat, "Unable to create attr: contentType");

  s_attr_prev_contentType = eAttr2.create("prevContentType", "prevContentType", Content::ContentTypes::Null, &stat);
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

  stat = addAttribute(s_attr_contentType);
  MCheckErr(stat, "Unable to add attr: contentType");

  stat = addAttribute(s_attr_prev_contentType);
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
          MPlug plug( nodeFn.object(), ExportNode::s_attr_contentType );

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
              plug.setAttribute( ExportNode::s_attr_contentNumber );
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
            MPlug plug( nodeFn.object(), ExportNode::s_attr_contentNumber );
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