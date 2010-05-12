#include "Precompile.h"
#include "SearchBar.h"

#include "Asset/AssetClass.h"
#include "Attribute/AttributeCategories.h"
#include "Common/Boost/Regex.h" 
#include "Console/Console.h"
#include "FileSystem/FileSystem.h"
#include "EntityAssetSet.h"
#include "Region.h"
#include "SceneEditor.h"
#include "Reflect/Object.h"
#include "Reflect/Registry.h"

using namespace Luna;

///////////////////////////////////////////////////////////////////////////////
// 
// 
SearchBar::SearchBar( SceneEditor* sceneEditor, wxWindowID id, const wxPoint& pos, const wxSize& size, long style )
: SearchPanel( sceneEditor, id, pos, size, style )
, m_SceneEditor( sceneEditor )
{
  // Initialize Search Options (must match up with SearchOptions::SearchOption)
  // m_SearchOption->Clear();
  // m_SearchOption->Append( "Name" );
  // m_SearchOption->Append( "ID" );
  // m_SearchOption->Append( "Entity Class" );
  // m_SearchOption->Append( "Runtime Class" );
  // m_SearchOption->SetSelection( 0 );
  
  // Initialize Engine Types
  m_EngineType->Clear();
  m_EngineType->Append( wxString( "(Any)" ) );
  const Reflect::Enumeration* engineTypes = Reflect::Registry::GetInstance()->GetEnumeration( Reflect::GetType< Asset::EngineTypes::EngineType >() );
  for ( u32 engineType = 0; engineType < Asset::EngineTypes::Count; ++engineType )
  {
    std::string label;
    bool check = engineTypes->GetElementLabel( engineType, label );
    NOC_ASSERT( check );
    m_EngineType->Append( wxString( label ) );
  }
  m_EngineType->SetSelection( 0 );
	
  // Initialize Bound Options (must match up with BoundOptions::BoundOption)
  // m_BoundsOption->Clear();
  // m_BoundsOption->Append( "AABB" );
  // m_BoundsOption->Append( "OBB" );
  // m_BoundsOption->SetSelection( 0 );
  
  // Initialize Results
  m_Results->ClearAll();
  m_Results->InsertColumn( ResultColumns::Name, "Name" );
  m_Results->InsertColumn( ResultColumns::EntityAsset, "Entity Class" );
  m_Results->InsertColumn( ResultColumns::RuntimeClass, "Runtime Class" );
  m_Results->InsertColumn( ResultColumns::Zone, "Zone" );
  m_Results->InsertColumn( ResultColumns::Region, "Region" );
  m_Results->InsertColumn( ResultColumns::EngineType, "Engine Type" );
  
  // Set Status
  m_Status->SetLabel( "" );
  
  // Set Attributes
  Attribute::V_Attribute attributes;
  Attribute::AttributeCategories* attributeCategories = Attribute::AttributeCategories::GetInstance();
  if ( attributeCategories )
  {
    Attribute::M_AttributeCategories::const_iterator categoryItr = attributeCategories->GetCategories().begin();
    Attribute::M_AttributeCategories::const_iterator categoryEnd = attributeCategories->GetCategories().end();
    for ( ; categoryItr != categoryEnd; ++categoryItr )
    {
      Attribute::M_Attribute::const_iterator attributeItr = categoryItr->second->Attributes().begin();
      Attribute::M_Attribute::const_iterator attributeEnd = categoryItr->second->Attributes().end();
      for ( ; attributeItr != attributeEnd; ++id, ++attributeItr )
      {
        attributes.push_back( attributeItr->second );
      }
    }
  }

  // Initialize Attributes
  m_Attributes->Clear();
  std::sort( attributes.begin(), attributes.end(), CompareAttributes );
  Attribute::V_Attribute::iterator attributeItr = attributes.begin();
  Attribute::V_Attribute::iterator attributeEnd = attributes.end();
  for ( int id = 0; attributeItr != attributeEnd; ++id, ++attributeItr )
  {
    m_IndexToAttribute.insert( std::make_pair( id, *attributeItr ) );
    m_Attributes->Insert( (*attributeItr)->GetClass()->m_UIName, id );
  }

  // Listeners
  Connect( m_SearchButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SearchBar::OnSearch ) );
  Connect( m_SelectButton->GetId(), wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( SearchBar::OnSelect ) );

  m_SceneEditor->GetSceneManager()->AddSceneRemovingListener( SceneChangeSignature::Delegate( this, &SearchBar::OnSceneRemoving ) );
}

SearchBar::~SearchBar()
{
  m_SceneEditor->GetSceneManager()->RemoveSceneRemovingListener( SceneChangeSignature::Delegate( this, &SearchBar::OnSceneRemoving ) );
}

void SearchBar::OnSearch( wxCommandEvent& args )
{
  m_Results->DeleteAllItems();

  SearchBarTraverser traverser( this );
  S_RegionDumbPtr regionSet;
  M_SceneToZone sceneToZone;

  SetupScenes( sceneToZone, regionSet );
  SetupSearchCriteria( traverser );
  SearchScenes( sceneToZone, traverser );
  DisplayResults( sceneToZone, regionSet, traverser );
}

void SearchBar::OnSelect( wxCommandEvent& args )
{
  typedef std::map< ScenePtr, OS_SelectableDumbPtr > M_SceneSelection;
  M_SceneSelection sceneSelection;
  
  int numResults = m_Results->GetItemCount();
  for ( int i = 0; i < numResults; ++i )
  {
    wxListItem currentItem;
    currentItem.SetMask( wxLIST_MASK_STATE );
    currentItem.SetStateMask( wxLIST_STATE_SELECTED );
    currentItem.SetId( i );
    currentItem.SetColumn( ResultColumns::Name );
    
    m_Results->GetItem( currentItem );

    int state = currentItem.GetState();
    if ( state == wxLIST_STATE_SELECTED )
    {
      HierarchyNodePtr node = m_ResultNodes[ i ];
      Luna::Scene* scene = node->GetScene();
      
      M_SceneSelection::iterator sceneSelectionItr = sceneSelection.find( scene );
      if ( sceneSelectionItr == sceneSelection.end() )
      {
        OS_SelectableDumbPtr selection;
        if ( wxIsShiftDown() )
        {
          selection = scene->GetSelection().GetItems();
        }

        sceneSelection.insert( std::make_pair( scene, selection ) );
        sceneSelectionItr = sceneSelection.find( scene );
      }
      
      NOC_ASSERT( sceneSelectionItr != sceneSelection.end() );
      
      sceneSelectionItr->second.Append( node );
    }
  }

  M_SceneSelection::iterator sceneSelectionItr = sceneSelection.begin();
  M_SceneSelection::iterator sceneSelectionEnd = sceneSelection.end();
  for ( ; sceneSelectionItr != sceneSelectionEnd; ++sceneSelectionItr )
  {
    Luna::Scene* scene = sceneSelectionItr->first;
    scene->Push( scene->GetSelection().SetItems( sceneSelectionItr->second ) );
  }
}

void SearchBar::OnSceneRemoving( const SceneChangeArgs& args )
{
  bool refresh = false;
  V_HierarchyNodeSmartPtr::iterator resultsItr = m_ResultNodes.begin();
  V_HierarchyNodeSmartPtr::iterator resultsEnd = m_ResultNodes.end();
  while ( resultsItr != resultsEnd )
  {
    HierarchyNode* node = *resultsItr;
    if ( node->GetScene() == args.m_Scene )
    {
      resultsItr = m_ResultNodes.erase( resultsItr );
      resultsEnd = m_ResultNodes.end();
      refresh = true;
    }
    else
    {
      ++resultsItr;
    }
  }

  if ( refresh )
  {
    S_RegionDumbPtr regionSet;
    M_SceneToZone sceneToZone;

    SetupScenes( sceneToZone, regionSet );
    RefreshResults( sceneToZone, regionSet );
  }
}

void SearchBar::SetupSearchCriteria( SearchBarTraverser& traverser )
{
  std::string searchText = m_SearchText->GetLineText( 0 );
  if ( searchText.size() )
  {
    switch ( m_SearchOption->GetSelection() )
    {
      case SearchOptions::Name:
      {
        traverser.AddSearchCriteria( new EntityNameCriteria( WildcardToRegex( searchText ) ) );
        break;
      }
      
      case SearchOptions::ID:
      {
        tuid id = TUID::Null;
        if ( TUID::Parse( searchText, id ) )
        {
          traverser.AddSearchCriteria( new EntityIDCriteria( id ) );
        }
        else
        {
          m_Status->SetLabel( "Invalid id '" + searchText + "' specified!" );
          return;
        }
        break;
      }
      
      case SearchOptions::EntityAsset:
      {
        tuid id = TUID::Null;
        if ( TUID::Parse( searchText, id ) )
        {
          traverser.AddSearchCriteria( new EntityAssetIDCriteria( id ) );
        }
        else
        {
          traverser.AddSearchCriteria( new EntityAssetNameCriteria( WildcardToRegex( searchText ) ) );
        }
        break;
      }
      
      case SearchOptions::RuntimeClass:
      {
        traverser.AddSearchCriteria( new RuntimeClassNameCriteria( WildcardToRegex( searchText ) ) );
        break;
      }
      
      default:
      {
        NOC_ASSERT( false );
        break;
      }
    }
  }
  
  int engineType = m_EngineType->GetSelection() - 1;
  if ( ( engineType >= 0 ) && ( engineType < Asset::EngineTypes::Count ) )
  {
    traverser.AddSearchCriteria( new EngineTypeCriteria( engineType ) );
  }
  
  std::string lowerBoundString = m_BoundsGreaterThan->GetLineText( 0 );
  std::string upperBoundString = m_BoundsLessThan->GetLineText( 0 );
  if ( ( lowerBoundString.size() > 0 ) || ( upperBoundString.size() > 0 ) )
  {
    float lowerBound = lowerBoundString.size() > 0 ? MAX( 0.0f, atof( lowerBoundString.c_str() ) ) : 0.0f;
    float upperBound = upperBoundString.size() > 0 ? MAX( 0.0f, atof( upperBoundString.c_str() ) ) : 0.0f;
    switch ( m_BoundsOption->GetSelection() )
    {
      case BoundOptions::AABB:
      {
        traverser.AddSearchCriteria( new AABBCriteria( lowerBound, upperBound ) );
        break;
      }
        
      case BoundOptions::OBB:
      {
        traverser.AddSearchCriteria( new OBBCriteria( lowerBound, upperBound ) );
        break;
      }
      
      default:
      {
        NOC_ASSERT( false );
        break;
      }
    }
  }
  
  Attribute::V_Attribute attributes;
  int numAttributes = (int) m_IndexToAttribute.size();
  for ( int i = 0; i < numAttributes; ++i )
  {
    if ( m_Attributes->IsChecked( i ) )
    {
      attributes.push_back( m_IndexToAttribute[ i ] );
    }
  }

  if ( attributes.size() )
  {
    traverser.AddSearchCriteria( new AttributeCriteria( attributes ) );
  }
}

void SearchBar::SetupScenes( M_SceneToZone& sceneToZone, S_RegionDumbPtr& regionSet )
{
  M_TuidToZone tuidToZone;
  Luna::Scene* rootScene = m_SceneEditor->GetSceneManager()->GetRootScene();
  if ( rootScene )
  {
    regionSet = rootScene->GetRegions();
    
    V_ZoneDumbPtr zones;
    rootScene->GetAll< Zone >( zones );
  
    V_ZoneDumbPtr::iterator zoneItr = zones.begin();
    V_ZoneDumbPtr::iterator zoneEnd = zones.end();
    for ( ; zoneItr != zoneEnd; ++zoneItr )
    {
      tuidToZone.insert( std::make_pair( (*zoneItr)->GetFileID(), *zoneItr ) );
    }
  }
  
  if ( m_SearchAllZones->GetValue() )
  {
    const M_SceneSmartPtr& scenes = m_SceneEditor->GetSceneManager()->GetScenes();
    M_SceneSmartPtr::const_iterator sceneItr = scenes.begin();
    M_SceneSmartPtr::const_iterator sceneEnd = scenes.end();
    for ( ; sceneItr != sceneEnd; ++sceneItr )
    {
      M_TuidToZone::iterator zoneItr = tuidToZone.find( sceneItr->second->GetFileID() );
      if ( zoneItr != tuidToZone.end() )
      {
        sceneToZone.insert( std::make_pair( sceneItr->second, zoneItr->second ) );
      }
    }
  }
  else
  {
    Luna::Scene* currentScene = m_SceneEditor->GetSceneManager()->GetCurrentScene();
    M_TuidToZone::iterator zoneItr = tuidToZone.find( currentScene->GetFileID() );
    if ( zoneItr != tuidToZone.end() )
    {
      sceneToZone.insert( std::make_pair( currentScene, zoneItr->second ) );
    }
  }
}

void SearchBar::SearchScenes( const M_SceneToZone& sceneToZone, SearchBarTraverser& traverser )
{
  M_SceneToZone::const_iterator sceneItr = sceneToZone.begin();
  M_SceneToZone::const_iterator sceneEnd = sceneToZone.end();
  for ( ; sceneItr != sceneEnd; ++sceneItr )
  {
    Luna::Transform* root = (Luna::Transform*) sceneItr->first->GetRoot();
    root->TraverseHierarchy( &traverser );
  }
}

void SearchBar::DisplayResults( const M_SceneToZone& sceneToZone, const S_RegionDumbPtr& regionSet, SearchBarTraverser& traverser )
{
  m_ResultNodes = traverser.GetSearchResults();
  RefreshResults( sceneToZone, regionSet );
}

void SearchBar::RefreshResults( const M_SceneToZone& sceneToZone, const S_RegionDumbPtr& regionSet )
{
  m_Results->DeleteAllItems();

  int numResults = (int) m_ResultNodes.size();
  if ( numResults > 1 )
  {
    char resultString[ 256 ] = { 0 };
    sprintf( resultString, "Found %d matches.", numResults );
    m_Status->SetLabel( resultString );
  }
  else if ( numResults == 1 )
  {
    m_Status->SetLabel( "Found 1 match." );
  }
  else
  {
    m_Status->SetLabel( "No matches found." );
  }

  const Reflect::Enumeration* engineTypes = Reflect::Registry::GetInstance()->GetEnumeration( Reflect::GetType< Asset::EngineTypes::EngineType >() );
  V_HierarchyNodeSmartPtr::const_iterator resultsItr = m_ResultNodes.begin();
  V_HierarchyNodeSmartPtr::const_iterator resultsEnd = m_ResultNodes.end();
  for ( int id = 0; resultsItr != resultsEnd; ++id, ++resultsItr )
  {
    std::string name = (*resultsItr)->GetName();
    std::string entityClassName = "";
    std::string entityClassPath = "";
    std::string runtimeClassName = "";
    std::string runtimeClassPath = "";
    std::string zone = "";
    std::string region = "";
    std::string engineType;
        
    Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *resultsItr );
    if ( entity )
    {
      Luna::EntityAssetSet* entityClassSet = entity->GetClassSet();
      if ( entityClassSet )
      {
        entityClassPath = entityClassSet->GetName();
        FileSystem::GetLeaf( entityClassPath, entityClassName );
        FileSystem::StripExtension( entityClassName );

        Asset::EntityAsset* entityClass = entityClassSet->GetEntityAsset();
        if ( entityClass )
        {
          std::string engineTypeString;
          Asset::EngineType engineTypeEnum = entityClass->GetEngineType();
          bool check = engineTypes->GetElementLabel( engineTypeEnum, engineTypeString );
          if ( check )
          {
            engineType = engineTypeString;
          }
        }
      }
      
      runtimeClassPath = entity->GetRuntimeClassName();
      runtimeClassName = runtimeClassPath;
    }

    Luna::Scene* scene = (*resultsItr)->GetScene();
    if ( scene )
    {
      M_SceneToZone::const_iterator sceneItr = sceneToZone.find( scene );
      if ( sceneItr != sceneToZone.end() )
      {
        zone = sceneItr->second->GetName();

        V_string regionNames;
        UniqueID::TUID zoneId = sceneItr->second->GetID(); 
        S_RegionDumbPtr::const_iterator regionItr = regionSet.begin();
        S_RegionDumbPtr::const_iterator regionEnd = regionSet.end();
        for ( ; regionItr != regionEnd; ++regionItr )
        {
          if ( (*regionItr)->GetPackage<Content::Region>()->HasZone( zoneId ) )
          {
            regionNames.push_back( (*regionItr)->GetName() );
          }
        }
        
        std::sort( regionNames.begin(), regionNames.end() );
        V_string::iterator regionNameItr = regionNames.begin();
        V_string::iterator regionNameEnd = regionNames.end();
        for ( ; regionNameItr != regionNameEnd; ++regionNameItr )
        {
          region = region + ( region.size() ? std::string( ", " ) : std::string( "" ) ) + *regionNameItr;
        }
      }
    }

    wxListItem nameListItem;
    nameListItem.SetMask( wxLIST_MASK_TEXT );
    nameListItem.SetText( name );
    nameListItem.SetId( id );
    nameListItem.SetColumn( ResultColumns::Name );

    wxListItem entityClassNameListItem;
    entityClassNameListItem.SetMask( wxLIST_MASK_TEXT );
    entityClassNameListItem.SetText( entityClassName );
    entityClassNameListItem.SetId( id );
    entityClassNameListItem.SetColumn( ResultColumns::EntityAsset );

    wxListItem runtimeClassNameListItem;
    runtimeClassNameListItem.SetMask( wxLIST_MASK_TEXT );
    runtimeClassNameListItem.SetText( runtimeClassName );
    runtimeClassNameListItem.SetId( id );
    runtimeClassNameListItem.SetColumn( ResultColumns::RuntimeClass );

    wxListItem zoneListItem;
    zoneListItem.SetMask( wxLIST_MASK_TEXT );
    zoneListItem.SetText( zone );
    zoneListItem.SetId( id );
    zoneListItem.SetColumn( ResultColumns::Zone );

    wxListItem regionListItem;
    regionListItem.SetMask( wxLIST_MASK_TEXT );
    regionListItem.SetText( region );
    regionListItem.SetId( id );
    regionListItem.SetColumn( ResultColumns::Region );
    
    wxListItem engineTypeListItem;
    engineTypeListItem.SetMask( wxLIST_MASK_TEXT );
    engineTypeListItem.SetText( engineType );
    engineTypeListItem.SetId( id );
    engineTypeListItem.SetColumn( ResultColumns::EngineType );
    
    m_Results->InsertItem( nameListItem );
    m_Results->SetItem( entityClassNameListItem );
    m_Results->SetItem( runtimeClassNameListItem );
    m_Results->SetItem( zoneListItem );
    m_Results->SetItem( regionListItem );
    m_Results->SetItem( engineTypeListItem );
  }
}

bool SearchBar::CompareAttributes( const Attribute::AttributePtr& rhs, const Attribute::AttributePtr& lhs )
{
  return ( rhs->GetClass()->m_UIName < lhs->GetClass()->m_UIName );
}

std::string SearchBar::WildcardToRegex( const std::string& str, bool partialMatch )
{
  std::string result = str;
  result = boost::regex_replace( result, boost::regex( "\\." ), "\\\\." );
  result = boost::regex_replace( result, boost::regex( "\\*" ), ".*" );
  result = boost::regex_replace( result, boost::regex( "\\?" ), "." );
  
  if ( partialMatch )
  {
    result = ".*" + result + ".*";
  }
  else
  {
    result = "^" + result + "$";
  }
  
  return result;
}

bool EntityNameCriteria::Validate( Luna::HierarchyNode* node )
{
  boost::smatch matchResults; 
  return boost::regex_match( node->GetName(), matchResults, boost::regex( m_Value, boost::regex::icase ) );
}

bool EntityIDCriteria::Validate( Luna::HierarchyNode* node )
{
  tuid nodeTuid = node->GetID();
  return ( nodeTuid == m_Value );
}

bool EntityAssetNameCriteria::Validate( Luna::HierarchyNode* node )
{
  Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( node );
  if ( !entity )
  {
    return false;
  }
  
  Luna::EntityAssetSet* classSet = entity->GetClassSet();
  if ( !classSet )
  {
    return false;
  }

  boost::smatch matchResults; 
  return boost::regex_match( classSet->GetName(), matchResults, boost::regex( m_Value, boost::regex::icase ) );
}

bool EntityAssetIDCriteria::Validate( Luna::HierarchyNode* node )
{
  Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( node );
  if ( !entity )
  {
    return false;
  }
  
  Luna::EntityAssetSet* classSet = entity->GetClassSet();
  if ( !classSet )
  {
    return false;
  }

  tuid nodeTuid = classSet->GetEntityAssetID();
  return ( nodeTuid == m_Value );
}

bool RuntimeClassNameCriteria::Validate( Luna::HierarchyNode* node )
{
  Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( node );
  if ( !entity )
  {
    return false;
  }
  
  boost::smatch matchResults;
  return boost::regex_match( entity->GetRuntimeClassName(), matchResults, boost::regex( m_Value, boost::regex::icase ) );
}

bool EngineTypeCriteria::Validate( Luna::HierarchyNode* node )
{
  Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( node );
  if ( !entity )
  {
    return false;
  }
  
  Luna::EntityAssetSet* classSet = entity->GetClassSet();
  if ( !classSet )
  {
    return false;
  }
  
  Asset::EntityAsset* entityClass = classSet->GetEntityAsset();
  if ( !entityClass )
  {
    return false;
  }

  Asset::EngineType engineType = entityClass->GetEngineType();
  return ( engineType == m_Value );
}

bool AABBCriteria::Validate( Luna::HierarchyNode* node )
{
  const Math::AlignedBox& alignedBox = node->GetGlobalBounds();
  float side = MAX( alignedBox.Width(), MAX( alignedBox.Length(), alignedBox.Height() ) );
  
  if ( ( m_LowerBound > 0.0f ) && ( side < m_LowerBound ) )
  {
    return false;
  }
  
  if ( ( m_UpperBound > 0.0f ) && ( side > m_UpperBound ) )
  {
    return false;
  }
  
  return true;
}

bool OBBCriteria::Validate( Luna::HierarchyNode* node )
{
  const Math::AlignedBox& alignedBox = node->GetGlobalBounds();

  Math::V_Vector3 vertices;
  alignedBox.GetVertices( vertices );
  
  Math::Vector3 min;
  Math::Vector3 max;
  Math::V_Vector3::iterator itr = vertices.begin();
  Math::V_Vector3::iterator end = vertices.end();
  if ( itr != end )
  {
    min = *itr;
    max = *itr;
    ++itr;

    for ( ; itr != end; ++itr )
    {
      min.x = MIN( min.x, (*itr).x );
      min.y = MIN( min.y, (*itr).y );
      min.z = MIN( min.z, (*itr).z );

      max.x = MAX( max.x, (*itr).x );
      max.y = MAX( max.y, (*itr).y );
      min.z = MAX( max.z, (*itr).z );
    }
  }

  float side = MAX( max.x - min.x, MAX( max.y - min.y, max.z - min.z ) );
  
  if ( ( m_LowerBound > 0.0f ) && ( side < m_LowerBound ) )
  {
    return false;
  }
  
  if ( ( m_UpperBound > 0.0f ) && ( side > m_UpperBound ) )
  {
    return false;
  }
  
  return true;
}

bool AttributeCriteria::Validate( Luna::HierarchyNode* node )
{
  Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( node );
  if ( !entity )
  {
    return false;
  }
  
  Luna::EntityAssetSet* classSet = entity->GetClassSet();
  if ( !classSet )
  {
    return false;
  }
  
  Asset::EntityAsset* entityClass = classSet->GetEntityAsset();
  if ( !entityClass )
  {
    return false;
  }
  
  Attribute::V_Attribute::iterator itr = m_Attributes.begin();
  Attribute::V_Attribute::iterator end = m_Attributes.end();
  for ( ; itr != end; ++itr )
  {
    if ( !entityClass->ContainsAttribute( (*itr)->GetType() ) )
    {
      return false;
    }
  }
  
  return true;
}
