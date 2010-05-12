#pragma once

#include "AttributeWrapper.h"
#include "Asset/FileBackedAttribute.h"


namespace Luna
{
  // Forwards
  class ContextMenuArgs;
  typedef Nocturnal::SmartPtr< ContextMenuArgs > ContextMenuArgsPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper around Asset::FileBackedAtttribute.
  // 
  class FileBackedAttribute : public Luna::AttributeWrapper
  {
  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::FileBackedAttribute, Luna::AttributeWrapper );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr CreateFileBackedAttribute( Reflect::Element* attribute, Luna::AssetManager* assetManager );

  protected:
    FileBackedAttribute( Asset::FileBackedAttribute* attribute, Luna::AssetManager* assetManager );
  public:
    virtual ~FileBackedAttribute();
    virtual std::string GetName() const NOC_OVERRIDE;
    virtual void PopulateContextMenu( ContextMenuItemSet& menu ) NOC_OVERRIDE;
    tuid GetFileID() const;
    void SetFileID( const tuid& fileID );
    std::string GetFilePath() const;

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
