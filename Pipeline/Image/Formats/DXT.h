#pragma once

#include "Platform/Types.h"
#include "Platform/Assert.h"

#include "DDS.h"
#include "Pipeline/Image/MipSet.h"
#include "Pipeline/Image/Image.h"

namespace Helium
{
  struct DXTOptions
  {
    uint32_t                   m_face;
    bool                  m_count;
    Image*                m_texture;
    MipSet*               m_mips;

    const MipGenOptions*  m_mip_gen_options[4];
  };

  bool DXTGenerateMipSet(const Image* top_mip, DXTOptions* options);
}