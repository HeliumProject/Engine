#pragma once

#include "SceneNode.h"

#include "Luna/API.h"

// Forwards
namespace Content
{
  class Shader;
}

namespace Luna
{
  // Forwards
  class Scene;

  class Shader : public Luna::SceneNode
  {
    // 
    // Member variables
    // 
  private:
    bool m_Alpha;
    u32 m_BaseTextureSize;
    IDirect3DTexture9* m_BaseTexture;

    //
    // Runtime Type Info
    //

    LUNA_DECLARE_TYPE( Shader, Luna::SceneNode );
    static void InitializeType();
    static void CleanupType();

  public:
    Shader( Luna::Scene* s, Content::Shader* shader );

    virtual i32 GetImageIndex() const NOC_OVERRIDE;
    virtual tstring GetApplicationTypeName() const NOC_OVERRIDE;

    virtual void Create() NOC_OVERRIDE;
    virtual void Delete() NOC_OVERRIDE;

    bool GetAlpha() const;
    IDirect3DTexture9* GetBaseTexture() const;

  private:
    bool LoadTexture();
  };
}
