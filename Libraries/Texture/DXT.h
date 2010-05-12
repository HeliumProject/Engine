#pragma once

#include "common/types.h"
#include "common/assert.h"

#include "DDS.h"
#include "MipSet.h"
#include "Texture.h"

namespace IG
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