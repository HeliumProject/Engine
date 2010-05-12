#pragma once

#include "FileBackedAttribute.h"
#include "Asset/TextureMapAttribute.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Wrapper around Asset::TextureMapAtttribute.
  // 
  class TextureMapAttribute : public Luna::FileBackedAttribute
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::TextureMapAttribute, Luna::FileBackedAttribute );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr CreateTextureMapAttribute( Reflect::Element* attribute, Luna::AssetManager* assetManager );

  protected:
    TextureMapAttribute( Asset::TextureMapAttribute* attribute, Luna::AssetManager* assetManager );
  public:
    virtual ~TextureMapAttribute();

    virtual Undo::CommandPtr CopyFrom( Luna::PersistentData* src ) NOC_OVERRIDE;
  };
  typedef Nocturnal::SmartPtr< Luna::TextureMapAttribute > TextureMapAttributePtr;
}
