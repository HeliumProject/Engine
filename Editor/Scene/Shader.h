#pragma once

#include "SceneNode.h"

#include "Editor/API.h"

// Forwards
namespace Content
{
  class Shader;
}

namespace Editor
{
  // Forwards
  class Scene;

  class Shader : public Editor::SceneNode
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

    LUNA_DECLARE_TYPE( Shader, Editor::SceneNode );
    static void InitializeType();
    static void CleanupType();

  public:
    Shader( Editor::Scene* s, Content::Shader* shader );

    virtual i32 GetImageIndex() const HELIUM_OVERRIDE;
    virtual tstring GetApplicationTypeName() const HELIUM_OVERRIDE;

    virtual void Create() HELIUM_OVERRIDE;
    virtual void Delete() HELIUM_OVERRIDE;

    bool GetAlpha() const;
    IDirect3DTexture9* GetBaseTexture() const;

  private:
    bool LoadTexture();
  };
}
