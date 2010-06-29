#pragma once

#include "FileBackedComponent.h"
#include "Pipeline/Asset/Components/TextureMapComponent.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Wrapper around Asset::TextureMapAtttribute.
  // 
  class TextureMapComponent : public Luna::FileBackedComponent
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::TextureMapComponent, Luna::FileBackedComponent );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr CreateTextureMapComponent( Reflect::Element* attribute, Luna::AssetManager* assetManager );

  protected:
    TextureMapComponent( Asset::TextureMapComponent* attribute, Luna::AssetManager* assetManager );
  public:
    virtual ~TextureMapComponent();

    virtual Undo::CommandPtr CopyFrom( Luna::PersistentData* src ) NOC_OVERRIDE;
  };
  typedef Nocturnal::SmartPtr< Luna::TextureMapComponent > TextureMapComponentPtr;
}
