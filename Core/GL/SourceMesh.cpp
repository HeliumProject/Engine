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

#define MAX_SHININESS 100.0 /* for Poser */

#include "SourceMesh.h"
#include "Texture.h"
#include "Util.h"

using namespace Core;

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
inline static Group* FindGroup(SourceMesh* mesh, const char* name)
{
  assert(mesh);
  
  for (Group* group = mesh->groups; group; group = group->next)
  {
    if (!strcmp(name, group->name))
    {
      return group;
    }
  }
  
  return NULL;
}

/* AddGroup: Add a group to the mesh */
static Group* AddGroup(SourceMesh* mesh, const char* name)
{
  Group* group;
  
  group = FindGroup(mesh, name);
  if (!group)
  {
    group = (Group*)malloc(sizeof(Group));
    group->name = Strdup(name);
    group->numtriangles = 0;
    group->triangles = NULL;
    group->next = mesh->groups;
    mesh->groups = group;
    mesh->numgroups++;
  }
  
  return group;
}

/* FindGroup: Find a material in the mesh */
static GLuint FindMaterial(SourceMesh* mesh, const char* name)
{
  GLuint i;
  
  assert(name != NULL);
  
  /* XXX doing a linear search on a string key'd list is pretty lame,
   but it works and is fast enough for now. */
  for (i = 0; i < mesh->nummaterials; i++)
  {
    const char* current_name = mesh->materials[i].name;
    if (!strcmp(current_name, name))
    {
      return i;
    }
  }
  
  /* didn't find the name, so print a warning and return the default material (0). */
  Warning("FindMaterial():  can't find material \"%s\".", name);
  return 0;
}

/* FindTexture: Find a texture in the mesh
 *
 * name  - name of the texture
 */
static GLuint FindOrAddTexture(SourceMesh* mesh, const char* name)
{
  GLuint i;
  char *dir, *filename;
  
  /* XXX doing a linear search on a string key'd list is pretty lame, but it works and is fast enough for now. */
  for (i = 0; i < mesh->numtextures; i++)
  {
    if (!strcmp(mesh->textures[i].name, name))
    {
      return i;
    }
  }
  
  dir = DirName(mesh->pathname);
  filename = (char*)malloc(sizeof(char) * (strlen(dir) + strlen(name) + 1));
  strcpy(filename, dir);
  strcat(filename, name);
  free(dir);
  
  /* didn't find the name, so print a warning and return the default texture (0). */
  mesh->numtextures++;
  mesh->textures = (Texture*)realloc(mesh->textures, sizeof(Texture)*mesh->numtextures);
  mesh->textures[mesh->numtextures-1].name = strdup(name);
  mesh->textures[mesh->numtextures-1].id =
  LoadTexture(filename, GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

  DBG(Warning("allocated texture %d (id=%d)",mesh->numtextures-1, mesh->textures[mesh->numtextures-1].id));
  
  free(filename);
  
  return mesh->numtextures-1;
}

/* ReadMTL: read a wavefront material library file
 *
 * name  - name of the material library
 */
static void ReadMTL(SourceMesh* mesh, const char* name)
{
  FILE* file;
  char* dir;
  char* filename;
  char* t_filename;
  char buf[128];
  
  dir = DirName(mesh->pathname);
  filename = (char*)malloc(sizeof(char) * (strlen(dir) + strlen(name) + 1));
  strcpy(filename, dir);
  strcat(filename, name);
  
  file = fopen(filename, "r");
  if (!file)
  {
    Error( "ReadMTL() failed: can't open material file \"%s\".", filename );
  }
  free(filename);
  
  /* count the number of materials in the file */
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
          Error("ReadMTL: Got \"%s\" instead of \"newmtl\" in file \"%s\"", buf, filename);
        }
        
        fgets(buf, sizeof(buf), file);
        mesh->nummaterials++;
        sscanf(buf, "%s %s", buf, buf);
        break;
      }
        
      default: /* eat up rest of line */
      {
        fgets(buf, sizeof(buf), file);
        break;
      }
    }
  }
  
  rewind(file);
  
  mesh->materials = (Material*)malloc(sizeof(Material) * mesh->nummaterials);

  /* set the default material */
  for (GLuint i = 0; i < mesh->nummaterials; i++)
  {
    mesh->materials[i].name = NULL;
    mesh->materials[i].ambient[0] = 0.2;
    mesh->materials[i].ambient[1] = 0.2;
    mesh->materials[i].ambient[2] = 0.2;
    mesh->materials[i].ambient[3] = 1.0;
    mesh->materials[i].diffuse[0] = 0.8;
    mesh->materials[i].diffuse[1] = 0.8;
    mesh->materials[i].diffuse[2] = 0.8;
    mesh->materials[i].diffuse[3] = 1.0;
    mesh->materials[i].specular[0] = 0.0;
    mesh->materials[i].specular[1] = 0.0;
    mesh->materials[i].specular[2] = 0.0;
    mesh->materials[i].specular[3] = 1.0;
    mesh->materials[i].shininess = 65.0;
    mesh->materials[i].texture = -1;
  }
  mesh->materials[0].name = Strdup("default");
  
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
          Error("ReadMTL: Got \"%s\" instead of \"newmtl\" in file \"%s\"", buf, filename);
        }
        
        fgets(buf, sizeof(buf), file);
        sscanf(buf, "%s %s", buf, buf);
        current_material = &mesh->materials[next_index++];
        current_material->name = Strdup(buf);
        break;
      }
        
      case 'N':
      {
        switch(buf[1])
        {
          case 's':
          {
            fscanf(file, "%f", &current_material->shininess);
            
            /* wavefront shininess is from [0, 1000], so scale for OpenGL */
            current_material->shininess /= MAX_SHININESS;
            current_material->shininess *= 128.0;
            break;
          }
          
          default:
          {
            Warning("ReadMTL: Command \"%s\" ignored", buf);
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
                   &current_material->ambient[0],
                   &current_material->ambient[1],
                   &current_material->ambient[2]);
            break;
          }
            
          case 'd':
          {
            fscanf(file, "%f %f %f",
                   &current_material->diffuse[0],
                   &current_material->diffuse[1],
                   &current_material->diffuse[2]);
            break;
          }
            
          case 's':
          {
            fscanf(file, "%f %f %f",
                   &current_material->specular[0],
                   &current_material->specular[1],
                   &current_material->specular[2]);
            break;
          }
          
          default:
          {
            Warning("ReadMTL: Command \"%s\" ignored", buf);
            fgets(buf, sizeof(buf), file);
            break;
          }
        }
        break;
      }
        
      case 'd': /* d = Dissolve factor (pseudo-transparency). Values are from 0-1. 0 is completely transparent, 1 is opaque. */
      {
        float alpha;
        fscanf(file, "%f", &alpha);
        current_material->diffuse[3] = alpha;
        break;
      }

      case 'm': /* texture map */
      {
        filename = (char*)malloc(FILENAME_MAX);
        fgets(filename, FILENAME_MAX, file);
        t_filename = StrStrip((char*)filename);
        free(filename);
        
        if(strncmp(buf, "map_Kd", 6) == 0)
        {
          current_material->texture = FindOrAddTexture(mesh, t_filename);
          free(t_filename);
        }
        else
        {
          Warning("map %s %s ignored", buf, t_filename);
          free(t_filename);
          fgets(buf, sizeof(buf), file);
        }
        break;
      }
        
      default:
      {
        Warning("ReadMTL: Command \"%s\" ignored", buf);
        fgets(buf, sizeof(buf), file);
        break;
      }
    }
  }
  
  free(dir);
  fclose(file);
}

/* WriteMTL: write a wavefront material library file
 *
 * mesh   - properly initialized SourceMesh structure
 * meshpath  - pathname of the mesh being written
 * mtllibname - name of the material library to be written
 */
static void WriteMTL(SourceMesh* mesh, const char* meshpath, const char* mtllibname)
{
  FILE* file;
  char* dir;
  char* filename;
  Material* material;
  GLuint i;
  
  dir = DirName(meshpath);
  filename = (char*)malloc(sizeof(char) * (strlen(dir)+strlen(mtllibname)+1));
  strcpy(filename, dir);
  strcat(filename, mtllibname);
  free(dir);
  
  /* open the file */
  file = fopen(filename, "w");
  if (!file)
  {
    Error( "WriteMTL() failed: can't open file \"%s\".", filename);
  }
  free(filename);
  
  /* spit out a header */
  fprintf(file, "#  \n");
  fprintf(file, "#  Wavefront MTL generated by Core library\n");
  fprintf(file, "#  \n");
  fprintf(file, "#  Core library\n");
  fprintf(file, "#  Nate Robins\n");
  fprintf(file, "#  ndr@pobox.com\n");
  fprintf(file, "#  http://www.pobox.com/~ndr\n");
  fprintf(file, "#  \n\n");
  
  for (i = 0; i < mesh->nummaterials; i++)
  {
    material = &mesh->materials[i];
    fprintf(file, "newmtl %s\n", material->name);
    fprintf(file, "Ka %f %f %f\n", 
            material->ambient[0], material->ambient[1], material->ambient[2]);
    fprintf(file, "Kd %f %f %f\n", 
            material->diffuse[0], material->diffuse[1], material->diffuse[2]);
    fprintf(file, "Ks %f %f %f\n", 
            material->specular[0],material->specular[1],material->specular[2]);
    fprintf(file, "Ns %f\n", material->shininess / 128.0 * MAX_SHININESS);
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
  group = AddGroup(mesh, "default");
  
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
            Warning("FirstPass(): Unknown token \"%s\".", buf);
            break;
          }
        }
        
        break;
      }
        
      case 'm':
      {
        if(strncmp(buf, "mtllib", 6) != 0)
        {
          Error("ReadOBJ: Got \"%s\" instead of \"mtllib\"", buf);
        }
        
        fgets(buf, sizeof(buf), file);
        sscanf(buf, "%s %s", buf, buf);
        mesh->mtllibname = StrStrip((char*)buf);
        ReadMTL(mesh, mesh->mtllibname);
        break;
      }
        
      case 'u': /* material name */
      {
        if(strncmp(buf, "usemtl", 6) != 0)
        {
          Error("ReadOBJ: Got \"%s\" instead of \"usemtl\"", buf);
        }
        
        fgets(buf, sizeof(buf), file);
        break;
      }
        
      case 'g': /* group */
      {
        /* eat up rest of line */
        fgets(buf, sizeof(buf), file);

        /* nuke '\n' */
        buf[strlen(buf)-1] = '\0';
        
        group = AddGroup(mesh, &buf[1]);
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
          sscanf(buf, "%d//%d", &v, &n);
          fscanf(file, "%d//%d", &v, &n);
          fscanf(file, "%d//%d", &v, &n);
          numtriangles++;
          group->numtriangles++;
          while(fscanf(file, "%d//%d", &v, &n) > 0)
          {
            numtriangles++;
            group->numtriangles++;
          }
        }
        else if (sscanf(buf, "%d/%d/%d", &v, &t, &n) == 3)
        {
          /* v/t/n */
          fscanf(file, "%d/%d/%d", &v, &t, &n);
          fscanf(file, "%d/%d/%d", &v, &t, &n);
          numtriangles++;
          group->numtriangles++;
          while(fscanf(file, "%d/%d/%d", &v, &t, &n) > 0)
          {
            numtriangles++;
            group->numtriangles++;
          }
        }
        else if (sscanf(buf, "%d/%d", &v, &t) == 2)
        {
          /* v/t */
          fscanf(file, "%d/%d", &v, &t);
          fscanf(file, "%d/%d", &v, &t);
          numtriangles++;
          group->numtriangles++;
          while(fscanf(file, "%d/%d", &v, &t) > 0)
          {
            numtriangles++;
            group->numtriangles++;
          }
        }
        else
        {
          /* v */
          fscanf(file, "%d", &v);
          fscanf(file, "%d", &v);
          numtriangles++;
          group->numtriangles++;
          while(fscanf(file, "%d", &v) > 0)
          {
            numtriangles++;
            group->numtriangles++;
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
  mesh->numvertices  = numvertices;
  mesh->numnormals   = numnormals;
  mesh->numuvs = numuvs;
  mesh->numtriangles = numtriangles;
  
  /* allocate memory for the triangles in each group */
  group = mesh->groups;
  while(group)
  {
    group->triangles = (GLuint*)malloc(sizeof(GLuint) * group->numtriangles);
    group->numtriangles = 0;
    group = group->next;
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
  vertices = mesh->vertices;
  normals = mesh->normals;
  uvs = mesh->uvs;
  group = mesh->groups;
  
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
        material = FindMaterial(mesh, buf);
        break;
      }
        
      case 'g': /* group */
      {
        /* eat up rest of line */
        fgets(buf, sizeof(buf), file);
        
        /* nuke '\n' */
        buf[strlen(buf)-1] = '\0';

        group = FindGroup(mesh, &buf[1]);
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
          mesh->triangles[numtriangles].vindices[0] = v;
          mesh->triangles[numtriangles].tindices[0] = -1;
          mesh->triangles[numtriangles].nindices[0] = n;
          fscanf(file, "%u//%u", &v, &n);
          mesh->triangles[numtriangles].vindices[1] = v;
          mesh->triangles[numtriangles].tindices[1] = -1;
          mesh->triangles[numtriangles].nindices[1] = n;
          fscanf(file, "%u//%u", &v, &n);
          mesh->triangles[numtriangles].vindices[2] = v;
          mesh->triangles[numtriangles].tindices[2] = -1;
          mesh->triangles[numtriangles].nindices[2] = n;
          mesh->triangles[numtriangles].material = material;
          group->triangles[group->numtriangles++] = numtriangles;
          numtriangles++;
          
          while(fscanf(file, "%u//%u", &v, &n) > 0)
          {
            mesh->triangles[numtriangles].vindices[0] = mesh->triangles[numtriangles-1].vindices[0];
            mesh->triangles[numtriangles].tindices[0] = mesh->triangles[numtriangles-1].tindices[0];
            mesh->triangles[numtriangles].nindices[0] = mesh->triangles[numtriangles-1].nindices[0];
            mesh->triangles[numtriangles].vindices[1] = mesh->triangles[numtriangles-1].vindices[2];
            mesh->triangles[numtriangles].tindices[1] = mesh->triangles[numtriangles-1].tindices[2];
            mesh->triangles[numtriangles].nindices[1] = mesh->triangles[numtriangles-1].nindices[2];
            mesh->triangles[numtriangles].vindices[2] = v;
            mesh->triangles[numtriangles].tindices[2] = -1;
            mesh->triangles[numtriangles].nindices[2] = n;
            mesh->triangles[numtriangles].material = material;
            group->triangles[group->numtriangles++] = numtriangles;
            numtriangles++;
          }
        }
        else if (sscanf(buf, "%u/%u/%u", &v, &t, &n) == 3)
        {
          /* v/t/n */
          mesh->triangles[numtriangles].vindices[0] = v;
          mesh->triangles[numtriangles].tindices[0] = t;
          mesh->triangles[numtriangles].nindices[0] = n;
          fscanf(file, "%u/%u/%u", &v, &t, &n);
          mesh->triangles[numtriangles].vindices[1] = v;
          mesh->triangles[numtriangles].tindices[1] = t;
          mesh->triangles[numtriangles].nindices[1] = n;
          fscanf(file, "%u/%u/%u", &v, &t, &n);
          mesh->triangles[numtriangles].vindices[2] = v;
          mesh->triangles[numtriangles].tindices[2] = t;
          mesh->triangles[numtriangles].nindices[2] = n;
          mesh->triangles[numtriangles].material = material;
          group->triangles[group->numtriangles++] = numtriangles;
          numtriangles++;
          while(fscanf(file, "%u/%u/%u", &v, &t, &n) > 0)
          {
            mesh->triangles[numtriangles].vindices[0] = mesh->triangles[numtriangles-1].vindices[0];
            mesh->triangles[numtriangles].tindices[0] = mesh->triangles[numtriangles-1].tindices[0];
            mesh->triangles[numtriangles].nindices[0] = mesh->triangles[numtriangles-1].nindices[0];
            mesh->triangles[numtriangles].vindices[1] = mesh->triangles[numtriangles-1].vindices[2];
            mesh->triangles[numtriangles].tindices[1] = mesh->triangles[numtriangles-1].tindices[2];
            mesh->triangles[numtriangles].nindices[1] = mesh->triangles[numtriangles-1].nindices[2];
            mesh->triangles[numtriangles].vindices[2] = v;
            mesh->triangles[numtriangles].tindices[2] = t;
            mesh->triangles[numtriangles].nindices[2] = n;
            mesh->triangles[numtriangles].material = material;
            group->triangles[group->numtriangles++] = numtriangles;
            numtriangles++;
          }
        }
        else if (sscanf(buf, "%u/%u", &v, &t) == 2)
        {
          /* v/t */
          mesh->triangles[numtriangles].vindices[0] = v;
          mesh->triangles[numtriangles].tindices[0] = t;
          mesh->triangles[numtriangles].nindices[0] = -1;
          fscanf(file, "%u/%u", &v, &t);
          mesh->triangles[numtriangles].vindices[1] = v;
          mesh->triangles[numtriangles].tindices[1] = t;
          mesh->triangles[numtriangles].nindices[1] = -1;
          fscanf(file, "%u/%u", &v, &t);
          mesh->triangles[numtriangles].vindices[2] = v;
          mesh->triangles[numtriangles].tindices[2] = t;
          mesh->triangles[numtriangles].nindices[2] = -1;
          mesh->triangles[numtriangles].material = material;
          group->triangles[group->numtriangles++] = numtriangles;
          numtriangles++;
          while(fscanf(file, "%u/%u", &v, &t) > 0)
          {
            mesh->triangles[numtriangles].vindices[0] = mesh->triangles[numtriangles-1].vindices[0];
            mesh->triangles[numtriangles].tindices[0] = mesh->triangles[numtriangles-1].tindices[0];
            mesh->triangles[numtriangles].nindices[0] = mesh->triangles[numtriangles-1].nindices[0];
            mesh->triangles[numtriangles].vindices[1] = mesh->triangles[numtriangles-1].vindices[2];
            mesh->triangles[numtriangles].tindices[1] = mesh->triangles[numtriangles-1].tindices[2];
            mesh->triangles[numtriangles].nindices[1] = mesh->triangles[numtriangles-1].nindices[2];
            mesh->triangles[numtriangles].vindices[2] = v;
            mesh->triangles[numtriangles].tindices[2] = t;
            mesh->triangles[numtriangles].nindices[2] = -1;
            mesh->triangles[numtriangles].material = material;
            group->triangles[group->numtriangles++] = numtriangles;
            numtriangles++;
          }
        }
        else
        {
          /* v */
          sscanf(buf, "%u", &v);
          mesh->triangles[numtriangles].vindices[0] = v;
          mesh->triangles[numtriangles].tindices[0] = -1;
          mesh->triangles[numtriangles].nindices[0] = -1;
          fscanf(file, "%u", &v);
          mesh->triangles[numtriangles].vindices[1] = v;
          mesh->triangles[numtriangles].tindices[1] = -1;
          mesh->triangles[numtriangles].nindices[1] = -1;
          fscanf(file, "%u", &v);
          mesh->triangles[numtriangles].vindices[2] = v;
          mesh->triangles[numtriangles].tindices[2] = -1;
          mesh->triangles[numtriangles].nindices[2] = -1;
          mesh->triangles[numtriangles].material = material;
          group->triangles[group->numtriangles++] = numtriangles;
          numtriangles++;
          while(fscanf(file, "%u", &v) > 0)
          {
            mesh->triangles[numtriangles].vindices[0] = mesh->triangles[numtriangles-1].vindices[0];
            mesh->triangles[numtriangles].tindices[0] = mesh->triangles[numtriangles-1].tindices[0];
            mesh->triangles[numtriangles].nindices[0] = mesh->triangles[numtriangles-1].nindices[0];
            mesh->triangles[numtriangles].vindices[1] = mesh->triangles[numtriangles-1].vindices[2];
            mesh->triangles[numtriangles].tindices[1] = mesh->triangles[numtriangles-1].tindices[2];
            mesh->triangles[numtriangles].nindices[1] = mesh->triangles[numtriangles-1].nindices[2];
            mesh->triangles[numtriangles].vindices[2] = v;
            mesh->triangles[numtriangles].tindices[2] = -1;
            mesh->triangles[numtriangles].nindices[2] = -1;
            mesh->triangles[numtriangles].material = material;
            group->triangles[group->numtriangles++] = numtriangles;
            numtriangles++;
          }
        }
        break;
      }
        
      default:
      {
        Warning("SecondPass(): Unknown token \"%s\".", buf);
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
: pathname (NULL)
, mtllibname (NULL)
, numvertices (0)
, vertices (NULL)
, numnormals (0)
, normals (NULL)
, numuvs (0)
, uvs (NULL)
, numtriangles (0)
, triangles (NULL)
, nummaterials (0)
, materials (NULL)
, numgroups (0)
, groups (NULL)
, numtextures (0)
, textures (NULL)
{

}

SourceMesh::~SourceMesh()
{
  Reset(); 
}

void SourceMesh::Reset()
{
  Group* group;
  GLuint i;

  if (pathname)
  {
    free((void*)pathname);
    pathname = NULL;
  }
  
  if (mtllibname)
  {
    free((void*)mtllibname);
    mtllibname = NULL;
  }
  
  if (vertices)
  {
    free(vertices);
    vertices = NULL;
  }
  
  if (normals)
  {
    free(normals);
    normals = NULL;
  }
  
  if (uvs)
  {
    free(uvs);
    uvs = NULL;
  }

  if (triangles)
  {
    free(triangles);
    triangles = NULL;
  }
  
  if (materials)
  {
    for (i = 0; i < nummaterials; i++)
    {
      free(materials[i].name);
    }
    
    free(materials);
    materials = NULL;
  }
  
  if (textures)
  {
    for (i = 0; i < numtextures; i++)
    {
      free(textures[i].name);
      glDeleteTextures(1,&textures[i].id);
    }
    
    free(textures);
    textures = NULL;
  }
  
  while(groups)
  {
    group = groups;
    groups = groups->next;
    free(group->name);
    free(group->triangles);
    free(group);
  }
}

void SourceMesh::Scale(GLfloat scale)
{
  GLuint i;
  
  for (i = 1; i <= numvertices; i++)
  {
    vertices[3 * i + 0] *= scale;
    vertices[3 * i + 1] *= scale;
    vertices[3 * i + 2] *= scale;
  }
}

GLfloat SourceMesh::Unitize()
{
  GLuint  i;
  GLfloat maxx, minx, maxy, miny, maxz, minz;
  GLfloat cx, cy, cz, w, h, d;
  GLfloat scale;
  
  assert(vertices);
  
  /* get the max/mins */
  maxx = minx = vertices[3 + 0];
  maxy = miny = vertices[3 + 1];
  maxz = minz = vertices[3 + 2];
  for (i = 1; i <= numvertices; i++)
  {
    if (maxx < vertices[3 * i + 0])
      maxx = vertices[3 * i + 0];
    if (minx > vertices[3 * i + 0])
      minx = vertices[3 * i + 0];
    
    if (maxy < vertices[3 * i + 1])
      maxy = vertices[3 * i + 1];
    if (miny > vertices[3 * i + 1])
      miny = vertices[3 * i + 1];
    
    if (maxz < vertices[3 * i + 2])
      maxz = vertices[3 * i + 2];
    if (minz > vertices[3 * i + 2])
      minz = vertices[3 * i + 2];
  }
  
  /* calculate mesh width, height, and depth */
  w = Abs(maxx) + Abs(minx);
  h = Abs(maxy) + Abs(miny);
  d = Abs(maxz) + Abs(minz);
  
  /* calculate center of the mesh */
  cx = (maxx + minx) / 2.0;
  cy = (maxy + miny) / 2.0;
  cz = (maxz + minz) / 2.0;
  
  /* calculate unitizing scale factor */
  scale = 2.0 / Max(Max(w, h), d);
  
  /* translate around center then scale */
  for (i = 1; i <= numvertices; i++)
  {
    vertices[3 * i + 0] -= cx;
    vertices[3 * i + 1] -= cy;
    vertices[3 * i + 2] -= cz;
    vertices[3 * i + 0] *= scale;
    vertices[3 * i + 1] *= scale;
    vertices[3 * i + 2] *= scale;
  }
  
  return scale;
}

void SourceMesh::GetDimensions(GLfloat* dimensions)
{
  GLuint i;
  GLfloat maxx, minx, maxy, miny, maxz, minz;
  
  assert(vertices);
  assert(dimensions);
  
  /* get the max/mins */
  maxx = minx = vertices[3 + 0];
  maxy = miny = vertices[3 + 1];
  maxz = minz = vertices[3 + 2];
  for (i = 1; i <= numvertices; i++)
  {
    if (maxx < vertices[3 * i + 0])
      maxx = vertices[3 * i + 0];
    if (minx > vertices[3 * i + 0])
      minx = vertices[3 * i + 0];
    
    if (maxy < vertices[3 * i + 1])
      maxy = vertices[3 * i + 1];
    if (miny > vertices[3 * i + 1])
      miny = vertices[3 * i + 1];
    
    if (maxz < vertices[3 * i + 2])
      maxz = vertices[3 * i + 2];
    if (minz > vertices[3 * i + 2])
      minz = vertices[3 * i + 2];
  }
  
  /* calculate mesh width, height, and depth */
  dimensions[0] = Abs(maxx) + Abs(minx);
  dimensions[1] = Abs(maxy) + Abs(miny);
  dimensions[2] = Abs(maxz) + Abs(minz);
}

void SourceMesh::ReverseWinding()
{
  GLuint i, swap;

  for (i = 0; i < numtriangles; i++)
  {
    swap = triangles[i].vindices[0];
    triangles[i].vindices[0] = triangles[i].vindices[2];
    triangles[i].vindices[2] = swap;
    
    if (numnormals)
    {
      swap = triangles[i].nindices[0];
      triangles[i].nindices[0] = triangles[i].nindices[2];
      triangles[i].nindices[2] = swap;
    }
    
    if (numuvs)
    {
      swap = triangles[i].tindices[0];
      triangles[i].tindices[0] = triangles[i].tindices[2];
      triangles[i].tindices[2] = swap;
    }
  }
  
  /* reverse vertex normals */
  for (i = 1; i <= numnormals; i++)
  {
    normals[3 * i + 0] = -normals[3 * i + 0];
    normals[3 * i + 1] = -normals[3 * i + 1];
    normals[3 * i + 2] = -normals[3 * i + 2];
  }
}

void SourceMesh::GenerateLinearTexCoords()
{
  Group *group;
  GLfloat dimensions[3];
  GLfloat x, y, scalefactor;
  GLuint i;

  if (uvs)
  {
    free(uvs);
  }
  
  numuvs = numvertices;
  uvs=(GLfloat*)malloc(sizeof(GLfloat)*2*(numuvs+1));
  
  GetDimensions(dimensions);
  scalefactor = 2.0 / 
  Abs(Max(Max(dimensions[0], dimensions[1]), dimensions[2]));
  
  /* do the calculations */
  for(i = 1; i <= numvertices; i++)
  {
    x = vertices[3 * i + 0] * scalefactor;
    y = vertices[3 * i + 2] * scalefactor;
    uvs[2 * i + 0] = (x + 1.0) / 2.0;
    uvs[2 * i + 1] = (y + 1.0) / 2.0;
  }
  
  /* go through and put texture coordinate indices in all the triangles */
  group = groups;
  while(group)
  {
    for(i = 0; i < group->numtriangles; i++)
    {
      triangles[group->triangles[i]].tindices[0] = triangles[group->triangles[i]].vindices[0];
      triangles[group->triangles[i]].tindices[1] = triangles[group->triangles[i]].vindices[1];
      triangles[group->triangles[i]].tindices[2] = triangles[group->triangles[i]].vindices[2];
    }    
    
    group = group->next;
  }

  DBG( Warning("GenerateLinearTexCoords(): generated %d linear texture coordinates", numuvs) );
}

void SourceMesh::GenerateSphericalTexCoords()
{
  Group* group;
  GLfloat theta, phi, rho, x, y, z, r;
  GLuint i;
  
  assert(normals);
  
  if (uvs)
  {
    free(uvs);
  }
  
  numuvs = numnormals;
  uvs=(GLfloat*)malloc(sizeof(GLfloat)*2*(numuvs+1));
  
  for (i = 1; i <= numnormals; i++)
  {
    z = normals[3 * i + 0];  /* re-arrange for pole distortion */
    y = normals[3 * i + 1];
    x = normals[3 * i + 2];
    r = sqrt((x * x) + (y * y));
    rho = sqrt((r * r) + (z * z));
    
    if(r == 0.0)
    {
      theta = 0.0;
      phi = 0.0;
    }
    else
    {
      if(z == 0.0)
      {
        phi = 3.14159265 / 2.0;
      }
      else
      {
        phi = acos(z / rho);
      }
      
      if(y == 0.0)
      {
        theta = 3.141592365 / 2.0;
      }
      else
      {
        theta = asin(y / r) + (3.14159265 / 2.0);
      }
    }
    
    uvs[2 * i + 0] = theta / 3.14159265;
    uvs[2 * i + 1] = phi / 3.14159265;
  }
  
  /* go through and put uv indices in all the triangles */
  group = groups;
  while(group)
  {
    for (i = 0; i < group->numtriangles; i++)
    {
      triangles[group->triangles[i]].tindices[0] = triangles[group->triangles[i]].nindices[0];
      triangles[group->triangles[i]].tindices[1] = triangles[group->triangles[i]].nindices[1];
      triangles[group->triangles[i]].tindices[2] = triangles[group->triangles[i]].nindices[2];
    }
    
    group = group->next;
  }
}

void SourceMesh::ReadOBJ(const char* filename)
{
  FILE* file;
  int i, j;
  
  /* open the file */
  file = fopen(filename, "r");
  if (!file)
  {
    Error( "ReadOBJ() failed: can't open data file \"%s\".", filename);
  }
 
  Reset();

  pathname = Strdup(filename);
  
  /* make a first pass through the file to get a count of the number
   of vertices, normals, uvs & triangles */
  FirstPass(this, file);

  /* allocate memory */
  vertices = (GLfloat*)malloc(sizeof(GLfloat) * 3 * (numvertices + 1));
  triangles = (Triangle*)malloc(sizeof(Triangle) * numtriangles);
  
  if (numnormals)
  {
    normals = (GLfloat*)malloc(sizeof(GLfloat) * 3 * (numnormals + 1));
  }
  
  if (numuvs)
  {
    uvs = (GLfloat*)malloc(sizeof(GLfloat) * 2 * (numuvs + 1));
  }
  
  /* rewind to beginning of file and read in the data this pass */
  rewind(file);
  
  SecondPass(this, file);

  /* verify the indices */
  for (i = 0; i < numtriangles; i++)
  {    
    for (j=0; j<3; j++)
    {
	    if (triangles[i].nindices[j] != -1)
      {
        if (triangles[i].nindices[j] <= 0 || triangles[i].nindices[j] > numnormals)
        {
          Error("normal index for triangle %d out of bounds (%d > %d)\n", i, triangles[i].nindices[j], numnormals);
        }
      }
      
	    if (triangles[i].vindices[j] != -1)
      {
        if (triangles[i].vindices[j] <= 0 || triangles[i].vindices[j] > numvertices)
        {
          Error("vertex index for triangle %d out of bounds (%d > %d)\n", i, triangles[i].vindices[j], numvertices);
        }
      }
    }
  }
  
  /* close the file */
  fclose(file);
}

void SourceMesh::WriteOBJ(const char* filename, GLuint mode)
{
  GLuint i;
  FILE* file;
  Group* group;
  GLuint material = -1;

  if (mode & Modes::Texture && !uvs)
  {
    Warning("WriteOBJ() warning: texture coordinate output requested with no texture coordinates defined.");
    mode &= ~Modes::Texture;
  }
  
  if (mode & Modes::Material && !materials)
  {
    Warning("WriteOBJ() warning: material output requested with no materials defined.");
    mode &= ~Modes::Material;
  }

  /* open the file */
  file = fopen(filename, "w");
  if (!file) {
    Error( "WriteOBJ() failed: can't open file \"%s\" to write.", filename);
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
  
  if (mode & Modes::Material && mtllibname)
  {
    fprintf(file, "\nmtllib %s\n\n", mtllibname);
    WriteMTL(this, filename, mtllibname);
  }
  
  /* spit out the vertices */
  fprintf(file, "\n");
  fprintf(file, "# %d vertices\n", numvertices);
  for (i = 1; i <= numvertices; i++)
  {
    fprintf(file, "v %f %f %f\n", 
            vertices[3 * i + 0],
            vertices[3 * i + 1],
            vertices[3 * i + 2]);
  }
  
  /* spit out the smooth/flat normals */
  fprintf(file, "\n");
  fprintf(file, "# %u normals\n", (unsigned int)numnormals);
  for (i = 1; i <= numnormals; i++)
  {
    fprintf(file, "vn %f %f %f\n", 
            normals[3 * i + 0],
            normals[3 * i + 1],
            normals[3 * i + 2]);
  }
  
  /* spit out the texture coordinates */
  if (mode & Modes::Texture)
  {
    fprintf(file, "\n");
    fprintf(file, "# %d uvs\n", numuvs);
    for (i = 1; i <= numuvs; i++)
    {
      fprintf(file, "vt %f %f\n", 
              uvs[2 * i + 0],
              uvs[2 * i + 1]);
    }
  }
  
  fprintf(file, "\n");
  fprintf(file, "# %d groups\n", numgroups);
  fprintf(file, "# %d faces (triangles)\n", numtriangles);
  fprintf(file, "\n");
  
  group = groups;
  while(group)
  {
    fprintf(file, "g %s\n", group->name);

    for (i = 0; i < group->numtriangles; i++)
    {
      if(triangles[group->triangles[i]].material && triangles[group->triangles[i]].material != material)
      {
        material = triangles[group->triangles[i]].material;
        fprintf(file, "usemtl %s\n", materials[material].name);
      }

      if (mode & Modes::Texture)
      {
        fprintf(file, "f %d/%d/%d %d/%d/%d %d/%d/%d\n",
                triangles[group->triangles[i]].vindices[0], 
                triangles[group->triangles[i]].nindices[0], 
                triangles[group->triangles[i]].tindices[0],
                triangles[group->triangles[i]].vindices[1],
                triangles[group->triangles[i]].nindices[1],
                triangles[group->triangles[i]].tindices[1],
                triangles[group->triangles[i]].vindices[2],
                triangles[group->triangles[i]].nindices[2],
                triangles[group->triangles[i]].tindices[2]);
      }
      else
      {
        fprintf(file, "f %d//%d %d//%d %d//%d\n",
                triangles[group->triangles[i]].vindices[0],
                triangles[group->triangles[i]].nindices[0],
                triangles[group->triangles[i]].vindices[1],
                triangles[group->triangles[i]].nindices[1],
                triangles[group->triangles[i]].vindices[2], 
                triangles[group->triangles[i]].nindices[2]);
      }
    }
    
    fprintf(file, "\n");
    group = group->next;
  }
  
  fclose(file);
}