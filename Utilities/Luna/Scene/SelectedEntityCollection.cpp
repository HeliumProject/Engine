#include "Precompile.h"
#include "SelectedEntityCollection.h"

#include "EntityAssetSet.h"

using namespace Luna;

SelectedEntityCollection::SelectedEntityCollection( Selection* selection, const tstring& name )
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

void SelectedEntityCollection::SetAssetsFromSelection( const OS_SelectableDumbPtr& selection )
{
    std::set< Nocturnal::Path > files;
  for ( OS_SelectableDumbPtr::Iterator selItr = selection.Begin(), selEnd = selection.End(); selItr != selEnd; ++selItr )
  {
    Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *selItr );
    if ( entity )
    {
      entity->AddClassChangedListener( EntityAssetChangeSignature::Delegate( this, &SelectedEntityCollection::OnEntityClassChanged ) );
      m_WatchedEntities.push_back( entity );
      files.insert( entity->GetClassSet()->GetEntityAssetPath() );
    }
  }

  SetAssetReferences( files );
}

void SelectedEntityCollection::OnEntityClassChanged( const EntityAssetChangeArgs& args )
{
  if ( m_Selection )
  {
    SetAssetsFromSelection( m_Selection->GetItems() );
  }
  else
  {
    OS_SelectableDumbPtr empty;
    SetAssetsFromSelection( empty );
  }
}

void SelectedEntityCollection::OnSelectionChanged( const OS_SelectableDumbPtr& args )
{
  RemoveEntityListeners();

  SetAssetsFromSelection( args );
}
