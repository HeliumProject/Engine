#include "Precompile.h"
#include "RegionsPanel.h"
#include "Region.h"
#include "Zone.h"

#include "Content/Region.h"
#include "Content/Zone.h"
#include "Core/Object.h"
#include "SceneNode.h"
#include "Application/UI/SortTreeCtrl.h"
#include "Application/UI/ImageManager.h"

#include <algorithm>

using namespace Luna; 
using namespace Nocturnal;

RegionsPanel::RegionsPanel( SceneEditor* editor, Luna::SceneManager* manager, wxWindow* parent)
: RegionsPanelBase( parent )
, m_Editor( editor )
, m_SceneManager( manager )
, m_Scene(NULL)
, m_LoadInProgress( false )
, m_IgnoreSelectionChange( false )
{
  // Hookup application event callbacks
  m_SceneManager->AddSceneAddedListener( SceneChangeSignature::Delegate ( this, &RegionsPanel::SceneAdded ) );
  m_SceneManager->AddSceneRemovingListener( SceneChangeSignature::Delegate ( this, &RegionsPanel::SceneRemoved ) );
  m_SceneManager->AddCurrentSceneChangedListener( SceneChangeSignature::Delegate ( this, &RegionsPanel::CurrentSceneChanged ) );

  ContextMenuItemPtr item; 

  // build the regions context menu: 
  item = new ContextMenuItem("Create Region"); 
  item->AddCallback( ContextMenuSignature::Delegate(this, &RegionsPanel::ContextCreateRegion) ); 
  m_RegionContextMenu.AppendItem( item ); 
  
  // build the region context menu
  item = new ContextMenuItem("Add or Remove Zones"); 
  item->AddCallback( ContextMenuSignature::Delegate(this, &RegionsPanel::ContextRegionAddRemoveZones) ); 
  m_PerRegionContextMenu.AppendItem( item ); 

  m_PerRegionContextMenu.AppendSeparator(); 

  item = new ContextMenuItem("Rename Region"); 
  item->AddCallback( ContextMenuSignature::Delegate(this, &RegionsPanel::ContextRename) ); 
  m_PerRegionContextMenu.AppendItem( item ); 

  item = new ContextMenuItem("Delete Region"); 
  item->AddCallback( ContextMenuSignature::Delegate(this, &RegionsPanel::ContextDeleteRegion) ); 
  m_PerRegionContextMenu.AppendItem( item ); 

  // build the zone context menu
  item = new ContextMenuItem("Remove from Region"); 
  item->AddCallback( ContextMenuSignature::Delegate(this, &RegionsPanel::ContextZoneRemoveFromRegion) ); 
  m_PerOwnedZoneContextMenu.AppendItem( item ); 

  // build the zone main context menu
  item = new ContextMenuItem("Change Region Membership"); 
  item->AddCallback( ContextMenuSignature::Delegate(this, &RegionsPanel::ContextZoneChangeRegions) ); 
  m_PerZoneContextMenu.AppendItem( item ); 

  m_RegionTree->SetImageList( Nocturnal::GlobalImageManager().GetGuiImageList() ); 

  m_RegionIcon = Nocturnal::GlobalImageManager().GetImageIndex( "region_16.png" );
  m_ZoneIcon   = Nocturnal::GlobalImageManager().GetImageIndex( "zone_16.png" ); 

  InitTree();
}

RegionsPanel::~RegionsPanel()
{
  // Unhook application callbacks
  m_SceneManager->RemoveSceneAddedListener( SceneChangeSignature::Delegate ( this, &RegionsPanel::SceneAdded ) );
  m_SceneManager->RemoveSceneRemovingListener( SceneChangeSignature::Delegate ( this, &RegionsPanel::SceneRemoved ) );
  m_SceneManager->RemoveCurrentSceneChangedListener( SceneChangeSignature::Delegate ( this, &RegionsPanel::CurrentSceneChanged ) );
}

void RegionsPanel::InitializeRegion(Luna::Region* region)
{
  region->AddChangedListener( ObjectChangeSignature::Delegate(this, &RegionsPanel::NodeChanged) ); 
  region->AddNameChangedListener( SceneNodeChangeSignature::Delegate(this, &RegionsPanel::NodeNameChanged) ); 
}

void RegionsPanel::CleanupRegion(Luna::Region* region)
{
  region->RemoveChangedListener( ObjectChangeSignature::Delegate(this, &RegionsPanel::NodeChanged) ); 
  region->RemoveNameChangedListener( SceneNodeChangeSignature::Delegate(this, &RegionsPanel::NodeNameChanged) ); 
}

void RegionsPanel::InitializeZone(Zone* zone)
{
  zone->AddNameChangedListener( SceneNodeChangeSignature::Delegate(this, &RegionsPanel::NodeNameChanged) ); 

  m_ZoneMap.insert( std::make_pair( zone->GetID(), zone ) ); 
}

void RegionsPanel::CleanupZone(Zone* zone)
{
  m_ZoneMap.erase( zone->GetID() ); 

  zone->RemoveNameChangedListener( SceneNodeChangeSignature::Delegate(this, &RegionsPanel::NodeNameChanged) ); 
}

void RegionsPanel::SceneAdded( const SceneChangeArgs& args )
{
  if ( m_SceneManager->IsRoot( args.m_Scene ) )
  {
    args.m_Scene->AddNodeAddedListener( NodeChangeSignature::Delegate ( this, &RegionsPanel::NodeAdded ) );
    args.m_Scene->AddNodeRemovedListener( NodeChangeSignature::Delegate ( this, &RegionsPanel::NodeRemoved ) );
    args.m_Scene->AddLoadStartedListener( LoadSignature::Delegate ( this, &RegionsPanel::SceneLoadStarted ) );
    args.m_Scene->AddLoadFinishedListener( LoadSignature::Delegate ( this, &RegionsPanel::SceneLoadFinished ) );
    args.m_Scene->AddSelectionChangedListener( SelectionChangedSignature::Delegate( this, &RegionsPanel::SelectionChanged ) ); 

    m_Scene = args.m_Scene; 
    m_RegionTree->Enable();
  }
}

void RegionsPanel::SceneRemoved( const SceneChangeArgs& args )
{
  if ( m_SceneManager->IsRoot( args.m_Scene )  )
  {
    NOC_ASSERT(m_SceneManager->GetRootScene() == m_Scene); 

    args.m_Scene->RemoveNodeAddedListener( NodeChangeSignature::Delegate ( this, &RegionsPanel::NodeAdded ) );
    args.m_Scene->RemoveNodeRemovedListener( NodeChangeSignature::Delegate ( this, &RegionsPanel::NodeRemoved ) );
    args.m_Scene->RemoveLoadStartedListener( LoadSignature::Delegate ( this, &RegionsPanel::SceneLoadStarted ) );
    args.m_Scene->RemoveLoadFinishedListener( LoadSignature::Delegate ( this, &RegionsPanel::SceneLoadFinished ) );
    args.m_Scene->AddSelectionChangedListener( SelectionChangedSignature::Delegate( this, &RegionsPanel::SelectionChanged ) ); 

#if 0 
    for each (Zone* zoneWrapper in m_Scene->GetZones() )
    { 
      CleanupZone(zoneWrapper); 
    }

    for each (Luna::Region* regionWrapper in m_Scene->GetRegions() )
    {
      CleanupRegion(regionWrapper); 
    }
#endif 

    m_ZoneMap.clear(); 
    m_Scene = NULL; 

    TeardownTree(); 
  }
}

void RegionsPanel::CurrentSceneChanged( const SceneChangeArgs& args )
{

}

void RegionsPanel::NodeAdded( const NodeChangeArgs& args )
{
  if(m_LoadInProgress)
  {
    return; 
  }


  ZonePtr zone = Reflect::ObjectCast< Zone >( args.m_Node ); 
  if(zone)
  {
    m_RegionTree->Freeze(); 

    AddZoneToRoot( zone ); 

    InitializeZone( zone ); 

    m_RegionTree->Sort( m_ZoneRoot, true ); 
    m_RegionTree->Thaw(); 
    return; 
  }

  LRegionPtr region = Reflect::ObjectCast< Luna::Region >( args.m_Node ); 
  if(region)
  {
    m_RegionTree->Freeze(); 

    InitializeRegion( region ); 

    BuildRegionSubtree( region ); 

    m_RegionTree->Sort( m_RegionRoot, true); 
    m_RegionTree->Thaw(); 

    return; 
  }

}

void RegionsPanel::NodeRemoved( const NodeChangeArgs& args )
{
  ZonePtr zone = Reflect::ObjectCast< Zone >(args.m_Node); 
  if(zone)
  {
    M_ZoneToId::iterator found = m_ZoneTreeMap.find(zone); 
    if(found == m_ZoneTreeMap.end())
    {
      NOC_BREAK(); 
      return; 
    }

    m_RegionTree->Delete( found->second ); 
    m_RegionTree->Sort( m_ZoneRoot ); 

    m_ZoneTreeMap.erase(found); 

    CleanupZone(zone); 

    // we let other event handlers deal with the removal of subnodes from regions
    return; 
  }

  LRegionPtr region = Reflect::ObjectCast< Luna::Region >(args.m_Node); 
  if(region)
  {
    RemoveRegionSubtree(region); 
    m_RegionTree->Sort(m_RegionRoot, false); 

    CleanupRegion(region); 
  }

}

bool RegionsPanel::SelectionChanging( const OS_SelectableDumbPtr& selection)
{
  return true; 
}

void RegionsPanel::SelectionChanged( const OS_SelectableDumbPtr& selection )
{
  m_IgnoreSelectionChange = true; 

  // change the selection in our UI
  // if there is exactly one thing selected, try to select that in our tree
  // otherwise, (0 or more than 1) select nothing in our tree
  // 
  
  if(selection.Size() == 1)
  { 
    Selectable* selectable = selection.Front(); 
    Luna::Region* regionWrapper = Reflect::ObjectCast<Luna::Region>(selectable); 
    Zone*   zoneWrapper   = Reflect::ObjectCast<Zone>(selectable); 

    wxTreeItemId treeSelection; 

    if(regionWrapper)
    {
      M_RegionToId::iterator found = m_RegionTreeMap.find(regionWrapper); 
      if(found != m_RegionTreeMap.end())
      {
        treeSelection = found->second; 
      }
    }
    else
    {
      M_ZoneToId::iterator found = m_ZoneTreeMap.find(zoneWrapper); 
      if(found != m_ZoneTreeMap.end())
      {
        treeSelection = found->second; 
      }
    }

    if(treeSelection)
    {
      m_RegionTree->SelectItem(treeSelection); 
    }
    else
    {
      m_RegionTree->Unselect(); 
    }
  }
  else
  {
    m_RegionTree->Unselect(); 
  }

  m_IgnoreSelectionChange = false; 
}

void RegionsPanel::SceneLoadStarted( const LoadArgs& args )
{
  m_LoadInProgress = true; 
}

void RegionsPanel::SceneLoadFinished( const LoadArgs& args )
{
  m_LoadInProgress = false; 

  BuildTree(); 
}

void RegionsPanel::CreateZoneNodes(Luna::Region* regionWrapper, wxTreeItemId regionNode, const S_ZoneDumbPtr& zones)
{
  Content::RegionPtr region = regionWrapper->GetPackage<Content::Region>(); 
  const V_TUID& zoneIds = region->GetZones(); 

  for(u32 z = 0; z < zoneIds.size(); ++z)
  {
    TUID zoneId = zoneIds[z]; 

    M_TuidToZone::iterator found = m_ZoneMap.find( zoneId ); 
    if(found == m_ZoneMap.end())
    {
      m_RegionTree->AppendItem( regionNode, "<unknown zone>", m_ZoneIcon, m_ZoneIcon, NULL); 
    }
    else
    {
      Zone* zoneWrapper = found->second; 
      m_RegionTree->AppendItem( regionNode, zoneWrapper->GetName(), 
                                m_ZoneIcon, m_ZoneIcon, 
                                new TreeData(zoneWrapper)); 
    }
  }
}

wxTreeItemId RegionsPanel::AddZoneToRoot( const ZonePtr& zoneWrapper )
{
  wxTreeItemId node = m_RegionTree->AppendItem( m_ZoneRoot, zoneWrapper->GetName(),
                                                m_ZoneIcon, m_ZoneIcon,
                                                new TreeData(zoneWrapper));

  m_ZoneTreeMap.insert( std::make_pair( zoneWrapper, node) ); 

  return node; 
}

wxTreeItemId RegionsPanel::BuildRegionSubtree(const LRegionPtr& regionWrapper)
{
  wxTreeItemId node = m_RegionTree->AppendItem( m_RegionRoot, regionWrapper->GetName(), 
                                                m_RegionIcon, m_RegionIcon, 
                                                new TreeData(regionWrapper)); 

  m_RegionTreeMap.insert( std::make_pair( regionWrapper, node ) ); 

  CreateZoneNodes( regionWrapper, node, m_Scene->GetZones() );

  return node; 
}

bool RegionsPanel::RemoveRegionSubtree(Luna::Region* regionWrapper, bool* expanded)
{
  M_RegionToId::iterator found = m_RegionTreeMap.find(regionWrapper); 
  if(found == m_RegionTreeMap.end())
  {
    NOC_BREAK(); 
    return false; 
  }

  wxTreeItemId node = found->second; 

  if(expanded)
  {
    *expanded = m_RegionTree->IsExpanded(node); 
  }

  m_RegionTree->Delete(node); 
  m_RegionTreeMap.erase(found); 
  return true; 
}

void RegionsPanel::InitTree()
{
  wxTreeItemId root = m_RegionTree->AddRoot("INVISIBLE_ROOT");
  m_RegionRoot      = m_RegionTree->AppendItem( root, "Regions", m_RegionIcon, m_RegionIcon ); 
  m_ZoneRoot        = m_RegionTree->AppendItem( root, "Zones",   m_ZoneIcon, m_ZoneIcon ); 
  m_RegionTree->Disable();
}

void RegionsPanel::BuildTree()
{
  Luna::Scene* scene = m_Scene; 
  
  const S_ZoneDumbPtr& zones = scene->GetZones(); 
  const S_RegionDumbPtr& regions = scene->GetRegions(); 

  m_RegionTree->Freeze(); 

  for(S_ZoneDumbPtr::const_iterator zit = zones.begin(); zit != zones.end(); ++zit)
  {
    Zone* zone = *zit; 
    AddZoneToRoot( zone ); 

    InitializeZone( zone ); 
  }
  

  for(S_RegionDumbPtr::const_iterator rit = regions.begin(); rit != regions.end(); ++rit)
  {
    Luna::Region* wrapper = *rit; 

    InitializeRegion( wrapper );  
    BuildRegionSubtree( wrapper ); 
  }

  m_RegionTree->Sort( m_RegionTree->GetRootItem() , true ); 
  m_RegionTree->Expand(m_RegionRoot); 
  m_RegionTree->Thaw(); 

}

void RegionsPanel::TeardownTree()
{
  m_RegionTree->DeleteAllItems(); 

  m_RegionRoot = 0; 
  m_ZoneRoot   = 0; 

  InitTree();
}

void RegionsPanel::OnTreeKeyDown( wxTreeEvent& event )
{
  wxTreeItemId node = m_RegionTree->GetSelection(); 

  if(node && event.GetKeyCode() == WXK_F2 && m_RegionTree->GetEditControl() == NULL)
  {
    TreeData* data = (TreeData*) m_RegionTree->GetItemData(node); 

    if(data && data->m_Object->HasType( Reflect::GetType<Luna::Region>() ) )
    {
      m_RegionTree->EditLabel(node);       
    }
  }

}


void RegionsPanel::OnBeginLabelEdit( wxTreeEvent& event )
{ 
  wxTreeItemId node = event.GetItem(); 

  if(node == m_RegionRoot || node == m_ZoneRoot)
  {
    event.Veto(); 
    return; 
  }

  TreeData* data = (TreeData*) m_RegionTree->GetItemData(node); 
  if(data)
  {
    LRegionPtr region = Reflect::ObjectCast< Luna::Region >(data->m_Object); 
    if(region)
    {
      event.Skip(); 
    }
  }

  event.Veto(); 
}

void RegionsPanel::OnEndLabelEdit( wxTreeEvent& event )
{ 
  if(event.IsEditCancelled())
  {
    return; 
  }

  if(!m_Scene->IsEditable())
  {
    event.Veto(); 
    return; 
  }

  wxTreeItemId node = event.GetItem(); 


  TreeData* data = (TreeData*) m_RegionTree->GetItemData(node); 
  if(data)
  {
    LRegionPtr region = Reflect::ObjectCast<Luna::Region>(data->m_Object); 
    if(region)
    {
      std::string name = event.GetLabel(); 
      region->Rename( name ); 

      m_RegionTree->EnsureVisible(node); 

      event.Veto(); 
      return; 
    }
  }
  

  event.Skip(); 
}

void RegionsPanel::OnTreeItemCollapsing( wxTreeEvent& event )
{ 
  wxTreeItemId node = event.GetItem(); 

  if(node == m_RegionRoot)
  {
    event.Veto(); 
    return; 
  }

  event.Skip(); 
}

void RegionsPanel::OnTreeItemMenu( wxTreeEvent& event )
{ 
  // we should be popping up a menu of some sort. 
  // okay. we use the current selection to help us decide what
  // we should be popping up

  // right now we only have single select. yay. 
  wxTreeItemId selected = m_RegionTree->GetSelection(); 

  if( selected )
  {
    ContextMenuItemSet* itemSet = NULL; 

    if(selected == m_RegionRoot)
    {
      itemSet = &m_RegionContextMenu; 
    }
    else
    {
      TreeData* treeData = (TreeData*) m_RegionTree->GetItemData(selected); 
      wxTreeItemId parent = m_RegionTree->GetItemParent(selected); 

      if(parent == m_ZoneRoot)
      {
        itemSet = &m_PerZoneContextMenu; 
      }
      else if(treeData)
      {
        if( treeData->m_Object->HasType( Reflect::GetType< Luna::Region >() ) )
        {
          itemSet = &m_PerRegionContextMenu; 
        }
        else if(treeData->m_Object->HasType( Reflect::GetType< Zone >() ) )
        {
          itemSet = &m_PerOwnedZoneContextMenu; 
        }
      }
    }

    if(itemSet)
    {
      // no merging right now!!! 

      ContextMenu menu; 
      ContextMenuArgsPtr args = new ContextMenuArgs; 

      ContextMenuGenerator::Build(*itemSet, &menu); 
      menu.Popup(this, args); 
    }
  }

  event.Skip(); 
}

void RegionsPanel::OnTreeSelChanged( wxTreeEvent& event )
{ 
  if(m_IgnoreSelectionChange)
  {
    event.Skip(); 
    return; 
  }

  if(m_Scene == NULL)
  {
    event.Skip(); 
    return; 
  }

  wxTreeItemId selection = event.GetItem(); 
  Selectable* lunaSelection = NULL; 

  if(selection)
  {
    TreeData* data = (TreeData*) m_RegionTree->GetItemData(selection); 
    if(data)
    {
      lunaSelection =  Reflect::ObjectCast<Selectable>(data->m_Object); 
    }
  }


  // so the second and third arguments are these fancy things that make it 
  // so we don't get called back by the scene when it is changing. 
  // i think it's kind of silly, frankly. 
  // consider changing m_IgnoreSelectionChange to a counting integer.
  //
  m_Scene->Push( m_Scene->GetSelection().SetItem( lunaSelection, 
                                                  SelectionChangingSignature::Delegate (),
                                                  SelectionChangedSignature::Delegate( this, &RegionsPanel::SelectionChanged ) ) 
                                                 );


  event.Skip(); 
}


void RegionsPanel::ContextCreateRegion(const ContextMenuArgsPtr&)
{
  if(!m_Scene->IsEditable())
  {
    return; 
  }

  // we don't worry about tree nodes here
  // 
  std::string name; 
  RegionCreateDialog dialog(this); 
  bool keepShowing = true; 
  while(keepShowing)
  {
    dialog.m_TextCtrl->SetFocus(); 
    int result = dialog.ShowModal(); 

    if(result != wxID_OK)
    {
      return; 
    }

    name = dialog.m_TextCtrl->GetValue();
    if(name == "")
    {
      wxMessageBox("You must give a name for the region", "Need a name"); 
      continue; 
    }

    // check that the name is unique
    bool nameOk = true; 

    Luna::SceneNode* other = m_Scene->FindNode( name ); 
    if(other)
    {
      nameOk = false; 
      int tryAgain = wxMessageBox("There is already a node named " + name + ".",
                                  "Already exists", wxOK|wxCANCEL); 

      if(tryAgain != wxOK)
      {
        return; 
      }
    }

    if(nameOk)
    {
      keepShowing = false; 
    }
  }

  // free the tree because we know were going to add the node and THEN get a rename 
  // 
  m_RegionTree->Freeze(); 


  LRegionPtr regionWrapper = new Luna::Region( m_Scene, new Content::Region() ); 

  Undo::BatchCommandPtr batch = new Undo::BatchCommand ();
  batch->Push( new SceneNodeExistenceCommand( Undo::ExistenceActions::Add, m_Scene, regionWrapper ) );
  m_Scene->Push( batch );
  m_Scene->Execute( false );

  // non-undo-able rename to set to user specified name.
  regionWrapper->Rename( name ); 

  M_RegionToId::iterator found = m_RegionTreeMap.find(regionWrapper); 
  if(found != m_RegionTreeMap.end())
  {
    m_RegionTree->SelectItem( found->second ); 
  }
  m_RegionTree->Thaw(); 
}

void RegionsPanel::ContextDeleteRegion(const ContextMenuArgsPtr&)
{
  if(!m_Scene->IsEditable())
  {
    return; 
  }

  wxTreeItemId selected = m_RegionTree->GetSelection(); 
  if(!selected)
  {
    return; 
  }

  TreeData* data = (TreeData*) m_RegionTree->GetItemData(selected); 
  if(!data || !data->m_Object->HasType( Reflect::GetType<Luna::Region>() ) )
  {
    return; 
  }

  LRegionPtr region = Reflect::ObjectCast<Luna::Region>(data->m_Object); 
  NOC_ASSERT( region->GetScene() == m_Scene ); 

  // Begin undo batch
  Undo::BatchCommandPtr batch = new Undo::BatchCommand ();
  batch->Push( m_Scene->GetSelection().Clear() );
  batch->Push( new SceneNodeExistenceCommand( Undo::ExistenceActions::Remove, m_Scene, region ) );

  m_Scene->Push( batch );
  m_Scene->Execute( false );

}

void RegionsPanel::ContextRename(const ContextMenuArgsPtr&)
{
  if(!m_Scene->IsEditable())
  {
    return; 
  }

  wxTreeItemId selected = m_RegionTree->GetSelection(); 

  if(!selected)
  {
    return; 
  }

  TreeData* data = (TreeData*) m_RegionTree->GetItemData(selected); 
  if(!data || !data->m_Object->HasType( Reflect::GetType<Luna::Region>() ) )
  {
    return; 
  }

  m_RegionTree->EditLabel(selected);

}

bool SceneNodeNameCompare(Luna::SceneNode* lhs,  Luna::SceneNode* rhs)
{
  return lhs->GetName() < rhs->GetName(); 
}

void RegionsPanel::ContextRegionAddRemoveZones(const ContextMenuArgsPtr&)
{
  if(!m_Scene->IsEditable())
  {
    return; 
  }

  wxTreeItemId selected = m_RegionTree->GetSelection(); 

  if(!selected)
  {
    return; 
  }

  TreeData* data = (TreeData*) m_RegionTree->GetItemData(selected); 

  if(!data || !data->m_Object->HasType( Reflect::GetType<Luna::Region>() ) )
  {
    return; 
  }


  Luna::Region* regionWrapper  = Reflect::AssertCast<Luna::Region>(data->m_Object); 
  Content::Region* region = regionWrapper->GetPackage<Content::Region>(); 

  wxString title = "Select zones for region " + regionWrapper->GetName(); 

  ChooserDialog   dialog(this, wxID_ANY, title); 
  wxCheckListBox* listBox = dialog.m_ListBox; 
  
  const S_ZoneDumbPtr& zoneSet = m_Scene->GetZones(); 

  if(zoneSet.empty())
  {
    wxMessageBox("There are no zones in this level. Add some using the zone panel.", "No Zones", wxOK); 
    return; 
  }


  std::vector<Zone*> zones; 
  zones.insert( zones.end(), zoneSet.begin(), zoneSet.end()); 
  std::sort( zones.begin(), zones.end(), SceneNodeNameCompare ); 

  std::vector<wxString> names; 
  for(u32 i = 0; i < zones.size(); i++)
  {
    names.push_back( zones[i]->GetPackage<Content::Zone>()->GetName() ); 
  }

  listBox->Set( (int) names.size(), &names[0] ); 

  std::vector<bool> currentlyOwned; 
  currentlyOwned.reserve(zones.size()); 

  // THIS WILL HAVE TO BE CHANGED FOR MULTISELECT
  for(u32 i = 0; i < zones.size(); i++)
  {
    TUID zoneId = zones[i]->GetPackage<Content::Zone>()->m_ID; 
    if(region->HasZone(zoneId))
    {
      listBox->Check( i ); 
      currentlyOwned.push_back( true ); 
    }
    else
    {
      currentlyOwned.push_back( false ); 
    }
  }
  
  int result = dialog.ShowModal(); 
  if(result != wxID_OK)
  {
    return; 
  }
  
  // now examine the dialogue box check status
  // doing this should trigger some UI updates
  // so freeze the tree control first and thaw afterwards

  m_RegionTree->Freeze(); 
  for(u32 i = 0; i < zones.size(); i++)
  {
    bool checked = listBox->IsChecked(i); 

    if(checked && !currentlyOwned[i])
    { 
      regionWrapper->AddZone( zones[i] ); 
    }
    else if(!checked && currentlyOwned[i])
    {
      regionWrapper->RemoveZone( zones[i] ); 
    }
  }
   
  M_RegionToId::iterator currentItr = m_RegionTreeMap.find( regionWrapper ); 
  NOC_ASSERT(currentItr != m_RegionTreeMap.end() ); 

  // since we recreate the node for this region, we need to re-select it
  // because otherwise, most likely the next region will end up selected
  m_RegionTree->SelectItem( currentItr->second, true ); 
  m_RegionTree->Thaw(); 
}

 
void RegionsPanel::ContextZoneRemoveFromRegion(const ContextMenuArgsPtr&)
{

  if(!m_Scene->IsEditable())
  {
    return; 
  }

  wxTreeItemId zoneNode = m_RegionTree->GetSelection(); 
  if(!zoneNode)
  {
    return; 
  }

  // selection should be a nested zone. 
  // we don't really keep track of those, but whatever. 
  // find the parent

  wxTreeItemId regionNode = m_RegionTree->GetItemParent(zoneNode);
  NOC_ASSERT( regionNode); 

  // get the zone wrapper and the region wrappers from these guys

  TreeData* regionData = (TreeData*) m_RegionTree->GetItemData(regionNode); 
  TreeData* zoneData   = (TreeData*) m_RegionTree->GetItemData(zoneNode); 

  if(!regionData || !zoneData)
  {
    NOC_BREAK(); 
    return; 
  }
  
  Luna::Region* regionWrapper = Reflect::ObjectCast<Luna::Region>(regionData->m_Object); 
  Zone*   zoneWrapper   = Reflect::ObjectCast<Zone>(zoneData->m_Object); 

  if(!regionWrapper || !zoneWrapper)
  {
    return; 
  }

  m_RegionTree->Freeze(); 
  regionWrapper->RemoveZone( zoneWrapper ); 

  M_RegionToId::iterator currentItr = m_RegionTreeMap.find( regionWrapper ); 
  NOC_ASSERT(currentItr != m_RegionTreeMap.end() ); 

  m_RegionTree->SelectItem( currentItr->second, true ); 

  m_RegionTree->Thaw(); 
}

void RegionsPanel::ContextZoneChangeRegions(const ContextMenuArgsPtr&)
{
  if(!m_Scene->IsEditable())
  {
    return; 
  }

  wxTreeItemId selected = m_RegionTree->GetSelection(); 

  if(!selected)
  {
    return; 
  }

  TreeData* data = (TreeData*) m_RegionTree->GetItemData(selected); 

  if(!data || !data->m_Object->HasType( Reflect::GetType<Zone>() ) )
  {
    return; 
  }


  Zone* zoneWrapper  = Reflect::AssertCast<Zone>(data->m_Object); 
  Content::Zone* zone = zoneWrapper->GetPackage<Content::Zone>(); 

  wxString title = "Select regions for zone " + zoneWrapper->GetName(); 

  ChooserDialog   dialog(this, wxID_ANY, title); 
  wxCheckListBox* listBox = dialog.m_ListBox; 

  const S_RegionDumbPtr& regionSet = m_Scene->GetRegions(); 

  if(regionSet.empty())
  {
    wxMessageBox("There are no regions in this level. You must add some regions.", "No Regions", wxOK); 
    return; 
  }

  std::vector<Luna::Region*> regions; 
  regions.insert( regions.end(), regionSet.begin(), regionSet.end()); 
  std::sort( regions.begin(), regions.end(), SceneNodeNameCompare ); 

  std::vector<wxString> names; 
  for(u32 i = 0; i < regions.size(); i++)
  {
    names.push_back( regions[i]->GetName() ); 
  }

  listBox->Set( (int) names.size(), &names[0] ); 

  std::vector<bool> currentlyOwned; 
  currentlyOwned.reserve(regions.size()); 

  TUID zoneId = zoneWrapper->GetID(); 

  // THIS WILL HAVE TO BE CHANGED FOR MULTISELECT
  for(u32 i = 0; i < regions.size(); i++)
  {
    if(regions[i]->GetPackage<Content::Region>()->HasZone(zoneId))
    {
      listBox->Check( i ); 
      currentlyOwned.push_back( true ); 
    }
    else
    {
      currentlyOwned.push_back( false ); 
    }
  }
  
  int result = dialog.ShowModal(); 
  if(result != wxID_OK)
  {
    return; 
  }
  
  // now examine the dialogue box check status
  // doing this should trigger some UI updates
  // so freeze the tree control first and thaw afterwards

  m_RegionTree->Freeze(); 
  for(u32 i = 0; i < regions.size(); i++)
  {
    bool checked = listBox->IsChecked(i); 

    if(checked && !currentlyOwned[i])
    { 
      regions[i]->AddZone( zoneWrapper ); 
    }
    else if(!checked && currentlyOwned[i])
    {
      regions[i]->RemoveZone( zoneWrapper ); 
    }
  }
  m_RegionTree->Thaw(); 
}

void RegionsPanel::NodeChanged( const ObjectChangeArgs& args )
{
  LRegionPtr regionWrapper = Reflect::ObjectCast< Luna::Region >(args.m_Object); 
  if(!regionWrapper)
  {
    return; 
  }

  Content::RegionPtr region = regionWrapper->GetPackage<Content::Region>(); 

  // we only care if the list of zones has changed..
  if( args.m_ElementArgs.m_Field != region->ZonesField() )
  {
    return; 
  }

  bool expanded = false; 
  bool found = RemoveRegionSubtree( regionWrapper, &expanded); 

  if(!found)
  {
    NOC_BREAK(); 
  }

  wxTreeItemId node = BuildRegionSubtree( regionWrapper ); 
  m_RegionTree->Sort( m_RegionRoot, true ); 
  if(expanded)
  {
    m_RegionTree->Expand(node); 
  }
}

void RegionsPanel::NodeNameChanged( const SceneNodeChangeArgs& args )
{
  LRegionPtr regionWrapper = Reflect::ObjectCast< Luna::Region >(args.m_Node); 
  if(regionWrapper)
  {
    // lookup the node in the tree for this region
    // 

    M_RegionToId::iterator found = m_RegionTreeMap.find(regionWrapper); 
    if(found == m_RegionTreeMap.end())
    {
      NOC_BREAK(); 
      return; 
    }

    wxTreeItemId node = found->second;
    NOC_ASSERT(node); 

    m_RegionTree->SetItemText( node, regionWrapper->GetName() ); 
    m_RegionTree->Sort( m_RegionRoot, false ); 
    return; 
  }

  ZonePtr zoneWrapper = Reflect::ObjectCast< Zone >(args.m_Node); 
  if(zoneWrapper)
  {
    M_ZoneToId::iterator found = m_ZoneTreeMap.find(zoneWrapper); 
    if(found == m_ZoneTreeMap.end())
    {
      NOC_BREAK(); 
      return; 
    }

    wxTreeItemId node = found->second; 
    NOC_ASSERT(node); 

    m_RegionTree->Freeze(); 

    m_RegionTree->SetItemText(node, zoneWrapper->GetName() ); 
    m_RegionTree->Sort(m_ZoneRoot, false); 
  
    // now we have to go through all the regions, see if they have this 
    // zone, and then do something about it if they do (rebuild their tree)
    // 
    
    const S_RegionDumbPtr& regions = m_Scene->GetRegions(); 
    for(S_RegionDumbPtr::const_iterator itr = regions.begin(); itr != regions.end(); ++itr)
    {
      Luna::Region* regionWrapper = *itr; 
      Content::Region* region = regionWrapper->GetPackage<Content::Region>(); 

      M_RegionToId::iterator regionItr = m_RegionTreeMap.find(regionWrapper);
      NOC_ASSERT(regionItr != m_RegionTreeMap.end()); 

      wxTreeItemId regionNode = regionItr->second; 
      wxTreeItemIdValue cookie; 
      wxTreeItemId zoneNode = m_RegionTree->GetFirstChild(regionNode, cookie); 
      while(zoneNode)
      {
        TreeData* data = (TreeData*) m_RegionTree->GetItemData(zoneNode); 
        if(data && data->m_Object == zoneWrapper)
        {   
          m_RegionTree->SetItemText(zoneNode, zoneWrapper->GetName() ); 
        }

        zoneNode = m_RegionTree->GetNextChild(regionNode, cookie); 
      }
      m_RegionTree->Sort( regionNode ); 
    }
    m_RegionTree->Thaw(); 
  }

}

