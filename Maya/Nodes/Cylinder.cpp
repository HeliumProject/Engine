#include "Precompile.h"
#include "Cylinder.h"

#include "MayaUtils/NodeTypes.h"

MObject Cylinder::s_DeletionSync;
const MTypeId Cylinder::s_TypeID( IGL_CYLINDER );
const char* Cylinder::s_TypeName = "cylinder";

MStatus Cylinder::Initialize()
{ 
  MFnNumericAttribute nAttr; 
  MStatus			 stat;

  s_DeletionSync = nAttr.create( "deletion_sync", "ds", MFnNumericData::kFloat );
  nAttr.setDefault( 0.0f );

  stat = addAttribute( s_DeletionSync );
  if ( !stat ) 
  {
    stat.perror( "addAttribute" );
    return stat;
  }

  return inheritAttributesFrom( Locator::s_TypeName );
}

MBoundingBox Cylinder::boundingBox() const
{   
  // Get the size
  MPoint corner1( -100.0, -100.0, -100.0 );
  MPoint corner2( 100.0, 100.0, 100.0 );

  return MBoundingBox( corner1, corner2 );
}

void Cylinder::draw( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status )
{ 
  Locator::draw( view, path, style, status );

  MFnDagNode parentDagNode;
  parentDagNode.setObject( MFnDagNode ( thisMObject() ).parent( 0 ) );

  MStatus mstatus;
  MPlug scaleX = parentDagNode.findPlug( "scaleX" );
 
  MMatrix begin_space_mat, goal_space_mat;

  MPlugArray scaleXPlugs;
  if( scaleX.connectedTo( scaleXPlugs, true, true ) && scaleXPlugs.length() == 3 )
  {
    MDagPath dp;
    MFnDagNode dn;
    for ( int i=0; i<3; i++ )
    {
       dn.setObject( scaleXPlugs[i].node() );
       if ( stricmp( dn.typeName().asChar(), "transform" ) == 0 )
       {
         dp = MDagPath::getAPathTo( dn.object(), &mstatus );
         if( !mstatus )
         {
           MString error ( "Error: Cannot get path to child piece of cylinder " );
           error +=  path.fullPathName().asChar();
           error += ". Get Reddy\n";
           MGlobal::displayError( error );
           return;
         }
         break;
       }
    }

    if( !mstatus )
    {
      MString error ( "Error: Cannot get path to child piece of cylinder " );
      error +=  path.fullPathName().asChar();
      error += "\n";
      MGlobal::displayError( error );
    }

    begin_space_mat = dp.inclusiveMatrix( &mstatus );
    if ( !mstatus )
    {
      MGlobal::displayError( "Error: Cannot get inclusive matrix to child piece of cylinder\n" );
    }

    dn.setObject( thisMObject() );
    dn.setObject( dn.parent( 0 ) );

    dp = MDagPath::getAPathTo( dn.object(), &mstatus );
    if( !mstatus )
    {
      MGlobal::displayError( "Error: Cannot get path to parent piece of cylinder\n" );
    }

    goal_space_mat = dp.inclusiveMatrix( &mstatus );
    if ( !mstatus )
    {
      MGlobal::displayError( "Error: Cannot get inclusive matrix to parent piece of cylinder\n" );
    }
  }

  // 
  MMatrix begin_to_goal_mat = begin_space_mat * goal_space_mat.inverse();

  // As there is no offset, the new_z position is the translation of the difference matrix
  MVector new_z;
  new_z.x = begin_to_goal_mat[3][0];
  new_z.y = begin_to_goal_mat[3][1];
  new_z.z = begin_to_goal_mat[3][2];

  double length = new_z.length();
  new_z.normalize();

  // Construct a reasonable x-axis for the new space
  MVector new_x;
  if( new_z.x < 0.99 && new_z.x > -0.99 )
  {
    new_x.x = 1.0;
    new_x.y = 0;
    new_x.z = 0;
  }
  else
  {
    new_x.x = 0.0;
    new_x.y = 1.0;
    new_x.z = 0.0;
  }

  // Project x on to z
  double dot = new_x * new_z;

  // The new x is its former value minus the its projected component on the new z-axis
  new_x = new_x - ( new_z * dot );
  new_x.normalize();

  // This is the easy one...  Cross x and z to get y
  MVector new_y = new_x ^ new_z;
  new_y.normalize();

  // Fill in the data for OpenGL
  double pray[4][4];

  pray[0][0] = new_x[0];
  pray[0][1] = new_x[1];
  pray[0][2] = new_x[2];
  pray[0][3] = 0;

  pray[1][0] = new_y[0];
  pray[1][1] = new_y[1];
  pray[1][2] = new_y[2];
  pray[1][3] = 0;

  pray[2][0] = new_z[0];
  pray[2][1] = new_z[1];
  pray[2][2] = new_z[2];
  pray[2][3] = 0;

  pray[3][0] = 0;
  pray[3][1] = 0;
  pray[3][2] = 0;
  pray[3][3] = 1;

  view.beginGL(); 

  checkGlErrors( "Problem before drawing cylinder" );

  glPushMatrix();	
  glMultMatrixd( ( GLdouble * )pray );

  GLUquadricObj *q = gluNewQuadric();
  if ( !drawWireframe( style ) )
  {
    gluQuadricDrawStyle( q, GLU_FILL );    
  }
  else
  {
    gluQuadricDrawStyle( q, GLU_LINE );
  }

  double r = 100.0;  // 1 meter in centimeters

  glTranslatef( 0, 0, 0 );    
  gluCylinder( q, r, r, length, 20, 5 );    

  gluDeleteQuadric( q );
  glPopMatrix();

  checkGlErrors( "Problem drawing cylinder" );
  
  view.endGL();
}

MStatus Cylinder::connectionBroken( const MPlug& plug, const MPlug& otherPlug, bool asSrc )
{
  if( plug == s_DeletionSync )
  {
    // The deletion of the transform is delayed until after this callback is complete.
    // The reason for the delay is that Maya will crash if a deletion of this object 
    // ( indirectly deleted when the transform is deleted ) happens while the code is executing 
    // within the callback.  This makes sense.  Deleting an object while running code within it
    // is a bad thing...

    MFnDagNode thisDagNode( thisMObject() );
    MGlobal::executeCommand( "source \"i8_collision\"" );
    MGlobal::executeCommand( "evalDeferred \"cleanupCylinder " + thisDagNode.fullPathName() + "\"" );
  }

  return MS::kUnknownParameter;
}

MObject CylinderChild::s_DeletionSync;
const MTypeId CylinderChild::s_TypeID( IGL_CYLINDER_CHILD );
const char* CylinderChild::s_TypeName = "cylinderChild";

MStatus CylinderChild::Initialize()
{
  MFnNumericAttribute nAttr; 
  MStatus			 stat;

  s_DeletionSync = nAttr.create( "deletion_sync", "ds", MFnNumericData::kFloat );
  nAttr.setDefault( 0.0f );

  stat = addAttribute( s_DeletionSync );
  if ( !stat ) 
  {
    stat.perror( "addAttribute" );
    return stat;
  }

  return inheritAttributesFrom( Locator::s_TypeName );
}

MStatus CylinderChild::connectionBroken( const MPlug& plug, const MPlug& otherPlug, bool asSrc )
{
  if( plug == s_DeletionSync )
  {
    // The deletion of the transform is delayed until after this callback is complete.
    // The reason for the delay is that Maya will crash if a deletion of this object 
    // ( indirectly deleted when the transform is deleted ) happens while the code is executing 
    // within the callback.  This makes sense.  Deleting an object while running code within it
    // is a bad thing...

    MFnDagNode thisDagNode( thisMObject() );
    MGlobal::executeCommand( "source \"i8_collision\"" );
    MGlobal::executeCommand( "evalDeferred \"cleanupCylinder " + thisDagNode.fullPathName() + "\"" );
  }

  return MS::kUnknownParameter;
}
