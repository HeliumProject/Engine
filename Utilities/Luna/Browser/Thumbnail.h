#pragma once

#include "Foundation/Memory/SmartPtr.h"

namespace Render
{
  class D3DManager;
}

namespace Luna
{
  class Thumbnail : public Nocturnal::RefCountBase< Thumbnail >
  {
  public:
    Thumbnail( Render::D3DManager* d3dManager );
    Thumbnail( Render::D3DManager* d3dManager, IDirect3DTexture9* texture );
    virtual ~Thumbnail();

    bool FromIcon( HICON icon );
    bool IsFromIcon() const;

    inline IDirect3DTexture9* GetTexture() const
    {
      return m_Texture;
    }

  private:
    Render::D3DManager* m_D3DManager;
    IDirect3DTexture9* m_Texture;
    bool m_IsFromIcon;
  };
  typedef Nocturnal::SmartPtr< Thumbnail > ThumbnailPtr;
  typedef std::vector< ThumbnailPtr > V_ThumbnailPtr;
}
