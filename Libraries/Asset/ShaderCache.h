#pragma once

#include "API.h"

#include "Common/Types.h"

#include "Reflect/Element.h"
#include "Reflect/Registry.h"
#include "Reflect/Serializers.h"

#include "TUID/TUID.h"

#include <map>

namespace Asset
{
  // stupid forward declares!
  class ASSET_API ShaderCache;
  typedef Nocturnal::SmartPtr< ShaderCache > ShaderCachePtr;

  class ASSET_API ShaderCache : public Reflect::Element
  {
  public:

    std::map< tuid, u32 > m_CacheTimes;
    std::map< tuid, std::string > m_ShaderNames;

    ShaderCache();

    static ShaderCachePtr Load();
    void Update();

    void GetShaderNames( V_string &shaderNames ) const;

    REFLECT_DECLARE_CLASS( ShaderCache, Reflect::Element );

    static void EnumerateClass( Reflect::Compositor<ShaderCache>& comp );
  };
}