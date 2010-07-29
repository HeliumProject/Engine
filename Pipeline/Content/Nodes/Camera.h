#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Content/Nodes/HierarchyNode.h"

namespace Content
{
  //
  // Camera
  //
  
  class PIPELINE_API Camera : public HierarchyNode
  {
  public:
    virtual void Host(ContentVisitor* visitor); 

    Math::Matrix4 m_View;
    float m_FOV;
    u32 m_Width;
    u32 m_Height;

    Camera ()
      : m_FOV ( 0.f )
      , m_Width( 0 )
      , m_Height( 0 )
    {

    }

    REFLECT_DECLARE_CLASS(Camera, HierarchyNode);
    static void EnumerateClass( Reflect::Compositor<Camera>& comp );
  };

  typedef Helium::SmartPtr<Camera> CameraPtr;
}