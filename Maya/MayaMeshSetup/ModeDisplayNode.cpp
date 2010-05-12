#include "stdafx.h"

#include "ModeDisplayNode.h"
#include "MeshSetupDisplayNode.h"

#include "../../MayaNodeTypes.h"

using namespace Maya;

const char* GLOBAL_MODE_NODE_NAME = "IGGlobalModeNode";

MTypeId ModeDisplayNode::m_NodeId( IGT_MODENODE_ID );
MString ModeDisplayNode::m_NodeName( "igModeNode" );

MObject ModeDisplayNode::m_Set;    // input set to visualize
MObject ModeDisplayNode::m_Color;    // color of visualized set faces

std::string ModeDisplayNode::s_ConnectedSetStr( "" );
MPointArray ModeDisplayNode::m_VertexBuffer;



ModeDisplayNode::ModeDisplayNode()
: m_Active( true )
{
}


ModeDisplayNode::~ModeDisplayNode()
{
}


MStatus ModeDisplayNode::compute( const MPlug& plug, MDataBlock& data )
{ 
  return MS::kUnknownParameter;
}


/////////////////////////////////////////////////////////////////////////////// 
// -Iterate over all meshes
// -Look for the attribute on the mesh matching the value of the 'm_Set' attribute on this node
// -If this is an integer array attribute
// -Loop over the faces listed in the array and draw them
//   o note: the first element of the array is skipped - it is assumed to be the total number of faces on the mesh
//
void ModeDisplayNode::draw
( 
 M3dView& view, 
 const MDagPath& path, 
 M3dView::DisplayStyle style,
 M3dView::DisplayStatus dispstatus 
 )
{ 
  MStatus status;
  MObject thisNode = thisMObject();

  // Get tjoint coordinates
  MPlug set_plug( thisNode, m_Set );

  if ( status == MS::kFailure )
  {
    status.perror( "set_plug()" );
  }

  MString setName;
  status = set_plug.getValue( setName );

  // bail out of not looking at any set...
  if ( setName.length() == 0 )
  {
    return;
  }

  double red = 1.0, green = 1.0, blue = 1.0;

  MPlug colorPlug( thisNode, m_Color );

  if ( colorPlug.numChildren() == 3 )
  {
    MPlug rPlug( colorPlug.child( 0 ) );
    MPlug gPlug( colorPlug.child( 1 ) );
    MPlug bPlug( colorPlug.child( 2 ) );

    rPlug.getValue( red );
    gPlug.getValue( green );
    bPlug.getValue( blue );
  }

  // Go ahead and get the drawing setup right here...
  view.beginGL();

  glPushAttrib( GL_ALL_ATTRIB_BITS );

  view.setDrawColor( MColor( ( float )red,( float )green,( float )blue, 0.5f ) );

  glDepthFunc( GL_LEQUAL );

  static GLfloat factor = -3.0f, units = -1.0f;
  glEnable ( GL_POLYGON_OFFSET_FILL );
  glPolygonOffset( factor, units );

  glBegin( GL_QUADS );

  {
    // start iterator over our meshes
    MStatus status( MStatus::kSuccess );
    MDagPath dagPath;
    MItDag itDag( MItDag::kDepthFirst, MFn::kMesh, &status );

    bool isDone = itDag.isDone( &status );
    while ( !isDone )
    {
      status = itDag.getPath( dagPath );

      //////////////////////////////////////////////////////////////////////////
      // process mesh
      MFnMesh meshFn( dagPath );

      MFnDagNode currentNode( meshFn.parent( 0 ) );

      bool isVisible = Maya::IsNodeVisible( currentNode );

      if ( !isVisible )
      {
        // prune subtree if not visible....
        itDag.prune();
      }
      else
      {
        MPlug facesPlug = meshFn.findPlug( setName, &status );

        if ( status == MS::kSuccess )
        {
          MObject plugVal;

          facesPlug.getValue( plugVal );

          if ( plugVal.hasFn( MFn::kIntArrayData ) )
          {
            MFnIntArrayData faceList( plugVal );

            status = meshFn.getPoints( m_VertexBuffer, MSpace::kWorld );

            // start at 1 because first element in face list is the number of faces the mesh has..
            for ( int i=1; i <= ( int )faceList.length(); ++i )
            {
              if ( faceList[ i ] >= 0 && faceList[ i ] < meshFn.numPolygons() )
              {
                MIntArray vertexList;
                meshFn.getPolygonVertices( faceList[ i ], vertexList );

                glVertex3d( m_VertexBuffer[vertexList[0]].x, m_VertexBuffer[vertexList[0]].y, m_VertexBuffer[vertexList[0]].z );
                glVertex3d( m_VertexBuffer[vertexList[1]].x, m_VertexBuffer[vertexList[1]].y, m_VertexBuffer[vertexList[1]].z );
                glVertex3d( m_VertexBuffer[vertexList[2]].x, m_VertexBuffer[vertexList[2]].y, m_VertexBuffer[vertexList[2]].z );
                if ( vertexList.length() > 3 )
                {
                  glVertex3d( m_VertexBuffer[vertexList[3]].x, m_VertexBuffer[vertexList[3]].y, m_VertexBuffer[vertexList[3]].z );
                }
                else
                {
                  glVertex3d( m_VertexBuffer[vertexList[0]].x, m_VertexBuffer[vertexList[0]].y, m_VertexBuffer[vertexList[0]].z );
                }
              }
            }
          }
        }
      }

      //////////////////////////////////////////////////////////////////////////

      status = itDag.next();
      isDone = itDag.isDone( &status );
    }
  }

  glEnd();
  glPopAttrib();	 
  view.endGL();	
}


void* ModeDisplayNode::Creator()
{
  return new ModeDisplayNode();
}


// Set up attributes...
MStatus ModeDisplayNode::Initialize()
{ 	
  MStatus status;	

  {
    MFnTypedAttribute attr;

    m_Set = attr.create( "inputDrawSet", "ids", MFnData::kString, MObject::kNullObj, &status );

    if ( status == MS::kFailure )
    {
      status.perror( "ModeDisplayNode::inititalize() - attr.create( 'inputDrawSet' )" );
    }

    status = addAttribute( m_Set );

    if ( status == MS::kFailure )
    {
      status.perror( "ModeDisplayNode::inititalize() - addAttribute( m_Set )" );
    }
  }

  {
    MFnNumericAttribute attr;

    m_Color = attr.createColor( "drawSetColor", "dsc", &status );

    if ( status == MS::kFailure )
    {
      status.perror( "ModeDisplayNode::inititalize() - attr.create( 'drawSetColor' )" );
    }

    status = addAttribute( m_Color );

    if ( status == MS::kFailure )
    {
      status.perror( "ModeDisplayNode::inititalize() - addAttribute( m_Color )" );
    }
  }

  {
    m_VertexBuffer.setLength( 10000 );
  }

  return MS::kSuccess;
}


///////////////////////////////////////////////////////////////////////////////
// Call this to remove all mode nodes from the scene.  This will
// not report an error in Maya if it can't delete one of the
// mode nodes.
//
// It is safe to call this multiple times.  It will automatically
// disconnect the ModeDisplayNode managed by this class.
void ModeDisplayNode::RemoveAllModeNodes()
{
  MStringArray junknodes;
  char tmp[1024];

  // make sure we are disconnected...
  ModeDisplayNode::DisconnectModeNodeFromSet();

  sprintf( tmp,"ls -type \"ModeDisplayNode\";" );
  MGlobal::executeCommand( tmp, junknodes, false, false );

  for( int i=0; i < ( int ) junknodes.length(); i++ )
  {
    MStringArray parent;
    
    sprintf( tmp,"listRelatives -p \"%s\";", junknodes[i].asChar() );
    MGlobal::executeCommand( tmp, parent, false, false );

    sprintf( tmp,"delete \"%s\";", parent[0].asChar() );
    MGlobal::executeCommand( tmp, parent, false, false );
  }
}


///////////////////////////////////////////////////////////////////////////////
// Checks the scene for all mode nodes, and returns the global
// mode node created by this class.  If the global mode node
// doesn't yet exist, this method will create a new mode
// node.
//
// This method will report errors in maya if there are multiple
// ModeDisplayNode objects in the scene.  It will also report errors
// if for some reason it couldn't create new ModeDisplayNode ( it
// will only try to create if none exist in the scene ).  In
// this case, "false" is returned.  Otherwise, if the method
// is successful, the "true" is returned.
bool ModeDisplayNode::GetModeNode( std::string& globalModeNodeName )
{
  MStringArray modeNodes;
  char cmd[1024];
  bool success = true;

  globalModeNodeName = "";

  sprintf( cmd,"ls -type \"ModeDisplayNode\";" );
  MGlobal::executeCommand( cmd, modeNodes, false, false );

  if ( modeNodes.length() < 1 )
  {
    if ( ModeDisplayNode::CreateModeNode() )
    {
      globalModeNodeName = GLOBAL_MODE_NODE_NAME;
    }
  }
  else if ( modeNodes.length() > 1 ) // multiple..  display warning..
  {
    MGlobal::displayError( "Error - more than 1 mode node exists!\n" );
  }
  else // just one mode node..
  {
    globalModeNodeName = modeNodes[0].asChar();
  }

  if( strcmp( globalModeNodeName.c_str(), GLOBAL_MODE_NODE_NAME ) != 0 )
  {
    success = false;
    globalModeNodeName = "";
    MGlobal::displayError( "Error - could not find or create global mode node!\n" );
  }

  return success;
}


///////////////////////////////////////////////////////////////////////////////
// Set the color of the global ModeDisplayNode.  If the global
// ModeDisplayNode doesn't exist, it will be created ( via a call
// to GetModeNode() ).  Once the color of the mode node is set,
// all of the polys in the set connected to the mode node will
// be rendered with that color.
//
// If the color is successfully set, then the method will return
// "true", otherwise, it will return "false".
bool ModeDisplayNode::SetModeNodeColor( double& red, double& green, double& blue )
{
  MStatus status = MS::kFailure;

  bool success = false;

  std::string modeNode;
  if ( ModeDisplayNode::GetModeNode( modeNode ) )
  {
    char cmd[1024];
    sprintf( cmd, "setAttr %s.drawSetColor %1.3f %1.3f %1.3f", modeNode.c_str(), red, green, blue );
    status = MGlobal::executeCommand( cmd, false, false );

    if ( status == MS::kSuccess )
    {
      success = true;
    }
  }

  return success;
}


///////////////////////////////////////////////////////////////////////////////
// Connects the global ModeDisplayNode to the given set.  If the global
// ModeDisplayNode doesn't exist, it will be created ( via a call to
// GetModeNode() ).  If the mode node could not be connected to the
// given set, then an error will be reported in maya, and the
// method will return "false".  If the operation is successful, the
// method will return "true".
//
// If the global ModeDisplayNode is connected to another set, it will
// automatically be disconnected.
bool ModeDisplayNode::ConnectModeNodeToSet( const char* setName )
{
  MStatus status = MS::kFailure;
  
  bool success = false;
  
  std::string modeNode;
  if ( ModeDisplayNode::GetModeNode( modeNode ) )
  {
    char cmd[1024];
    sprintf( cmd, "setAttr -type \"string\" %s.inputDrawSet \"%s\"", modeNode.c_str(), setName );
    status = MGlobal::executeCommand( cmd, false, false );

    if ( status == MS::kSuccess )
    {
      success = true;
    }
  }

  return success;
}


///////////////////////////////////////////////////////////////////////////////
// Disconnects the global ModeDisplayNode from the connected set.  If the
// global ModeDisplayNode was _not_ connected to the set by calling
// ModeDisplayNode::ConnectModeNodeToSet( .. ), then this method will _not_
// disconnected the ModeDisplayNode!  So, only use this method
// if you are using the other methods to connect the ModeDisplayNode.
//
// If the global ModeDisplayNode is disconnected ( or wasn't connected
// to begin with ), then the method returns "true".  Otherwise,
// if the node could not be disconnected, the method returns
// "false".
bool ModeDisplayNode::DisconnectModeNodeFromSet()
{
  MStatus status = MS::kFailure;
  
  bool success = false;  

  std::string modeNode;
  if ( ModeDisplayNode::GetModeNode( modeNode ) )
  {
    char cmd[1024];
    sprintf( cmd, "setAttr -type \"string\" %s.inputDrawSet \"\"", modeNode.c_str() );
    status = MGlobal::executeCommand( cmd, false, false );

    if ( status == MS::kSuccess )
    {
      success = true;
    }
  }

  return success;
}


///////////////////////////////////////////////////////////////////////////////
// Creates a new global ModeDisplayNode.  Returns "true" if the mode
// node was successfully created, "false" otherwise.
bool ModeDisplayNode::CreateModeNode()
{
  MStatus status = MS::kFailure;

  bool success = true;
  
  char cmd[128];
  sprintf( cmd,"createNode \"ModeDisplayNode\" -n \"%s\"",GLOBAL_MODE_NODE_NAME );
  status = MGlobal::executeCommand( cmd, false, false );

  if ( status == MS::kFailure )
  {
    MGlobal::displayError( "Could not create global mode node!\n" );
    success = false;
  }

  return success;
}