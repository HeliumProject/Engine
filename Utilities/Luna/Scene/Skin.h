#pragma once

#include "SceneNode.h"

// Forwards
namespace Content
{
  class Skin;
  class Influence;
}

namespace Luna
{
  class Scene;
  class Transform;
  class Mesh;

  typedef std::vector< Luna::Transform* > V_TransformDumbPtr;

  class Skin : public Luna::SceneNode
  {
    //
    // Member variables
    //
    
  protected:
    Luna::Mesh* m_Mesh;
    V_TransformDumbPtr m_InfluenceObjects;

    Math::V_Matrix4 m_DeformMatrices;
    Math::V_Matrix4 m_SkinMatrices;

    //
    // Runtime Type Info
    //

    LUNA_DECLARE_TYPE( Skin, Luna::SceneNode );
    static void InitializeType();
    static void CleanupType();

    // 
    // Member functions
    // 
  public:
    Skin( Luna::Scene* scene, Content::Skin* skin );
    virtual ~Skin();
    virtual void Initialize() NOC_OVERRIDE;
    virtual void Evaluate(GraphDirection direction) NOC_OVERRIDE;

  private:
    void BlendMatrix(const Luna::Transform* transform, const Content::Influence* influence, Math::Matrix4& matrix);
  };
}
