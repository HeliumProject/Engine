#include "stdafx.h"

#include "MaterialDisplayNode.h"
#include "MeshSetupDisplayNode.h"

#include "MayaUtils/ErrorHelpers.h"
#include "MayaUtils/NodeTypes.h"
#include "MayaUtils/Utils.h"

#include "Common/Checksum/CRC32.h"
#include "Common/String/utilities.h"
#include "Content/Material.h"
#include "Math/Utils.h"

using namespace Maya;


static const char* s_MaterialNodeName = "MaterialNode";

MTypeId MaterialDisplayNode::m_NodeId( IGL_MATERIAL_DISP_NODE_ID );
MString MaterialDisplayNode::m_NodeName( "MaterialDisplayNode" );


//
// Callbacks
//
MIntArray MaterialDisplayNode::m_CallbackIDs; 


//
// Attributes
//

MObject MaterialDisplayNode::m_DisplayFlagAttr;
MString MaterialDisplayNode::m_DisplayFlagAttrName( "DisplayFlag" );


MaterialDisplayFlag MaterialDisplayNode::m_DisplayFlag = MaterialDisplayFlags::Off;
V_PaintPolyColor MaterialDisplayNode::m_PaintPolyColorVec;
//VM_StringMObject MaterialDisplayNode::m_MaterialComponentAttrbutes;

//
// Blind Data Attribute Names
//

const char* g_MaterialComponentsAttrStrings[Content::MaterialComponents::Unknown] = 
{ 
  "MaterialSubstance",
  "MaterialAcoustic",
  "MaterialPhysics",
};

// stolen from the engine; was u16_to_rgb_map
const u8 s_U16ToRGB[] = { 15, 23, 31, 14, 22, 30, 13, 21, 29, 12, 20, 28, 11, 19, 27, 10 };



///////////////////////////////////////////////////////////////////////////////
//
// MaterialDisplayNode Class
// 
///////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
// 
//
MaterialDisplayNode::MaterialDisplayNode()
{
}


///////////////////////////////////////////////////////////////////////////////
// 
//
MaterialDisplayNode::~MaterialDisplayNode()
{
}



///////////////////////////////////////////////////////////////////////////////
// 
//
void* MaterialDisplayNode::Creator()
{
  return new MaterialDisplayNode();
}


///////////////////////////////////////////////////////////////////////////////
// Set up the attributes.
//
MStatus MaterialDisplayNode::Initialize()
{
  MStatus status( MStatus::kSuccess );

  MAYA_START_EXCEPTION_HANDLING();

  // MaterialComponentAttrbutes
  //m_MaterialComponentAttrbutes.resize( 3, M_StringMObject() );

  // DisplayFlag
  MFnNumericAttribute nAttr; 

  m_DisplayFlagAttr = nAttr.create( m_DisplayFlagAttrName, m_DisplayFlagAttrName, MFnNumericData::kInt );
  nAttr.setDefault( MaterialDisplayFlags::Off );

  status = addAttribute( m_DisplayFlagAttr );
  MCheckErr( status, "Failed to addAttribute: DisplayFlag" );

  MAYA_FINISH_EXCEPTION_HANDLING();

  return status;
}


///////////////////////////////////////////////////////////////////////////////
// After the node is created, make sure it's not writen to file, and set the
// visibility to false.
//
void MaterialDisplayNode::postConstructor()
{  
  MPxLocatorNode::setDoNotWrite( true );
}


///////////////////////////////////////////////////////////////////////////////
// 
//
MStatus MaterialDisplayNode::AddCallbacks() 
{
  MStatus status( MStatus::kSuccess );

  MAYA_START_EXCEPTION_HANDLING();

  m_CallbackIDs.append( ( int ) MSceneMessage::addCallback( MSceneMessage::kBeforeNew, MaterialDisplayNode::FlushCallback, NULL, &status ) );
  if ( !status )
  {
    MGlobal::displayError("Unable to add Before New callback MaterialDisplayNode::FlushCallback.\n");  
  }

  m_CallbackIDs.append( ( int ) MSceneMessage::addCallback( MSceneMessage::kBeforeOpen, MaterialDisplayNode::FlushCallback, NULL, &status ) );
  if ( !status )
  {
    MGlobal::displayError("Unable to add before open callback MaterialDisplayNode::FlushCallback for.\n"); 
  }

  m_CallbackIDs.append( ( int ) MDGMessage::addNodeAddedCallback( MaterialDisplayNode::NodeAddedCallback, MaterialDisplayNode::m_NodeName, NULL, &status ) );
  if ( !status )
  {
    MGlobal::displayError("Unable to add user callback MaterialDisplayNode::NodeAddedCallback.\n");
  }

  m_CallbackIDs.append( ( int ) MDGMessage::addNodeRemovedCallback( MaterialDisplayNode::NodeRemovedCallback, MaterialDisplayNode::m_NodeName, NULL, &status ) );
  if ( !status )
  {
    MGlobal::displayError("Unable to add user callback MaterialDisplayNode::NodeRemovedCallback.\n");
  }

  //m_CallbackIDs.append( ( int ) MModelMessage::addAfterDuplicateCallback( MaterialDisplayNode::AfterDuplicateCallback, NULL, &status ) );
  //if ( !status )
  //{
  //  MGlobal::displayError("Unable to add user callback MaterialDisplayNode::AfterDuplicateCallback.\n");
  //}

  MAYA_FINISH_EXCEPTION_HANDLING();

  return status; 
}


///////////////////////////////////////////////////////////////////////////////
// 
//
MStatus MaterialDisplayNode::RemoveCallbacks()
{
  MStatus status( MStatus::kSuccess );

  MAYA_START_EXCEPTION_HANDLING();

  // Remove all the callbacks
  status = MMessage::removeCallbacks(m_CallbackIDs);
  if ( !status )
  {
    MGlobal::displayError("Unable to delete MaterialDisplayNode callbacks.");  
  }

  MAYA_FINISH_EXCEPTION_HANDLING();

  return status; 
}


///////////////////////////////////////////////////////////////////////////////
// 
//
void MaterialDisplayNode::NodeAddedCallback( MObject& node, void* clientData )
{
  MAYA_START_EXCEPTION_HANDLING();

  if( MFileIO::isReadingFile() )
  {
    return;
  }

  MAYA_FINISH_EXCEPTION_HANDLING();
}


///////////////////////////////////////////////////////////////////////////////
// 
//
void MaterialDisplayNode::NodeRemovedCallback( MObject& node, void* clientData )
{
  MAYA_START_EXCEPTION_HANDLING();


  MAYA_FINISH_EXCEPTION_HANDLING();
}

///////////////////////////////////////////////////////////////////////////////
// 
//
void MaterialDisplayNode::FlushCallback( void* clientData )
{
  MAYA_START_EXCEPTION_HANDLING();

  // MaterialComponentAttrbutes
  //if ( m_MaterialComponentAttrbutes.size() == Content::MaterialComponents::Unknown )
  //{
  //  m_MaterialComponentAttrbutes[Content::MaterialComponents::Substance].clear();
  //  m_MaterialComponentAttrbutes[Content::MaterialComponents::Acoustic].clear();
  //  m_MaterialComponentAttrbutes[Content::MaterialComponents::Physics].clear();
  //}
  //m_MaterialComponentAttrbutes.clear();
  //m_MaterialComponentAttrbutes.resize( 3, M_StringMObject() );

  m_DisplayFlag = MaterialDisplayFlags::Off;
  m_PaintPolyColorVec.clear();

  MAYA_FINISH_EXCEPTION_HANDLING();
}

///////////////////////////////////////////////////////////////////////////////
// 
//
MStatus MaterialDisplayNode::compute( const MPlug& plug, MDataBlock& data )
{ 
  return MS::kUnknownParameter;
}


///////////////////////////////////////////////////////////////////////////////
// Gets the Content::MaterialComponent given a valid MaterialDisplayFlag
//
inline Content::MaterialComponent GetMaterialComponent( const MaterialDisplayFlag materialDispFlag )
{
  switch( materialDispFlag )
  {
  default:
    NOC_BREAK();
    return Content::MaterialComponents::Unknown;
    break;

  case MaterialDisplayFlags::Off:
    return Content::MaterialComponents::Unknown;
    break;

  case MaterialDisplayFlags::Substance:
    return Content::MaterialComponents::Substance;
    break;

  case MaterialDisplayFlags::Acoustic:
    return Content::MaterialComponents::Acoustic;
    break;

  case MaterialDisplayFlags::Physics:
    return Content::MaterialComponents::Physics;
    break;
  }

  return Content::MaterialComponents::Unknown;
}

///////////////////////////////////////////////////////////////////////////////
// 
//
void MaterialDisplayNode::draw
( 
 M3dView& view, 
 const MDagPath& path, 
 M3dView::DisplayStyle style,
 M3dView::DisplayStatus dispstatus 
 )
{
  MStatus status( MStatus::kSuccess );

  if ( ( m_DisplayFlag == MaterialDisplayFlags::Off )
    || ( m_PaintPolyColorVec.empty() ) )
  {
    return;
  }

  view.beginGL();

  glPushAttrib( GL_ALL_ATTRIB_BITS ); // GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT

  glShadeModel( GL_FLAT );
  glDepthFunc( GL_LEQUAL );

  static GLfloat factor = -3.0f, units = -1.0f;
  glEnable ( GL_POLYGON_OFFSET_FILL );
  //glEnable( GL_POLYGON_STIPPLE );
  glPolygonOffset( factor, units );

  glBegin( GL_QUADS );

  u32 numPolys = (u32) m_PaintPolyColorVec.size();
  for ( u32 index = 0 ; index < numPolys ; ++index )
  {
    PaintPolyColor& paintPolyColor = m_PaintPolyColorVec[index];

    glColor4f( (GLfloat) paintPolyColor.m_DrawColor.r, (GLfloat) paintPolyColor.m_DrawColor.g, (GLfloat) paintPolyColor.m_DrawColor.b, (GLfloat) paintPolyColor.m_DrawColor.a );

    //f32 transparency = 0.5f;
    //transparency = Math::Clamp( transparency, 0.0f, 1.0f );
    //glPolygonStipple( (const GLubyte*) s_StippleMask[(16 - int(transparency * 16.0f))]);

    glVertex3d( (GLdouble) paintPolyColor.m_Point[0].X, (GLdouble) paintPolyColor.m_Point[0].Y, (GLdouble) paintPolyColor.m_Point[0].Z );
    glVertex3d( (GLdouble) paintPolyColor.m_Point[1].X, (GLdouble) paintPolyColor.m_Point[1].Y, (GLdouble) paintPolyColor.m_Point[1].Z );
    glVertex3d( (GLdouble) paintPolyColor.m_Point[2].X, (GLdouble) paintPolyColor.m_Point[2].Y, (GLdouble) paintPolyColor.m_Point[2].Z );
    glVertex3d( (GLdouble) paintPolyColor.m_Point[3].X, (GLdouble) paintPolyColor.m_Point[3].Y, (GLdouble) paintPolyColor.m_Point[3].Z );
    
  }

  glEnd();
  glPopAttrib();	 
  view.endGL();		
}



///////////////////////////////////////////////////////////////////////////////
// Creates and adds a MaterialDisplayNode to the scene
//
MObject MaterialDisplayNode::GetMaterialDisplayNode( MStatus* returnStatus )
{
  MStatus status( MStatus::kSuccess );
  MObject materialDispNode( MObject::kNullObj );

  MAYA_START_EXCEPTION_HANDLING();

  MObjectArray materialDispNodes;
  Maya::findNodesOfType( materialDispNodes, MaterialDisplayNode::m_NodeId, MFn::kLocator );

  u32 numMatDispNodes = materialDispNodes.length();

  NOC_ASSERT( numMatDispNodes <= 1 );

  if ( numMatDispNodes == 0 )
  {
    // create the node
    MFnDagNode nodeFn;
    materialDispNode = nodeFn.create( MaterialDisplayNode::m_NodeId, MObject::kNullObj, &status );
    if( materialDispNode == MObject::kNullObj )
    {
      status = MStatus::kFailure;
    }
    else
    {
      status = nodeFn.setDoNotWrite( true );
      nodeFn.setName( s_MaterialNodeName, &status );
    }
  }
  else 
  {
    materialDispNode = materialDispNodes[0];

    // delete the extra nodes if there are any
    if ( numMatDispNodes > 1 )
    {
      for ( u32 index = 1 ; index < numMatDispNodes ; ++index )
      {
        MGlobal::deleteNode( materialDispNodes[index] );
      }
    }
  }

  MAYA_FINISH_EXCEPTION_HANDLING();
  
  if ( returnStatus )
  {
    *returnStatus = status;
  }

  return materialDispNode;
}


///////////////////////////////////////////////////////////////////////////////
// Removes all MaterialDisplayNodes from the scene, and data stored on associated meshes
//
MStatus MaterialDisplayNode::DeleteMaterialDisplayNodes()
{
  MStatus status( MStatus::kSuccess );

  MAYA_START_EXCEPTION_HANDLING();

  MObjectArray materialDispNodes;
  Maya::findNodesOfType( materialDispNodes, MaterialDisplayNode::m_NodeId, MFn::kLocator );

  u32 numMatDispNodes = materialDispNodes.length();
  for ( u32 index = 0 ; index < numMatDispNodes ; ++index )
  {
    status = MGlobal::deleteNode( materialDispNodes[index] );
  }

  MAYA_FINISH_EXCEPTION_HANDLING();
  
  return status;
}


///////////////////////////////////////////////////////////////////////////////
// Assign the given material to selected objects
//
MStatus MaterialDisplayNode::AssignMaterialToSelection( const Content::MaterialPtr& material )
{ 
  MStatus status( MStatus::kSuccess );

  MAYA_START_EXCEPTION_HANDLING();

  MObject materialDispNode = GetMaterialDisplayNode( &status );

  class AssignMaterialToSelectionFunctor : public Maya::ProcessObjectFunctor
  {
  public:
    Content::MaterialPtr m_Material;

    AssignMaterialToSelectionFunctor( const Content::MaterialPtr& material )//, MPlug substanceAttrPlug, MPlug acousticAttrPlug, MPlug physicsAttrPlug )
      : m_Material( material )
    {
    }

    virtual ~AssignMaterialToSelectionFunctor() {}

    virtual MStatus ProcessLocatorNode( const MDagPath& dagPath )
    {
      MStatus status( MStatus::kSuccess );

      MFnDagNode dagNodeFn( dagPath );
      MObject object = dagNodeFn.object();

      status = SetObjectMaterial( object, m_Material );

      return status;
    }

    virtual MStatus ProcessMesh( const MDagPath& dagPath, MObject& components )
    {
      MStatus status( MStatus::kSuccess );
      
      // If we have components to iterate over
      if ( components.isNull() || components.apiType() == MFn::kMeshPolygonComponent ) 
      {
        MFnMesh meshFn( dagPath );

        status = CreateMaterialBlindData( meshFn );

        MItMeshPolygon itPoly( dagPath, components, &status );
        while ( !itPoly.isDone() )  
        {
          status = SetFaceMaterial( meshFn, itPoly.index(), m_Material );
          itPoly.next();
        }
      }
      // otherwise they are in some other mode, like vertex, and we don't handle that
      else
      {
        NOC_BREAK();
      }

      return status;
    }
  };

  status = ProcessSelectedObjects( AssignMaterialToSelectionFunctor( material ) ); //, MPlug( materialDispNode, substanceAttr ), MPlug( materialDispNode, acousticAttr ), MPlug( materialDispNode, physicsAttr ) ) );

  status = UpdateDisplayDrawPolys( materialDispNode );

  MAYA_FINISH_EXCEPTION_HANDLING();

  return status;
}



///////////////////////////////////////////////////////////////////////////////
// Assign the given material to selected objects
//
MStatus MaterialDisplayNode::AssignMaterialToSelection( const Content::MaterialComponent materialComponent, const std::string& component )
{ 
  MStatus status( MStatus::kSuccess );

  MAYA_START_EXCEPTION_HANDLING();

  MObject materialDispNode = GetMaterialDisplayNode( &status );

  class AssignMaterialComponentToSelectionFunctor : public Maya::ProcessObjectFunctor
  {
  public:
    Content::MaterialComponent m_MaterialComponent;
    std::string       m_Component;

    AssignMaterialComponentToSelectionFunctor( const Content::MaterialComponent materialComponent, const std::string& component )
      : m_MaterialComponent( materialComponent )
      , m_Component( component )
    {
    }

    virtual ~AssignMaterialComponentToSelectionFunctor() {}

    virtual MStatus ProcessLocatorNode( const MDagPath& dagPath )
    {
      MStatus status( MStatus::kSuccess );

      MFnDagNode dagNodeFn( dagPath );
      MObject object = dagNodeFn.object();

      status = Maya::SetStringAttribute( object, g_MaterialComponentsAttrStrings[m_MaterialComponent], m_Component );
      MCheckErr( status, "Failed to set Material blind data." );

      return status;
    }

    virtual MStatus ProcessMesh( const MDagPath& dagPath, MObject& components )
    {
      MStatus status( MStatus::kSuccess );
      
      MString stringData( m_Component.c_str() );

      // If we have components to iterate over
      if ( components.isNull() || components.apiType() == MFn::kMeshPolygonComponent ) 
      {
        MFnMesh meshFn( dagPath );

        status = CreateMaterialBlindData( meshFn );

        MItMeshPolygon itPoly( dagPath, components, &status );
        while ( !itPoly.isDone() )  
        {
          status = meshFn.setStringBlindData( itPoly.index(), MFn::kMeshPolygonComponent, MATERIAL_BLIND_DATA_ID, g_MaterialComponentsAttrStrings[m_MaterialComponent], stringData );
          MCheckErr( status, "Failed to set Material Substance blind data." );

          itPoly.next();
        }
      }
      // otherwise they are in some other mode, like vertex, and we don't handle that
      else
      {
        NOC_BREAK();
      }

      return status;
    }
  };

  status = ProcessSelectedObjects( AssignMaterialComponentToSelectionFunctor( materialComponent, component ) );

  status = UpdateDisplayDrawPolys( materialDispNode );

  MAYA_FINISH_EXCEPTION_HANDLING();

  return status;
}



///////////////////////////////////////////////////////////////////////////////
// Updates the display to show what materials are assigned to what objects
//
MStatus MaterialDisplayNode::UpdateDisplay( const MaterialDisplayFlag materialDisplayFlag )
{
  MStatus status( MStatus::kSuccess );

  MAYA_START_EXCEPTION_HANDLING();

  MObject materialDispNode = GetMaterialDisplayNode( &status );
  NOC_ASSERT( materialDispNode != MObject::kNullObj );

  MObject materialDispFlagAttr;
  MFnNumericAttribute numericAttrFn;

  MFnDependencyNode nodeFn( materialDispNode );

  if ( nodeFn.hasAttribute( MaterialDisplayNode::m_DisplayFlagAttrName, &status ) )
  {
    materialDispFlagAttr = nodeFn.attribute( MaterialDisplayNode::m_DisplayFlagAttrName );
  }
  else
  {
    materialDispFlagAttr = numericAttrFn.create( MaterialDisplayNode::m_DisplayFlagAttrName, MaterialDisplayNode::m_DisplayFlagAttrName, MFnNumericData::kInt );
    nodeFn.addAttribute( materialDispFlagAttr );
  }

  numericAttrFn.setObject( materialDispFlagAttr );

  MPlug materialDispFlagPlug( materialDispNode, materialDispFlagAttr );

  materialDispFlagPlug.setLocked( false );
  materialDispFlagPlug.setValue( (int) materialDisplayFlag );

  status = UpdateDisplayDrawPolys( materialDispNode );

  MAYA_FINISH_EXCEPTION_HANDLING();
  
  return status;
}


///////////////////////////////////////////////////////////////////////////////
// Update the cached list of polys to draw
//
MStatus MaterialDisplayNode::UpdateDisplayDrawPolys( MObject& node )
{
  MStatus status( MStatus::kSuccess );

  m_DisplayFlag = MaterialDisplayFlags::Off;
  m_PaintPolyColorVec.clear();

  // get the Display Flag
  MPlug materialDispFlagPlug( node, m_DisplayFlagAttr );

  int materialDispFlagInt = 0;
  status = materialDispFlagPlug.getValue( materialDispFlagInt );

  m_DisplayFlag = ( MaterialDisplayFlag ) materialDispFlagInt;

  Content::MaterialComponent materialComponent = GetMaterialComponent( m_DisplayFlag );


  class UpdateDisplayDrawPolysFunctor : public Maya::ProcessObjectFunctor
  {
  public:
    Content::MaterialComponent m_MaterialComponent;
    MaterialDisplayFlag m_DisplayFlag;

    UpdateDisplayDrawPolysFunctor( const Content::MaterialComponent materialComponent, const MaterialDisplayFlag materialDispFlag )
      : m_MaterialComponent( materialComponent )
      , m_DisplayFlag( materialDispFlag )
    {
    }

    virtual ~UpdateDisplayDrawPolysFunctor() {}

    virtual MStatus ProcessLocatorNode( const MDagPath& dagPath )
    {
      MStatus status( MStatus::kSuccess );

      MFnDagNode dagNodeFn( dagPath );
      MObject object = dagNodeFn.object();

      MColor drawColor( 1.0f, 1.0f, 1.0f );
      bool forceDrawSolid = false;

      if ( ( m_DisplayFlag != MaterialDisplayFlags::Off )
        && ( m_MaterialComponent != Content::MaterialComponents::Unknown ) )
      {

        // get the material component
        std::string stringData;
        status = Maya::GetStringAttribute( object, g_MaterialComponentsAttrStrings[m_MaterialComponent], stringData );
        
        if ( status == MStatus::kSuccess && !stringData.empty() )
        {
          Math::Color3 color( 255, 255, 255 );
          Content::Material::StripPrefix( m_MaterialComponent, stringData );
          color = GetColorFromMaterialComponent( stringData );
          
          float red, green, blue;
          color.Get( red, green, blue );
          
          drawColor.r = red;
          drawColor.g = green;
          drawColor.b = blue;

          forceDrawSolid = true;
        }
      }    

      MObject drawColorAttr;
      MFnNumericAttribute nAttr;

      MFnDependencyNode nodeFn( object );

      if ( nodeFn.hasAttribute( "drawColor", &status ) )
      {
        drawColorAttr = nodeFn.attribute( "drawColor" );
      }
      else
      {
        drawColorAttr = nAttr.createColor( "drawColor", "drawColor", &status );
        nAttr.setDefault( 1.0f, 1.0f, 1.0f );
        nAttr.setHidden( true );
        status = nodeFn.addAttribute( drawColorAttr );
      }

      nAttr.setObject( drawColorAttr );

      MPlug drawColorPlug( object, drawColorAttr );
      MFnNumericData numericDataFn;

      MObject drawColorValue = numericDataFn.create( MFnNumericData::k3Float );
      numericDataFn.setData( drawColor.r, drawColor.g, drawColor.b );

      drawColorPlug.setLocked( false );
      drawColorPlug.setValue( drawColorValue );
      drawColorPlug.setLocked( true );

      MObject forceDrawSolidAttr;
      
      if ( nodeFn.hasAttribute( "forceDrawSolid", &status ) )
      {
        forceDrawSolidAttr = nodeFn.attribute( "forceDrawSolid" );
      }
      else
      {
        forceDrawSolidAttr = nAttr.create( "forceDrawSolid", "forceDrawSolid", MFnNumericData::kBoolean );
        nAttr.setDefault( false );
        nAttr.setHidden( true );
        nodeFn.addAttribute( forceDrawSolidAttr );
      }

      nAttr.setObject( forceDrawSolidAttr );

      MPlug forceDrawSolidPlug( object, forceDrawSolidAttr );

      forceDrawSolidPlug.setLocked( false );
      forceDrawSolidPlug.setValue( forceDrawSolid );
      forceDrawSolidPlug.setLocked( true );

      return status;
    }

    virtual MStatus ProcessMesh( const MDagPath& dagPath, MObject& components )
    {
      MStatus status( MStatus::kSuccess );

      MFnMesh meshFn( dagPath );

      if ( ( m_DisplayFlag == MaterialDisplayFlags::Off )
        || ( m_MaterialComponent == Content::MaterialComponents::Unknown ) )
      {
        return status;
      }

      // early out if this mesh has not material blind data
      if ( !meshFn.isBlindDataTypeUsed( MATERIAL_BLIND_DATA_ID, &status ) )
      {
        return status;
      }

      Math::Color3 color( 0, 0, 0 );
      f32 red = 0.0f;
      f32 green = 0.0f;
      f32 blue = 0.0f;

      PaintPolyColor paintPolyColor;
      paintPolyColor.m_Point.resize( 4 );

      MPointArray vertexArray;
      meshFn.getPoints( vertexArray, MSpace::kWorld );

      int numPolys = meshFn.numPolygons( &status );
      for ( int faceIndex = 0 ; faceIndex < numPolys ; ++faceIndex )
      {
        if ( !meshFn.hasBlindData( faceIndex, MFn::kMeshPolygonComponent, MATERIAL_BLIND_DATA_ID, &status ) )
        {
          continue;
        }

        MString stringData;
        status = meshFn.getStringBlindData( faceIndex, MFn::kMeshPolygonComponent, MATERIAL_BLIND_DATA_ID, g_MaterialComponentsAttrStrings[m_MaterialComponent], stringData );

        if ( status != MStatus::kSuccess || stringData.length() < 1 )
        {
          continue;
        }

        std::string component( stringData.asChar() );
        Content::Material::StripPrefix( m_MaterialComponent, component );
        color = GetColorFromMaterialComponent( component );
        color.Get( red, green, blue );

        paintPolyColor.m_DrawColor.set( MColor::kRGB, red, green, blue, 0.5f );

        MIntArray vertexList;
        status = meshFn.getPolygonVertices( faceIndex, vertexList );

        paintPolyColor.m_Point[0].Set( vertexArray[vertexList[0]].x, vertexArray[vertexList[0]].y, vertexArray[vertexList[0]].z );
        paintPolyColor.m_Point[1].Set( vertexArray[vertexList[1]].x, vertexArray[vertexList[1]].y, vertexArray[vertexList[1]].z );
        paintPolyColor.m_Point[2].Set( vertexArray[vertexList[2]].x, vertexArray[vertexList[2]].y, vertexArray[vertexList[2]].z );
        if ( vertexList.length() > 3 )
        {
          paintPolyColor.m_Point[3].Set( vertexArray[vertexList[3]].x, vertexArray[vertexList[3]].y, vertexArray[vertexList[3]].z );
        }
        else
        {
          paintPolyColor.m_Point[3].Set( vertexArray[vertexList[0]].x, vertexArray[vertexList[0]].y, vertexArray[vertexList[0]].z );
        }

        m_PaintPolyColorVec.push_back( paintPolyColor );
      }
      
      return status;
    }
  };
  
  status = ProcessAllSceneObjects( UpdateDisplayDrawPolysFunctor( materialComponent, m_DisplayFlag ) );

  return status;
}



///////////////////////////////////////////////////////////////////////////////
// Generates a Math::Color3 based on the hash of the given string
// 
// Stolen from engine.
//
Math::Color3 MaterialDisplayNode::GetColorFromMaterialComponent( const std::string& component )
{
  u32 hash = Nocturnal::Crc32( component.data(), (u32) component.length() );

  u32 color = 0xff;
  for ( u32 i = 0; i < 16; ++i )
  {
    u32 bit = (hash >> i) & 0x1;
    color |= bit << s_U16ToRGB[i];
  }
  return Math::Color3( ( color >> 24 ) & 0xFF, ( color >> 16 ) & 0xFF, ( color >> 8 )  & 0xFF );
}


///////////////////////////////////////////////////////////////////////////////
// Clears all material data from the selected objects
//
MStatus MaterialDisplayNode::ClearMaterialsFromObjects()
{
  MStatus status( MStatus::kSuccess );

  MAYA_START_EXCEPTION_HANDLING();

  class ClearMaterialsFromObjectsFunctor : public Maya::ProcessObjectFunctor
  {
  public:
    ClearMaterialsFromObjectsFunctor(){}
    virtual ~ClearMaterialsFromObjectsFunctor() {}

    virtual MStatus ProcessLocatorNode( const MDagPath& dagPath )
    {
      MStatus status( MStatus::kSuccess );

      MFnDagNode dagNodeFn( dagPath );
      MObject object = dagNodeFn.object();

      status = RemoveObjectMaterial( object );

      return status;
    }

    virtual MStatus ProcessMesh( const MDagPath& dagPath, MObject& components )
    {
      MStatus status( MStatus::kSuccess );

      // If we have components to iterate over
      if ( components.isNull() || components.apiType() == MFn::kMeshPolygonComponent ) 
      {
        MFnMesh meshFn( dagPath );

        MItMeshPolygon itPoly( dagPath, components, &status );
        while ( !itPoly.isDone() )  
        {
          status = meshFn.clearBlindData( itPoly.index(), MFn::kMeshPolygonComponent, MATERIAL_BLIND_DATA_ID );
          itPoly.next();
        }
      }
      // otherwise they are in some other mode, like vertex, and we don't handle that
      else
      {
        NOC_BREAK();
      }

      return status;
    }
  };
  
  status = ProcessSelectedObjects( ClearMaterialsFromObjectsFunctor() );

  MObject materialDispNode = GetMaterialDisplayNode( &status );
  status = UpdateDisplayDrawPolys( materialDispNode );

  MAYA_FINISH_EXCEPTION_HANDLING();

  return status;
}


///////////////////////////////////////////////////////////////////////////////
// Clears all material data from the selected objects
//
MStatus MaterialDisplayNode::ClearMaterialsFromObjects( const Content::MaterialComponent materialComponent )
{
  MStatus status( MStatus::kSuccess );

  MAYA_START_EXCEPTION_HANDLING();

  class ClearMaterialComponentFromObjectsFunctor : public Maya::ProcessObjectFunctor
  {
  public:
    Content::MaterialComponent m_MaterialComponent;

    ClearMaterialComponentFromObjectsFunctor( const Content::MaterialComponent materialComponent )
      : m_MaterialComponent( materialComponent )
    {
    }

    virtual ~ClearMaterialComponentFromObjectsFunctor() {}

    virtual MStatus ProcessLocatorNode( const MDagPath& dagPath )
    {
      MStatus status( MStatus::kSuccess );

      MFnDagNode dagNodeFn( dagPath );
      MObject object = dagNodeFn.object();

      status = Maya::RemoveAttribute( object, g_MaterialComponentsAttrStrings[m_MaterialComponent] );
      MCheckErr( status, "Failed to remove Material attribute." );

      return status;
    }

    virtual MStatus ProcessMesh( const MDagPath& dagPath, MObject& components )
    {
      MStatus status( MStatus::kSuccess );

      // If we have components to iterate over
      if ( components.isNull() || components.apiType() == MFn::kMeshPolygonComponent ) 
      {
        MFnMesh meshFn( dagPath );

        MString stringData( "" );

        MItMeshPolygon itPoly( dagPath, components, &status );
        while ( !itPoly.isDone() )  
        {
          //status = meshFn.setStringBlindData( itPoly.index(), MFn::kMeshPolygonComponent, MATERIAL_BLIND_DATA_ID, g_MaterialComponentsAttrStrings[m_MaterialComponent], stringData );
          status = meshFn.clearBlindData( itPoly.index(), MFn::kMeshPolygonComponent, MATERIAL_BLIND_DATA_ID, g_MaterialComponentsAttrStrings[m_MaterialComponent] );
          itPoly.next();
        }
      }
      // otherwise they are in some other mode, like vertex, and we don't handle that
      else
      {
        NOC_BREAK();
      }

      return status;
    }
  };
  
  status = ProcessSelectedObjects( ClearMaterialComponentFromObjectsFunctor( materialComponent ) );

  MObject materialDispNode = GetMaterialDisplayNode( &status );
  status = UpdateDisplayDrawPolys( materialDispNode );

  MAYA_FINISH_EXCEPTION_HANDLING();

  return status;
}


///////////////////////////////////////////////////////////////////////////////
// Select the objects in the scene that are assigned with the given material
//
MStatus MaterialDisplayNode::SelectObjectsWithMaterial( const Content::MaterialPtr& material )
{
  MStatus status( MStatus::kSuccess );

  MAYA_START_EXCEPTION_HANDLING();

  MSelectionList selectionList;

  class SelectObjectsWithMaterialFunctor : public Maya::ProcessObjectFunctor
  {
  public:
    Content::MaterialPtr m_Material;
    MSelectionList*      m_SelectionList; 

    SelectObjectsWithMaterialFunctor( const Content::MaterialPtr& material, MSelectionList* selectionList = NULL )
      : m_Material( material )
      , m_SelectionList( selectionList )
    {
    }

    virtual ~SelectObjectsWithMaterialFunctor() {}

    virtual MStatus ProcessLocatorNode( const MDagPath& dagPath )
    {
      MStatus status( MStatus::kSuccess );

      MFnDagNode dagNodeFn( dagPath );
      MObject object = dagNodeFn.object();

      Content::MaterialPtr faceMaterial = GetObjectMaterial( object, &status );

      if ( faceMaterial->Equals( m_Material ) )
      {
        if ( !m_SelectionList->hasItem( dagPath, MObject::kNullObj, &status ) )
        {
          status = m_SelectionList->add( dagPath, MObject::kNullObj );	
        }
      }

      return status;
    }

    virtual MStatus ProcessMesh( const MDagPath& dagPath, MObject& components )
    {
      MStatus status( MStatus::kSuccess );
      
      if ( m_SelectionList == NULL )
      {
        return status;
      }

      MFnMesh meshFn( dagPath );

      // early out if this mesh has not material blind data
      if ( !meshFn.isBlindDataTypeUsed( MATERIAL_BLIND_DATA_ID, &status ) )
      {
        return status;
      }

      // If we have components to iterate over
      if ( components.isNull() || components.apiType() == MFn::kMeshPolygonComponent ) 
      {
        MItMeshPolygon itPoly( dagPath, components, &status );
        while ( !itPoly.isDone() )  
        {
          if ( meshFn.hasBlindData( itPoly.index(), MFn::kMeshPolygonComponent, MATERIAL_BLIND_DATA_ID, &status ) )
          {
            Content::MaterialPtr faceMaterial = GetFaceMaterial( meshFn, itPoly.index(), &status );

            if ( faceMaterial->Equals( m_Material ) )
            {
              MDagPath dagPath;
              status = meshFn.getPath( dagPath );
              if ( !m_SelectionList->hasItem( dagPath, itPoly.polygon(), &status ) )
              {
                status = m_SelectionList->add( dagPath, itPoly.polygon() );	
              }
            }
          }

          itPoly.next();
        }
      }
      // otherwise they are in some other mode, like vertex, and we don't handle that
      else
      {
        NOC_BREAK();
      }

      return status;
    }
  };
  
  status = ProcessAllSceneObjects( SelectObjectsWithMaterialFunctor( material, &selectionList ) );
  status = MGlobal::setActiveSelectionList( selectionList );

  //MDagPath dagPath;
  //MObject components;
  //u32 numSelected = selectionList.length();
  //for ( u32 index = 0; index < numSelected ; index++ )
  //{
  //  status = selectionList.getDagPath( index, dagPath, components );
  //  std::string thing = dagPath.fullPathName().asChar();
  //  std::cout << thing.c_str() << std::endl << std::flush ;
  //}

  MAYA_FINISH_EXCEPTION_HANDLING();

  return status;
}


///////////////////////////////////////////////////////////////////////////////
// Select the objects in the scene that are assigned with the given material
//
MStatus MaterialDisplayNode::SelectObjectsWithMaterial( const Content::MaterialComponent materialComponent, const std::string& component )
{
  MStatus status( MStatus::kSuccess );

  MAYA_START_EXCEPTION_HANDLING();

  MSelectionList selectionList;

  class SelectObjectsWithMaterialComponentFunctor : public Maya::ProcessObjectFunctor
  {
  public:
    Content::MaterialComponent m_MaterialComponent;
    std::string       m_Component;
    MSelectionList*      m_SelectionList; 

    SelectObjectsWithMaterialComponentFunctor( const Content::MaterialComponent materialComponent, const std::string& component, MSelectionList* selectionList = NULL )
      : m_MaterialComponent( materialComponent )
      , m_Component( component )
      , m_SelectionList( selectionList )
    {
    }

    virtual ~SelectObjectsWithMaterialComponentFunctor() {}

    virtual MStatus ProcessLocatorNode( const MDagPath& dagPath )
    {
      MStatus status( MStatus::kSuccess );

      MFnDagNode dagNodeFn( dagPath );
      MObject object = dagNodeFn.object();

      std::string component;
      status = Maya::GetStringAttribute( object, g_MaterialComponentsAttrStrings[m_MaterialComponent], component );  

      if ( status == MStatus::kSuccess && m_Component.compare( component ) == 0 )
      {
        if ( !m_SelectionList->hasItem( dagPath, MObject::kNullObj, &status ) )
        {
          status = m_SelectionList->add( dagPath, MObject::kNullObj );	
        }
      }

      return status;
    }

    virtual MStatus ProcessMesh( const MDagPath& dagPath, MObject& components )
    {
      MStatus status( MStatus::kSuccess );
      
      if ( m_SelectionList == NULL )
      {
        return status;
      }

      MFnMesh meshFn( dagPath );

      // early out if this mesh has not material blind data
      if ( !meshFn.isBlindDataTypeUsed( MATERIAL_BLIND_DATA_ID, &status ) )
      {
        return status;
      }

      // If we have components to iterate over
      if ( components.isNull() || components.apiType() == MFn::kMeshPolygonComponent ) 
      {
        MString stringData;
        MItMeshPolygon itPoly( dagPath, components, &status );
        while ( !itPoly.isDone() )  
        {
          status = meshFn.getStringBlindData( itPoly.index(), MFn::kMeshPolygonComponent, MATERIAL_BLIND_DATA_ID, g_MaterialComponentsAttrStrings[m_MaterialComponent], stringData );
          if ( status == MStatus::kSuccess && m_Component.compare( stringData.asChar() ) == 0 )
          {
            MDagPath dagPath;
            status = meshFn.getPath( dagPath );
            if ( !m_SelectionList->hasItem( dagPath, itPoly.polygon(), &status ) )
            {
              status = m_SelectionList->add( dagPath, itPoly.polygon() );	
            }
          }

          itPoly.next();
        }
      }
      // otherwise they are in some other mode, like vertex, and we don't handle that
      else
      {
        NOC_BREAK();
      }

      return status;
    }
  };
  
  status = ProcessAllSceneObjects( SelectObjectsWithMaterialComponentFunctor( materialComponent, component, &selectionList ) );
  status = MGlobal::setActiveSelectionList( selectionList );

  //MDagPath dagPath;
  //MObject components;
  //u32 numSelected = selectionList.length();
  //for ( u32 index = 0; index < numSelected ; index++ )
  //{
  //  status = selectionList.getDagPath( index, dagPath, components );
  //  std::string thing = dagPath.fullPathName().asChar();
  //  std::cout << thing.c_str() << std::endl << std::flush ;
  //}

  MAYA_FINISH_EXCEPTION_HANDLING();

  return status;
}


///////////////////////////////////////////////////////////////////////////////
// Creates the material blind data template for the polys
//
MStatus MaterialDisplayNode::CreateMaterialBlindData( MFnMesh& meshFn )
{
  MStatus status( MStatus::kSuccess );

  MAYA_START_EXCEPTION_HANDLING();
	
  if ( !meshFn.isBlindDataTypeUsed( MATERIAL_BLIND_DATA_ID, &status ) )
	{
		MStringArray names, formatNames;
		
    names.append( g_MaterialComponentsAttrStrings[Content::MaterialComponents::Substance] );
		formatNames.append( "string" );

		names.append( g_MaterialComponentsAttrStrings[Content::MaterialComponents::Acoustic] );
		formatNames.append( "string" );
		
		names.append( g_MaterialComponentsAttrStrings[Content::MaterialComponents::Physics] );
		formatNames.append( "string" );

		status = meshFn.createBlindDataType( MATERIAL_BLIND_DATA_ID, names, names, formatNames );
	}

  MAYA_FINISH_EXCEPTION_HANDLING();

  return status;
}


///////////////////////////////////////////////////////////////////////////////
// Creates the material blind data template for the polys
//
MStatus MaterialDisplayNode::SetFaceMaterial( MFnMesh& meshFn, const i32 faceIndex, const Content::MaterialPtr& material )
{
  MStatus status( MStatus::kSuccess );

  MAYA_START_EXCEPTION_HANDLING();
	
  MString stringData;

  stringData = material->m_Substance.c_str();
  status = meshFn.setStringBlindData( faceIndex, MFn::kMeshPolygonComponent, MATERIAL_BLIND_DATA_ID, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Substance], stringData );
  MCheckErr( status, "Failed to set Material Substance blind data." );
  
  stringData = material->m_Acoustic.c_str();
  status = meshFn.setStringBlindData( faceIndex, MFn::kMeshPolygonComponent, MATERIAL_BLIND_DATA_ID, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Acoustic], stringData );
  MCheckErr( status, "Failed to set Material Acoustics blind data." );
  
  stringData = material->m_Physics.c_str();
  status = meshFn.setStringBlindData( faceIndex, MFn::kMeshPolygonComponent, MATERIAL_BLIND_DATA_ID, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Physics], stringData );
  MCheckErr( status, "Failed to set Material Physics blind data." );

  MAYA_FINISH_EXCEPTION_HANDLING();

  return status;
}



///////////////////////////////////////////////////////////////////////////////
// Creates the material blind data template for the polys
//
MStatus MaterialDisplayNode::SetObjectMaterial( MObject& object, const Content::MaterialPtr& material )
{
  MStatus status( MStatus::kSuccess );

  MAYA_START_EXCEPTION_HANDLING();

  status = Maya::SetStringAttribute( object, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Substance], material->m_Substance );
  MCheckErr( status, "Failed to set Material Substance blind data." );
  
  status = Maya::SetStringAttribute( object, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Acoustic], material->m_Acoustic );
  MCheckErr( status, "Failed to set Material Acoustics blind data." );
  
  status = Maya::SetStringAttribute( object, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Physics], material->m_Physics );
  MCheckErr( status, "Failed to set Material Physics blind data." );

  MAYA_FINISH_EXCEPTION_HANDLING();

  return status;
}


#define CHECK_MSTATUS_SET_RETURNSTATUS(status,returnStatus)                         \
if ( returnStatus != NULL )                                                         \
{                                                                                   \
  *returnStatus = status;                                                           \
}

#define CHECK_MSTATUS_RETURN_NULL(status,returnStatus,msg)                          \
if( status != MS::kSuccess )                                                        \
{                                                                                   \
  ErrorHelper( status, msg, __FILE__, __FUNCTION__, __LINE__ );                     \
  CHECK_MSTATUS_SET_RETURNSTATUS(status,returnStatus)                               \
  return NULL;                                                                      \
}

#define CHECK_MSTATUS_SET_STRING(status,returnStatus,data,str)                      \
if( status != MS::kSuccess )                                                        \
{                                                                                   \
  CHECK_MSTATUS_SET_RETURNSTATUS(status,returnStatus)                               \
  str = "";                                                                         \
}                                                                                   \
else                                                                                \
{                                                                                   \
  str = data.asChar();                                                              \
}

///////////////////////////////////////////////////////////////////////////////
// Gets the material bind data for the specified face.  Returns kFailure if
// there is no blind data for the face.
//
Content::MaterialPtr MaterialDisplayNode::GetFaceMaterial( const MFnMesh& meshFn, const i32 faceIndex, MStatus* returnStatus )
{
  Content::MaterialPtr material = new Content::Material;
  MStatus status( MStatus::kSuccess );

  MAYA_START_EXCEPTION_HANDLING();

  if ( !meshFn.isBlindDataTypeUsed( MATERIAL_BLIND_DATA_ID, &status ) )
  {
    return NULL;
  }
  CHECK_MSTATUS_RETURN_NULL( status, returnStatus, "Failed to get material blind data." );

  MString stringData;

  //status = meshFn.getStringBlindData( faceIndex, MFn::kMeshPolygonComponent, MATERIAL_BLIND_DATA_ID, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Substance], stringData );
  stringData = meshFn.stringBlindDataComponentId( faceIndex, MFn::kMeshPolygonComponent, MATERIAL_BLIND_DATA_ID, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Substance], &status );
  CHECK_MSTATUS_SET_STRING( status, returnStatus, stringData, material->m_Substance );
  
  //status = meshFn.getStringBlindData( faceIndex, MFn::kMeshPolygonComponent, MATERIAL_BLIND_DATA_ID, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Acoustic], stringData );
  stringData = meshFn.stringBlindDataComponentId( faceIndex, MFn::kMeshPolygonComponent, MATERIAL_BLIND_DATA_ID, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Acoustic], &status );
  CHECK_MSTATUS_SET_STRING( status, returnStatus, stringData, material->m_Acoustic );
  
  //status = meshFn.getStringBlindData( faceIndex, MFn::kMeshPolygonComponent, MATERIAL_BLIND_DATA_ID, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Physics], stringData );
  stringData = meshFn.stringBlindDataComponentId( faceIndex, MFn::kMeshPolygonComponent, MATERIAL_BLIND_DATA_ID, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Physics], &status );
  CHECK_MSTATUS_SET_STRING( status, returnStatus, stringData, material->m_Physics );

  return material;

  MAYA_FINISH_EXCEPTION_HANDLING();

  return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// Gets the material attributes for the object.  
//
Content::MaterialPtr MaterialDisplayNode::GetObjectMaterial( const MObject& object, MStatus* returnStatus )
{
  Content::MaterialPtr material = new Content::Material;
  MStatus status( MStatus::kSuccess );

  MAYA_START_EXCEPTION_HANDLING();

  status = Maya::GetStringAttribute( object, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Substance], material->m_Substance );  
  status = Maya::GetStringAttribute( object, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Acoustic], material->m_Acoustic );  
  status = Maya::GetStringAttribute( object, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Physics], material->m_Physics );

  CHECK_MSTATUS_SET_RETURNSTATUS( status, returnStatus )

  return material;

  MAYA_FINISH_EXCEPTION_HANDLING();

  return NULL;
}


///////////////////////////////////////////////////////////////////////////////
// Creates the material blind data template for the polys
//
MStatus MaterialDisplayNode::RemoveObjectMaterial( MObject& object )
{
  MStatus status( MStatus::kSuccess );

  MAYA_START_EXCEPTION_HANDLING();

  status = Maya::RemoveAttribute( object, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Substance] );  
  status = Maya::RemoveAttribute( object, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Acoustic] );  
  status = Maya::RemoveAttribute( object, g_MaterialComponentsAttrStrings[Content::MaterialComponents::Physics] );

  MAYA_FINISH_EXCEPTION_HANDLING();

  return status;
}
