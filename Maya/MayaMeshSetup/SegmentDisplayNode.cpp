#include "stdafx.h"

#include "SegmentDisplayNode.h"

#include "../../MayaNodeTypes.h"

using namespace Maya;

#define MODE_DISPLAY_SEG_ID 2
#define MODE_UN_FALLGED_POLYS_ONLY 3
#define MODE_DISPLAY_CURRENT_SEG 4

MTypeId SegmentDisplayNode::m_NodeId( IGL_SEGMENT_DISP_NODE_ID );
MString SegmentDisplayNode::m_NodeName( "igSegmentDispNode" );

float g_SegmentColors [][3] = { {0.0f, 1.0f, 1.0f},
{1.0f, 0.0f, 0.0f},
{1.0f, 0.0f, 1.0f},
{1.0f, 1.0f, 0.0f},
{0.0f, 1.0f, 0.0f},
{1.0f, 0.5f, 0.5f},
{0.5f, 0.5f, 0.25f},
{0.5f, 0.25f, 0.5f},
{0.25f, 0.5f, 0.5f},
{0.75f, 0.35f, 0.25f},
{0.1f, 0.75f, 0.75f},
{0.6f, 0.75f, 0.75f},
{0.5f, 0.5f, 0.75f},
{0.75f, 0.15f, 0.75f},
{0.75f, 0.15f, 0.25f},
{0.75f, 0.75f, 0.5f}  };

static const float g_InvalidSegmentColor[] = {0.0, 0.0, 1.0};

MObject SegmentDisplayNode::displayOn;
MObject SegmentDisplayNode::currentSegment;


SegmentDisplayNode::SegmentDisplayNode()
{
}


SegmentDisplayNode::~SegmentDisplayNode()
{
}


MStatus SegmentDisplayNode::Initialize()
{
  MStatus status;
  MFnNumericAttribute nAttr; 

  displayOn = nAttr.create( "displayOn", "dp", MFnNumericData::kInt );
  nAttr.setDefault( 0 );

  status = addAttribute( displayOn );
  if ( !status ) 
  {
    status.perror( "addAttribute displayOn" );
    return status;
  }

  currentSegment = nAttr.create( "currentSegment", "cs", MFnNumericData::kInt );
  nAttr.setDefault( 0 );

  status = addAttribute( currentSegment );
  if ( !status ) 
  {
    status.perror( "addAttribute currentSegment" );
    return status;
  }
  return MS::kSuccess;
}


void SegmentDisplayNode::RenderMesh( MFnMesh& meshFn, M3dView& view, int displayMode, int& unsegmentedPolys, int currentSegment )
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

  float red = g_InvalidSegmentColor[0];
  float green = g_InvalidSegmentColor[1];
  float blue = g_InvalidSegmentColor[2];
  
  //std::cout<< "\tIs a mesh mesh " << meshFn.fullPathName().asChar() << " is selected " <<  red << ","<< green <<","<< blue<< std::endl;

  bool has_blind_data = false;
  if ( Maya::IsNodeVisible( currentNode ) )
  {
    //// Not always reliable
    //if ( meshFn.hasBlindData( MFn::kMeshPolygonComponent, SEGMENT_BLIND_DATA_ID, &status ) )
    //{
    //  has_blind_data = true;
    //}

    //if ( !has_blind_data )
    //{
    //  std::cout<<"Maya is stupid\n";
    //}

    MPointArray vertexArray;
    meshFn.getPoints( vertexArray, MSpace::kWorld );
    int numPolys = meshFn.numPolygons( &status );
    int segmentId = -1;

    for ( int faceIdx = 0 ; faceIdx < numPolys ; ++faceIdx )
    {
      //if ( has_blind_data )
      //{
      //  std::cout<<"Has blind data\n";
      //}

      segmentId = -1;
      meshFn.getIntBlindData( faceIdx, MFn::kMeshPolygonComponent, SEGMENT_BLIND_DATA_ID, "segmentId", segmentId );

      if ( displayMode == MODE_DISPLAY_CURRENT_SEG && segmentId != currentSegment )
      {
        continue;
      }

      if ( segmentId >=0 && segmentId <=15 )
      {
        sprintf( str, "Seg_%d", segmentId );
        red = g_SegmentColors[segmentId][0];
        green = g_SegmentColors[segmentId][1];
        blue = g_SegmentColors[segmentId][2];

        //if display only unfalgged polys mode then continue
        if ( displayMode == MODE_UN_FALLGED_POLYS_ONLY ) continue;
      }
      else
      {
        red = g_InvalidSegmentColor[0];
        green = g_InvalidSegmentColor[1];
        blue = g_InvalidSegmentColor[2];
        sprintf( str, "Seg_UnAssigned" );
        unsegmentedPolys++;
      }

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

        if ( displayMode == MODE_DISPLAY_SEG_ID || displayMode == MODE_UN_FALLGED_POLYS_ONLY ||  displayMode == MODE_DISPLAY_CURRENT_SEG )
        {
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
                if ( area*50 > view_area || displayMode == MODE_UN_FALLGED_POLYS_ONLY ||  displayMode == MODE_DISPLAY_CURRENT_SEG )
                {
                  glColor4f( 0.0, 0.0, 0.0, 1.0f );
                  MPoint textPos( center.x, center.y, center.z );
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

        if ( displayMode == MODE_DISPLAY_SEG_ID || displayMode == MODE_UN_FALLGED_POLYS_ONLY ||  displayMode == MODE_DISPLAY_CURRENT_SEG )
        {
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
                  if ( area*50 > view_area || displayMode == MODE_UN_FALLGED_POLYS_ONLY ||  displayMode == MODE_DISPLAY_CURRENT_SEG )
                  {
                    glColor4f( 0.0, 0.0, 0.0, 1.0f );
                    MPoint textPos( center.x, center.y, center.z );
                    MString distanceText( str );
                    view.drawText( distanceText, textPos, M3dView::kCenter );
                  }
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


void SegmentDisplayNode::IterateGroupNode( const MDagPath& path, M3dView& view, int displayMode, int& unsegmentedPolys, int currentSegment )
{
  MStatus status = MStatus::kSuccess;

  MDagPath dagPath;
  MItDag itDag( MItDag::kDepthFirst, MFn::kMesh, &status );
  status = itDag.reset( path, MItDag::kDepthFirst,  MFn::kMesh );

  bool isDone = itDag.isDone( &status );
  std::string last_mesh( "" ); //for some reason maya give it twice transform selected is not world parented

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
        RenderMesh( meshFn, view, displayMode, unsegmentedPolys, currentSegment );
        //std::cout<< "\tIs a mesh mesh " << meshFn.fullPathName().asChar() << " is selected" << std::endl;
      }
    }
    
    status = itDag.next();
    isDone = itDag.isDone( &status );
  }
}


MStatus SegmentDisplayNode::compute( const MPlug& plug, MDataBlock& data )
{ 
  return MS::kUnknownParameter;
}


void SegmentDisplayNode::draw
(
 M3dView& view,
 const MDagPath& path, 
 M3dView::DisplayStyle style,
 M3dView::DisplayStatus dispstatus 
 )
{ 
  MDagPath node;
  MObject component;
  MSelectionList list;
  MFnDagNode nodeFn;
  MGlobal::getActiveSelectionList( list );

  MStatus status;

  MFnDagNode seg_dag( thisMObject(), &status );
  if ( !status )
  {
    MGlobal::displayError( "Error: Cannot get hose dag node\n" );
  }
  seg_dag.setObject( thisMObject() );

  MPlug dispPlug = seg_dag.findPlug( "displayOn" );
  int displayMode = 0;
  dispPlug.getValue( displayMode );

  MPlug curSegPlug = seg_dag.findPlug( "currentSegment" );
  int currentSegment = 0;
  curSegPlug.getValue( currentSegment );

  if ( !displayMode ) 
  {
    //Is this necessary to output every frame? -- drr
    //std::cout<<"Display mode is "<<displayMode<<std::endl;
    return;
  }

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

  int numUnsegmentedPolys = 0;

  for ( unsigned int index = 0; index < list.length(); index++ )
  {
    list.getDagPath( index, node, component );
    nodeFn.setObject( node );
    std::string name = nodeFn.fullPathName().asChar();

    //if not coll then ignore
    //if ( ( name.find( "|geom", 0 ) == std::string::npos ) && ( name.find( "|mn", 0 ) == std::string::npos ) && ( name.find( "|bn", 0 ) == std::string::npos ) )
    //{
    //  continue;
    //}

    if ( node.apiType() == MFn::kMesh )
    {
      // process mesh
      MFnMesh meshFn( node );
      RenderMesh( meshFn, view, displayMode, numUnsegmentedPolys, currentSegment );
      // std::cout<< "Is a mesh mesh " <<nodeFn.name().asChar() << " is selected" << std::endl;
    }
    else if ( node.apiType() == MFn::kTransform )
    {
      //std::cout<< " Entering Transform " << nodeFn.fullPathName().asChar() << " is selected " << nodeFn.typeName().asChar()<<std::endl;
      IterateGroupNode( node, view, displayMode, numUnsegmentedPolys, currentSegment );
    }

    //std::cout<< "Not a mesh " << nodeFn.name().asChar() << " is selected " << nodeFn.typeName().asChar()<<std::endl;
  } 

  if ( displayMode == MODE_UN_FALLGED_POLYS_ONLY )
  {
    int view_width = view.portWidth( &status );
    int view_height = view.portHeight( &status );
    glColor4f( 0.0, 0.0, 0.0, 1.0f );
    
    MPoint textPos;
    MVector vec;
    view.viewToWorld( 0, 0, textPos, vec );

    char str[256];
    sprintf( str, "Un-segmented Poly Count: %d", numUnsegmentedPolys );
    MString distanceText( str );
    
    view.drawText( distanceText, textPos, M3dView::kLeft );
  }

  glPopAttrib();	 

  view.endGL();		
}


void* SegmentDisplayNode::Creator()
{
  return new SegmentDisplayNode();
}