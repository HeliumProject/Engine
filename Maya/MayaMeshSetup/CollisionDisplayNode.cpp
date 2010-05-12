#include "stdafx.h"

#include "CollisionDisplayNode.h"
#include "ModeDisplayNode.h"

#include "../../MayaNodeTypes.h"

using namespace Maya;

MTypeId CollisionDisplayNode::m_NodeId( IGL_COLL_DISP_NODE_ID );
MString CollisionDisplayNode::m_NodeName( "igCollDispNode" );


CollisionDisplayNode::CollisionDisplayNode()
{
}


CollisionDisplayNode::~CollisionDisplayNode()
{
}


void CollisionDisplayNode::RenderMesh( MFnMesh& meshFn, M3dView& view )
{
  MStatus status( MStatus::kSuccess );
  MFnDagNode currentNode( meshFn.parent( 0 ) );
  MString mat_name;
  int view_width = view.portWidth( &status );
  int view_height = view.portHeight( &status );
  int view_area = view_width * view_height;
  short p1_x, p1_y, p2_x, p2_y, p3_x, p3_y, p4_x, p4_y;
  int e1_x, e1_y, e2_x, e2_y, area;
  char str[100];

  float red = 1.0f;
  float green = 1.0f;
  float blue = 1.0f;

  {
    MPlug plug;
    MString guid( "" );
    plug = meshFn.findPlug( "CollMaterialGuid", &status );

    if( status == MStatus::kSuccess )
    {
      plug.getValue( guid );
    }

    MCommandResult result;
    char cmd[256];
    sprintf( cmd,"materialInfo -GetUidColor \"%s\"", guid.asChar() );
    status = MGlobal::executeCommand( cmd, result, false, false );

    if( status )
    {
      MDoubleArray rgb;
      status = result.getResult( rgb );
      if ( status && rgb.length() == 3 )
      {
        red = ( float ) rgb[0];
        green = ( float ) rgb[1];
        blue = ( float ) rgb[2];
      }
    }

    sprintf( cmd,"materialInfo -GetUidMatName \"%s\"", guid.asChar() );
    status = MGlobal::executeCommand( cmd, result, false, false );
    if( status )
    {
      status = result.getResult( mat_name );
    }
  }

  //std::cout<< "\tIs a mesh mesh " << meshFn.fullPathName().asChar() << " is selected " <<  red << ","<< green <<","<< blue<< std::endl;

  if ( ModeDisplayNode::IsNodeVisible( currentNode ) )
  {
    MPointArray vertexArray;
    meshFn.getPoints( vertexArray, MSpace::kWorld );
    int numPolys = meshFn.numPolygons( &status );

    for ( int faceIdx=0; faceIdx<numPolys; faceIdx++ )
    {
      MIntArray vertexList;

      meshFn.getPolygonVertices( faceIdx, vertexList );

      int numVerts = vertexList.length();

      if ( numVerts == 3 )
      {
        MPoint center;
        MPoint pnt1 = vertexArray[vertexList[0]];
        MPoint pnt2 = vertexArray[vertexList[1]];
        MPoint pnt3 = vertexArray[vertexList[2]];
        MPoint pnt4;  // hack pnt 4
        center = ( pnt1 + pnt2 + pnt3 ) * 0.3333333;


        if ( view.worldToView ( pnt1, p1_x, p1_y, &status ) )
        {
          if ( view.worldToView ( pnt2, p2_x, p2_y, &status ) )
          {
            if ( view.worldToView ( pnt3, p3_x, p3_y, &status ) )
            {
              e1_x = p2_x - p1_x;
              e2_x = p3_x - p1_x;
              e1_y = p2_y - p1_y;
              e2_y = p3_y - p1_y;
              area = abs( ( e1_x*e2_y - e2_x*e1_x )/2 );
              if ( area*50 > view_area )
              {
                glColor4f( 0.0, 0.0, 0.0, 1.0f );
                MPoint textPos( center.x, center.y, center.z );
                sprintf( str, mat_name.asChar() );
                MString distanceText( str );
                view.drawText( distanceText, textPos, M3dView::kCenter );
              }
            }
          }
        }
        pnt1 += ( center - pnt1 ) * 0.01;
        pnt2 += ( center - pnt2 ) * 0.01;
        pnt3 += ( center - pnt3 ) * 0.01;
        pnt4 = pnt1;

        //For sake of text drawing I have to Call glBegin after text
        glColor4f( red, green, blue, 0.5f );
        glBegin( GL_QUADS );
        glVertex3d( pnt1.x, pnt1.y, pnt1.z );
        glVertex3d( pnt2.x, pnt2.y, pnt2.z );
        glVertex3d( pnt3.x, pnt3.y, pnt3.z );
        glVertex3d( pnt4.x, pnt4.y, pnt4.z );
        glEnd();
      }
      else if ( numVerts == 4 )
      {
        MPoint center;

        //view.setDrawColor( MColor( ( float )0.0f,( float )0.0f,( float )1.0f, 1.0f ) );
        MPoint pnt1 = vertexArray[vertexList[0]];
        MPoint pnt2 = vertexArray[vertexList[1]];
        MPoint pnt3 = vertexArray[vertexList[2]];
        MPoint pnt4 = vertexArray[vertexList[3]];
        center = ( pnt1 + pnt2 + pnt3 + pnt4 ) * 0.25;

        if ( view.worldToView ( pnt1, p1_x, p1_y, &status ) )
        {
          if ( view.worldToView ( pnt2, p2_x, p2_y, &status ) )
          {
            if ( view.worldToView ( pnt3, p3_x, p3_y, &status ) )
            {
              e1_x = p2_x - p1_x;
              e2_x = p3_x - p1_x;
              e1_y = p2_y - p1_y;
              e2_y = p3_y - p1_y;
              area = abs( ( e1_x*e2_y - e2_x*e1_x )/2 );
              p4_x, p4_y;
              if ( view.worldToView ( pnt3, p4_x, p4_y, &status ) )
              {
                e1_x = p3_x - p1_x;
                e2_x = p4_x - p1_x;
                e1_y = p3_y - p1_y;
                e2_y = p4_y - p1_y;
                area += abs( ( e1_x*e2_y - e2_x*e1_x )/2 );
                if ( area*50 > view_area )
                {
                  glColor4f( 0.0, 0.0, 0.0, 1.0f );
                  MPoint textPos( center.x, center.y, center.z );
                  sprintf( str, mat_name.asChar() );
                  MString distanceText( str );
                  view.drawText( distanceText, textPos, M3dView::kCenter );
                }
              }
            }
          }
        }

        pnt1 += ( center - pnt1 ) * 0.01;
        pnt2 += ( center - pnt2 ) * 0.01;
        pnt3 += ( center - pnt3 ) * 0.01;
        pnt4 += ( center - pnt4 ) * 0.01;

        glColor4f( red, green, blue, 0.5f );
        glBegin( GL_QUADS );
        glVertex3d( pnt1.x, pnt1.y, pnt1.z );
        glVertex3d( pnt2.x, pnt2.y, pnt2.z );
        glVertex3d( pnt3.x, pnt3.y, pnt3.z );
        glVertex3d( pnt4.x, pnt4.y, pnt4.z );
        glEnd();
      }
    }
  }
}


void CollisionDisplayNode::IterateGroupNode( const MDagPath& path, M3dView& view )
{
  MStatus status( MStatus::kSuccess );
  MDagPath dagPath;
  MItDag itDag( MItDag::kDepthFirst, MFn::kMesh, &status );
  status = itDag.reset( path, MItDag::kDepthFirst, MFn::kMesh );

  bool isDone = itDag.isDone( &status );
  std::string last_mesh( "" ); //for some reason maya give it twice  transform selected is not world parented
  while ( !isDone )
  {
    status = itDag.getPath( dagPath );

    // process mesh
    MFnMesh meshFn( dagPath );
    //ignore rendering if the mesh is intermediate crap
    bool isIntermediate = meshFn.isIntermediateObject( &status );
    if ( !isIntermediate )
    {
      if ( last_mesh != meshFn.fullPathName().asChar() )
      {
        RenderMesh( meshFn, view );
        //std::cout<< "\tIs a mesh mesh " << meshFn.fullPathName().asChar() << " is selected" << std::endl;
      }
    }

    status = itDag.next();
    isDone = itDag.isDone( &status );
  }
}


MStatus CollisionDisplayNode::compute( const MPlug& plug, MDataBlock& data )
{ 
  return MS::kUnknownParameter;
}


void CollisionDisplayNode::draw
( 
 M3dView& view, 
 const MDagPath& path, 
 M3dView::DisplayStyle style,
 M3dView::DisplayStatus dispstatus 
 )
{ 
  MSelectionList list;
  MGlobal::getActiveSelectionList( list );
  //std::cout<<"DRAW\n";

  // Go ahead and get the drawing setup right here...
  view.beginGL();

  //view.setDrawColor( MColor( ( float )1.0,( float )1.0f,( float )1.0f, 0.5f ) );

  glPushAttrib( GL_ALL_ATTRIB_BITS /*GL_CURRENT_BIT | GL_DEPTH_BUFFER_BIT*/ );

  glShadeModel( GL_FLAT );
  glDepthFunc( GL_LEQUAL );

  static GLfloat factor=-3.0f, units=-1.0f;
  glEnable ( GL_POLYGON_OFFSET_FILL );
  glEnable( GL_CULL_FACE );
  glFrontFace( GL_CCW );
  glPolygonOffset( factor, units );

  MDagPath node;
  MObject component;
  MFnDagNode nodeFn;
  for ( unsigned int index = 0; index < list.length(); index++ )
  {
    list.getDagPath( index, node, component );
    nodeFn.setObject( node );
    std::string name = nodeFn.fullPathName().asChar();

    //if not coll then ignore
    if ( name.find( "|coll", 0 ) == std::string::npos )
    {
      continue;
    }
    if ( node.apiType() == MFn::kMesh )
    {
      // process mesh
      MFnMesh meshFn( node );
      RenderMesh( meshFn, view );
      // std::cout<< "Is a mesh mesh " <<nodeFn.name().asChar() << " is selected" << std::endl;
    }
    else if ( node.apiType() == MFn::kTransform )
    {
      //std::cout<< " Entering Transform " << nodeFn.fullPathName().asChar() << " is selected " << nodeFn.typeName().asChar()<<std::endl;
      IterateGroupNode( node, view );
    }

    //std::cout<< "Not a mesh " << nodeFn.name().asChar() << " is selected " << nodeFn.typeName().asChar()<<std::endl;
  } 

  glPopAttrib();	 
  view.endGL();		
}


void* CollisionDisplayNode::Creator()
{
  return new CollisionDisplayNode();
}