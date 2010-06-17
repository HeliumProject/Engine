#pragma once

#include "Pipeline/API.h"
#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Asset/AssetType.h"
#include "Pipeline/Asset/Attributes/TextureMapAttribute.h" // we share some enums with the texture maps (AlphaType)

#include "Foundation/TUID.h"

namespace Finder
{
  class FileSpec;
}

namespace Asset
{
  typedef std::set< AssetTypes::AssetType > S_AssetType;

  // UV Wrap Mode
  namespace WrapModes
  {
    enum WrapMode
    {
      UV_WRAP,
      UV_CLAMP,
    };
    static void WrapModeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(UV_WRAP, "UV_WRAP", "WRAP");
      info->AddElement(UV_CLAMP, "UV_CLAMP", "CLAMP");
    }
  }
  typedef WrapModes::WrapMode WrapMode;

  // WetSurface Type
  namespace WetSurfaceTypes
  {
    enum WetSurfaceType
    {
      WET_SURFACE_NONE, 
      WET_SURFACE_SKIN,
      WET_SURFACE_DIRT,
      WET_SURFACE_CLOTH, 
      WET_SURFACE_BRICK, 
      WET_SURFACE_FOLIAGE, 
      WET_SURFACE_GUN,
      WET_SURFACE_METAL,
    };

    static void WetSurfaceTypeEnumerateEnumeration( Reflect::Enumeration* info )
    {
      info->AddElement(WET_SURFACE_NONE,     "WET_SURFACE_NONE",        "None");
      info->AddElement(WET_SURFACE_SKIN,     "WET_SURFACE_SKIN",        "Skin");
      info->AddElement(WET_SURFACE_DIRT,     "WET_SURFACE_DIRT",        "Dirt");
      info->AddElement(WET_SURFACE_CLOTH,    "WET_SURFACE_CLOTH",       "Cloth");
      info->AddElement(WET_SURFACE_BRICK,    "WET_SURFACE_BRICK",       "Brick");
      info->AddElement(WET_SURFACE_FOLIAGE,  "WET_SURFACE_FOLIAGE",     "Foliage");
      info->AddElement(WET_SURFACE_GUN,      "WET_SURFACE_GUN",         "Gun");
      info->AddElement(WET_SURFACE_METAL,    "WET_SURFACE_METAL",       "Metal");
   }
  }
  typedef WetSurfaceTypes::WetSurfaceType WetSurfaceType;

  // Base class for all shader types
  class PIPELINE_API ShaderAsset NOC_ABSTRACT : public AssetClass
  {
  public:
    bool m_DoubleSided;
    WrapMode m_WrapModeU;
    WrapMode m_WrapModeV;
    AlphaType m_AlphaMode;
    WetSurfaceType m_WetSurfaceMode;

    ShaderAsset()
      : m_DoubleSided( false )
      , m_WrapModeU( WrapModes::UV_WRAP )
      , m_WrapModeV( WrapModes::UV_WRAP )
      , m_AlphaMode ( AlphaTypes::ALPHA_OPAQUE )
      , m_WetSurfaceMode( WetSurfaceTypes::WET_SURFACE_BRICK)
    {
    }

    // verify that this shader can be associated with the given asset type
    bool ValidateAssetType( AssetTypes::AssetType assetType );

    // Returns the set of asset types that this kind of shader is allowed to
    // be attached to.  Override as required.
    virtual void GetAllowableAssetTypes( S_AssetType& assetTypes ) const;

    virtual bool ValidateClass( std::string& error ) const NOC_OVERRIDE;

    virtual bool ValidateCompatible( const Attribute::AttributePtr& attr, std::string& error ) const NOC_OVERRIDE;

    virtual bool IsBuildable() const NOC_OVERRIDE;

    virtual bool IsCinematicShader() const;

    //
    //  There is an important abstraction here that allows different shader types to have
    // different format versions.  otherwise a logic change that could only require a small
    // subset of the shaders to rebuild would force *all* the shaders to rebuild
    //
    virtual const Finder::FileSpec& GetBuiltFileSpec() const;

    // Marks any texture channels with the specified texture as needing to be resent to the
    // devkit (or not).
    virtual void SetTextureDirty( const Nocturnal::Path& path, bool dirty );

    REFLECT_DECLARE_ABSTRACT( ShaderAsset, AssetClass );

    static void EnumerateClass( Reflect::Compositor<ShaderAsset>& comp );
  };

  typedef Nocturnal::SmartPtr< ShaderAsset > ShaderAssetPtr;
  typedef std::vector< ShaderAssetPtr > V_ShaderAsset;
}