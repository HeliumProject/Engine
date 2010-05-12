#pragma once

#include "AssetClass.h"
#include "AssetCommand.h"
#include "AssetNode.h"

#include "Editor/DocumentManager.h"
#include "Inspect/ReflectClipboardData.h"
#include "Core/Selection.h"
#include "Undo/Queue.h"

namespace AssetBuilder
{
  class AssetBuiltArgs;
  typedef Nocturnal::SmartPtr< AssetBuiltArgs > AssetBuiltArgsPtr;
};

namespace Luna
{
  // Forwards
  class AssetEditor;
  class AssetDocument;
  struct DocumentChangedArgs;

  class HierarchyChangeToken;
  typedef Nocturnal::SmartPtr< HierarchyChangeToken > LHierarchyChangeTokenPtr;

  // Typedefs
  typedef std::map< tuid, Luna::AssetClassPtr > M_AssetClassSmartPtr;
  typedef std::set< AttributeWrapperPtr > S_AttributeSmartPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Arguments for events concerning the loading or unloading of asset classes.
  // 
  struct AssetLoadArgs
  {
    Luna::AssetClass* m_AssetClass;

    AssetLoadArgs( Luna::AssetClass* asset )
    : m_AssetClass( asset )
    {
    }
  };
  typedef Nocturnal::Signature< void, const AssetLoadArgs& > AssetLoadSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Arguments for events concerning major hierarchy changes.
  // 
  struct HierarchyChangeArgs
  {
    // Nothing yet...
  };
  typedef Nocturnal::Signature< void, const HierarchyChangeArgs& > HierarchyChangeSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Manager of all data pertaining to the Asset Editor.
  // 
  class AssetManager : public DocumentManager
  {
  private:
    typedef std::map< const Undo::Command*, Luna::AssetCommandPtr > M_AssetCommandSmartPtr;

  private:
    AssetEditor* m_AssetEditor;
    Undo::Queue m_UndoQueue;
    Selection m_Selection;
    M_AssetClassSmartPtr m_AssetClasses;
    M_AssetCommandSmartPtr m_AssetCommands;
    Luna::AssetNodePtr m_RootNode;

  public:
    AssetManager( AssetEditor* assetEditor );
    virtual ~AssetManager();

    AssetEditor* GetAssetEditor() const;
    Selection& GetSelection();
    Undo::Queue& GetUndoQueue();
    Luna::AssetNode* GetRootNode() const;

    friend class HierarchyChangeToken;
    LHierarchyChangeTokenPtr GetHierarchyChangeToken();

    const M_AssetClassSmartPtr& GetAssets() const;
    Luna::AssetClass* FindAsset( const tuid& fileID );

    AssetDocument* FindAssetDocument( const Luna::AssetClass* asset ) const;

    virtual DocumentPtr OpenPath( const std::string& path, std::string& error ) NOC_OVERRIDE;
    Luna::AssetClass* Open( const std::string& path, std::string& error, bool addToRoot = false );

    virtual bool Save( DocumentPtr document, std::string& error ) NOC_OVERRIDE;
    bool Save( Luna::AssetClass* asset, bool prompt, std::string& error );
    bool SaveSelected( std::string& error );
    
    bool CloseSelected();
    virtual bool CloseAll() NOC_OVERRIDE;

    void CheckOutSelected();
    void RevisionHistorySelected();

    size_t GetSelectedAssets( S_AssetClassDumbPtr& list ) const;
    size_t GetSelectedAssetIDs( S_tuid& fileIDs ) const;
    size_t GetSelectedAttributes( S_AttributeSmartPtr& list ) const;
    
    bool IsEditable() const;
    bool IsEditable( const Luna::AssetClass* assetClass ) const;
    
    bool Push( const Undo::CommandPtr& command );
    
    void CanCopySelection( bool& canCopy, bool& canMove ) const;
    Inspect::ReflectClipboardDataPtr CopySelection( S_AssetNodeDumbPtr& parents, bool& canBeMoved ) const;

    void DuplicateAsset( Luna::AssetClassPtr assetClass );
    void RenameAsset( Luna::AssetClassPtr assetClass );
    void DeleteAsset( Luna::AssetClassPtr assetClass );

    void AssetBuilt( const AssetBuilder::AssetBuiltArgsPtr& args );

    // Temporary HACK
    void ClearUndoQueue();

    // Helpers
  private:
    Luna::AssetClass* CreateAssetClass( const Asset::AssetClassPtr& package );
    AssetDocument* CreateDocument( Luna::AssetClass* asset );
    void OnDocumentClosed( const DocumentChangedArgs& args );
    void CloseAsset( Luna::AssetClassPtr assetClass );

    // Callbacks
  private:
    bool UndoingOrRedoing( const Undo::QueueChangeArgs& args );

    // Listeners
  private:
    AssetLoadSignature::Event m_AssetLoaded;
  public:
    void AddAssetLoadedListener( const AssetLoadSignature::Delegate& listener )
    {
      m_AssetLoaded.Add( listener );
    }
    void RemoveAssetLoadedListener( const AssetLoadSignature::Delegate& listener )
    {
      m_AssetLoaded.Remove( listener );
    }
  private:
    AssetLoadSignature::Event m_AssetUnloading;
  public:
    void AddAssetUnloadingListener( const AssetLoadSignature::Delegate& listener )
    {
      m_AssetUnloading.Add( listener );
    }
    void RemoveAssetUnloadingListener( const AssetLoadSignature::Delegate& listener )
    {
      m_AssetUnloading.Remove( listener );
    }
  private:
    HierarchyChangeSignature::Event m_HierarchyChangeStarting;
  public:
    void AddHierarchyChangeStartingListener( const HierarchyChangeSignature::Delegate& listener )
    {
      m_HierarchyChangeStarting.Add( listener );
    }
    void RemoveHierarchyChangeStartingListener( const HierarchyChangeSignature::Delegate& listener )
    {
      m_HierarchyChangeStarting.Remove( listener );
    }
  private:
    HierarchyChangeSignature::Event m_HierarchyChangeFinished;
  public:
    void AddHierarchyChangeFinishedListener( const HierarchyChangeSignature::Delegate& listener )
    {
      m_HierarchyChangeFinished.Add( listener );
    }
    void RemoveHierarchyChangeFinishedListener( const HierarchyChangeSignature::Delegate& listener )
    {
      m_HierarchyChangeFinished.Remove( listener );
    }
  };


  /////////////////////////////////////////////////////////////////////////////
  // Class for passing an asset manager pointer through client data (such as
  // in context menu callbacks).
  // 
  class AssetManagerClientData : public Luna::Object
  {
  public:
    Luna::AssetManager* m_AssetManager;

    LUNA_DECLARE_TYPE( Luna::AssetManagerClientData, Luna::Object );
    static void InitializeType()
    {
      Reflect::RegisterClass<Luna::AssetManagerClientData>( "Luna::AssetManagerClientData" );
    }

    static void CleanupType()
    {
      Reflect::UnregisterClass<Luna::AssetManagerClientData>();
    }

    AssetManagerClientData( Luna::AssetManager* manager )
      : m_AssetManager( manager )
    {
    }
  };
}
