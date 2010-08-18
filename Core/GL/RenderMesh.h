#pragma once

#include "Render.h"
#include "SourceMesh.h"

namespace Core
{
  namespace RenderMeshBuffers
  {
    enum RenderMeshBuffer
    {
      Vertex,
      Normal,
      UV,
      Count,
    };
  }
  typedef RenderMeshBuffers::RenderMeshBuffer RenderMeshBuffer;

  /* RenderMesh: Structure that defines a mesh. */
  struct RenderMesh
  {
    GLuint mode;
    GLuint buffers[ RenderMeshBuffers::Count ];
    RenderCommandBuffer commands;
    
    RenderMesh();
    ~RenderMesh();
    
    /* Build: Flattens the mesh to into buffers based on the mode specified.
     *
     * mode     - a bitwise OR of values describing what is to be rendered.
     *            Modes::None     -  render with only vertices
     *            Modes::Texture  -  render with texture coords
     *            Modes::Material - render with material settings
     */
    void Build( SourceMesh* mesh, GLuint mode = Modes::Default );
    
    /* Draw: Render the buffers to the current OpenGL context
     */
    void Draw();
    
    /* Destroy: Release resources
     */
    void Destroy();
  };
}
