#include "Precompile.h"

#include "AssetManager.h"

#include "AssetDocument.h"
#include "AssetEditor.h"
#include "AssetReferenceNode.h"
#include "AttributeNode.h"
#include "HierarchyChangeToken.h"
#include "PersistentDataFactory.h"

#include "AssetBuilder/AssetBuilder.h"
#include "AssetManager/AssetManager.h"
#include "AssetManager/DeleteAssetWizard.h"
#include "AssetManager/DuplicateAssetWizard.h"
#include "AssetManager/RenameAssetWizard.h"
#include "Common/Container/Insert.h" 
#include "Console/Console.h"
#include "File/Manager.h"
#include "Editor/DocumentManager.h"

// Using
using namespace Luna;

// Typedefs
typedef Nocturnal::OrderedSet< Luna::AssetClass* > OS_AssetClassDumbPtr;

LUNA_DEFINE_TYPE( Luna::AssetManagerClientData );

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Luna::AssetManager::AssetManager( AssetEditor* assetEditor )
: DocumentManager( assetEditor )
, m_AssetEditor( assetEditor )
{
  m_RootNode = new Luna::AssetNode( this );
  m_RootNode->SetName( "ROOT" );

  m_UndoQueue.AddUndoingListener( Undo::QueueChangingSignature::Delegate ( this, &AssetManager::UndoingOrRedoing ) );
  m_UndoQueue.AddRedoingListener( Undo::QueueChangingSignature::Delegate ( this, &AssetManager::UndoingOrRedoing ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
Luna::AssetManager::~AssetManager()
{
  // Not necessary to remove undo queue listeners: we own the undo queue, so we will
  // always exist at least as long as it does.
}

///////////////////////////////////////////////////////////////////////////////
// Returns the Asset Editor pointer.
// 
AssetEditor* Luna::AssetManager::GetAssetEditor() const
{
  return m_AssetEditor;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the selection object owned by this class.
// 
Selection& Luna::AssetManager::GetSelection()
{
  return m_Selection;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the undo queue owned by this object so that other classes can
// generate undo-able commands.
// 
Undo::Queue& Luna::AssetManager::GetUndoQueue()
{
  return m_UndoQueue;
}

///////////////////////////////////////////////////////////////////////////////
// All nodes in the Asset Editor are children of this node.
// 
Luna::AssetNode* Luna::AssetManager::GetRootNode() const
{
  return m_RootNode.Ptr();
}

///////////////////////////////////////////////////////////////////////////////
// Returns a new hierarchy change token.  The outliner will remain frozen until
// you release your reference to this smart pointer.
// 
LHierarchyChangeTokenPtr Luna::AssetManager::GetHierarchyChangeToken()
{
  return new HierarchyChangeToken( this );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the list of assets that are currently open.
// 
const M_AssetClassSmartPtr& Luna::AssetManager::GetAssets() const
{
  return m_AssetClasses;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the asset class with the specifed tuid (if it has been loaded).
// 
Luna::AssetClass* Luna::AssetManager::FindAsset( const tuid& fileID )
{
  M_AssetClassSmartPtr::const_iterator found = m_AssetClasses.find( fileID );
  if ( found != m_AssetClasses.end() )
  {
    return found->second;
  }

  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Returns the document that belongs to the specified asset class.  Throws 
// an exception if an editor file was not found.
// 
AssetDocument* Luna::AssetManager::FindAssetDocument( const Luna::AssetClass* asset ) const
{
  AssetDocument* doc = Reflect::ObjectCast< AssetDocument >( FindDocument( asset->GetFilePath() ) );
  if ( doc == NULL )
  {
    throw Nocturnal::Exception( "Asset class %s does not have a corresponding document.", asset->GetName().c_str() );
  }
  return doc;
}

///////////////////////////////////////////////////////////////////////////////
// Opens the specified file, which is expected to be an Asset Class.  
// 
DocumentPtr Luna::AssetManager::OpenPath( const std::string& path, std::string& error )
{
  Luna::AssetClass* asset = Open( path, error, true );
  if ( asset )
  {
    return FindAssetDocument( asset );
  }
  return NULL;
}

///////////////////////////////////////////////////////////////////////////////
// Opens the specified file and returns the Luna::AssetClass that wraps it.  Calling
// this function on an item that is already open is considered an error.
// 
Luna::AssetClass* Luna::AssetManager::Open( const std::string& path, std::string& error, bool addToRoot )
{
  wxBusyCursor busyCursor;

  if ( path.empty() )
  {
    error = "Cannot open an empty file path!";
    return NULL;
  }

  // The Asset Editor can only open files with a TUID

  tuid fileID = TUID::Null;
  
  try
  {
    fileID = File::GlobalManager().Open( path );
  }
  catch ( const Nocturnal::Exception& e )
  {
    error = "Failed to open '" + path + "': " + e.Get();
    return NULL;
  }

  if ( fileID == TUID::Null )
  {
    error = "The file path (";
    error += path.c_str();
    error += ") does not exist in the resolver and the system failed to open it.";
    return NULL;
  }

  // Check for NULL.
  if ( fileID == TUID::Null )
  {
    error = "Cannot open a NULL file ID.";
    return NULL;
  }

  // If the asset is already open, return it
  Luna::AssetClass* found = FindAsset( fileID );
  if ( found )
  {
    // Select the first asset reference
    Push( m_Selection.SetItem( *found->GetAssetReferenceNodes().begin() ) );
    return found;
  }

  // Try to load the file from disk.
  Asset::AssetClassPtr package;
  try
  {
    package = Asset::AssetClass::FindAssetClass( fileID, false );
  }
  catch ( const Nocturnal::Exception& e )
  {
    error = "Unable to load asset: " + e.Get();
    return NULL;
  }

  NOC_ASSERT( package.ReferencesObject() );

  // Bookkeeping
  Luna::AssetClass* assetClass = CreateAssetClass( package );
  AssetDocument* file = CreateDocument( assetClass );

  // Warn the user if the file they just opened is out of date.
  if ( !IsUpToDate( file ) )
  {
    std::string msg;
    msg = "The version of " + file->GetFileName() + " is not up to date on your computer.  You will not be able to check it out.";
    wxMessageBox( msg.c_str(), "Warning", wxCENTER | wxOK | wxICON_WARNING, GetAssetEditor() );
  }

  if ( addToRoot )
  {
    Luna::AssetReferenceNodePtr ref = new Luna::AssetReferenceNode( this, assetClass->GetFileID(), NULL );
    ref->Load();
    m_RootNode->AddChild( ref );

    // Select the newly created item
    Push( m_Selection.SetItem( ref.Ptr() ) );
  }

  // Notify listeners
  m_AssetLoaded.Raise( AssetLoadArgs( assetClass ) );

  return assetClass;
}

///////////////////////////////////////////////////////////////////////////////
// Saves the specified document, if it is a valid Asset Editor document.
// 
bool Luna::AssetManager::Save( DocumentPtr document, std::string& error )
{
  AssetDocument* assetDoc = Reflect::ObjectCast< AssetDocument >( document );
  if ( !assetDoc )
  {
    std::string docName = document->GetFileName();
    docName[0] = toupper( docName[0] );
    error = docName + " is not a valid Asset Editor document.";
    return false;
  }

  return Save( assetDoc->GetAssetClass(), true, error );
}

///////////////////////////////////////////////////////////////////////////////
// Saves the asset and calls the base class's save function to handle updating
// the document's settings.
// 
bool Luna::AssetManager::Save( Luna::AssetClass* asset, bool prompt, std::string& error )
{
  AssetDocument* doc = FindAssetDocument( asset );
  if ( !doc )
  {
    NOC_BREAK();
    error = "Internal Error: No document for asset '" + asset->GetName() + "'.";
    return false;
  }

  bool save = true;
  bool abort = false;
  if ( prompt )
  {
    switch ( QuerySave( doc ) )
    {
    case SaveActions::Save:
    case SaveActions::SaveAll:
      save = true;
      break;

    case SaveActions::Skip:
    case SaveActions::SkipAll:
      save = false;
      abort = false;
      break;

    case SaveActions::Abort:
    default:
      save = false;
      abort = true;
      break;
    }
  }

  if ( save )
  {
    if ( asset->Save( error ) )
    {
      return __super::Save( doc, error );
    }
    else
    {
      error = "Failed to save '" + asset->GetName() + "'.\n" + error;
      return false;
    }
  }

  return !abort;
}

///////////////////////////////////////////////////////////////////////////////
// Saves all the currently selected asset classes.
// 
bool Luna::AssetManager::SaveSelected( std::string& error )
{
  bool savedAll = true;
  bool prompt = true;
  S_AssetClassDumbPtr assets;
  GetSelectedAssets( assets );
  S_AssetClassDumbPtr::const_iterator assetItr = assets.begin();
  S_AssetClassDumbPtr::const_iterator assetEnd = assets.end();
  for ( ; assetItr != assetEnd; ++assetItr )
  {
    Luna::AssetClass* asset = *assetItr;
    AssetDocument* doc = FindAssetDocument( asset );
    if ( !doc )
    {
      NOC_BREAK();
      continue;
    }

    bool abort = false;
    bool save = true;
    if ( prompt )
    {
      switch ( QuerySave( doc ) )
      {
      case SaveActions::SaveAll:
        save = true;
        prompt = false;
        break;

      case SaveActions::Save:
        save = true;
        prompt = true;
        break;

      case SaveActions::Skip:
        save = false;
        prompt = true;
        break;

      case SaveActions::SkipAll:
        save = false;
      case SaveActions::Abort:
      default:
        abort = true;
        break; 
      }
    }

    if ( abort )
    {
      break;
    }

    if ( save )
    {
      std::string currentError;
      savedAll &= Save( asset, false, currentError );

      if ( !currentError.empty() )
      {
        if ( !error.empty() )
        {
          error += "\n";
        }
        error += currentError;
      }
    }
  }

  return savedAll;
}

///////////////////////////////////////////////////////////////////////////////
// Recursive helper function to get all the asset classes that are descendants
// of the specified node.
// 
static inline void GetNestedAssets( Luna::AssetNode* node, OS_AssetClassDumbPtr& assets )
{
  OS_AssetNodeSmartPtr::Iterator childItr = node->GetChildren().Begin();
  OS_AssetNodeSmartPtr::Iterator childEnd = node->GetChildren().End();
  for ( ; childItr != childEnd; ++childItr )
  {
    GetNestedAssets( *childItr, assets );
    Luna::AssetReferenceNode* assetRef = Reflect::ObjectCast< Luna::AssetReferenceNode >( *childItr );
    if ( assetRef )
    {
      assets.Append( assetRef->GetAssetClass() );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Static helper function to travel up the hierarchy and find the first occurrence
// of an Luna::AssetReferenceNode.
// 
static inline Luna::AssetReferenceNode* FindParentAssetRef( Luna::AssetNode* node )
{
  Luna::AssetNode* current = node;
  Luna::AssetReferenceNode* assetRefNode = NULL;
  while ( current && !assetRefNode )
  {
    assetRefNode = Reflect::ObjectCast< Luna::AssetReferenceNode >( current );
    current = current->GetParent();
  }
  return assetRefNode;
}

///////////////////////////////////////////////////////////////////////////////
// Closes all the currently selected assets.
// 
bool Luna::AssetManager::CloseSelected()
{
  OS_AssetClassDumbPtr assetsToClose;

  // Build the list of assets to close.
  OS_SelectableDumbPtr::Iterator selItr = m_Selection.GetItems().Begin();
  OS_SelectableDumbPtr::Iterator selEnd = m_Selection.GetItems().End();
  for ( ; selItr != selEnd; ++selItr )
  {
    Luna::AssetNode* node = Reflect::AssertCast< Luna::AssetNode >( *selItr );
    Luna::AssetReferenceNode* assetRef = FindParentAssetRef( node );
    assetsToClose.Append( node->GetAssetClass() );
    GetNestedAssets( assetRef, assetsToClose );
  }

  if ( assetsToClose.Size() > 0 )
  {
    // Build the list of editor files to close
    OS_DocumentSmartPtr files;
    m_Selection.Clear();
    OS_AssetClassDumbPtr::Iterator assetItr = assetsToClose.Begin();
    OS_AssetClassDumbPtr::Iterator assetEnd = assetsToClose.End();
    for ( ; assetItr != assetEnd; ++assetItr )
    {
      files.Append( FindAssetDocument( *assetItr ) );
    }

    // Close all the files.
    return CloseDocuments( files );
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Closes all currently open assets.
// 
bool Luna::AssetManager::CloseAll()
{
  // Build an ordered list of the assets, with all nested assets appearing 
  // before root level ones.
  OS_AssetClassDumbPtr assetsToClose;
  OS_AssetNodeSmartPtr::Iterator childItr = m_RootNode->GetChildren().Begin();
  OS_AssetNodeSmartPtr::Iterator childEnd = m_RootNode->GetChildren().End();
  for ( ; childItr != childEnd; ++childItr )
  {
    Luna::AssetReferenceNode* assetRef = Reflect::ObjectCast< Luna::AssetReferenceNode >( *childItr );
    if ( assetRef )
    {
      assetsToClose.Append( assetRef->GetAssetClass() );
      GetNestedAssets( assetRef, assetsToClose );
    }
  }

  OS_DocumentSmartPtr files;
  if ( assetsToClose.Size() > 0 )
  {
    // Build the list of editor files to close
    OS_AssetClassDumbPtr::Iterator assetItr = assetsToClose.Begin();
    OS_AssetClassDumbPtr::Iterator assetEnd = assetsToClose.End();
    for ( ; assetItr != assetEnd; ++assetItr )
    {
      files.Append( FindAssetDocument( *assetItr ) );
    }

  }

  // Close all the files.
  m_Selection.Clear();
  return CloseDocuments( files );
}

///////////////////////////////////////////////////////////////////////////////
// Checks out the selected asset classes from revision control.
// 
void Luna::AssetManager::CheckOutSelected()
{
  S_AssetClassDumbPtr selectedAssets;
  GetSelectedAssets( selectedAssets );

  S_AssetClassDumbPtr::const_iterator selItr = selectedAssets.begin();
  S_AssetClassDumbPtr::const_iterator selEnd = selectedAssets.end();
  for ( ; selItr != selEnd; ++selItr )
  {
    // Try to check out the asset class.
    Luna::AssetClass* assetClass = *selItr;
    AssetDocument* document = FindAssetDocument( assetClass );
    NOC_ASSERT( document );
    if ( document )
    {
      CheckOut( document );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Displays the revision history for all selected assets.
// 
void Luna::AssetManager::RevisionHistorySelected()
{
  S_tuid fileIDs;
  if ( GetSelectedAssetIDs( fileIDs ) > 0 )
  {
    S_tuid::const_iterator idItr = fileIDs.begin();
    S_tuid::const_iterator idEnd = fileIDs.end();
    for ( ; idItr != idEnd; ++idItr )
    {
      std::string path = File::GlobalManager().GetPath( *idItr );
      if ( !path.empty() )
      {
        m_AssetEditor->RevisionHistory( path );
      }
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over the currently selected items and determines what asset class
// each one is part of.  The set of asset classes that is passed in will be 
// populated and this function returns the number of items in that set.
// 
size_t Luna::AssetManager::GetSelectedAssets( S_AssetClassDumbPtr& list ) const
{
  OS_SelectableDumbPtr::Iterator selItr = m_Selection.GetItems().Begin();
  OS_SelectableDumbPtr::Iterator selEnd = m_Selection.GetItems().End();
  for ( ; selItr != selEnd; ++selItr )
  {
    Luna::AssetNode* node = Reflect::ObjectCast< Luna::AssetNode >( *selItr );
    if ( node )
    {
      list.insert( node->GetAssetClass() );
    }
  }

  return list.size();
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over the currently selected assets and finds the unique set of 
// tuids for all the asset classes that the selcted nodes are part of.
// 
size_t Luna::AssetManager::GetSelectedAssetIDs( S_tuid& fileIDs ) const
{
  OS_SelectableDumbPtr::Iterator selItr = m_Selection.GetItems().Begin();
  OS_SelectableDumbPtr::Iterator selEnd = m_Selection.GetItems().End();
  for ( ; selItr != selEnd; ++selItr )
  {
    Luna::AssetNode* node = Reflect::ObjectCast< Luna::AssetNode >( *selItr );
    if ( node )
    {
      Luna::AssetClass* asset = node->GetAssetClass();
      tuid file = asset->GetFileID();
      if ( file != TUID::Null )
      {
        fileIDs.insert( file );
      }
    }
  }

  return fileIDs.size();
}

///////////////////////////////////////////////////////////////////////////////
// Fills out the list with all the attributes that are currently selected.
// Returns the number of items in the list.
// 
size_t Luna::AssetManager::GetSelectedAttributes( S_AttributeSmartPtr& list ) const
{
  OS_SelectableDumbPtr::Iterator selItr = m_Selection.GetItems().Begin();
  OS_SelectableDumbPtr::Iterator selEnd = m_Selection.GetItems().End();
  for ( ; selItr != selEnd; ++selItr )
  {
    Luna::AttributeNode* node = Reflect::ObjectCast< Luna::AttributeNode >( *selItr );
    if ( node )
    {
      list.insert( node->GetAttribute() );
    }
  }
  return list.size();
}

///////////////////////////////////////////////////////////////////////////////
// Makes sure every selected asset is editable.  Returns true if all of them
// are.
// 
bool Luna::AssetManager::IsEditable() const
{
  bool isEditable = true;

  S_AssetClassDumbPtr classes;
  GetSelectedAssets( classes );

  S_AssetClassDumbPtr::const_iterator itr = classes.begin();
  S_AssetClassDumbPtr::const_iterator end = classes.end();
  for ( ; itr != end; ++itr )
  {
    Luna::AssetClass* asset = *itr;
    isEditable &= IsEditable( asset );
  }

  return isEditable;
}

///////////////////////////////////////////////////////////////////////////////
// Returns true if the specified asset is allowed to be edited (either, it is 
// checked out, or the user has specified explicitly that they want to edit it).
// 
bool Luna::AssetManager::IsEditable( const Luna::AssetClass* assetClass ) const
{
  AssetDocument* doc = FindAssetDocument( assetClass );
  NOC_ASSERT( doc );

  if ( doc )
  {
    if ( !AttemptChanges( doc ) )
    {
      return false;
    }
    doc->SetModified( true );
  }

  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Pushes a command onto the undo queue.
// 
bool Luna::AssetManager::Push( const Undo::CommandPtr& command )
{
  if ( !command.ReferencesObject() )
  {
    // allow the null change
    return true;
  }

  if ( command->IsSignificant() && !IsEditable() )
  {
    // we are significant and not editable, abort operation
    command->Undo();

    // we aborted, restore state if necessary
    return false;
  }

  // Update our map of commands to the assets that they modify
  S_AssetClassDumbPtr selectedAssets;
  GetSelectedAssets( selectedAssets );
  Luna::AssetCommandPtr assetCmd = new Luna::AssetCommand();
  assetCmd->SetCommand( command );
  assetCmd->SetAssets( selectedAssets );
  if ( !m_AssetCommands.insert( M_AssetCommandSmartPtr::value_type( command.Ptr(), assetCmd ) ).second )
  {
    // If you hit this, the same command is being pushed multiple times, which should not be happening
    NOC_BREAK();
  }

  // Put the command in the queue
  m_UndoQueue.Push( command );

  // change committed
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Returns flags indicating whether the current selection can be copied or 
// moved/cut.
// 
void Luna::AssetManager::CanCopySelection( bool& canCopy, bool& canMove ) const
{
  if ( m_Selection.GetItems().Empty() )
  {
    canCopy = canMove = false;
    return;
  }

  canCopy = canMove = true;
  OS_SelectableDumbPtr::Iterator selItr = m_Selection.GetItems().Begin();
  OS_SelectableDumbPtr::Iterator selEnd = m_Selection.GetItems().End();
  for ( ; selItr != selEnd; ++selItr )
  {
    Luna::AssetNode* node = Reflect::TryCast< Luna::AssetNode >( *selItr );
    
    canCopy &= node->CanBeCopied();
    canMove &= canCopy && node->CanBeMoved();

    if ( !canCopy && !canMove )
    {
      return;
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over the selected items and builds the clipboard data that can
// be used for drag/drop or copy/paste.  This function also returns the parents
// of all the selected items (so that delete can be called on the parents if
// the desired operation is a cut or move).
// 
Inspect::ReflectClipboardDataPtr Luna::AssetManager::CopySelection( S_AssetNodeDumbPtr& parents, bool& canBeMoved ) const
{
  Inspect::ReflectClipboardDataPtr clipboardData;
  parents.clear();
  canBeMoved = true;
  OS_SelectableDumbPtr::Iterator selItr = m_Selection.GetItems().Begin();
  OS_SelectableDumbPtr::Iterator selEnd = m_Selection.GetItems().End();
  for ( ; selItr != selEnd; ++selItr )
  {
    Luna::AssetNode* node = Reflect::AssertCast< Luna::AssetNode >( *selItr );
    if ( node->CanBeCopied() )
    {
      canBeMoved &= node->CanBeMoved();
      parents.insert( node->GetParent() );
      Inspect::ReflectClipboardDataPtr currentData = node->GetClipboardData();

      if ( !clipboardData.ReferencesObject() )
      {
        clipboardData = currentData;
      }
      else
      {
        if ( !clipboardData->Merge( currentData ) )
        {
          clipboardData = NULL;
        }
      }

      if ( !clipboardData.ReferencesObject() )
      {
        break;
      }
    }
    else
    {
      clipboardData = NULL;
      break;
    }
  }

  return clipboardData;
}

///////////////////////////////////////////////////////////////////////////////
// Duplicate the specified asset and open the duplicate.
// 
void Luna::AssetManager::DuplicateAsset( Luna::AssetClassPtr assetClass )
{
  Asset::AssetClassPtr assetPkg = assetClass->GetPackage< Asset::AssetClass >();
  if ( !assetPkg.ReferencesObject() )
  {
    return;
  }

  ::AssetManager::DuplicateAssetWizard wizard( m_AssetEditor, assetPkg );
  if ( wizard.Run() )
  {
    Asset::AssetClassPtr newAssetPkg = wizard.GetNewAssetClass();
    if ( !newAssetPkg.ReferencesObject() )
    {
      return;
    }

    // Open the new asset
    std::string error;
    if ( !OpenPath( wizard.GetNewFileLocation(), error ) )
    {
      wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, m_AssetEditor );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Rename the specified asset.
// 
void Luna::AssetManager::RenameAsset( Luna::AssetClassPtr assetClass )
{
  Asset::AssetClassPtr assetPkg = assetClass->GetPackage< Asset::AssetClass >();
  if ( !assetPkg.ReferencesObject() )
  {
    return;
  }

  std::string oldAssetPath = assetPkg->GetFilePath();

  ::AssetManager::RenameAssetWizard wizard( m_AssetEditor, assetPkg );
  if ( wizard.Run() )
  {
    // open the new asset in the AssetEditor
    AssetDocument* doc = FindAssetDocument( assetClass );
#pragma TODO( "Close nested assets first???" )
    CloseDocument( doc );
    std::string unused;
    OpenPath( wizard.GetNewFileLocation(), unused );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Delete the specified asset.
// 
void Luna::AssetManager::DeleteAsset( Luna::AssetClassPtr assetClass )
{
  Asset::AssetClassPtr assetPkg = assetClass->GetPackage< Asset::AssetClass >();
  if ( !assetPkg.ReferencesObject() )
  {
    return;
  }

  std::string oldAssetPath = assetPkg->GetFilePath();

  ::AssetManager::DeleteAssetWizard wizard( m_AssetEditor, assetPkg );
  if ( wizard.Run() )
  {
    CloseDocument( FindAssetDocument( assetClass ), false );
  }
}

///////////////////////////////////////////////////////////////////////////////
// TEMPORARY HACK
// 
void Luna::AssetManager::ClearUndoQueue()
{
#pragma TODO( "Fix code that is calling this function to not invalidate the undo queue" )
  m_UndoQueue.Reset();
  m_AssetCommands.clear();
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to create a new asset class and add it to the list managed
// by this object.
// 
Luna::AssetClass* Luna::AssetManager::CreateAssetClass( const Asset::AssetClassPtr& package )
{
  Luna::AssetClassPtr assetClass = PersistentDataFactory::GetInstance()->CreateTyped< Luna::AssetClass >( package.Ptr(), this );
  NOC_ASSERT( assetClass );
  Nocturnal::Insert<M_AssetClassSmartPtr>::Result inserted = m_AssetClasses.insert( M_AssetClassSmartPtr::value_type( assetClass->GetFileID(), assetClass ) );
  if ( !inserted.second )
  {
    throw Nocturnal::Exception( "Asset class '%s' is already in the Asset Manager!", assetClass->GetName().c_str() );
  }
  return assetClass;
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to create a new editor file for the specified asset class
// and add the editor file to the list managed by this object.
// 
AssetDocument* Luna::AssetManager::CreateDocument( Luna::AssetClass* assetClass )
{
  AssetDocumentPtr doc = new AssetDocument( assetClass );
  doc->AddDocumentClosedListener( DocumentChangedSignature::Delegate( this, &AssetManager::OnDocumentClosed ) );
  if ( !AddDocument( doc ) )
  {
    // Shouldn't happen... means there's a bug in the code.
    throw Nocturnal::Exception( "Asset class '%s' already has a document!", assetClass->GetName().c_str() );
  }
  return doc;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a document has been closed.  Cleans up the associated asset.
// 
void Luna::AssetManager::OnDocumentClosed( const DocumentChangedArgs& args )
{
  const AssetDocument* doc = Reflect::ConstObjectCast< AssetDocument >( args.m_Document );
  NOC_ASSERT( doc );

  if ( doc )
  {
    CloseAsset( doc->GetAssetClass() );
    doc->RemoveDocumentClosedListener( DocumentChangedSignature::Delegate( this, &AssetManager::OnDocumentClosed ) );
    m_AssetClasses.erase( doc->GetAssetClass()->GetFileID() );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Closes the specified file and removes the object from the asset manager.
// 
void Luna::AssetManager::CloseAsset( Luna::AssetClassPtr assetClass )
{
  if ( assetClass.ReferencesObject() )
  {
    // Get rid of any root level nodes that are references to the asset we are unloading.
    // This feels a bit hacky... should try to find a better way to do this.
    S_AssetNodeSmartPtr nodesToDelete;
    OS_SelectableDumbPtr newSelection = GetSelection().GetItems();
    OS_AssetNodeSmartPtr::Iterator rootItr = m_RootNode->GetChildren().Begin();
    OS_AssetNodeSmartPtr::Iterator rootEnd = m_RootNode->GetChildren().End();
    for ( ; rootItr != rootEnd; ++rootItr )
    {
      const Luna::AssetNodePtr& child = *rootItr;
      Luna::AssetReferenceNode* assetRef = Reflect::ObjectCast< Luna::AssetReferenceNode >( child );
      if ( assetRef )
      {
        if ( assetRef->GetAssetClass() == assetClass )
        {
          nodesToDelete.insert( assetRef );

          newSelection.Remove( assetRef );
        }
      }
    }

    if ( nodesToDelete.size() > 0 )
    {
      GetSelection().SetItems( newSelection );
    }

    for each ( const Luna::AssetNodePtr& node in nodesToDelete )
    {
      m_RootNode->RemoveChild( node );
    }

    // Warn listeners that this asset is about to be destroyed.
    m_AssetUnloading.Raise( AssetLoadArgs( assetClass ) );

    m_AssetClasses.erase( assetClass->GetFileID() );

    // For now, we have to invalidate the undo queue since there might be commands
    // in there that reference the now deleted asset.
#pragma TODO( "Remove invalid commands from the queue instead of clearing all of them." )
    ClearUndoQueue();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a command is about to be undone or redone.  If the command
// is significant, this function will verify that all the affected asset classes
// are editable before allowing it to continue.
// 
bool Luna::AssetManager::UndoingOrRedoing( const Undo::QueueChangeArgs& args )
{
  bool allow = true;
  if ( args.m_Command->IsSignificant() )
  {
    M_AssetCommandSmartPtr::const_iterator found = m_AssetCommands.find( args.m_Command );
    if ( found != m_AssetCommands.end() )
    {
      const S_AssetClassDumbPtr& assets = found->second->GetAssets();
      S_AssetClassDumbPtr::const_iterator assetItr = assets.begin();
      S_AssetClassDumbPtr::const_iterator assetEnd = assets.end();
      for ( ; assetItr != assetEnd && allow; ++assetItr )
      {
        Luna::AssetClass* asset = *assetItr;
        allow &= IsEditable( asset ); // Breaks out of loop if not editable
      }
    }
  }
  return allow;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an asset is built
// 
void Luna::AssetManager::AssetBuilt( const AssetBuilder::AssetBuiltArgsPtr& args )
{
  Luna::AssetClass* assetClassPtr  = FindAsset( args->m_AssetId );
  if(assetClassPtr)
  {
    assetClassPtr->BuildFinished();
  }
}
