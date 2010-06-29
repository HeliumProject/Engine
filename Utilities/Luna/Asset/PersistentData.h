#pragma once

#include "AssetNode.h"
#include "Core/Persistent.h"

namespace Inspect
{
  class ReflectClipboardData;
  typedef Nocturnal::SmartPtr< ReflectClipboardData > ReflectClipboardDataPtr;
}

namespace Luna
{
  // Forwards
  class AssetManager;
  class PersistentData;
  typedef Nocturnal::SmartPtr< Luna::PersistentData > PersistentDataPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Provides common functionality for all Luna::Persistent derived objects in the
  // Asset Editor.
  // 
  class PersistentData : public Luna::Persistent
  {
  protected:
    Luna::AssetManager* m_AssetManager;

  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::PersistentData, Luna::Persistent );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr CreatePersistentData( Reflect::Element* element, Luna::AssetManager* assetManager );

  protected:
    PersistentData( Reflect::Element* element, Luna::AssetManager* assetManager );
  public:
    virtual ~PersistentData();
    Luna::AssetManager* GetAssetManager() const;
    virtual Inspect::ReflectClipboardDataPtr GetClipboardData() const;
    virtual bool CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data );
    virtual bool HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch = NULL );
    virtual Undo::CommandPtr CopyFrom( Luna::PersistentData* src );
  };
}
