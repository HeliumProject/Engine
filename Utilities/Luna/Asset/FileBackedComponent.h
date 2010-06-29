#pragma once

#include "ComponentWrapper.h"
#include "Pipeline/Asset/Components/FileBackedComponent.h"


namespace Luna
{
  // Forwards
  class ContextMenuArgs;
  typedef Nocturnal::SmartPtr< ContextMenuArgs > ContextMenuArgsPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper around Asset::FileBackedAtttribute.
  // 
  class FileBackedComponent : public Luna::ComponentWrapper
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::FileBackedComponent, Luna::ComponentWrapper );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr CreateFileBackedComponent( Reflect::Element* attribute, Luna::AssetManager* assetManager );

  protected:
    FileBackedComponent( Asset::FileBackedComponent* attribute, Luna::AssetManager* assetManager );
  public:
    virtual ~FileBackedComponent();
    virtual tstring GetName() const NOC_OVERRIDE;
    virtual void PopulateContextMenu( ContextMenuItemSet& menu ) NOC_OVERRIDE;
    Nocturnal::Path& GetPath() const;
    void SetPath( Nocturnal::Path& path );
    tstring GetFilePath() const;
    void SetFilePath( const tstring& path );

  protected:
    void OnElementChanged( const Reflect::ElementChangeArgs& args );
    void OnPreferenceChanged( const Reflect::ElementChangeArgs& args );
    void OnOpen( const ContextMenuArgsPtr& args );
    void OnChangePath( const ContextMenuArgsPtr& args );
    void OnChangePathFinder( const ContextMenuArgsPtr& args );
    void OnCheckOutPath( const ContextMenuArgsPtr& args );
    void OnRevisionHistoryPath( const ContextMenuArgsPtr& args );
  };
}
