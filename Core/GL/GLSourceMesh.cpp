/*    
Nate Robins, 1997, 2000
nate@pobox.com, http://www.pobox.com/~nate

Wavefront OBJ mesh file format reader/writer/manipulator.

Includes routines for generating smooth normals with
preservation of edges, welding redundant vertices & texture
coordinate generation (spheremap and planar projections) + more.

changes by F. Devernay:
- warning/error functions in glmutil.c
- added StrStrip function to handle filenames with spaces
- removed "static" from Draw variables (so that the code is reentrant)

changes by Geoff Evans:
- port to C++, OpenGL ES support, Apple support
- RenderMesh class w/ Draw functions
*/

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <windows.h> // ugh, need this to make windows GL work

#define MAX_SHININESS 100.0 /* for Poser */

#include "GLSourceMesh.h"
#include "GLTexture.h"
#include "GLUtil.h"

#include "Foundation/Log.h"
#include "Platform/String.h"

using namespace Helium;
using namespace Helium::Core;
using namespace Helium::Core::GL;

/* Max: returns the maximum of two floats */
inline static GLfloat Max(GLfloat a, GLfloat b) 
{
    if (b > a)
        return b;
    return a;
}

/* Abs: returns the absolute value of a float */
inline static GLfloat Abs(GLfloat f)
{
    if (f < 0)
        return -f;
    return f;
}

/* Dot: compute the dot product of two vectors
*
* u - array of 3 GLfloats (GLfloat u[3])
* v - array of 3 GLfloats (GLfloat v[3])
*/
inline static GLfloat Dot(GLfloat* u, GLfloat* v)
{
    assert(u); assert(v);

    return u[0]*v[0] + u[1]*v[1] + u[2]*v[2];
}

/* Cross: compute the cross product of two vectors
*
* u - array of 3 GLfloats (GLfloat u[3])
* v - array of 3 GLfloats (GLfloat v[3])
* n - array of 3 GLfloats (GLfloat n[3]) to return the cross product in
*/
inline static void Cross(GLfloat* u, GLfloat* v, GLfloat* n)
{
    assert(u); assert(v); assert(n);

    n[0] = u[1]*v[2] - u[2]*v[1];
    n[1] = u[2]*v[0] - u[0]*v[2];
    n[2] = u[0]*v[1] - u[1]*v[0];
}

/* Normalize: normalize a vector
*
* v - array of 3 GLfloats (GLfloat v[3]) to be normalized
*/
inline static void Normalize(GLfloat* v)
{
    assert(v);

    GLfloat l = (GLfloat)sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    v[0] /= l;
    v[1] /= l;
    v[2] /= l;
}

/* FindGroup: Find a group in the mesh */
inline static Group* FindGroup( SourceMesh* mesh, const tstring& name )
{
    HELIUM_ASSERT( mesh );

    for ( Group* group = mesh->m_Groups; group; group = group->m_Next)
    {
        if ( name == group->m_Name )
        {
            return group;
        }
    }

    return NULL;
}

/* AddGroup: Add a group to the mesh */
static Group* AddGroup( SourceMesh* mesh, const tstring& name )
{
    Group* group = FindGroup( mesh, name );
    if ( !group )
    {
        group = (Group*)malloc( sizeof( Group ) );
        group->m_Name = name;
        group->m_TriangleCount = 0;
        group->m_Triangles = NULL;
        group->m_Next = mesh->m_Groups;
        mesh->m_Groups = group;
        mesh->m_GroupCount++;
    }

    return group;
}

/* FindGroup: Find a material in the mesh */
static GLuint FindMaterial( SourceMesh* mesh, const tstring& name )
{
    HELIUM_ASSERT( !name.empty() );

    /* XXX doing a linear search on a string key'd list is pretty lame,
    but it works and is fast enough for now. */
    for ( GLuint i = 0; i < mesh->m_MaterialCount; i++ )
    {
        if ( mesh->m_Materials[ i ].m_Name == name )
        {
            return i;
        }
    }

    /* didn't find the name, so print a warning and return the default material (0). */
    Log::Warning( TXT( "FindMaterial():  can't find material \"%s\"." ), name);
    return 0;
}

/* FindTexture: Find a texture in the mesh
*
* name  - name of the texture
*/
static GLuint FindOrAddTexture(SourceMesh* mesh, const tstring& name)
{
    /* XXX doing a linear search on a string key'd list is pretty lame, but it works and is fast enough for now. */
    for ( GLuint i = 0; i < mesh->m_TextureCount; i++ )
    {
        if ( mesh->m_Textures[ i ].m_Name == name )
        {
            return i;
        }
    }

    /* didn't find the name, so print a warning and return the default texture (0). */
    mesh->m_TextureCount++;
    mesh->m_Textures = (Texture*)realloc(mesh->m_Textures, sizeof(Texture)*mesh->m_TextureCount);
    mesh->m_Textures[ mesh->m_TextureCount - 1 ].m_Name = name;
    Path texturePath( mesh->m_Path.Directory() + name );
    mesh->m_Textures[ mesh->m_TextureCount - 1 ].m_ID = LoadTexture( texturePath.c_str(), GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE );

    DBG( Log::Warning( TXT( "allocated texture %d (id=%d)" ),mesh->m_TextureCount - 1, mesh->m_Textures[ mesh->m_TextureCount - 1 ].m_ID ) );

    return mesh->m_TextureCount - 1;
}

/* ReadMTL: read a wavefront material library file
*
* name  - name of the material library
*/
static void ReadMTL( SourceMesh* mesh, const tstring& name )
{
    FILE* file;
    char buf[128];

    tstring filename = mesh->m_Path.Directory() + name;
    file = _tfopen( filename.c_str(), TXT( "r" ) );
    if ( !file )
    {
        Log::Error( TXT( "ReadMTL() failed: can't open material file \"%s\"." ), filename.c_str() );
    }

    /* count the number of materials in the file */
    while( fscanf( file, "%s", buf ) != EOF )
    {
        switch( buf[0] )
        {
        case '#': /* comment */
            {
                /* eat up rest of line */
                fgets( buf, sizeof( buf ), file );
                break;
            }

        case 'n': /* newmtl */
            {
                if( strncmp( buf, "newmtl", 6 ) != 0 )
                {
                    Log::Error( TXT( "ReadMTL: Got \"%s\" instead of \"newmtl\" in file \"%s\"" ), buf, filename.c_str() );
                }

                fgets( buf, sizeof( buf ), file );
                mesh->m_MaterialCount++;
                sscanf( buf, "%s %s", buf, buf );
                break;
            }

        default: /* eat up rest of line */
            {
                fgets( buf, sizeof( buf ), file );
                break;
            }
        }
    }

    rewind(file);

    mesh->m_Materials = (Material*)malloc(sizeof(Material) * mesh->m_MaterialCount);

    /* set the default material */
    for (GLuint i = 0; i < mesh->m_MaterialCount; i++)
    {
        mesh->m_Materials[i].m_Name = TXT( "" );
        mesh->m_Materials[i].m_Ambient[0] = 0.2f;
        mesh->m_Materials[i].m_Ambient[1] = 0.2f;
        mesh->m_Materials[i].m_Ambient[2] = 0.2f;
        mesh->m_Materials[i].m_Ambient[3] = 1.0f;
        mesh->m_Materials[i].m_Diffuse[0] = 0.8f;
        mesh->m_Materials[i].m_Diffuse[1] = 0.8f;
        mesh->m_Materials[i].m_Diffuse[2] = 0.8f;
        mesh->m_Materials[i].m_Diffuse[3] = 1.0f;
        mesh->m_Materials[i].m_Specular[0] = 0.0f;
        mesh->m_Materials[i].m_Specular[1] = 0.0f;
        mesh->m_Materials[i].m_Specular[2] = 0.0f;
        mesh->m_Materials[i].m_Specular[3] = 1.0f;
        mesh->m_Materials[i].m_Shininess = 65.0f;
        mesh->m_Materials[i].m_Texture = -1;
    }
    mesh->m_Materials[0].m_Name = TXT( "default" );

    GLuint next_index = 0;
    Material* current_material = NULL;

    /* now, read in the data */
    while(fscanf(file, "%s", buf) != EOF)
    {
        switch(buf[0])
        {
        case '#': /* comment */
            {
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
            }

        case 'n': /* newmtl */
            {
                if(strncmp(buf, "newmtl", 6) != 0)
                {
                    Log::Error( TXT( "ReadMTL: Got \"%s\" instead of \"newmtl\" in file \"%s\"" ), buf, filename.c_str() );
                }

                fgets(buf, sizeof(buf), file);
                sscanf(buf, "%s %s", buf, buf);
                current_material = &mesh->m_Materials[next_index++];
                Helium::ConvertString( buf, current_material->m_Name );
                break;
            }

        case 'N':
            {
                switch(buf[1])
                {
                case 's':
                    {
                        fscanf(file, "%f", &current_material->m_Shininess);

                        /* wavefront shininess is from [0, 1000], so scale for OpenGL */
                        current_material->m_Shininess /= MAX_SHININESS;
                        current_material->m_Shininess *= 128.0;
                        break;
                    }

                default:
                    {
                        Log::Warning( TXT( "ReadMTL: Command \"%s\" ignored") , buf );
                        fgets(buf, sizeof(buf), file);
                        break;
                    }
                }
                break;
            }

        case 'K':
            {
                switch(buf[1])
                {
                case 'a':
                    {
                        fscanf(file, "%f %f %f",
                            &current_material->m_Ambient[0],
                            &current_material->m_Ambient[1],
                            &current_material->m_Ambient[2]);
                        break;
                    }

                case 'd':
                    {
                        fscanf(file, "%f %f %f",
                            &current_material->m_Diffuse[0],
                            &current_material->m_Diffuse[1],
                            &current_material->m_Diffuse[2]);
                        break;
                    }

                case 's':
                    {
                        fscanf(file, "%f %f %f",
                            &current_material->m_Specular[0],
                            &current_material->m_Specular[1],
                            &current_material->m_Specular[2]);
                        break;
                    }

                default:
                    {
                        Log::Warning( TXT( "ReadMTL: Command \"%s\" ignored" ), buf );
                        fgets( buf, sizeof( buf ), file );
                        break;
                    }
                }
                break;
            }

        case 'd': /* d = Dissolve factor (pseudo-transparency). Values are from 0-1. 0 is completely transparent, 1 is opaque. */
            {
                float alpha;
                fscanf(file, "%f", &alpha);
                current_material->m_Diffuse[3] = alpha;
                break;
            }

        case 'm': /* texture map */
            {
                char filenameBuf[ FILENAME_MAX ];
                fgets( filenameBuf, FILENAME_MAX, file );
                tstring temp;
                Helium::ConvertString( filenameBuf, temp );
                Path path( temp );

                if(strncmp(buf, "map_Kd", 6) == 0)
                {
                    current_material->m_Texture = FindOrAddTexture( mesh, path.c_str() );
                }
                else
                {
                    Log::Warning( TXT( "map %s %s ignored" ), buf, path.c_str() );
                    fgets( buf, sizeof( buf ), file );
                }
                break;
            }

        default:
            {
                Log::Warning( TXT( "ReadMTL: Command \"%s\" ignored" ), buf );
                fgets( buf, sizeof( buf ), file );
                break;
            }
        }
    }

    fclose(file);
}

/* WriteMTL: write a wavefront material library file
*
* mesh   - properly initialized SourceMesh structure
* meshpath  - pathname of the mesh being written
* mtllibname - name of the material library to be written
*/
static void WriteMTL( SourceMesh* mesh, const Path& meshPath, const tstring& materialLibraryName )
{
    FILE* file;
    Material* material;
    GLuint i;

    Path materialLibraryPath( meshPath.Directory() + materialLibraryName );
    /* open the file */
    file = _tfopen( materialLibraryPath.c_str(), TXT( "w" ) );
    if (!file)
    {
        Log::Error( TXT( "WriteMTL() failed: can't open file \"%s\"." ), materialLibraryPath.c_str() );
    }

    /* spit out a header */
    fprintf(file, "#  \n");
    fprintf(file, "#  Wavefront MTL generated by Core library\n");
    fprintf(file, "#  \n");
    fprintf(file, "#  Core library\n");
    fprintf(file, "#  Nate Robins\n");
    fprintf(file, "#  ndr@pobox.com\n");
    fprintf(file, "#  http://www.pobox.com/~ndr\n");
    fprintf(file, "#  \n\n");

    for (i = 0; i < mesh->m_MaterialCount; i++)
    {
        material = &mesh->m_Materials[i];
        fprintf(file, "newmtl %s\n", material->m_Name);
        fprintf(file, "Ka %f %f %f\n", 
            material->m_Ambient[0], material->m_Ambient[1], material->m_Ambient[2]);
        fprintf(file, "Kd %f %f %f\n", 
            material->m_Diffuse[0], material->m_Diffuse[1], material->m_Diffuse[2]);
        fprintf(file, "Ks %f %f %f\n", 
            material->m_Specular[0],material->m_Specular[1],material->m_Specular[2]);
        fprintf(file, "Ns %f\n", material->m_Shininess / 128.0 * MAX_SHININESS);
        fprintf(file, "\n");
    }
}

/* FirstPass: first pass at a Wavefront OBJ file that gets all the
* statistics of the mesh (such as #vertices, #normals, etc)
*
* file  - (fopen'd) file descriptor 
*/
static void FirstPass(SourceMesh* mesh, FILE* file) 
{
    GLuint numvertices;        /* number of vertices in mesh */
    GLuint numnormals;         /* number of normals in mesh */
    GLuint numuvs;             /* number of uvs in mesh */
    GLuint numtriangles;       /* number of triangles in mesh */
    Group* group;               /* current group */
    unsigned v, n, t;
    char buf[128];

    /* make a default group */
    group = AddGroup( mesh, TXT( "default" ) );

    numvertices = numnormals = numuvs = numtriangles = 0;
    while(fscanf(file, "%s", buf) != EOF)
    {
        switch(buf[0])
        {
        case '#': /* comment */
            {
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
            }

        case 'v': /* v, vn, vt */
            {
                switch(buf[1])
                {
                case '\0': /* vertex */
                    {
                        /* eat up rest of line */
                        fgets(buf, sizeof(buf), file);
                        numvertices++;
                        break;
                    }

                case 'n': /* normal */
                    {
                        /* eat up rest of line */
                        fgets(buf, sizeof(buf), file);
                        numnormals++;
                        break;
                    }

                case 't': /* uv */
                    {
                        /* eat up rest of line */
                        fgets(buf, sizeof(buf), file);
                        numuvs++;
                        break;
                    }

                default:
                    {
                        Log::Warning( TXT( "FirstPass(): Unknown token \"%s\"." ), buf );
                        break;
                    }
                }

                break;
            }

        case 'm':
            {
                if(strncmp(buf, "mtllib", 6) != 0)
                {
                    Log::Error( TXT( "ReadOBJ: Got \"%s\" instead of \"mtllib\"" ), buf );
                }

                fgets(buf, sizeof(buf), file);
                sscanf(buf, "%s %s", buf, buf);
                Helium::ConvertString( buf, mesh->m_MaterialLibraryName );
                ReadMTL( mesh, mesh->m_MaterialLibraryName );
                break;
            }

        case 'u': /* material name */
            {
                if(strncmp(buf, "usemtl", 6) != 0)
                {
                    Log::Error( TXT( "ReadOBJ: Got \"%s\" instead of \"usemtl\"" ), buf );
                }

                fgets( buf, sizeof( buf ), file );
                break;
            }

        case 'g': /* group */
            {
                /* eat up rest of line */
                fgets( buf, sizeof( buf ), file );

                /* nuke '\n' */
                buf[ strlen( buf ) - 1 ] = '\0';

                tstring temp;
                Helium::ConvertString( &buf[ 1 ], temp );
                group = AddGroup( mesh, temp );
                break;
            }

        case 'f': /* face */
            {
                v = n = t = 0;
                fscanf( file, "%s", buf );

                /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                if ( strstr( buf, "//" ) )
                {
                    /* v//n */
                    sscanf(buf, "%d//%d", &v, &n);
                    fscanf(file, "%d//%d", &v, &n);
                    fscanf(file, "%d//%d", &v, &n);
                    numtriangles++;
                    group->m_TriangleCount++;
                    while(fscanf(file, "%d//%d", &v, &n) > 0)
                    {
                        numtriangles++;
                        group->m_TriangleCount++;
                    }
                }
                else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3)
                {
                    /* v/t/n */
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    fscanf(file, "%d/%d/%d", &v, &t, &n);
                    numtriangles++;
                    group->m_TriangleCount++;
                    while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0)
                    {
                        numtriangles++;
                        group->m_TriangleCount++;
                    }
                }
                else if (sscanf(buf, "%d/%d", &v, &t) == 2)
                {
                    /* v/t */
                    fscanf(file, "%d/%d", &v, &t);
                    fscanf(file, "%d/%d", &v, &t);
                    numtriangles++;
                    group->m_TriangleCount++;
                    while(fscanf(file, "%d/%d", &v, &t) > 0)
                    {
                        numtriangles++;
                        group->m_TriangleCount++;
                    }
                }
                else
                {
                    /* v */
                    fscanf(file, "%d", &v);
                    fscanf(file, "%d", &v);
                    numtriangles++;
                    group->m_TriangleCount++;
                    while(fscanf(file, "%d", &v) > 0)
                    {
                        numtriangles++;
                        group->m_TriangleCount++;
                    }
                }
                break;
            }

        default: /* eat up rest of line */
            {
                fgets(buf, sizeof(buf), file);
                break;
            }
        }
    }

    /* set the stats in the mesh structure */
    mesh->m_VertexCount  = numvertices;
    mesh->m_NormalCount = numnormals;
    mesh->m_UVCount = numuvs;
    mesh->m_TriangleCount = numtriangles;

    /* allocate memory for the triangles in each group */
    group = mesh->m_Groups;
    while( group )
    {
        group->m_Triangles = (GLuint*)malloc( sizeof( GLuint ) * group->m_TriangleCount );
        group->m_TriangleCount = 0;
        group = group->m_Next;
    }
}

/* SecondPass: second pass at a Wavefront OBJ file that gets all
* the data.
*
* file  - (fopen'd) file descriptor 
*/
static void SecondPass(SourceMesh* mesh, FILE* file) 
{
    GLuint numvertices;   /* number of vertices in mesh */
    GLuint numnormals;    /* number of normals in mesh */
    GLuint numuvs;        /* number of uvs in mesh */
    GLuint numtriangles;  /* number of triangles in mesh */
    GLfloat* vertices;    /* array of vertices  */
    GLfloat* normals;     /* array of normals */
    GLfloat* uvs;         /* array of texture coordinates */
    Group* group;         /* current group pointer */
    GLuint material;      /* current material */
    unsigned int v, n, t;
    char buf[128];

    /* set the pointer shortcuts */
    vertices = mesh->m_Vertices;
    normals = mesh->m_Normals;
    uvs = mesh->m_UVs;
    group = mesh->m_Groups;

    /* on the second pass through the file, read all the data into the
    allocated arrays */
    numvertices = numnormals = numuvs = 1;
    numtriangles = 0;
    material = 0;
    while(fscanf(file, "%s", buf) != EOF)
    {
        switch(buf[0])
        {
        case '#': /* comment */
            {
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);
                break;
            }

        case 'v': /* v, vn, vt */
            {
                switch(buf[1])
                {
                case '\0': /* vertex */
                    {
                        fscanf(file, "%f %f %f", 
                            &vertices[3 * numvertices + 0], 
                            &vertices[3 * numvertices + 1], 
                            &vertices[3 * numvertices + 2]);
                        numvertices++;
                        break;
                    }

                case 'n': /* normal */
                    {
                        fscanf(file, "%f %f %f", 
                            &normals[3 * numnormals + 0],
                            &normals[3 * numnormals + 1], 
                            &normals[3 * numnormals + 2]);
                        numnormals++;
                        break;
                    }

                case 't': /* uv */
                    {
                        fscanf(file, "%f %f", 
                            &uvs[2 * numuvs + 0],
                            &uvs[2 * numuvs + 1]);
                        numuvs++;
                        break;
                    }
                }
                break;
            }

        case 'm': /* mtllib */
            {
                fgets(buf, sizeof(buf), file);
                break;
            }

        case 'u': /* usemtl */
            {
                fgets(buf, sizeof(buf), file);
                sscanf(buf, "%s %s", buf, buf);
                tstring temp;
                Helium::ConvertString( buf, temp );
                material = FindMaterial( mesh, temp );
                break;
            }

        case 'g': /* group */
            {
                /* eat up rest of line */
                fgets(buf, sizeof(buf), file);

                /* nuke '\n' */
                buf[strlen(buf)-1] = '\0';

                tstring temp;
                Helium::ConvertString( &buf[ 1 ], temp );
                group = FindGroup( mesh, temp );
                break;
            }

        case 'f': /* face */
            {
                v = n = t = 0;        
                fscanf(file, "%s", buf);

                /* can be one of %d, %d//%d, %d/%d, %d/%d/%d %d//%d */
                if (strstr(buf, "//"))
                {
                    /* v//n */
                    sscanf(buf, "%u//%u", &v, &n);
                    mesh->m_Triangles[numtriangles].m_VIndices[0] = v;
                    mesh->m_Triangles[numtriangles].m_TIndices[0] = -1;
                    mesh->m_Triangles[numtriangles].m_NIndices[0] = n;
                    fscanf(file, "%u//%u", &v, &n);
                    mesh->m_Triangles[numtriangles].m_VIndices[1] = v;
                    mesh->m_Triangles[numtriangles].m_TIndices[1] = -1;
                    mesh->m_Triangles[numtriangles].m_NIndices[1] = n;
                    fscanf(file, "%u//%u", &v, &n);
                    mesh->m_Triangles[numtriangles].m_VIndices[2] = v;
                    mesh->m_Triangles[numtriangles].m_TIndices[2] = -1;
                    mesh->m_Triangles[numtriangles].m_NIndices[2] = n;
                    mesh->m_Triangles[numtriangles].m_Material = material;
                    group->m_Triangles[group->m_TriangleCount++] = numtriangles;
                    numtriangles++;

                    while(fscanf(file, "%u//%u", &v, &n) > 0)
                    {
                        mesh->m_Triangles[numtriangles].m_VIndices[0] = mesh->m_Triangles[numtriangles-1].m_VIndices[0];
                        mesh->m_Triangles[numtriangles].m_TIndices[0] = mesh->m_Triangles[numtriangles-1].m_TIndices[0];
                        mesh->m_Triangles[numtriangles].m_NIndices[0] = mesh->m_Triangles[numtriangles-1].m_NIndices[0];
                        mesh->m_Triangles[numtriangles].m_VIndices[1] = mesh->m_Triangles[numtriangles-1].m_VIndices[2];
                        mesh->m_Triangles[numtriangles].m_TIndices[1] = mesh->m_Triangles[numtriangles-1].m_TIndices[2];
                        mesh->m_Triangles[numtriangles].m_NIndices[1] = mesh->m_Triangles[numtriangles-1].m_NIndices[2];
                        mesh->m_Triangles[numtriangles].m_VIndices[2] = v;
                        mesh->m_Triangles[numtriangles].m_TIndices[2] = -1;
                        mesh->m_Triangles[numtriangles].m_NIndices[2] = n;
                        mesh->m_Triangles[numtriangles].m_Material = material;
                        group->m_Triangles[group->m_TriangleCount++] = numtriangles;
                        numtriangles++;
                    }
                }
                else if (sscanf(buf, "%u/%u/%u", &v, &t, &n) == 3)
                {
                    /* v/t/n */
                    mesh->m_Triangles[numtriangles].m_VIndices[0] = v;
                    mesh->m_Triangles[numtriangles].m_TIndices[0] = t;
                    mesh->m_Triangles[numtriangles].m_NIndices[0] = n;
                    fscanf(file, "%u/%u/%u", &v, &t, &n);
                    mesh->m_Triangles[numtriangles].m_VIndices[1] = v;
                    mesh->m_Triangles[numtriangles].m_TIndices[1] = t;
                    mesh->m_Triangles[numtriangles].m_NIndices[1] = n;
                    fscanf(file, "%u/%u/%u", &v, &t, &n);
                    mesh->m_Triangles[numtriangles].m_VIndices[2] = v;
                    mesh->m_Triangles[numtriangles].m_TIndices[2] = t;
                    mesh->m_Triangles[numtriangles].m_NIndices[2] = n;
                    mesh->m_Triangles[numtriangles].m_Material = material;
                    group->m_Triangles[group->m_TriangleCount++] = numtriangles;
                    numtriangles++;
                    while(fscanf(file, "%u/%u/%u", &v, &t, &n) > 0)
                    {
                        mesh->m_Triangles[numtriangles].m_VIndices[0] = mesh->m_Triangles[numtriangles-1].m_VIndices[0];
                        mesh->m_Triangles[numtriangles].m_TIndices[0] = mesh->m_Triangles[numtriangles-1].m_TIndices[0];
                        mesh->m_Triangles[numtriangles].m_NIndices[0] = mesh->m_Triangles[numtriangles-1].m_NIndices[0];
                        mesh->m_Triangles[numtriangles].m_VIndices[1] = mesh->m_Triangles[numtriangles-1].m_VIndices[2];
                        mesh->m_Triangles[numtriangles].m_TIndices[1] = mesh->m_Triangles[numtriangles-1].m_TIndices[2];
                        mesh->m_Triangles[numtriangles].m_NIndices[1] = mesh->m_Triangles[numtriangles-1].m_NIndices[2];
                        mesh->m_Triangles[numtriangles].m_VIndices[2] = v;
                        mesh->m_Triangles[numtriangles].m_TIndices[2] = t;
                        mesh->m_Triangles[numtriangles].m_NIndices[2] = n;
                        mesh->m_Triangles[numtriangles].m_Material = material;
                        group->m_Triangles[group->m_TriangleCount++] = numtriangles;
                        numtriangles++;
                    }
                }
                else if (sscanf(buf, "%u/%u", &v, &t) == 2)
                {
                    /* v/t */
                    mesh->m_Triangles[numtriangles].m_VIndices[0] = v;
                    mesh->m_Triangles[numtriangles].m_TIndices[0] = t;
                    mesh->m_Triangles[numtriangles].m_NIndices[0] = -1;
                    fscanf(file, "%u/%u", &v, &t);
                    mesh->m_Triangles[numtriangles].m_VIndices[1] = v;
                    mesh->m_Triangles[numtriangles].m_TIndices[1] = t;
                    mesh->m_Triangles[numtriangles].m_NIndices[1] = -1;
                    fscanf(file, "%u/%u", &v, &t);
                    mesh->m_Triangles[numtriangles].m_VIndices[2] = v;
                    mesh->m_Triangles[numtriangles].m_TIndices[2] = t;
                    mesh->m_Triangles[numtriangles].m_NIndices[2] = -1;
                    mesh->m_Triangles[numtriangles].m_Material = material;
                    group->m_Triangles[group->m_TriangleCount++] = numtriangles;
                    numtriangles++;
                    while(fscanf(file, "%u/%u", &v, &t) > 0)
                    {
                        mesh->m_Triangles[numtriangles].m_VIndices[0] = mesh->m_Triangles[numtriangles-1].m_VIndices[0];
                        mesh->m_Triangles[numtriangles].m_TIndices[0] = mesh->m_Triangles[numtriangles-1].m_TIndices[0];
                        mesh->m_Triangles[numtriangles].m_NIndices[0] = mesh->m_Triangles[numtriangles-1].m_NIndices[0];
                        mesh->m_Triangles[numtriangles].m_VIndices[1] = mesh->m_Triangles[numtriangles-1].m_VIndices[2];
                        mesh->m_Triangles[numtriangles].m_TIndices[1] = mesh->m_Triangles[numtriangles-1].m_TIndices[2];
                        mesh->m_Triangles[numtriangles].m_NIndices[1] = mesh->m_Triangles[numtriangles-1].m_NIndices[2];
                        mesh->m_Triangles[numtriangles].m_VIndices[2] = v;
                        mesh->m_Triangles[numtriangles].m_TIndices[2] = t;
                        mesh->m_Triangles[numtriangles].m_NIndices[2] = -1;
                        mesh->m_Triangles[numtriangles].m_Material = material;
                        group->m_Triangles[group->m_TriangleCount++] = numtriangles;
                        numtriangles++;
                    }
                }
                else
                {
                    /* v */
                    sscanf(buf, "%u", &v);
                    mesh->m_Triangles[numtriangles].m_VIndices[0] = v;
                    mesh->m_Triangles[numtriangles].m_TIndices[0] = -1;
                    mesh->m_Triangles[numtriangles].m_NIndices[0] = -1;
                    fscanf(file, "%u", &v);
                    mesh->m_Triangles[numtriangles].m_VIndices[1] = v;
                    mesh->m_Triangles[numtriangles].m_TIndices[1] = -1;
                    mesh->m_Triangles[numtriangles].m_NIndices[1] = -1;
                    fscanf(file, "%u", &v);
                    mesh->m_Triangles[numtriangles].m_VIndices[2] = v;
                    mesh->m_Triangles[numtriangles].m_TIndices[2] = -1;
                    mesh->m_Triangles[numtriangles].m_NIndices[2] = -1;
                    mesh->m_Triangles[numtriangles].m_Material = material;
                    group->m_Triangles[group->m_TriangleCount++] = numtriangles;
                    numtriangles++;
                    while(fscanf(file, "%u", &v) > 0)
                    {
                        mesh->m_Triangles[numtriangles].m_VIndices[0] = mesh->m_Triangles[numtriangles-1].m_VIndices[0];
                        mesh->m_Triangles[numtriangles].m_TIndices[0] = mesh->m_Triangles[numtriangles-1].m_TIndices[0];
                        mesh->m_Triangles[numtriangles].m_NIndices[0] = mesh->m_Triangles[numtriangles-1].m_NIndices[0];
                        mesh->m_Triangles[numtriangles].m_VIndices[1] = mesh->m_Triangles[numtriangles-1].m_VIndices[2];
                        mesh->m_Triangles[numtriangles].m_TIndices[1] = mesh->m_Triangles[numtriangles-1].m_TIndices[2];
                        mesh->m_Triangles[numtriangles].m_NIndices[1] = mesh->m_Triangles[numtriangles-1].m_NIndices[2];
                        mesh->m_Triangles[numtriangles].m_VIndices[2] = v;
                        mesh->m_Triangles[numtriangles].m_TIndices[2] = -1;
                        mesh->m_Triangles[numtriangles].m_NIndices[2] = -1;
                        mesh->m_Triangles[numtriangles].m_Material = material;
                        group->m_Triangles[group->m_TriangleCount++] = numtriangles;
                        numtriangles++;
                    }
                }
                break;
            }

        default:
            {
                Log::Warning( TXT( "SecondPass(): Unknown token \"%s\"." ), buf );
                fgets(buf, sizeof(buf), file);
                break;
            }
        }
    }

    /* announce the memory requirements */
    DBG( Warning(" Memory: %d bytes",
        numvertices  * 3*sizeof(GLfloat) +
        numnormals   * 3*sizeof(GLfloat) * (numnormals ? 1 : 0) +
        numuvs * 3*sizeof(GLfloat) * (numuvs ? 1 : 0) +
        numtriangles * sizeof(Triangle)) );
}

SourceMesh::SourceMesh()
: m_VertexCount( 0 )
, m_Vertices( NULL )
, m_NormalCount( 0 )
, m_Normals( NULL )
, m_UVCount( 0 )
, m_UVs( NULL )
, m_TriangleCount( 0 )
, m_Triangles( NULL )
, m_MaterialCount( 0 )
, m_Materials( NULL )
, m_GroupCount( 0 )
, m_Groups( NULL )
, m_TextureCount( 0 )
, m_Textures( NULL )
{
}

SourceMesh::~SourceMesh()
{
    Reset(); 
}

void SourceMesh::Reset()
{
    m_Path.Clear();
    m_MaterialLibraryName = TXT( "" );

    if ( m_Vertices )
    {
        free( m_Vertices );
        m_Vertices = NULL;
    }

    if ( m_Normals )
    {
        free( m_Normals );
        m_Normals = NULL;
    }

    if ( m_UVs )
    {
        free( m_UVs );
        m_UVs = NULL;
    }

    if ( m_Triangles )
    {
        free( m_Triangles );
        m_Triangles = NULL;
    }

    if ( m_Materials )
    {
        free( m_Materials );
        m_Materials = NULL;
    }

    if ( m_Textures )
    {
        for ( GLuint i = 0; i < m_TextureCount; ++i )
        {
            glDeleteTextures( 1, &m_Textures[ i ].m_ID );
        }

        free( m_Textures );
        m_Textures = NULL;
    }

    while( m_Groups )
    {
        Group* group = m_Groups;
        m_Groups = m_Groups->m_Next;
        free( group->m_Triangles );
        free( group );
    }
}

void SourceMesh::Scale( GLfloat scale )
{
    for ( GLuint i = 1; i <= m_VertexCount; i++)
    {
        m_Vertices[3 * i + 0] *= scale;
        m_Vertices[3 * i + 1] *= scale;
        m_Vertices[3 * i + 2] *= scale;
    }
}

GLfloat SourceMesh::Unitize()
{
    GLuint  i;
    GLfloat maxx, minx, maxy, miny, maxz, minz;
    GLfloat cx, cy, cz, w, h, d;
    GLfloat scale;

    assert(m_Vertices);

    /* get the max/mins */
    maxx = minx = m_Vertices[3 + 0];
    maxy = miny = m_Vertices[3 + 1];
    maxz = minz = m_Vertices[3 + 2];
    for (i = 1; i <= m_VertexCount; i++)
    {
        if (maxx < m_Vertices[3 * i + 0])
            maxx = m_Vertices[3 * i + 0];
        if (minx > m_Vertices[3 * i + 0])
            minx = m_Vertices[3 * i + 0];

        if (maxy < m_Vertices[3 * i + 1])
            maxy = m_Vertices[3 * i + 1];
        if (miny > m_Vertices[3 * i + 1])
            miny = m_Vertices[3 * i + 1];

        if (maxz < m_Vertices[3 * i + 2])
            maxz = m_Vertices[3 * i + 2];
        if (minz > m_Vertices[3 * i + 2])
            minz = m_Vertices[3 * i + 2];
    }

    /* calculate mesh width, height, and depth */
    w = Abs(maxx) + Abs(minx);
    h = Abs(maxy) + Abs(miny);
    d = Abs(maxz) + Abs(minz);

    /* calculate center of the mesh */
    cx = (maxx + minx) / 2.0f;
    cy = (maxy + miny) / 2.0f;
    cz = (maxz + minz) / 2.0f;

    /* calculate unitizing scale factor */
    scale = 2.0f / Max(Max(w, h), d);

    /* translate around center then scale */
    for (i = 1; i <= m_VertexCount; i++)
    {
        m_Vertices[3 * i + 0] -= cx;
        m_Vertices[3 * i + 1] -= cy;
        m_Vertices[3 * i + 2] -= cz;
        m_Vertices[3 * i + 0] *= scale;
        m_Vertices[3 * i + 1] *= scale;
        m_Vertices[3 * i + 2] *= scale;
    }

    return scale;
}

void SourceMesh::GetDimensions(GLfloat* dimensions)
{
    GLuint i;
    GLfloat maxx, minx, maxy, miny, maxz, minz;

    assert(m_Vertices);
    assert(dimensions);

    /* get the max/mins */
    maxx = minx = m_Vertices[3 + 0];
    maxy = miny = m_Vertices[3 + 1];
    maxz = minz = m_Vertices[3 + 2];
    for (i = 1; i <= m_VertexCount; i++)
    {
        if (maxx < m_Vertices[3 * i + 0])
            maxx = m_Vertices[3 * i + 0];
        if (minx > m_Vertices[3 * i + 0])
            minx = m_Vertices[3 * i + 0];

        if (maxy < m_Vertices[3 * i + 1])
            maxy = m_Vertices[3 * i + 1];
        if (miny > m_Vertices[3 * i + 1])
            miny = m_Vertices[3 * i + 1];

        if (maxz < m_Vertices[3 * i + 2])
            maxz = m_Vertices[3 * i + 2];
        if (minz > m_Vertices[3 * i + 2])
            minz = m_Vertices[3 * i + 2];
    }

    /* calculate mesh width, height, and depth */
    dimensions[0] = Abs(maxx) + Abs(minx);
    dimensions[1] = Abs(maxy) + Abs(miny);
    dimensions[2] = Abs(maxz) + Abs(minz);
}

void SourceMesh::ReverseWinding()
{
    GLuint i, swap;

    for (i = 0; i < m_TriangleCount; i++)
    {
        swap = m_Triangles[i].m_VIndices[0];
        m_Triangles[i].m_VIndices[0] = m_Triangles[i].m_VIndices[2];
        m_Triangles[i].m_VIndices[2] = swap;

        if (m_NormalCount)
        {
            swap = m_Triangles[i].m_NIndices[0];
            m_Triangles[i].m_NIndices[0] = m_Triangles[i].m_NIndices[2];
            m_Triangles[i].m_NIndices[2] = swap;
        }

        if (m_UVCount)
        {
            swap = m_Triangles[i].m_TIndices[0];
            m_Triangles[i].m_TIndices[0] = m_Triangles[i].m_TIndices[2];
            m_Triangles[i].m_TIndices[2] = swap;
        }
    }

    /* reverse vertex m_Normals */
    for (i = 1; i <= m_NormalCount; i++)
    {
        m_Normals[3 * i + 0] = -m_Normals[3 * i + 0];
        m_Normals[3 * i + 1] = -m_Normals[3 * i + 1];
        m_Normals[3 * i + 2] = -m_Normals[3 * i + 2];
    }
}

void SourceMesh::GenerateLinearTexCoords()
{
    Group *group;
    GLfloat dimensions[3];
    GLfloat x, y, scalefactor;
    GLuint i;

    if (m_UVs)
    {
        free(m_UVs);
    }

    m_UVCount = m_VertexCount;
    m_UVs=(GLfloat*)malloc(sizeof(GLfloat)*2*(m_UVCount+1));

    GetDimensions(dimensions);
    scalefactor = 2.0f / Abs(Max(Max(dimensions[0], dimensions[1]), dimensions[2]));

    /* do the calculations */
    for(i = 1; i <= m_VertexCount; i++)
    {
        x = m_Vertices[3 * i + 0] * scalefactor;
        y = m_Vertices[3 * i + 2] * scalefactor;
        m_UVs[2 * i + 0] = (x + 1.0f) / 2.0f;
        m_UVs[2 * i + 1] = (y + 1.0f) / 2.0f;
    }

    /* go through and put texture coordinate indices in all the m_Triangles */
    group = m_Groups;
    while(group)
    {
        for(i = 0; i < group->m_TriangleCount; i++)
        {
            m_Triangles[group->m_Triangles[i]].m_TIndices[0] = m_Triangles[group->m_Triangles[i]].m_VIndices[0];
            m_Triangles[group->m_Triangles[i]].m_TIndices[1] = m_Triangles[group->m_Triangles[i]].m_VIndices[1];
            m_Triangles[group->m_Triangles[i]].m_TIndices[2] = m_Triangles[group->m_Triangles[i]].m_VIndices[2];
        }    

        group = group->m_Next;
    }

    DBG( Warning("GenerateLinearTexCoords(): generated %d linear texture coordinates", m_UVCount) );
}

void SourceMesh::GenerateSphericalTexCoords()
{
    Group* group;
    GLfloat theta, phi, rho, x, y, z, r;
    GLuint i;

    assert(m_Normals);

    if (m_UVs)
    {
        free(m_UVs);
    }

    m_UVCount = m_NormalCount;
    m_UVs=(GLfloat*)malloc(sizeof(GLfloat)*2*(m_UVCount+1));

    for (i = 1; i <= m_NormalCount; i++)
    {
        z = m_Normals[3 * i + 0];  /* re-arrange for pole distortion */
        y = m_Normals[3 * i + 1];
        x = m_Normals[3 * i + 2];
        r = sqrt((x * x) + (y * y));
        rho = sqrt((r * r) + (z * z));

        if(r == 0.0)
        {
            theta = 0.0;
            phi = 0.0;
        }
        else
        {
            if(z == 0.0f)
            {
                phi = GL_PI / 2.0f;
            }
            else
            {
                phi = acos(z / rho);
            }

            if(y == 0.0f)
            {
                theta = GL_PI / 2.0f;
            }
            else
            {
                theta = asin(y / r) + (GL_PI / 2.0f);
            }
        }

        m_UVs[2 * i + 0] = theta / GL_PI;
        m_UVs[2 * i + 1] = phi / GL_PI;
    }

    /* go through and put uv indices in all the m_Triangles */
    group = m_Groups;
    while(group)
    {
        for (i = 0; i < group->m_TriangleCount; i++)
        {
            m_Triangles[group->m_Triangles[i]].m_TIndices[0] = m_Triangles[group->m_Triangles[i]].m_NIndices[0];
            m_Triangles[group->m_Triangles[i]].m_TIndices[1] = m_Triangles[group->m_Triangles[i]].m_NIndices[1];
            m_Triangles[group->m_Triangles[i]].m_TIndices[2] = m_Triangles[group->m_Triangles[i]].m_NIndices[2];
        }

        group = group->m_Next;
    }
}

void SourceMesh::ReadOBJ( const Path& path )
{
    FILE* file;
    unsigned int i, j;

    /* open the file */
    file = _tfopen( path.c_str(), TXT( "r" ) );
    if ( !file )
    {
        Log::Error( TXT( "ReadOBJ() failed: can't open data file \"%s\"." ), path.c_str() );
    }

    Reset();

    /* make a first pass through the file to get a count of the number
    of m_Vertices, m_Normals, m_UVs & m_Triangles */
    FirstPass( this, file );

    /* allocate memory */
    m_Vertices = (GLfloat*)malloc(sizeof(GLfloat) * 3 * (m_VertexCount + 1));
    m_Triangles = (Triangle*)malloc(sizeof(Triangle) * m_TriangleCount);

    if (m_NormalCount)
    {
        m_Normals = (GLfloat*)malloc(sizeof(GLfloat) * 3 * (m_NormalCount + 1));
    }

    if (m_UVCount)
    {
        m_UVs = (GLfloat*)malloc(sizeof(GLfloat) * 2 * (m_UVCount + 1));
    }

    /* rewind to beginning of file and read in the data this pass */
    rewind(file);

    SecondPass(this, file);

    /* verify the indices */
    for (i = 0; i < m_TriangleCount; i++)
    {    
        for (j=0; j<3; j++)
        {
            if (m_Triangles[i].m_NIndices[j] != -1)
            {
                if (m_Triangles[i].m_NIndices[j] <= 0 || m_Triangles[i].m_NIndices[j] > m_NormalCount)
                {
                    Log::Error( TXT( "normal index for triangle %d out of bounds (%d > %d)\n" ), i, m_Triangles[i].m_NIndices[j], m_NormalCount );
                }
            }

            if (m_Triangles[i].m_VIndices[j] != -1)
            {
                if (m_Triangles[i].m_VIndices[j] <= 0 || m_Triangles[i].m_VIndices[j] > m_VertexCount)
                {
                    Log::Error( TXT( "vertex index for triangle %d out of bounds (%d > %d)\n" ), i, m_Triangles[i].m_VIndices[j], m_VertexCount );
                }
            }
        }
    }

    /* close the file */
    fclose(file);
}

void SourceMesh::WriteOBJ( const Path& path, GLuint mode)
{
    GLuint i;
    FILE* file;
    Group* group;
    GLuint material = -1;

    if (mode & Modes::Texture && !m_UVs)
    {
        Log::Warning( TXT( "WriteOBJ() warning: texture coordinate output requested with no texture coordinates defined." ) );
        mode &= ~Modes::Texture;
    }

    if (mode & Modes::Material && !m_Materials)
    {
        Log::Warning( TXT( "WriteOBJ() warning: material output requested with no materials defined." ) );
        mode &= ~Modes::Material;
    }

    /* open the file */
    file = _tfopen( path.c_str(), TXT( "w" ) );
    if ( !file )
    {
        Log::Error( TXT( "WriteOBJ() failed: can't open file \"%s\" to write." ), path.c_str() );
    }

    /* spit out a header */
    fprintf(file, "#  \n");
    fprintf(file, "#  Wavefront OBJ generated by Core library\n");
    fprintf(file, "#  \n");
    fprintf(file, "#  Core library\n");
    fprintf(file, "#  Nate Robins\n");
    fprintf(file, "#  ndr@pobox.com\n");
    fprintf(file, "#  http://www.pobox.com/~ndr\n");
    fprintf(file, "#  \n");

    if (mode & Modes::Material && !m_MaterialLibraryName.empty() )
    {
        fprintf(file, "\nmtllib %s\n\n", m_MaterialLibraryName);
        WriteMTL( this, path, m_MaterialLibraryName );
    }

    /* spit out the m_Vertices */
    fprintf(file, "\n");
    fprintf(file, "# %d m_Vertices\n", m_VertexCount);
    for (i = 1; i <= m_VertexCount; i++)
    {
        fprintf(file, "v %f %f %f\n", 
            m_Vertices[3 * i + 0],
            m_Vertices[3 * i + 1],
            m_Vertices[3 * i + 2]);
    }

    /* spit out the smooth/flat m_Normals */
    fprintf(file, "\n");
    fprintf(file, "# %u m_Normals\n", (unsigned int)m_NormalCount);
    for (i = 1; i <= m_NormalCount; i++)
    {
        fprintf(file, "vn %f %f %f\n", 
            m_Normals[3 * i + 0],
            m_Normals[3 * i + 1],
            m_Normals[3 * i + 2]);
    }

    /* spit out the texture coordinates */
    if (mode & Modes::Texture)
    {
        fprintf(file, "\n");
        fprintf(file, "# %d m_UVs\n", m_UVCount);
        for (i = 1; i <= m_UVCount; i++)
        {
            fprintf(file, "vt %f %f\n", 
                m_UVs[2 * i + 0],
                m_UVs[2 * i + 1]);
        }
    }

    fprintf(file, "\n");
    fprintf(file, "# %d m_Groups\n", m_GroupCount);
    fprintf(file, "# %d faces (m_Triangles)\n", m_TriangleCount);
    fprintf(file, "\n");

    group = m_Groups;
    while(group)
    {
        fprintf(file, "g %s\n", group->m_Name.c_str());

        for (i = 0; i < group->m_TriangleCount; i++)
        {
            if(m_Triangles[group->m_Triangles[i]].m_Material && m_Triangles[group->m_Triangles[i]].m_Material != material)
            {
                material = m_Triangles[group->m_Triangles[i]].m_Material;
                fprintf(file, "usemtl %s\n", m_Materials[ material ].m_Name);
            }

            if (mode & Modes::Texture)
            {
                fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                    m_Triangles[group->m_Triangles[i]].m_VIndices[0], 
                    m_Triangles[group->m_Triangles[i]].m_NIndices[0], 
                    m_Triangles[group->m_Triangles[i]].m_TIndices[0],
                    m_Triangles[group->m_Triangles[i]].m_VIndices[1],
                    m_Triangles[group->m_Triangles[i]].m_NIndices[1],
                    m_Triangles[group->m_Triangles[i]].m_TIndices[1],
                    m_Triangles[group->m_Triangles[i]].m_VIndices[2],
                    m_Triangles[group->m_Triangles[i]].m_NIndices[2],
                    m_Triangles[group->m_Triangles[i]].m_TIndices[2]);
            }
            else
            {
                fprintf(file, "f %d//%d %d//%d %d//%d\n",
                    m_Triangles[group->m_Triangles[i]].m_VIndices[0],
                    m_Triangles[group->m_Triangles[i]].m_NIndices[0],
                    m_Triangles[group->m_Triangles[i]].m_VIndices[1],
                    m_Triangles[group->m_Triangles[i]].m_NIndices[1],
                    m_Triangles[group->m_Triangles[i]].m_VIndices[2], 
                    m_Triangles[group->m_Triangles[i]].m_NIndices[2]);
            }
        }

        fprintf(file, "\n");
        group = group->m_Next;
    }

    fclose(file);
}