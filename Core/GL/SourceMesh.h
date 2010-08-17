#pragma once

/*    
 SourceMesh.h
 
 Adapted from glm by Nate Robins, 1997, 2000
 nate@pobox.com, http://www.pobox.com/~nate
 
 Wavefront OBJ mesh file format reader/writer/manipulator.
 
 Includes routines for generating smooth normals with
 preservation of edges, welding redundant vertices & texture
 coordinate generation (spheremap and planar projections) + more. 
 */

#include "OpenGL.h"

namespace Core
{
  namespace Modes
  {
    enum Mode
    {
      None      = (0),           /* render with only vertices */
      Texture   = (1 << 1),      /* render with texture coords */
      Material  = (1 << 2),      /* render with materials */
      Default   = Texture | Material
    };
  }
  typedef Modes::Mode Mode;
  
  /* Material: Structure that defines a material in a mesh. */
  struct Material
  {
    char* name;                   /* name of material */
    GLfloat diffuse[4];           /* diffuse component */
    GLfloat ambient[4];           /* ambient component */
    GLfloat specular[4];          /* specular component */
    GLfloat shininess;            /* specular exponent */
    GLint texture;                /* color map */
  };
  
  /* Triangle: Structure that defines a triangle in a mesh. */
  struct Triangle
  {
    GLuint vindices[3];           /* array of triangle vertex indices */
    GLuint nindices[3];           /* array of triangle normal indices */
    GLuint tindices[3];           /* array of triangle uv indices*/
    GLuint material;
  };
  
  /* Texture: Structure that defines a texture in a mesh. */
  struct Texture
  {
    char* name;
    GLuint id;                    /* OpenGL texture ID */
  };
  
  /* Group: Structure that defines a group in a mesh. */
  struct Group
  {
    char*   name;           /* name of this group */
    GLuint  numtriangles;   /* number of triangles in this group */
    GLuint* triangles;      /* array of triangle indices */
    Group*  next;           /* pointer to next group in mesh */
  };
  
  /* SourceMesh: Structure that defines a mesh. */
  struct SourceMesh
  {
    const char* pathname;      /* path to this mesh */
    const char* mtllibname;    /* name of the material library */
    
    GLuint    numvertices;     /* number of vertices in mesh */
    GLfloat*  vertices;        /* array of vertices  */
    
    GLuint    numnormals;      /* number of normals in mesh */
    GLfloat*  normals;         /* array of normals */
    
    GLuint    numuvs;          /* number of uvs in mesh */
    GLfloat*  uvs;             /* array of texture coordinates */

    GLuint    numtriangles;    /* number of triangles in mesh */
    Triangle* triangles;       /* array of triangles */
    
    GLuint    nummaterials;    /* number of materials in mesh */
    Material* materials;       /* array of materials */
    
    GLuint    numgroups;       /* number of groups in mesh */
    Group*    groups;          /* linked list of groups */
    
    GLuint    numtextures;     /* number of textures in mesh */
    Texture*  textures;        /* array of textures */
    
    /* Construct/Destruct */
    SourceMesh();
    ~SourceMesh();
    
    /* Reset object and release all memory
     */
    void Reset();
    
    /* GetDimensions: Calculates the dimensions (width, height, depth) of
     * a mesh.
     *
     * dimensions - array of 3 GLfloats (GLfloat dimensions[3])
     */
    void GetDimensions(GLfloat* dimensions);
    
    /* Scale: Scales a mesh by a given amount.
     * 
     * scale - scalefactor (0.5 = half as large, 2.0 = twice as large)
     */
    void Scale(GLfloat scale);
    
    /* Unitize: "unitize" a mesh by translating it to the origin and
     * scaling it to fit in a unit cube around the origin.  Returns the
     * scalefactor used.
     */
    GLfloat Unitize();
    
    /* ReverseWinding: Reverse the polygon winding for all polygons in
     * this mesh.  Default winding is counter-clockwise.  Also changes
     * the direction of the normals.
     */
    void ReverseWinding();

    /* GenerateLinearTexCoords: Generates texture coordinates according to a
     * linear projection of the texture map.  It generates these by
     * linearly mapping the vertices onto a square.
     */
    void GenerateLinearTexCoords();
    
    /* GenerateSphericalTexCoords: Generates texture coordinates according to a
     * spherical projection of the texture map.  Sometimes referred to as
     * spheremap, or reflection map texture coordinates.  It generates
     * these by using the normal to calculate where that vertex would map
     * onto a sphere.  Since it is impossible to map something flat
     * perfectly onto something spherical, there is distortion at the
     * poles.  This particular implementation causes the poles along the X
     * axis to be distorted.
     */
    void GenerateSphericalTexCoords();
    
    /* ReadOBJ: Reads a mesh description from a Wavefront .OBJ file.
     * Returns a pointer to the created object which should be free'd with
     * Delete().
     */
    void ReadOBJ(const char* filename);
    
    /* WriteOBJ: Writes a mesh description in Wavefront .OBJ format to
     * a file.
     *
     * filename - name of the file to write the Wavefront .OBJ format data to
     * mode     - a bitwise or of values describing what is written to the file
     *            Modes::None    -  write only vertices
     *            Modes::Flat    -  write facet normals
     *            Modes::Smooth  -  write vertex normals
     *            Modes::Texture -  write texture coords
     *            Modes::Flat and Modes::Smooth should not both be specified.
     */
    void WriteOBJ(const char* filename, GLuint mode = Modes::Default);
  };
}
