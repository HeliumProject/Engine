#pragma once

#include "API.h"
#include "TextureEnums.h"

#include "Texture/Texture.h"

#include "TextureProcess/TextureProcess.h"

#include "Reflect/Serializers.h"

namespace Asset
{
  class ASSET_API TextureEntry : public Reflect::Element
  {
  public:
    // name for the generated .h file
    std::string m_EnumerationName;

    // the texture file path to the data
    std::string m_TextureFile;

    // the output color format for the packer
    TextureColorFormat m_TextureFormat;

    // scaling options for the packer
    float m_RelativeScaleX;
    float m_RelativeScaleY;
    bool m_ForcePowerOf2;

    // how to setup the runtime register values
    TextureWrapMode m_WrapU;
    TextureWrapMode m_WrapV;
    TextureWrapMode m_WrapW;

    TextureEntry()
      : m_TextureFormat ( TextureColorFormats::DXT5 )
      , m_RelativeScaleX (1.f)
      , m_RelativeScaleY (1.f)
      , m_ForcePowerOf2 (false)
      , m_WrapU (TextureWrapModes::Clamp)
      , m_WrapV (TextureWrapModes::Clamp)
      , m_WrapW (TextureWrapModes::Clamp)
    {

    }

    REFLECT_DECLARE_CLASS(TextureEntry, Reflect::Element);

    static void EnumerateClass( Reflect::Compositor<TextureEntry>& comp );

    void ToDefinition(const TextureProcess::DefinitionPtr& definition)
    {
      definition->m_enum = m_EnumerationName;
      definition->m_texture_file = m_TextureFile;
      definition->m_output_format = (IG::OutputColorFormat)m_TextureFormat;
      definition->m_force_power_of_2 = m_ForcePowerOf2;
      definition->m_relscale_x = m_RelativeScaleX;
      definition->m_relscale_y = m_RelativeScaleY;
      definition->m_runtime.m_wrap_u = (IG::UVAddressMode)m_WrapU;
      definition->m_runtime.m_wrap_v = (IG::UVAddressMode)m_WrapV;
      definition->m_runtime.m_wrap_w = (IG::UVAddressMode)m_WrapW;
    }

    void FromDefinition(const TextureProcess::DefinitionPtr& definition)
    {
      m_EnumerationName = definition->m_enum;
      m_TextureFile = definition->m_texture_file;
      m_TextureFormat = (TextureColorFormat)definition->m_output_format;
      m_ForcePowerOf2 = definition->m_force_power_of_2;
      m_RelativeScaleX = definition->m_relscale_x;
      m_RelativeScaleY = definition->m_relscale_y;
      m_WrapU = (TextureWrapMode)definition->m_runtime.m_wrap_u;
      m_WrapV = (TextureWrapMode)definition->m_runtime.m_wrap_v;
      m_WrapW = (TextureWrapMode)definition->m_runtime.m_wrap_w;
    }
  };

  typedef Nocturnal::SmartPtr<TextureEntry> TextureEntryPtr;
  typedef std::vector<TextureEntryPtr> V_TextureEntry;
}