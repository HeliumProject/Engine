#include "stdafx.h"

#include "CollisionModeDisplayNode.h"
#include "MeshSetupDisplayNode.h"

#include "MayaUtils/NodeTypes.h"

using namespace Maya;

static const char* s_CollisionModeNodeName = "CollisionModeNode";

MTypeId CollisionModeDisplayNode::m_NodeId( IGL_COLL_MODE_DISP_NODE_ID ); // Was: IGT_MODENODE_ID
MString CollisionModeDisplayNode::m_NodeName( "igModeNode" );  // Should be: "CollisionModeDisplayNode"

MObject CollisionModeDisplayNode::m_Set;      // input set to visualize
MObject CollisionModeDisplayNode::m_Color;    // color of visualized set objects

MPointArray CollisionModeDisplayNode::m_VertexBuffer;


const char* Maya::CollisionModeDisplayFlagsStrings[CollisionModeDisplayFlags::Count] = 
{ 
  "Off",
  "CameraIgnore",
  "Impassable",
  "NonTraversable",
  "Transparent"
};


CollisionModeDisplayNode::CollisionModeDisplayNode()
: m_Active( true )
{
}


CollisionModeDisplayNode::~CollisionModeDisplayNode()
{
}


MStatus CollisionModeDisplayNode::compute( const MPlug& plug, MDataBlock& data )
{ 
  return MS::kUnknownParameter;
}


/////////////////////////////////////////////////////////////////////////////// 
// -Iterate over all meshes
// -Look for the attribute on the mesh matching the value of the 'm_Set' attribute on this node
// -If this is an integer array attribute
// -Loop over the objects listed in the array and draw them
//   o note: the first element of the array is skipped - it is assumed to be the total number of objects on the mesh
//
void CollisionModeDisplayNode::draw
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

            // start at 1 because first element in face list is the number of objects the mesh has..
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


void* CollisionModeDisplayNode::Creator()
{
  return new CollisionModeDisplayNode();
}


// Set up attributes...
MStatus CollisionModeDisplayNode::Initialize()
{ 	
  MStatus status;	

  {
    MFnTypedAttribute attr;

    m_Set = attr.create( "inputDrawSet", "ids", MFnData::kString, MObject::kNullObj, &status );

    if ( status == MS::kFailure )
    {
      status.perror( "CollisionModeDisplayNode::inititalize() - attr.create( 'inputDrawSet' )" );
    }

    status = addAttribute( m_Set );

    if ( status == MS::kFailure )
    {
      status.perror( "CollisionModeDisplayNode::inititalize() - addAttribute( m_Set )" );
    }
  }

  {
    MFnNumericAttribute attr;

    m_Color = attr.createColor( "drawSetColor", "dsc", &status );

    if ( status == MS::kFailure )
    {
      status.perror( "CollisionModeDisplayNode::inititalize() - attr.create( 'drawSetColor' )" );
    }

    status = addAttribute( m_Color );

    if ( status == MS::kFailure )
    {
      status.perror( "CollisionModeDisplayNode::inititalize() - addAttribute( m_Color )" );
    }
  }

  {
    m_VertexBuffer.setLength( 10000 );
  }

  return MS::kSuccess;
}
