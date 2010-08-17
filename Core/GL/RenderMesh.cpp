#include <stdlib.h>
#include <assert.h>

#include "RenderMesh.h"
#include "Util.h"

using namespace Core;

RenderMesh::RenderMesh()
: mode ( 0 )
{
}

RenderMesh::~RenderMesh()
{
}

struct Vector
{
  float x,y,z;
};

struct UV
{
  float u,v;
};

void RenderMesh::Build( SourceMesh* mesh, GLuint mode )
{  
  /* copy some info into the render mesh */

  this->mode = mode;
  
  /* do a bit of warning */
  
  if (mode & Modes::Texture && !mesh->uvs)
  {
    Warning("Draw() warning: texture render mode requested with no texture coordinates defined.");
    mode &= ~Modes::Texture;
  }
  if (mode & Modes::Material && !mesh->materials)
  {
    Warning("Draw() warning: material render mode requested with no materials defined.");
    mode &= ~Modes::Material;
  }

  /* build command queue, build buffer object data */
  
  NTL::Array<Vector> vertices;
  NTL::Array<Vector> normals;
  NTL::Array<UV> uvs;

  vertices.Reserve( mesh->numvertices );
  normals.Reserve( mesh->numnormals );
  uvs.Reserve( mesh->numuvs );
  
  Material* current_material = NULL;  
  Texture* current_texture = NULL;
  uint32_t start = 0, count = 0;

  for ( Group* group = mesh->groups; group; group = group->next )
  {    
    for ( uint32_t t = 0; t < group->numtriangles; t++ )
    {
      Triangle* triangle = &mesh->triangles[group->triangles[t]];

      if ( mode & ( Modes::Material | Modes::Texture ) )
      {
        assert( triangle->material >= 0 );

        Material* material = triangle->material >= 0 ? &mesh->materials[ triangle->material ] : &mesh->materials[ 0 ];      
        if ( material != current_material )
        {
          current_material = material;

          if ( count )
          {
            DrawPrimitives cmd;
            cmd.m_Primitive = GL_TRIANGLES;
            cmd.m_Start = start;
            cmd.m_Count = count;
            commands.Append( cmd );
            
            start += count;
            count = 0;
          }

          if ( mode & Modes::Material )
          {
            {
              SetMaterialVector cmd;
              
              cmd.m_Type = GL_AMBIENT;
              cmd.m_R = material->ambient[0];
              cmd.m_G = material->ambient[1];
              cmd.m_B = material->ambient[2];
              cmd.m_A = material->ambient[3];
              commands.Append( cmd );
              
              cmd.m_Type = GL_DIFFUSE;
              cmd.m_R = material->diffuse[0];
              cmd.m_G = material->diffuse[1];
              cmd.m_B = material->diffuse[2];
              cmd.m_A = material->diffuse[3];
              commands.Append( cmd );
              
              cmd.m_Type = GL_SPECULAR;
              cmd.m_R = material->specular[0];
              cmd.m_G = material->specular[1];
              cmd.m_B = material->specular[2];
              cmd.m_A = material->specular[3];
              commands.Append( cmd );
            }
            
            {
              SetMaterialScalar cmd;
              
              cmd.m_Type = GL_SHININESS;
              cmd.m_Param = material->shininess;
              commands.Append( cmd );
            }
          }
          
          if ( mode & Modes::Texture )
          {
            Texture* texture = material->texture >= 0 ? &mesh->textures[ material->texture ] : 0;        
            if ( texture != current_texture )
            {
              current_texture = texture;

              SetTexture cmd;
              cmd.m_Texture = texture ? texture->id : 0;
              commands.Append( cmd );
            }
          }
        }
      }
      
      for ( uint32_t v = 0; v < 3; v++ )
      {
        assert(triangle->vindices[v]>=1 && triangle->vindices[v]<=mesh->numvertices);
        vertices.Append( *(Vector*)&mesh->vertices[3 * triangle->vindices[v]] );

        if (triangle->nindices[v]!=-1)
        {
          assert(triangle->nindices[v]>=1 && triangle->nindices[v]<=mesh->numnormals);
          normals.Append( *(Vector*)&mesh->normals[3 * triangle->nindices[v]] );
        }
        else
        {
          static Vector v;
          v.x = v.y = v.z = 0.f;
          normals.Append( v );
        }
        
        if (triangle->tindices[v]!=-1)
        {
          assert(triangle->tindices[v]>=1 && triangle->tindices[v]<=mesh->numuvs);
          uvs.Append( *(UV*)&mesh->uvs[2 * triangle->tindices[v]] );
        }
        else
        {
          static UV uv;
          uv.u = uv.v = 0.f;
          uvs.Append( uv );
        }
        
        count++;
      }
    }
  }
  
  if ( count )
  {
    DrawPrimitives cmd;
    cmd.m_Primitive = GL_TRIANGLES;
    cmd.m_Start = start;
    cmd.m_Count = count;
    commands.Append( cmd );

    start += count;
    count = 0;
  }

  DBG( commands.Dump() );
  Log( "Total vertices: %d", start );
  
  /* copy buffers into gl runtime */
  glDeleteBuffers(RenderMeshBuffers::Count, buffers);  
  DBG( GLErrorCheck() );
  glGenBuffers(RenderMeshBuffers::Count, buffers);
  DBG( GLErrorCheck() );

  glBindBuffer( GL_ARRAY_BUFFER, buffers[ RenderMeshBuffers::Vertex ] );
  DBG( GLErrorCheck() );
  glBufferData( GL_ARRAY_BUFFER, vertices.GetSize() * sizeof(Vector), vertices.GetData(), GL_STATIC_DRAW );
  DBG( GLErrorCheck() );

  glBindBuffer( GL_ARRAY_BUFFER, buffers[ RenderMeshBuffers::Normal ] );
  DBG( GLErrorCheck() );
  glBufferData( GL_ARRAY_BUFFER, normals.GetSize() * sizeof(Vector), normals.GetData(), GL_STATIC_DRAW );
  DBG( GLErrorCheck() );

  glBindBuffer( GL_ARRAY_BUFFER, buffers[ RenderMeshBuffers::UV ] );
  DBG( GLErrorCheck() );
  glBufferData( GL_ARRAY_BUFFER, uvs.GetSize() * sizeof(UV), uvs.GetData(), GL_STATIC_DRAW );
  DBG( GLErrorCheck() );

  glBindBuffer( GL_ARRAY_BUFFER, NULL );
  DBG( GLErrorCheck() );
}

void RenderMesh::Draw()
{
  glEnableClientState( GL_VERTEX_ARRAY );
  DBG( GLErrorCheck() );
  glBindBuffer( GL_ARRAY_BUFFER, buffers[ RenderMeshBuffers::Vertex ] );
  DBG( GLErrorCheck() );
  glVertexPointer( 3, GL_FLOAT, 0, NULL ); 
  DBG( GLErrorCheck() );

  glEnableClientState( GL_NORMAL_ARRAY );
  DBG( GLErrorCheck() );
  glBindBuffer( GL_ARRAY_BUFFER, buffers[ RenderMeshBuffers::Normal ] );
  DBG( GLErrorCheck() );
  glNormalPointer( GL_FLOAT, 0, NULL ); 
  DBG( GLErrorCheck() );

  if (mode & Modes::Texture)
  {
    glEnable( GL_TEXTURE_2D );
    DBG( GLErrorCheck() );
    glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );
    DBG( GLErrorCheck() );

    glEnableClientState( GL_TEXTURE_COORD_ARRAY );
    DBG( GLErrorCheck() );
    glBindBuffer( GL_ARRAY_BUFFER, buffers[ RenderMeshBuffers::UV ] );
    DBG( GLErrorCheck() );
    glTexCoordPointer( 2, GL_FIXED, 0, NULL );
    DBG( GLErrorCheck() );
  }

  glBindBuffer( GL_ARRAY_BUFFER, NULL );
  DBG( GLErrorCheck() );

  // issue draw commands
  commands.Execute();

  glDisableClientState( GL_VERTEX_ARRAY );
  DBG( GLErrorCheck() );
  glDisableClientState( GL_NORMAL_ARRAY );
  DBG( GLErrorCheck() );
  glDisableClientState( GL_TEXTURE_COORD_ARRAY );
  DBG( GLErrorCheck() );
}

void RenderMesh::Destroy()
{
  glDeleteBuffers(RenderMeshBuffers::Count, buffers);  
  DBG( GLErrorCheck() );
}