#pragma once

#include "Platform/Types.h"
#include "Platform/Assert.h"

#include "DDS.h"
#include "Pipeline/Texture/Image/MipSet.h"
#include "Pipeline/Texture/Texture.h"

namespace Nocturnal
{
  struct DXTOptions
  {
    u32                   m_face;
    bool                  m_count;
    Texture*              m_texture;
    MipSet*               m_mips;

    const MipGenOptions*  m_mip_gen_options[4];
  };

  bool DXTGenerateMipSet(const Texture* top_mip, DXTOptions* options);
}