#include "Precompile.h"
#include "AssetClass.h"

#include "AssetEditor.h"
#include "AssetDocument.h"
#include "AssetManager.h"
#include "ComponentContainer.h"
#include "ComponentExistenceCommand.h"
#include "ComponentNode.h"
#include "ContextMenuCallbacks.h"
#include "FieldFactory.h"
#include "PersistentDataFactory.h"

#include "Foundation/Component/ComponentHandle.h"
#include "Pipeline/Asset/Classes/ShaderAsset.h"
#include "Foundation/Container/Insert.h" 
#include "Foundation/String/Natural.h"
#include "Foundation/Log.h"
#include "Application/Inspect/DragDrop/ClipboardElementArray.h"
#include "Core/Enumerator.h"
#include "Pipeline/Asset/Manifests/EntityManifest.h"
#include "Application/UI/ImageManager.h"

#include <algorithm>

// Using
using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::AssetClass );


///////////////////////////////////////////////////////////////////////////////
// Static initialization for all Luna::AssetClass types.
// 
void AssetClass::InitializeType()
{
  Reflect::RegisterClass<Luna::AssetClass>( TXT( "Luna::AssetClass" ) );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::AssetClass >(), &Luna::AssetClass::Create );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup of all Luna::AssetClass types.
// 
void AssetClass::CleanupType()
{
  Reflect::UnregisterClass<Luna::AssetClass>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function for making an instance of this class.
// 
Luna::PersistentDataPtr AssetClass::Create( Reflect::Element* assetClass, Luna::AssetManager* manager )
{
  return new Luna::AssetClass( Reflect::AssertCast< Asset::AssetClass >( assetClass ), manager );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
AssetClass::AssetClass( Asset::AssetClass* assetClass, Luna::AssetManager* manager )
: Luna::PersistentData( assetClass, manager )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
AssetClass::~AssetClass()
{
}

///////////////////////////////////////////////////////////////////////////////
// Loads the persistent data into this asset class.
// 
void AssetClass::Pack()
{
  M_ComponentSmartPtr::const_iterator componentItr = m_Components.begin();
  M_ComponentSmartPtr::const_iterator componentEnd = m_Components.end();
  for ( ; componentItr != componentEnd; ++componentItr )
  {
    const Luna::ComponentWrapperPtr& component = componentItr->second;
    component->Pack();
  }

  __super::Pack();

  tstring error;
  if ( !GetPackage< Asset::AssetClass >()->ValidateClass( error ) )
  {
    tstring msg = TXT( "The following error was encountered while packing '" ) + GetName() + TXT( "'.\n\n" );
    msg += error;
    wxMessageBox( msg.c_str(), TXT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, GetAssetManager()->GetAssetEditor() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Loads the persistent data into this asset class.
// 
void AssetClass::Unpack()
{
  __super::Unpack();

  Asset::AssetClass* package = GetPackage< Asset::AssetClass >();

  NOC_ASSERT( package );
  if ( !m_Components.empty() )
  {
    NOC_ASSERT( package->GetComponents().empty() );
  }

  Component::M_Component::const_iterator componentItr = package->GetComponents().begin();
  Component::M_Component::const_iterator componentEnd = package->GetComponents().end();
  for ( ; componentItr != componentEnd; ++componentItr )
  {
    const Component::ComponentPtr& component = componentItr->second;
    Luna::ComponentWrapperPtr componentWrapper = PersistentDataFactory::GetInstance()->CreateTyped< Luna::ComponentWrapper >( component, GetAssetManager() );
    if ( !componentWrapper.ReferencesObject() )
    {
      throw ( Nocturnal::Exception( TXT( "Internal error - Unable to create component in Luna::AssetClass::Unpack" ) ) );
    }
    AddComponent( componentWrapper, false );
  }

  tstring error;
  if ( !package->ValidateClass( error ) )
  {
    tstring msg = TXT( "The following error was encountered while unpacking '" ) + GetName() + TXT( "'.\n\n" );
    msg += error;
    wxMessageBox( msg.c_str(), TXT( "Error" ), wxCENTER | wxICON_ERROR | wxOK, GetAssetManager()->GetAssetEditor() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Saves this asset class to disk.  Returns true on success.
// 
bool AssetClass::Save( tstring& error )
{
  Pack();

  Asset::AssetClass* assetClass = GetPackage< Asset::AssetClass >();
  tstring path = GetFilePath();
  if ( path.empty() )
  {
    Log::Error( TXT( "No file path for asset.\n" ) );
    return false;
  }

  bool saved = false;
  try
  {
    Reflect::Archive::ToFile( assetClass, path, new Asset::AssetVersion(), NULL );
    saved = true;
  }
  catch ( const Nocturnal::Exception& e )
  {
    error = e.Get();
    saved = false;
  }

  return saved;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the name of this asset, determined by the file name on disk where
// this asset is located.
// 
const tstring& AssetClass::GetName() const
{
  if ( m_Name.empty() )
  {
    const Asset::AssetClass* package = GetPackage< Asset::AssetClass >();
    try
    {
      m_Name = package->GetShortName();
    }
    catch ( Nocturnal::Exception& e )
    {
      Log::Error( TXT( "Unable to get asset name: %s\n" ), e.What() );
      NOC_BREAK();
    }
  }

  return m_Name;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the path to this asset's location on disk.
// 
tstring AssetClass::GetFilePath()
{
    Asset::AssetClass* package = GetPackage< Asset::AssetClass >();
    return package->GetPath().Get();
}

///////////////////////////////////////////////////////////////////////////////
// Returns the icon to use for this asset.
// 
tstring AssetClass::GetIcon() const
{
  return TXT( "null.png" );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of components belonging to this asset.
// 
const M_ComponentSmartPtr& AssetClass::GetComponents()
{
  return m_Components;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the component in the specified slot.
// 
Luna::ComponentWrapper* AssetClass::FindComponent( i32 slot )
{
  M_ComponentSmartPtr::const_iterator found = m_Components.find( slot );
  if ( found != m_Components.end() )
  {
    return found->second;
  }
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Fills out the specified menu set with the items appropriate to editing any
// asset class.
// 
void AssetClass::PopulateContextMenu( ContextMenuItemSet& menu )
{
  Asset::AssetClass* assetPkg = GetPackage< Asset::AssetClass >();
  Luna::ObjectPtr clientData = new Luna::AssetManagerClientData( m_AssetManager );

  const size_t numSelected = m_AssetManager->GetSelection().GetItems().Size();
  const bool isShader = assetPkg->HasType( Reflect::GetType< Asset::ShaderAsset >() );

  bool areAssetsLocked = true;
  const tchar* networkLockAssetsPath = _tgetenv( TXT( "NOC_NETWORK_LOCK_ASSETS_FILE" ) );
  if ( networkLockAssetsPath != NULL )
  {
    // Faster than FileSystem::Exists
    WIN32_FILE_ATTRIBUTE_DATA lockFileStats;
    if ( ::GetFileAttributesEx( networkLockAssetsPath, GetFileExInfoStandard, &lockFileStats ) )
    {
      areAssetsLocked = true;
    }
    else
    {
      areAssetsLocked = false;
    }
  }

  //std::vector< tstring > staticContentFiles;
  //::AssetManager::GetStaticContentFiles( GetFileID(), staticContentFiles );

  //const bool canPreview = staticContentFiles.size() ? true : false;
  //tstring previewTip = canPreview
  //  ? "Preview the asset in the preview panel."
  //  : "This asset cannot be previewed in the preview panel.";

  const bool canDuplicate = ( numSelected == 1 ) && ( isShader && !areAssetsLocked );
  tstring duplicateToolTip = canDuplicate 
    ? TXT( "Create a copy of this asset using the Asset Manager wizard." )
    : TXT( "Duplicating assets is not allowed or not available at this time." );

  const bool canRename = ( numSelected == 1 ) && ( !areAssetsLocked ) ;
  tstring renameToolTip = canRename 
    ? TXT( "Rename or Move this asset using the Asset Manager wizard." )
    : TXT( "Renaming assets is not allowed or not available at this time." );

  const bool canDelete = ( numSelected == 1 ) && ( !areAssetsLocked );
  tstring deleteToolTip = canDelete 
    ? TXT( "Delete this asset using the Asset Manager wizard." )
    : TXT( "Deleting assets is not allowed or not available at this time." );

  ContextMenuItemPtr menuItem = new ContextMenuItem( TXT( "Preview" ) );
  menuItem->AddCallback( ContextMenuSignature::Delegate( m_AssetManager->GetAssetEditor(), &AssetEditor::OnAssetPreview ) );
  menuItem->Enable( false ); // TODO: enable this
  menu.AppendItem( menuItem );

  menu.AppendSeparator();
  menuItem = new ContextMenuItem( TXT( "Add Component" ) );
  menuItem->AddCallback( ContextMenuSignature::Delegate( m_AssetManager->GetAssetEditor(), &AssetEditor::PromptAddComponents ) );
  menu.AppendItem( menuItem );

  menu.AppendSeparator();
  menuItem = new ContextMenuItem( TXT( "Save" ), TXT( "Save selected Asset(s)" ), Nocturnal::GlobalImageManager().GetBitmap( TXT( "save.png" ) ) );
  menuItem->AddCallback( ContextMenuSignature::Delegate( &Luna::OnSaveSelectedAssets ), clientData );
  menu.AppendItem( menuItem );
  menu.AppendSeparator();

  SubMenuPtr rcsSubMenu = new SubMenu( TXT( "Perforce" ) );

  menuItem = new ContextMenuItem( TXT( "Check Out" ) );
  menuItem->AddCallback( ContextMenuSignature::Delegate( &Luna::OnCheckOutSelectedAssets ), clientData );
  rcsSubMenu->AppendItem( menuItem );

  menuItem = new ContextMenuItem( TXT( "Revision History" ) );
  menuItem->AddCallback( ContextMenuSignature::Delegate( &Luna::OnRevisionHistorySelectedAssets ), clientData );
  rcsSubMenu->AppendItem( menuItem );

  menu.AppendItem( rcsSubMenu );

  menu.AppendSeparator();
  menuItem = new ContextMenuItem( TXT( "Close" ) );
  menuItem->AddCallback( ContextMenuSignature::Delegate( &Luna::OnCloseSelectedAssets ), clientData );
  menu.AppendItem( menuItem );

  menu.AppendSeparator();
  menuItem = new ContextMenuItem( TXT( "Expand" ) );
  menuItem->AddCallback( ContextMenuSignature::Delegate( m_AssetManager->GetAssetEditor(), &AssetEditor::OnExpandSelectedAssets ) );
  menu.AppendItem( menuItem );

  menuItem = new ContextMenuItem( TXT( "Collapse" ) );
  menuItem->AddCallback( ContextMenuSignature::Delegate( m_AssetManager->GetAssetEditor(), &AssetEditor::OnCollapseSelectedAssets ) );
  menu.AppendItem( menuItem );
}

///////////////////////////////////////////////////////////////////////////////
// Registers the specified node as a reference of this asset class.
// 
void AssetClass::RegisterAssetReferenceNode( Luna::AssetReferenceNode* node )
{
  // Sanity check: node better NOT be in the list
  NOC_ASSERT( m_References.find( node ) == m_References.end() );
  m_References.insert( node );
}

///////////////////////////////////////////////////////////////////////////////
// Unregisters the specifed node as a reference of this asset class.
// 
void AssetClass::UnregisterAssetReferenceNode( Luna::AssetReferenceNode* node )
{
  // Sanity check: node better be in the list
  NOC_ASSERT( m_References.find( node ) != m_References.end() );
  m_References.erase( node );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of nodes that are references of this asset class.
// 
const S_AssetReferenceNodeDumbPtr& AssetClass::GetAssetReferenceNodes() const
{
  return m_References;
}

///////////////////////////////////////////////////////////////////////////////
// Called when a control in the enumerator is changed while this asset is 
// selected.  Derived classes can NOC_OVERRIDE this function to do additional work.
// 
void AssetClass::Changed( Inspect::Control* control )
{
  GetPackage< Asset::AssetClass >()->RaiseChanged();
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to determine if the clipboard data references one or more
// components.
// 
static inline bool IsComponentData( const Inspect::ReflectClipboardDataPtr& data )
{
  Inspect::ClipboardElementArray* elementArray = Reflect::ObjectCast< Inspect::ClipboardElementArray >( data );
  if ( elementArray )
  {
    const Reflect::Class* classToCheck = Reflect::Registry::GetInstance()->GetClass( elementArray->GetCommonBaseTypeID() );
    if ( classToCheck->HasType( Reflect::GetType< Component::ComponentBase >() ) )
    {
      // Clipboard contains one or more components.
      return true;
    }
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Assets can accept clipboard data from components or other assets.
// 
bool AssetClass::CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data )
{
  if ( __super::CanHandleClipboardData( data ) )
  {
    return true;
  }

  return IsComponentData( data );
}

///////////////////////////////////////////////////////////////////////////////
// Copies data from the clipboard into this asset.
// 
bool AssetClass::HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch )
{
  if ( CanHandleClipboardData( data ) )
  {
    // Make a copy of the data so that we can use it directly without fear of 
    // modifying what's on the clipboard.
    Inspect::ClipboardElementArrayPtr dataCopy = Reflect::AssertCast< Inspect::ClipboardElementArray >( data->Clone() );
    if ( IsComponentData( data ) )
    {
      // Build a list of components from the source data.
      Component::M_Component srcComponents;
      Reflect::V_Element::const_iterator elementItr = dataCopy->m_Elements.begin();
      Reflect::V_Element::const_iterator elementEnd = dataCopy->m_Elements.end();
      for ( ; elementItr != elementEnd; ++elementItr )
      {
        Component::ComponentBase* componentPkg = Reflect::ObjectCast< Component::ComponentBase >( *elementItr );
        if ( componentPkg )
        {
          srcComponents.insert( Component::M_Component::value_type( componentPkg->GetSlot(), componentPkg ) );
        }
      }

      // Copy all the component data onto this object.
      if ( !srcComponents.empty() )
      {
        batch->Push( CopyComponentsFrom( srcComponents ) );
      }

      return true;
    }
    else
    {
      return __super::HandleClipboardData( data, op, batch );
    }
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Handles copying settings from another asset class, into this one.
// 
Undo::CommandPtr AssetClass::CopyFrom( Luna::PersistentData* src )
{
  Undo::BatchCommandPtr batch = new Undo::BatchCommand();
  if ( src->HasType( Reflect::GetType< Luna::AssetClass >() ) )
  {
    // Note that we are cloning the source data here.  This way we can manipulate
    // its members without changing the actual data source.
    Asset::AssetClassPtr srcAsset = Reflect::AssertCast< Asset::AssetClass >( src->GetPackage< Asset::AssetClass >()->Clone() );
    Luna::AssetClassPtr newSource = PersistentDataFactory::GetInstance()->CreateTyped< Luna::AssetClass >( srcAsset, m_AssetManager );
    Asset::AssetClass* dstAsset = GetPackage< Asset::AssetClass >();

    // Keep around a copy of the source components so that they can be cleared
    // from the source.  This prevents them from clobbering the components
    // that already exist on this asset (which are wrapped in Components and 
    // need to be maintained).
    Component::M_Component srcAttribs = srcAsset->GetComponents();
    srcAsset->Clear();

    // Replace any other data on our cloned source that should not be overwritten.
    {
      srcAsset->SetDescription( dstAsset->GetDescription() );
    }

    // Perform the copy of the asset class
    batch->Push( __super::CopyFrom( newSource ) ); 

    // Perform the copy of each component
    batch->Push( CopyComponentsFrom( srcAttribs ) );
  }
  else
  {
    // Trying to copy a non-asset onto an asset.  Let the base class deal with that.
    batch->Push( __super::CopyFrom( src ) );
  }

  // Return the batch, or NULL if the batch is empty
  if ( !batch->IsEmpty() )
  {
    return batch;
  }

  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to deal with copying components into this asset from the
// given source.
// 
Undo::CommandPtr AssetClass::CopyComponentsFrom( const Component::M_Component& srcAttribs )
{
  Undo::BatchCommandPtr batch = new Undo::BatchCommand();

  // Copy data from each source component into the component that occupies the same
  // slot on this class.
  Component::M_Component leftOvers = srcAttribs;
  Component::M_Component::const_iterator srcAttrItr = srcAttribs.begin();
  Component::M_Component::const_iterator srcAttrEnd = srcAttribs.end();
  for ( ; srcAttrItr != srcAttrEnd; ++srcAttrItr )
  {
    i32 slot = srcAttrItr->first;
    Component::ComponentBase* srcPkg = srcAttrItr->second;
    Luna::ComponentWrapper* destAttr = FindComponent( slot );
    if ( destAttr )
    {
      batch->Push( destAttr->CopyFrom( PersistentDataFactory::GetInstance()->CreateTyped< Luna::ComponentWrapper >( srcPkg, m_AssetManager ) ) );
      leftOvers.erase( slot );
    }
  }

  // If there are any components left in the source list, try adding them to this asset.
  if ( !leftOvers.empty() )
  {
    Asset::AssetClass* pkg = GetPackage< Asset::AssetClass >();
    Component::M_Component::const_iterator leftOverItr = leftOvers.begin();
    Component::M_Component::const_iterator leftOverEnd = leftOvers.end();
    for ( ; leftOverItr != leftOverEnd; ++leftOverItr )
    {
      // If the component can be added to this asset, perform the command.
      tstring unused;
      if ( pkg->ValidateComponent( leftOverItr->second, unused ) )
      {
        Luna::ComponentWrapperPtr srcAttrib = PersistentDataFactory::GetInstance()->CreateTyped< Luna::ComponentWrapper >( leftOverItr->second, m_AssetManager );
        batch->Push( new ComponentExistenceCommand( Undo::ExistenceActions::Add, this, srcAttrib ) );
      }
    }
  }

  if ( !batch->IsEmpty() )
  {
    return batch;
  }

  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified component and returns true if successful.  On success,
// interested listeners will be notified.
// 
bool AssetClass::AddComponent( const Luna::ComponentWrapperPtr& component )
{
  if ( AddComponent( component, true ) )
  {
    m_ComponentAdded.Raise( ComponentExistenceArgs( this, component.Ptr() ) );
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Removes the specified component and returns true if successful.  On success,
// interested listeners will be notified.
// 
bool AssetClass::RemoveComponent( const Luna::ComponentWrapperPtr& component )
{
  Luna::ComponentWrapperPtr preventDeletion = component;
  if ( RemoveComponent( component, true ) )
  {
    m_ComponentRemoved.Raise( ComponentExistenceArgs( this, component.Ptr() ) );
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to add a new component to this asset class.  The updatePackage
// flag indicates whether the change needs to be pushed down into the persistent
// data or not.
// 
bool AssetClass::AddComponent( const Luna::ComponentWrapperPtr& component, bool updatePackage )
{
  if ( updatePackage )
  {
    Asset::AssetClass* package = GetPackage< Asset::AssetClass >();
    try
    {
      package->SetComponent( component->GetPackage< Component::ComponentBase >() );
    }
    catch ( const Nocturnal::Exception& e )
    {
      Log::Error( TXT( "%s\n" ), e.What() );
      return false;
    }
  }

  Nocturnal::Insert<M_ComponentSmartPtr>::Result inserted = m_Components.insert( M_ComponentSmartPtr::value_type( component->GetSlot(), component ) );
  if ( !inserted.second )
  {
    throw Nocturnal::Exception( TXT( "Attempted to add the same component (%s) twice to asset (%s)" ), component->GetName().c_str(), GetName().c_str() );
  }

  component->SetAssetClass( this );
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to remove the specified component from this asset.  You can 
// specify whether or not to update the persistent data.
// 
bool AssetClass::RemoveComponent( const Luna::ComponentWrapperPtr& component, bool updatePackage )
{
  M_ComponentSmartPtr::iterator found = m_Components.find( component->GetSlot() );
  if ( found == m_Components.end() )
  {
    Log::Error( TXT( "Unable to remove component %s from asset %s because it has already been removed.\n" ), component->GetName().c_str(), GetName().c_str() );
    return false;
  }

  if ( updatePackage )
  {
    Asset::AssetClass* package = GetPackage< Asset::AssetClass >();
    try
    {
      package->RemoveComponent( component->GetSlot() );
    }
    catch ( const Nocturnal::Exception& )
    {
      Log::Error( TXT( "Unable to update persistent data for removal of component %s from asset %s.\n" ), component->GetName().c_str(), GetName().c_str() );
      return false;
    }
  }

  found->second->SetAssetClass( NULL );
  m_Components.erase( found );
  return true;
}
