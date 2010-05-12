#include "Precompile.h"
#include "SelectedEntityCollection.h"

using namespace Luna;

SelectedEntityCollection::SelectedEntityCollection( Selection* selection, const std::string& name )
: AssetCollection( name, AssetCollectionFlags::Dynamic | AssetCollectionFlags::Temporary )
, m_Selection( selection )
{
  m_Selection->AddChangedListener( SelectionChangedSignature::Delegate( this, &SelectedEntityCollection::OnSelectionChanged ) );
}

SelectedEntityCollection::~SelectedEntityCollection()
{
  ClearSelection();
}

void SelectedEntityCollection::ClearSelection()
{
  RemoveEntityListeners();
  if ( m_Selection )
  {
    m_Selection->RemoveChangedListener( SelectionChangedSignature::Delegate( this, &SelectedEntityCollection::OnSelectionChanged ) );  
    m_Selection = NULL;
  }
}

void SelectedEntityCollection::RemoveEntityListeners()
{
  for ( Luna::V_EntityDumbPtr::const_iterator itr = m_WatchedEntities.begin(), end = m_WatchedEntities.end();
    itr != end; ++itr )
  {
    Luna::Entity* entity = *itr;
    entity->RemoveClassChangedListener( EntityAssetChangeSignature::Delegate( this, &SelectedEntityCollection::OnEntityClassChanged ) );
  }
  m_WatchedEntities.clear();
}

void SelectedEntityCollection::SetAssetIDsFromSelection( const OS_SelectableDumbPtr& selection )
{
  S_tuid fileIDs;
  for ( OS_SelectableDumbPtr::Iterator selItr = selection.Begin(), selEnd = selection.End();
    selItr != selEnd; ++selItr )
  {
    Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *selItr );
    if ( entity )
    {
      entity->AddClassChangedListener( EntityAssetChangeSignature::Delegate( this, &SelectedEntityCollection::OnEntityClassChanged ) );
      m_WatchedEntities.push_back( entity );
      tuid id = entity->GetPackage< Asset::Entity >()->GetEntityAssetID();
      if ( id != TUID::Null )
      {
        fileIDs.insert( id );
      }
    }
  }

  SetAssetIDs( fileIDs );
}

void SelectedEntityCollection::OnEntityClassChanged( const EntityAssetChangeArgs& args )
{
  if ( m_Selection )
  {
    SetAssetIDsFromSelection( m_Selection->GetItems() );
  }
  else
  {
    OS_SelectableDumbPtr empty;
    SetAssetIDsFromSelection( empty );
  }
}

void SelectedEntityCollection::OnSelectionChanged( const OS_SelectableDumbPtr& args )
{
  RemoveEntityListeners();

  SetAssetIDsFromSelection( args );
}
