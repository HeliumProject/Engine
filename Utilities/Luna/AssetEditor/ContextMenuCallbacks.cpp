#include "Precompile.h"
#include "ContextMenuCallbacks.h"

#include "AssetEditor.h"
#include "AssetManager.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// Pass through function for the context menu callback of saving the selected
// assets.
// 
void Luna::OnSaveSelectedAssets( const ContextMenuArgsPtr& args )
{
  Luna::AssetManagerClientData* clientData = Reflect::ObjectCast< Luna::AssetManagerClientData >( args->GetClientData() );
  if ( clientData )
  {
    std::string error;
    if ( !clientData->m_AssetManager->SaveSelected( error ) )
    {
      wxMessageBox( error.c_str(), "Error", wxCENTER | wxICON_ERROR | wxOK, clientData->m_AssetManager->GetAssetEditor() );
    }
  }
}

///////////////////////////////////////////////////////////////////////////////
// Pass through function for the context menu callback of checking out the
// selected files.
// 
void Luna::OnCheckOutSelectedAssets( const ContextMenuArgsPtr& args )
{
  Luna::AssetManagerClientData* clientData = Reflect::ObjectCast< Luna::AssetManagerClientData >( args->GetClientData() );
  if ( clientData )
  {
    clientData->m_AssetManager->CheckOutSelected();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Helper function for context menu callback to show the revision history.
// 
void Luna::OnRevisionHistorySelectedAssets( const ContextMenuArgsPtr& args )
{
  Luna::AssetManagerClientData* clientData = Reflect::ObjectCast< Luna::AssetManagerClientData >( args->GetClientData() );
  if ( clientData )
  {
    clientData->m_AssetManager->RevisionHistorySelected();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Helper function for context menu callback to close the current selection.
// 
void Luna::OnCloseSelectedAssets( const ContextMenuArgsPtr& args )
{
  Luna::AssetManagerClientData* clientData = Reflect::ObjectCast< Luna::AssetManagerClientData >( args->GetClientData() );
  if ( clientData )
  {
    clientData->m_AssetManager->CloseSelected();
  }
}
