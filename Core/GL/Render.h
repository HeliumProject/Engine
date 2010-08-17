#pragma once

#include "OpenGL.h"
#include "NTL/Array.h"
#include "NTL/Buffer.h"

namespace Core
{
  void GLErrorCheck();
  
  struct RenderCommand
  {
    virtual void Dump() = 0;
    virtual void Execute() = 0;
  };

  struct SetMaterialScalar : RenderCommand
  {
    virtual void Dump();
    virtual void Execute();

    GLuint m_Type;
    GLfloat m_Param;
  };
  
  struct SetMaterialVector : RenderCommand
  {
    virtual void Dump();
    virtual void Execute();

    GLuint m_Type;
    GLfloat m_R, m_G, m_B, m_A;
  };
  
  struct SetTexture : RenderCommand
  {
    virtual void Dump();
    virtual void Execute();
    
    GLuint m_Texture;
  };
  
  struct DrawPrimitives : RenderCommand
  {
    virtual void Dump();
    virtual void Execute();

    GLuint m_Primitive;
    GLuint m_Start;
    GLuint m_Count;
  };
  
  class RenderCommandBuffer
  {
  public:
    void Clear();
    void Dump();
    void Execute();
    
    template< class T >
    T* Append( const T& command )
    {
      T* result = m_Buffer.Append( command );
      uintptr_t offset = ((uintptr_t)result - (uintptr_t)m_Buffer.GetData());
      m_Offsets.Append( offset );
      return result;
    }

  private:
    NTL::Buffer            m_Buffer;
    NTL::Array<uintptr_t>  m_Offsets;
  };
}
