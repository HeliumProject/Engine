#include "Precompile.h"
#include "UpdateClassAttribute.h" 
#include "AssetClass.h"
#include "AssetEditor.h"
#include "AssetManager.h"
#include "AttributeNode.h"
#include "RuntimeDataNode.h"
#include "PersistentDataFactory.h"
#include "Editor/ClassChooserDlg.h"
#include "Editor/SessionManager.h"
#include "Core/Enumerator.h"



using namespace Luna; 

LUNA_DEFINE_TYPE( Luna::UpdateClassAttribute ); 

static std::string g_LastSelectedClass; 


void UpdateClassAttribute::InitializeType()
{
  Reflect::RegisterClass<Luna::UpdateClassAttribute>( "Luna::UpdateClassAttribute" ); 
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Asset::UpdateClassAttribute>(), 
                                                   &UpdateClassAttribute::Create ); 
}

void UpdateClassAttribute::CleanupType()
{
  Reflect::UnregisterClass<Luna::UpdateClassAttribute>();
}

Luna::PersistentDataPtr UpdateClassAttribute::Create(Reflect::Element* attribute, Luna::AssetManager* manager)
{
  return new Luna::UpdateClassAttribute( Reflect::AssertCast< Asset::UpdateClassAttribute >(attribute), 
                                    manager ); 
}

UpdateClassAttribute::UpdateClassAttribute( Asset::UpdateClassAttribute* attribute, Luna::AssetManager* manager)
  : Luna::AttributeWrapper( attribute, manager )
{
  // rectify runtime data on create
  // 
  attribute->RectifyRuntimeData(); 

  // on thing we could do is register a SymbolBuilder::ResetListener here
  // we don't do that because we just keep the stale data if we can't resolve a runtime class
  // in attribute->RectifyRuntimeData
  //
  // an additional complication is that there are ordering concerns, where our callback
  // would be either before or after the various reset callbacks that the individual
  // RuntimeData classes register (they are contained in the attribute) which would mean
  // that when we requested to rebuild our child tree nodes, not all of our runtime
  // data nodes would be up-to-date
  // 
  // right now, things are fine since we don't remove the stale runtime datas
  // you could even argue that keeping the old data is good. 
  // 
}

UpdateClassAttribute::~UpdateClassAttribute()
{
}

void UpdateClassAttribute::PopulateContextMenu( ContextMenuItemSet& menu )
{
  ContextMenuItemPtr menuItem = new ContextMenuItem( "Add Update Class" ); 
  menuItem->AddCallback( ContextMenuSignature::Delegate(&UpdateClassAttribute::OnAddClass) ); 

  menu.AppendSeparator(); 
  menu.AppendItem(menuItem); 

}


void UpdateClassAttribute::OnAddClass(const ContextMenuArgsPtr& args)
{
  AssetEditor*  assetEditor  = (AssetEditor*) SessionManager::GetInstance()->LaunchEditor(EditorTypes::Asset); 
  Luna::AssetManager* assetManager = assetEditor->GetAssetManager(); 

  std::vector<Luna::UpdateClassAttribute*> selectedAttributes; 

  // Build the list of LUpdateClassAttributes to modify
  //
  OS_SelectableDumbPtr::Iterator selItr = assetManager->GetSelection().GetItems().Begin();
  OS_SelectableDumbPtr::Iterator selEnd = assetManager->GetSelection().GetItems().End();
  for ( ; selItr != selEnd; ++selItr )                                                                                                     
  { 
    Luna::AttributeNode* node = Reflect::ObjectCast< Luna::AttributeNode >(*selItr); 
    Luna::UpdateClassAttribute* attr = Reflect::ObjectCast< Luna::UpdateClassAttribute >(node->GetAttribute()); 

    if(attr)
    {
      selectedAttributes.push_back(attr); 
    }
  }    

  if(selectedAttributes.size() == 0)
  {
    return; 
  }

  S_string validBaseClasses;
  validBaseClasses.insert( "MobyBaseUpdate" ); 

  ClassChooserDlg classChooserDlg( assetEditor, validBaseClasses );
  classChooserDlg.SetSelectedClass( g_LastSelectedClass ); 

  if ( classChooserDlg.ShowModal() != wxID_OK )
  {
    return; 
  }

  // The user clicked OK to change the runtime class
  // if we have a new value, apply it... 
  std::string runtimeClass = classChooserDlg.GetSelectedClass();
  if(runtimeClass.empty())
  {
    return; 
  }

  std::vector< std::string > errors; 
  
  for(size_t i = 0; i < selectedAttributes.size(); ++i)
  {
    Luna::AssetClassPtr assetClass = selectedAttributes[i]->GetAssetClass(); 
    Asset::UpdateClassAttribute* attribute = selectedAttributes[i]->GetPackage<Asset::UpdateClassAttribute>(); 

    if ( !assetManager->IsEditable( assetClass ) )
    {
      errors.push_back( assetClass->GetName() + " is not editable" ); 
      continue; 
    }

    if(attribute->HasClass(runtimeClass))
    {
      errors.push_back( assetClass->GetName() + " already has update class " + runtimeClass ); 
      continue; 
    }

    attribute->AddValidClass(runtimeClass); 

    // raise an event to let our parent know we need to refresh
    //
    selectedAttributes[i]->m_ChildrenRefresh.Raise( AttributeChangeArgs( selectedAttributes[i] )); 
  }

  g_LastSelectedClass = runtimeClass; 
  assetManager->GetSelection().Refresh(); 

  if(errors.size() > 0)
  {
    std::string error = "The following errors were encountered:\n\n"; 
    for(size_t e = 0; e < errors.size(); e++)
    {
      error += errors[e]; 
      error += "\n"; 
    }
    error += "\n"; 
    wxMessageBox(error, "Errors!", wxICON_ERROR, assetEditor);    
  }
}

void UpdateClassAttribute::OnRemoveClass(const ContextMenuArgsPtr& args)
{
  // we're actually going to delete all of the selected children now
  // this should be interesting. 
  // 

  if ( !m_AssetManager->IsEditable( GetAssetClass() ) )
  {
    return; 
  }

  std::vector<Luna::RuntimeDataNode*> nodesToRemove; 

  // Build the list of class nodes that we're going to remove..
  //
  OS_SelectableDumbPtr selection = GetAssetManager()->GetSelection().GetItems(); 

  OS_SelectableDumbPtr::Iterator selItr = selection.Begin();
  OS_SelectableDumbPtr::Iterator selEnd = selection.End();
  for ( ; selItr != selEnd; ++selItr )                                                                                                     
  {                 
    Luna::RuntimeDataNode* node = Reflect::ObjectCast< Luna::RuntimeDataNode >(*selItr); 
    if(node)
    {
      nodesToRemove.push_back(node); 
    }
  }

  if(nodesToRemove.size() == 0)
  {
    return; 
  }

  Asset::UpdateClassAttributePtr attribute = GetPackage<Asset::UpdateClassAttribute>(); 
  for(size_t i = 0; i < nodesToRemove.size(); ++i)
  {
    attribute->RemoveValidClass( nodesToRemove[i]->m_RuntimeData->GetRuntimeClass() );
    selection.Remove( nodesToRemove[i] ); 
  }
  
  // apply our changes to the selection
  GetAssetManager()->GetSelection().SetItems( selection );

  // notify our parent that we have changed
  m_ChildrenRefresh.Raise( AttributeChangeArgs(this) ); 

}

void UpdateClassAttribute::CreateChildren( Luna::AssetNode* parentNode )
{
  m_TreeNode = parentNode; 

  Asset::UpdateClassAttributePtr attribute = GetPackage< Asset::UpdateClassAttribute >(); 
  const Content::V_RuntimeData& updateClasses = attribute->GetUpdateClasses(); 

  for(size_t i = 0; i < updateClasses.size(); ++i)
  {
    const Content::RuntimeDataPtr& runtimeData = updateClasses[i];

    AssetNodePtr node = new Luna::RuntimeDataNode( runtimeData, GetAssetManager() );
    node->SetName( runtimeData->GetRuntimeClass() ); 
    node->CreateChildren();
    parentNode->AddChild( node );

    // i would really like this to be BEFORE all the default menu items :( 
    //
    ContextMenuItemSet& contextMenu = node->GetContextMenu();
    ContextMenuItemPtr menuItem = new ContextMenuItem( "Remove" );
    menuItem->AddCallback( ContextMenuSignature::Delegate( this, &UpdateClassAttribute::OnRemoveClass ) );

    contextMenu.AppendSeparator();
    contextMenu.AppendItem( menuItem );
  }
}

void UpdateClassAttribute::ConnectProperties( EnumerateElementArgs& args )
{
  args.EnumerateElement( GetPackage< Asset::UpdateClassAttribute >() ); 
}
