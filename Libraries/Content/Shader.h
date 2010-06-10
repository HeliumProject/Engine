#pragma once

#include "SceneNode.h"
#include "UID/TUID.h"

namespace Content
{
  //
  // Shader stores UV modifiers, base material color
  //

  class CONTENT_API Shader : public SceneNode
  {
  public:
    // Does the texture wrap horizontally
    bool m_WrapU;

    // Does the texture wrap vertically
    bool m_WrapV;

    // Repetition frequency horizontally
    float m_RepeatU;

    // Repetition frequency vertically
    float m_RepeatV;

    // Base material color (128 bit)
    Math::Vector4 m_BaseColor;

    Shader ()
      : m_WrapU (true)
      , m_WrapV (true)
      , m_RepeatU (1.0)
      , m_RepeatV (1.0)
    {
    }

    Shader (const Nocturnal::UID::TUID& id)
      : SceneNode (id)
      , m_WrapU (true)
      , m_WrapV (true)
      , m_RepeatU (1.0)
      , m_RepeatV (1.0)
    {
    }

    virtual void Host(ContentVisitor* visitor); 

  public:

    REFLECT_DECLARE_CLASS(Shader, SceneNode);

    static void EnumerateClass( Reflect::Compositor<Shader>& comp );
  };

  typedef Nocturnal::SmartPtr<Shader> ShaderPtr;
  typedef std::vector<ShaderPtr> V_Shader;

  struct CompareShaders
  {
   bool operator ()( const ShaderPtr& lhs, const ShaderPtr& rhs )
   {
     return lhs->m_ID < rhs->m_ID;
   }    
  };
  typedef std::set<ShaderPtr> S_Shader;
}