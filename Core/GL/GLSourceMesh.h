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

#include "Foundation/File/Path.h"

namespace Helium
{
    namespace Core
    {
        namespace GL
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
                tstring m_Name;                   /* name of material */
                GLfloat m_Diffuse[4];           /* diffuse component */
                GLfloat m_Ambient[4];           /* ambient component */
                GLfloat m_Specular[4];          /* specular component */
                GLfloat m_Shininess;            /* specular exponent */
                GLint   m_Texture;                /* color map */
            };

            /* Triangle: Structure that defines a triangle in a mesh. */
            struct Triangle
            {
                GLuint m_VIndices[3];           /* array of triangle vertex indices */
                GLuint m_NIndices[3];           /* array of triangle normal indices */
                GLuint m_TIndices[3];           /* array of triangle uv indices*/
                GLuint m_Material;
            };

            /* Texture: Structure that defines a texture in a mesh. */
            struct Texture
            {
                tstring m_Name;
                GLuint  m_ID;                    /* OpenGL texture ID */
            };

            /* Group: Structure that defines a group in a mesh. */
            struct Group
            {
                tstring m_Name;           /* name of this group */
                GLuint  m_TriangleCount;   /* number of triangles in this group */
                GLuint* m_Triangles;      /* array of triangle indices */
                Group*  m_Next;           /* pointer to next group in mesh */
            };

            /* SourceMesh: Structure that defines a mesh. */
            class SourceMesh
            {
            public:
                Helium::Path m_Path;                /* path to this mesh */
                tstring      m_MaterialLibraryName; /* name of the material library */

                GLuint    m_VertexCount;     /* number of vertices in mesh */
                GLfloat*  m_Vertices;        /* array of vertices  */

                GLuint    m_NormalCount;      /* number of normals in mesh */
                GLfloat*  m_Normals;         /* array of normals */

                GLuint    m_UVCount;          /* number of uvs in mesh */
                GLfloat*  m_UVs;             /* array of texture coordinates */

                GLuint    m_TriangleCount;    /* number of triangles in mesh */
                Triangle* m_Triangles;       /* array of triangles */

                GLuint    m_MaterialCount;    /* number of materials in mesh */
                Material* m_Materials;       /* array of materials */

                GLuint    m_GroupCount;       /* number of groups in mesh */
                Group*    m_Groups;          /* linked list of groups */

                GLuint    m_TextureCount;     /* number of textures in mesh */
                Texture*  m_Textures;        /* array of textures */

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
                void ReadOBJ( const Path& path );

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
                void WriteOBJ( const Path& path, GLuint mode = Modes::Default );
            };
        }
    }
}