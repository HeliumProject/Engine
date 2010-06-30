#include "Precompile.h"

#include "DestructionCmd.h"

#include <maya/MArgDatabase.h>
#include <maya/MGlobal.h>
#include <maya/MPointArray.h>
#include <maya/MItSelectionList.h>
#include <maya/MItMeshEdge.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MFloatPointArray.h>
#include <maya/MFnMesh.h>
#include <maya/MDagPath.h>

#include <vector>
#include <list>
#include <map>
#include <set>
#include <stdarg.h>

#define BIGASS_FLOAT 1.0e37f

struct DestructionVec
{
  double m_x;
  double m_y;
  double m_z;
  double m_w;

  DestructionVec()
  {
    m_x = 0.0;
    m_y = 0.0;
    m_z = 0.0;
    m_w = 0.0;
  }
};

struct DestructionMatrix
{
  DestructionVec m_v0;
  DestructionVec m_v1;
  DestructionVec m_v2;
  DestructionVec m_v3;

  void Identity()
  {
    m_v0.m_x = 1.0f;
    m_v1.m_y = 1.0f;
    m_v2.m_z = 1.0f;
    m_v3.m_w = 1.0f;
  }
};

struct DestructionOBB
{
  DestructionVec m_v0;
  DestructionVec m_v1;
  DestructionVec m_v2;
  DestructionVec m_v3;
};

struct DestructionTri
{
  DestructionVec m_v0;
  DestructionVec m_v1;
  DestructionVec m_v2;
  DestructionVec m_normal;

  void Set(DestructionVec &v0, DestructionVec &v1, DestructionVec &v2, DestructionVec &normal)
  {
    m_v0 = v0;
    m_v1 = v1;
    m_v2 = v2;
    m_normal = normal;
  }
};


struct DestructionAabb
{
  DestructionVec m_min, m_max;
};


struct DestructionMesh
{
  DestructionAabb  m_aabb;
  DestructionTri  *m_tris;
  unsigned         m_ntris;
};

void SubVector3(DestructionVec &rv, const DestructionVec &v0, const DestructionVec &v1)
{
  rv.m_x = v0.m_x - v1.m_x;  
  rv.m_y = v0.m_y - v1.m_y;  
  rv.m_z = v0.m_z - v1.m_z;  
}

void AddVector3(DestructionVec &rv, const DestructionVec &v0, const DestructionVec &v1)
{
  rv.m_x = v0.m_x + v1.m_x;  
  rv.m_y = v0.m_y + v1.m_y;  
  rv.m_z = v0.m_z + v1.m_z;  
}

void ScalarMulVector3(DestructionVec &v0, double s)
{
  v0.m_x *= s;
  v0.m_y *= s;
  v0.m_z *= s;
}


double DotVector3(const DestructionVec &v0, const DestructionVec &v1)
{
  return v0.m_x * v1.m_x + v0.m_y * v1.m_y + v0.m_z * v1.m_z;
}

void NormalizeVector3(DestructionVec &v0)
{
  double rlen = 1.0f / sqrt(DotVector3(v0, v0));

  v0.m_x *= rlen;
  v0.m_y *= rlen;
  v0.m_z *= rlen;
}

void CrossVector3(DestructionVec &rv, const DestructionVec &v0, const DestructionVec &v1)
{
  rv.m_x = v0.m_y * v1.m_z - v0.m_z * v1.m_y;
  rv.m_y = v0.m_z * v1.m_x - v0.m_x * v1.m_z;
  rv.m_z = v0.m_x * v1.m_y - v0.m_y * v1.m_x;
}

void TriNormal(DestructionVec &normal, DestructionTri &tri)
{
  DestructionVec dv0, dv1;
  SubVector3(dv0, tri.m_v1, tri.m_v0);
  SubVector3(dv1, tri.m_v2, tri.m_v1);
  CrossVector3(normal, dv0, dv1);
  NormalizeVector3(normal);
}

void MinVector3(DestructionVec &rv, const DestructionVec &v0)
{
  rv.m_x = v0.m_x < rv.m_x ? v0.m_x : rv.m_x;
  rv.m_y = v0.m_y < rv.m_y ? v0.m_y : rv.m_y;
  rv.m_z = v0.m_z < rv.m_z ? v0.m_z : rv.m_z;
}

void MaxVector3(DestructionVec &rv, const DestructionVec &v0)
{
  rv.m_x = v0.m_x > rv.m_x ? v0.m_x : rv.m_x;
  rv.m_y = v0.m_y > rv.m_y ? v0.m_y : rv.m_y;
  rv.m_z = v0.m_z > rv.m_z ? v0.m_z : rv.m_z;
}

void LerpVector3(DestructionVec &rv, const DestructionVec &v0, const DestructionVec &v1, double s)
{
  rv.m_x = v0.m_x + (v1.m_x - v0.m_x) * s;
  rv.m_y = v0.m_y + (v1.m_y - v0.m_y) * s;
  rv.m_z = v0.m_z + (v1.m_z - v0.m_z) * s;
}


void AabbFromTris(DestructionAabb &aabb, DestructionTri *tris, unsigned ntris)
{
  aabb.m_min.m_x = BIGASS_FLOAT;
  aabb.m_min.m_y = BIGASS_FLOAT;
  aabb.m_min.m_z = BIGASS_FLOAT;
  aabb.m_max.m_x = -BIGASS_FLOAT;
  aabb.m_max.m_y = -BIGASS_FLOAT;
  aabb.m_max.m_z = -BIGASS_FLOAT;

  for (unsigned i=0; i<ntris; i++)
  {
    DestructionVec *verts = &tris[i].m_v0;
    for (unsigned j=0; j<3; j++)
    {
      MinVector3(aabb.m_min, verts[j]);
      MaxVector3(aabb.m_max, verts[j]);
    }
  }
}

unsigned VecMaxDim3(const DestructionVec &v0)
{
  unsigned idx = 0;
  double maxDim = v0.m_x;

  for (unsigned i=1; i<3; i++)
  {
    if ( ((double *)&v0.m_x)[i] > maxDim )
    {
      maxDim = ((double *)&v0.m_x)[i];
      idx = i;
    }
  }

  return idx;
}

void SplitTris(DestructionTri *tris0, unsigned &ntris0, DestructionTri *tris1, unsigned &ntris1, 
               DestructionTri *tris, unsigned ntris, DestructionVec &plane, unsigned max_tris)
{
  ntris0=0, ntris1=0;

  for (unsigned i=0; i<ntris; i++)
  {
    if ((ntris0 >= max_tris-1) || (ntris1 >= max_tris-1))
    {
      printf("SplitTris: buffer too small!\n");
      return;
    }

    DestructionVec v0 = tris[i].m_v0;
    DestructionVec v1 = tris[i].m_v1;
    DestructionVec v2 = tris[i].m_v2;

    // dot verts with plane
    double d0 = DotVector3(v0, plane) + plane.m_w;
    double d1 = DotVector3(v1, plane) + plane.m_w;
    double d2 = DotVector3(v2, plane) + plane.m_w;


    // if all verts on the same side of the plane, just add the triangle to the appropriate submesh
    unsigned mask = ((d2<0) << 2) | ((d1<0) << 1) | ((d0<0) << 0);
    if (mask == 0)
    {
      tris0[ntris0++] = tris[i];
      continue;
    }
    if (mask == 7)
    {
      tris1[ntris1++] = tris[i];
      continue;
    }

    // otherwise we must split the triangle into 3
    DestructionVec vt, v01, v02;
    double   dt;

    // order so that v0 is the odd one out
    if (mask==2 || mask==5)
    {
      vt=v0, v0=v1, v1=v2, v2=vt;
      dt=d0, d0=d1, d1=d2, d2=dt;
    }
    else if (mask==4 || mask==3)
    {
      vt=v2, v2=v1, v1=v0, v0=vt;
      dt=d2, d2=d1, d1=d0, d0=dt;
    }

    // generate intersection points
    LerpVector3(v01, v0, v1, -d0 / (d1-d0));
    LerpVector3(v02, v0, v2, -d0 / (d2-d0));

    // add the chopped-off tri, and 2 tris for the quad that's left
    DestructionVec &normal = tris[i].m_normal;
    if (d0 >= 0)
    {
      tris0[ntris0++].Set(v0,v01,v02,normal);
      tris1[ntris1++].Set(v1,v02,v01,normal);
      tris1[ntris1++].Set(v1,v2, v02,normal);
    }
    else
    {
      tris1[ntris1++].Set(v0,v01,v02,normal);
      tris0[ntris0++].Set(v1,v02,v01,normal);
      tris0[ntris0++].Set(v1,v2, v02,normal);
    }
  }
}


void SplitMesh(DestructionMesh &mesh0, DestructionMesh &mesh1, DestructionMesh &mesh, unsigned axis, double split, unsigned max_tris)
{
  // set up the splitting plane
  DestructionVec plane;
  plane.m_w = split;
  ((double *)&plane)[axis] = -1.0;

  // split mesh
  SplitTris(mesh0.m_tris, mesh0.m_ntris, mesh1.m_tris, mesh1.m_ntris, mesh.m_tris, mesh.m_ntris, plane, max_tris);

  // split aabb
  mesh0.m_aabb = mesh.m_aabb;
  mesh1.m_aabb = mesh.m_aabb;

  ((double *)&mesh0.m_aabb.m_max)[axis] = split;
  ((double *)&mesh1.m_aabb.m_min)[axis] = split;

  // shrink aabbs where possible
  AabbFromTris(mesh0.m_aabb, mesh0.m_tris, mesh0.m_ntris);
  AabbFromTris(mesh1.m_aabb, mesh1.m_tris, mesh1.m_ntris);
}

void CopyMesh(DestructionMesh &dest_mesh, DestructionMesh &src_mesh)
{
  for (unsigned i=0; i<src_mesh.m_ntris; i++)
  {
    dest_mesh.m_tris[i] = src_mesh.m_tris[i];
  }
  dest_mesh.m_aabb  = src_mesh.m_aabb;
  dest_mesh.m_ntris = src_mesh.m_ntris;
}

void OutputObb(DestructionOBB *obbs, DestructionMesh &mesh, unsigned &num_obbs, DestructionMatrix &mat_OW)
{
  DestructionVec min = mesh.m_aabb.m_min;
  DestructionVec max = mesh.m_aabb.m_max;
  DestructionVec center, radii;
  AddVector3(center, min, max);
  ScalarMulVector3(center, 0.5);
  SubVector3(radii, max, center);

  obbs[num_obbs].m_v0.m_x = 1;
  obbs[num_obbs].m_v0.m_y = 0;
  obbs[num_obbs].m_v0.m_z = 0;
  obbs[num_obbs].m_v0.m_w = radii.m_x;

  obbs[num_obbs].m_v1.m_x = 0;
  obbs[num_obbs].m_v1.m_y = 1;
  obbs[num_obbs].m_v1.m_z = 0;
  obbs[num_obbs].m_v1.m_w = radii.m_y;

  obbs[num_obbs].m_v2.m_x = 0;
  obbs[num_obbs].m_v2.m_y = 0;
  obbs[num_obbs].m_v2.m_z = 1;
  obbs[num_obbs].m_v2.m_w = radii.m_z;

  obbs[num_obbs].m_v3 = center;
  
  num_obbs++;
}

unsigned FitObbs(DestructionOBB *obbs, DestructionTri *tris, unsigned ntris, double granularity, unsigned max_obbs, char *scratch, unsigned scratch_size)
{
  unsigned num_obbs = 0;

  // scratchpad is now free again

  // array for intermediate meshes
  DestructionMesh meshes[4];

  // divide the scratch mem in 4 for tri arrays
  meshes[0].m_tris = (DestructionTri *)(scratch + 0*scratch_size/4);
  meshes[1].m_tris = (DestructionTri *)(scratch + 1*scratch_size/4);
  meshes[2].m_tris = (DestructionTri *)(scratch + 2*scratch_size/4);
  meshes[3].m_tris = (DestructionTri *)(scratch + 3*scratch_size/4);
  unsigned max_tris = (scratch_size/4) / sizeof(DestructionTri);

  // use identity matrix
  DestructionMatrix mat_OW;
  mat_OW.Identity();

  // copy tris into mesh
  for (unsigned i=0; i<ntris; i++)
  {
    meshes[0].m_tris[i].m_v0 = tris[i].m_v0;
    meshes[0].m_tris[i].m_v1 = tris[i].m_v1;
    meshes[0].m_tris[i].m_v2 = tris[i].m_v2;
  }

  // set up initial mesh containing the transformed tris
  meshes[0].m_ntris = ntris;
  AabbFromTris(meshes[0].m_aabb, meshes[0].m_tris, ntris);

  // find major axis and minor axes
  DestructionVec diff;
  SubVector3(diff, meshes[0].m_aabb.m_max, meshes[0].m_aabb.m_min);
  unsigned major_axis  = VecMaxDim3(diff);
  unsigned minor_axis1 = (major_axis+1) % 3;
  unsigned minor_axis2 = (major_axis+2) % 3;

  // split mesh 0 repeatedly along minor axis 1
  double min1 = ((double *)&meshes[0].m_aabb.m_min)[minor_axis1];
  double max1 = ((double *)&meshes[0].m_aabb.m_max)[minor_axis1];
  unsigned num1 = (unsigned)ceil((max1-min1) / granularity);
  double step1 = (max1-min1) / (double)num1;
  double split1 = min1 + step1;
  for (unsigned i=0; i<num1; i++, split1+=step1)
  {
    if (i<num1-1)
    {
      // split the mesh
      SplitMesh(meshes[1], meshes[2], meshes[0], minor_axis1, split1, max_tris);

      // no longer need source mesh
      CopyMesh(meshes[0], meshes[2]);
    }
    else
    {
      // num1==1: no need to split; or i==num1-1: use leftover half from previous split
      CopyMesh(meshes[1], meshes[0]);
    }
    
    // split mesh 1 repeatedly along minor axis 2
    double min2 = ((double *)&meshes[1].m_aabb.m_min)[minor_axis2];
    double max2 = ((double *)&meshes[1].m_aabb.m_max)[minor_axis2];
    unsigned num2 = (unsigned)ceil((max2-min2) / granularity);
    double step2 = (max2-min2) / (double)num2;
    double split2 = min2 + step2;
    for (unsigned j=0; j<num2; j++, split2+=step2)
    {
      if (j<num2-1)
      {
        // split the mesh
        SplitMesh(meshes[2], meshes[3], meshes[1], minor_axis2, split2, max_tris);

        // no longer need source mesh
        CopyMesh(meshes[1], meshes[3]);
      }
      else
      {
        // num2==1: no need to split; or j==num2-1: use leftover half from previous split
        CopyMesh(meshes[2], meshes[1]);
      }

      // output an obb for the mesh split off
      if (num_obbs < max_obbs)
      {
        OutputObb(obbs, meshes[2], num_obbs, mat_OW);
      }
    }
  }


  return num_obbs;
}


//#define DESTRUCTION_RUN_VERBOSE

void PrintDestructionTriangle(unsigned index, DestructionTri *tri)
{
  char tmp_string[512];
  sprintf(tmp_string, "  index: %d\n", index);
  MGlobal::displayInfo(tmp_string);
  sprintf(tmp_string, "     v0: %f %f %f\n", tri->m_v0.m_x, tri->m_v0.m_y, tri->m_v0.m_z);
  MGlobal::displayInfo(tmp_string);
  sprintf(tmp_string, "     v1: %f %f %f\n", tri->m_v1.m_x, tri->m_v1.m_y, tri->m_v1.m_z);
  MGlobal::displayInfo(tmp_string);
  sprintf(tmp_string, "     v2: %f %f %f\n", tri->m_v2.m_x, tri->m_v2.m_y, tri->m_v2.m_z);
  MGlobal::displayInfo(tmp_string);
  sprintf(tmp_string, "   norm: %f %f %f\n", tri->m_normal.m_x, tri->m_normal.m_y, tri->m_normal.m_z);
  MGlobal::displayInfo(tmp_string);

}

void PrintOBB(unsigned index, DestructionOBB *obb)
{
  char tmp_string[512];
  sprintf(tmp_string, "  index: %d\n", index);
  MGlobal::displayInfo(tmp_string);
  sprintf(tmp_string, "     v0: %f %f %f %f\n", obb->m_v0.m_x, obb->m_v0.m_y, obb->m_v0.m_z, obb->m_v0.m_w);
  MGlobal::displayInfo(tmp_string);
  sprintf(tmp_string, "     v1: %f %f %f %f\n", obb->m_v1.m_x, obb->m_v1.m_y, obb->m_v1.m_z, obb->m_v1.m_w);
  MGlobal::displayInfo(tmp_string);
  sprintf(tmp_string, "     v2: %f %f %f %f\n", obb->m_v2.m_x, obb->m_v2.m_y, obb->m_v2.m_z, obb->m_v2.m_w);
  MGlobal::displayInfo(tmp_string);
  sprintf(tmp_string, " center: %f %f %f\n", obb->m_v3.m_x, obb->m_v3.m_y, obb->m_v3.m_z);
  MGlobal::displayInfo(tmp_string);

}


double *g_DestructionPointData = NULL;
unsigned *g_DestructionIndexData = NULL;
unsigned g_DestructionIndexCount = 0;
unsigned g_DestructionVertexCount = 0;
unsigned g_DestructionCurPointCount = 0;
unsigned g_DestructionCurIndexCount = 0;
DestructionTri *g_DestructionTriangles = NULL;
unsigned g_NumDestructionTriangles = 0;
unsigned g_MaxOBB = 0;
double g_Granularity = 0.0;


MString DestructionCmd::CommandName( "destructionutil" );

DestructionCmd::DestructionCmd()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////

DestructionCmd::~DestructionCmd()
{
}

////////////////////////////////////////////////////////////////////////////////////////////////

MSyntax DestructionCmd::newSyntax()
{
  MSyntax syntax;

  syntax.addFlag("-vc", "-vertexcount", MSyntax::kLong);
  syntax.addFlag("-ic", "-indexcount", MSyntax::kLong);
  syntax.addFlag("-ap", "-addpoint", MSyntax::kDouble);
  syntax.addFlag("-ai", "-addindex", MSyntax::kLong);
  syntax.addFlag("-cl", "-clear", MSyntax::kBoolean);
  syntax.addFlag("-f", "-finalize", MSyntax::kDouble);
  syntax.addFlag("-sg", "-granularity", MSyntax::kDouble);
  syntax.addFlag("-mo","-maxobb", MSyntax::kLong);

  return syntax;
}


MStatus DestructionCmd::doIt(const MArgList &args)
{
  MStatus status;

  status = ParseArgs(args);
  if( status.statusCode() != MS::kSuccess )
  {
    return status;
  }

  return MS::kSuccess;
}

////////////////////////////////////////////////////////////////////////////////////////////////

void* DestructionCmd::creator()
{
  return new DestructionCmd();
}


MStatus DestructionCmd::ParseArgs(const MArgList &args)
{
  MStatus status;
  MArgDatabase arg_data(syntax(), args, &status);

  char info_buffer[512];

  if( status == MS::kSuccess && arg_data.isFlagSet( "-vc" ) )
  {
    if(g_DestructionVertexCount > 0)
    {
      sprintf(info_buffer, "num vertices already set, please clear\n");
      MGlobal::displayInfo(info_buffer);
    }
    else
    {
      status = arg_data.getFlagArgument( "-vc", 0, g_DestructionVertexCount );
      g_DestructionPointData = (double *)malloc(sizeof(double) * g_DestructionVertexCount * 3);
#ifdef DESTRUCTION_RUN_VERBOSE
      sprintf(info_buffer, "num vertices %d\n", g_DestructionVertexCount);
      MGlobal::displayInfo(info_buffer);
#endif
    }
  }
  if( status == MS::kSuccess && arg_data.isFlagSet( "-ic" ))
  {
    if(g_DestructionIndexCount > 0)
    {
      sprintf(info_buffer, "num indices already set, please clear\n");
      MGlobal::displayInfo(info_buffer);
    }
    else
    {
      status = arg_data.getFlagArgument( "-ic", 0, g_DestructionIndexCount );
      g_DestructionIndexData = (unsigned *)malloc(sizeof(unsigned) * g_DestructionIndexCount);
#ifdef DESTRUCTION_RUN_VERBOSE
      sprintf(info_buffer, "num indices %d\n", g_DestructionIndexCount);
      MGlobal::displayInfo(info_buffer);
#endif
    }
  }
  if( status == MS::kSuccess && arg_data.isFlagSet( "-ap" ))
  {
    status = arg_data.getFlagArgument( "-ap", 0, g_DestructionPointData[g_DestructionCurPointCount++] );
#ifdef DESTRUCTION_RUN_VERBOSE
    sprintf(info_buffer, "added point %f\n", g_DestructionPointData[g_DestructionCurPointCount-1]);
    MGlobal::displayInfo(info_buffer);
#endif
  }
  if( status == MS::kSuccess && arg_data.isFlagSet( "-ai" ))
  {
    status = arg_data.getFlagArgument( "-ai", 0, g_DestructionIndexData[g_DestructionCurIndexCount++] );
#ifdef DESTRUCTION_RUN_VERBOSE
    sprintf(info_buffer, "added index %d\n", g_DestructionIndexData[g_DestructionCurIndexCount-1]);
    MGlobal::displayInfo(info_buffer);
#endif
  }
  if( status == MS::kSuccess && arg_data.isFlagSet( "-sg" ))
  {
    status = arg_data.getFlagArgument( "-sg", 0, g_Granularity );
#ifdef DESTRUCTION_RUN_VERBOSE
    sprintf(info_buffer, "granularity %f\n", g_Granularity);
    MGlobal::displayInfo(info_buffer);
#endif
  }
  if( status == MS::kSuccess && arg_data.isFlagSet( "-mo" ))
  {
    status = arg_data.getFlagArgument( "-mo", 0, g_MaxOBB );
#ifdef DESTRUCTION_RUN_VERBOSE
    sprintf(info_buffer, "max obb %d\n", g_MaxOBB);
    MGlobal::displayInfo(info_buffer);
#endif
  }


  if( status == MS::kSuccess && arg_data.isFlagSet( "-f" ))
  {
    bool do_finalize;
    status = arg_data.getFlagArgument( "-f", 0, do_finalize );
    sprintf(info_buffer, "finalizing destruction util, generating obb (max = %d) with %f granularity\n", g_MaxOBB, g_Granularity);
    MGlobal::displayInfo(info_buffer);

    // we've finalized.. now it's time to re-construct the triangulated mesh
    unsigned num_triangles = g_DestructionIndexCount / 3;
    
    // allocate our DestructionTriangles
    g_DestructionTriangles = (DestructionTri *)malloc(sizeof(DestructionTri) * num_triangles);


    for(unsigned x=0;x<num_triangles;x++)
    {
      g_DestructionTriangles[x].m_v0.m_x = g_DestructionPointData[g_DestructionIndexData[x * 3 + 0] * 3 + 0];
      g_DestructionTriangles[x].m_v0.m_y = g_DestructionPointData[g_DestructionIndexData[x * 3 + 0] * 3 + 1];
      g_DestructionTriangles[x].m_v0.m_z = g_DestructionPointData[g_DestructionIndexData[x * 3 + 0] * 3 + 2];

      g_DestructionTriangles[x].m_v1.m_x = g_DestructionPointData[g_DestructionIndexData[x * 3 + 1] * 3 + 0];
      g_DestructionTriangles[x].m_v1.m_y = g_DestructionPointData[g_DestructionIndexData[x * 3 + 1] * 3 + 1];
      g_DestructionTriangles[x].m_v1.m_z = g_DestructionPointData[g_DestructionIndexData[x * 3 + 1] * 3 + 2];

      g_DestructionTriangles[x].m_v2.m_x = g_DestructionPointData[g_DestructionIndexData[x * 3 + 2] * 3 + 0];
      g_DestructionTriangles[x].m_v2.m_y = g_DestructionPointData[g_DestructionIndexData[x * 3 + 2] * 3 + 1];
      g_DestructionTriangles[x].m_v2.m_z = g_DestructionPointData[g_DestructionIndexData[x * 3 + 2] * 3 + 2];

      TriNormal(g_DestructionTriangles[x].m_normal, g_DestructionTriangles[x]);


      // print out the finished triangles
#ifdef DESTRUCTION_RUN_VERBOSE
      PrintDestructionTriangle(x, &g_DestructionTriangles[x]);
#endif
    }

    // now get us some obbs
    // temporarily allocate some obb's
    //unsigned max_obbs = 50;
    DestructionOBB *obbs = (DestructionOBB *)malloc(sizeof(DestructionOBB) * g_MaxOBB);

    char *scratch = (char *)malloc(1024 * 1024 * 10); // allocate 10 meg for scratch

    unsigned num_obb = FitObbs(obbs, g_DestructionTriangles, num_triangles, g_Granularity, g_MaxOBB, scratch, 1024 * 1024);

#ifdef DESTRUCTION_RUN_VERBOSE
    sprintf(info_buffer, "Generated %d obbs\n", num_obb);
    MGlobal::displayInfo(info_buffer);
#endif


    for(unsigned x=0;x<num_obb;x++)
    {
      status = MGlobal::executeCommand("select -cl", false, false);
      status = MGlobal::executeCommand("createCuboid", false, false);
      char command_str[512];
      sprintf(command_str, "move -absolute %f %f %f", obbs[x].m_v3.m_x, obbs[x].m_v3.m_y, obbs[x].m_v3.m_z);
      status = MGlobal::executeCommand(command_str, false, false);
      //sprintf(command_str, "scale -absolute %f %f %f", obbs[x].m_v0.m_w * 0.01, obbs[x].m_v1.m_w  * 0.01, obbs[x].m_v2.m_w  * 0.01);
      sprintf(command_str, "scale -absolute %f %f %f", obbs[x].m_v0.m_w, obbs[x].m_v1.m_w, obbs[x].m_v2.m_w);
      status = MGlobal::executeCommand(command_str, false, false);
#ifdef DESTRUCTION_RUN_VERBOSE
      PrintOBB(x, &obbs[x]);
#endif
    }

    free(scratch);
    free(obbs);

  }

  if( status == MS::kSuccess && arg_data.isFlagSet( "-cl" ))
  {
    bool clear_it;
    status = arg_data.getFlagArgument( "-cl", 0, clear_it );
    sprintf(info_buffer, "clearing destruction mesh info\n");
    MGlobal::displayInfo(info_buffer);
    if(g_DestructionPointData)
    {
      free(g_DestructionPointData);
    }
    if(g_DestructionIndexData)
    {
      free(g_DestructionIndexData);
    }
    if(g_DestructionTriangles)
    {
      free(g_DestructionTriangles);
    }
    g_DestructionPointData = NULL;
    g_DestructionIndexData = NULL;
    g_DestructionVertexCount = 0;
    g_DestructionIndexCount = 0;
    g_DestructionCurPointCount = 0;
    g_DestructionCurIndexCount = 0;
    g_DestructionTriangles = NULL;
  }




  return status;
}



////////////////////////////////////////////////////////////////////////////////////////////////
