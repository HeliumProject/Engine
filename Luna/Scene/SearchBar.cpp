#include "Precompile.h"
#include "SearchBar.h"

#include "Pipeline/Asset/AssetClass.h"
#include "Pipeline/Component/ComponentCategories.h"
#include "Foundation/Boost/Regex.h" 
#include "Foundation/Log.h"
#include "EntityAssetSet.h"
#include "SceneEditor.h"
#include "Foundation/Reflect/Object.h"
#include "Foundation/Reflect/Registry.h"

using namespace Luna;
using namespace Nocturnal;

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
    m_AssetType->Clear();
    m_AssetType->Append( wxT( "(Any)" ) );
    const Reflect::Enumeration* assetTypes = Reflect::Registry::GetInstance()->GetEnumeration( Reflect::GetType< Asset::AssetTypes::AssetType >() );
    for ( u32 assetType = 0; assetType < Asset::AssetTypes::Count; ++assetType )
    {
        tstring label;
        bool check = assetTypes->GetElementLabel( assetType, label );
        NOC_ASSERT( check );
        m_AssetType->Append( wxString( label ) );
    }
    m_AssetType->SetSelection( 0 );

    // Initialize Bound Options (must match up with BoundOptions::BoundOption)
    // m_BoundsOption->Clear();
    // m_BoundsOption->Append( "AABB" );
    // m_BoundsOption->Append( "OBB" );
    // m_BoundsOption->SetSelection( 0 );

    // Initialize Results
    m_Results->ClearAll();
    m_Results->InsertColumn( ResultColumns::Name, wxT( "Name" ) );
    m_Results->InsertColumn( ResultColumns::EntityAsset, wxT( "Entity Class" ) );
    m_Results->InsertColumn( ResultColumns::Zone, wxT( "Zone" ) );
    m_Results->InsertColumn( ResultColumns::Region, wxT( "Region" ) );
    m_Results->InsertColumn( ResultColumns::AssetType, wxT( "Engine Type" ) );

    // Set Status
    m_Status->SetLabel( wxT( "" ) );

    // Set Components
    Component::V_Component components;
    Component::ComponentCategories* componentCategories = Component::ComponentCategories::GetInstance();
    if ( componentCategories )
    {
        Component::M_ComponentCategories::const_iterator categoryItr = componentCategories->GetCategories().begin();
        Component::M_ComponentCategories::const_iterator categoryEnd = componentCategories->GetCategories().end();
        for ( ; categoryItr != categoryEnd; ++categoryItr )
        {
            Component::M_Component::const_iterator componentItr = categoryItr->second->Components().begin();
            Component::M_Component::const_iterator componentEnd = categoryItr->second->Components().end();
            for ( ; componentItr != componentEnd; ++id, ++componentItr )
            {
                components.push_back( componentItr->second );
            }
        }
    }

    // Initialize Components
    std::sort( components.begin(), components.end(), CompareComponents );
    Component::V_Component::iterator componentItr = components.begin();
    Component::V_Component::iterator componentEnd = components.end();
    for ( int id = 0; componentItr != componentEnd; ++id, ++componentItr )
    {
        m_IndexToComponent.insert( std::make_pair( id, *componentItr ) );
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
    M_SceneToZone sceneToZone;

    SetupScenes( sceneToZone );
    SetupSearchCriteria( traverser );
    SearchScenes( sceneToZone, traverser );
    DisplayResults( sceneToZone, traverser );
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
        M_SceneToZone sceneToZone;

        SetupScenes( sceneToZone );
        RefreshResults( sceneToZone );
    }
}

void SearchBar::SetupSearchCriteria( SearchBarTraverser& traverser )
{
    tstring searchText = m_SearchText->GetLineText( 0 );
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
                TUID parseId;
                if ( parseId.FromString( searchText ) )
                {
                    traverser.AddSearchCriteria( new EntityIDCriteria( (tuid)parseId ) );
                }
                else
                {
                    m_Status->SetLabel( TXT( "Invalid id '" ) + searchText + TXT( "' specified!" ) );
                    return;
                }
                break;
            }

        case SearchOptions::EntityAsset:
            {
                traverser.AddSearchCriteria( new EntityAssetNameCriteria( WildcardToRegex( searchText ) ) );
                break;
            }

        default:
            {
                NOC_ASSERT( false );
                break;
            }
        }
    }

    int assetType = m_AssetType->GetSelection() - 1;
    if ( ( assetType >= 0 ) && ( assetType < Asset::AssetTypes::Count ) )
    {
        traverser.AddSearchCriteria( new AssetTypeCriteria( assetType ) );
    }

    tstring lowerBoundString = m_BoundsGreaterThan->GetLineText( 0 );
    tstring upperBoundString = m_BoundsLessThan->GetLineText( 0 );
    if ( ( lowerBoundString.size() > 0 ) || ( upperBoundString.size() > 0 ) )
    {
        float lowerBound = lowerBoundString.size() > 0 ? MAX( 0.0f, _tstof( lowerBoundString.c_str() ) ) : 0.0f;
        float upperBound = upperBoundString.size() > 0 ? MAX( 0.0f, _tstof( upperBoundString.c_str() ) ) : 0.0f;
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
}

void SearchBar::SetupScenes( M_SceneToZone& sceneToZone )
{
    M_TuidToZone tuidToZone;
    Luna::Scene* rootScene = m_SceneEditor->GetSceneManager()->GetRootScene();
    if ( rootScene )
    {
        V_ZoneDumbPtr zones;
        rootScene->GetAll< Zone >( zones );

        V_ZoneDumbPtr::iterator zoneItr = zones.begin();
        V_ZoneDumbPtr::iterator zoneEnd = zones.end();
        for ( ; zoneItr != zoneEnd; ++zoneItr )
        {
            tuidToZone.insert( std::make_pair( (*zoneItr)->GetID(), *zoneItr ) );
        }
    }

    if ( m_SearchAllZones->GetValue() )
    {
        const M_SceneSmartPtr& scenes = m_SceneEditor->GetSceneManager()->GetScenes();
        M_SceneSmartPtr::const_iterator sceneItr = scenes.begin();
        M_SceneSmartPtr::const_iterator sceneEnd = scenes.end();
        for ( ; sceneItr != sceneEnd; ++sceneItr )
        {
            M_TuidToZone::iterator zoneItr = tuidToZone.find( sceneItr->second->GetId() );
            if ( zoneItr != tuidToZone.end() )
            {
                sceneToZone.insert( std::make_pair( sceneItr->second, zoneItr->second ) );
            }
        }
    }
    else
    {
        Luna::Scene* currentScene = m_SceneEditor->GetSceneManager()->GetCurrentScene();
        M_TuidToZone::iterator zoneItr = tuidToZone.find( currentScene->GetId() );
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

void SearchBar::DisplayResults( const M_SceneToZone& sceneToZone, SearchBarTraverser& traverser )
{
    m_ResultNodes = traverser.GetSearchResults();
    RefreshResults( sceneToZone );
}

void SearchBar::RefreshResults( const M_SceneToZone& sceneToZone )
{
    m_Results->DeleteAllItems();

    int numResults = (int) m_ResultNodes.size();
    if ( numResults > 1 )
    {
        tchar resultString[ 256 ] = { 0 };
        _stprintf( resultString, TXT( "Found %d matches." ), numResults );
        m_Status->SetLabel( resultString );
    }
    else if ( numResults == 1 )
    {
        m_Status->SetLabel( wxT( "Found 1 match." ) );
    }
    else
    {
        m_Status->SetLabel( wxT( "No matches found." ) );
    }

    const Reflect::Enumeration* assetTypes = Reflect::Registry::GetInstance()->GetEnumeration( Reflect::GetType< Asset::AssetTypes::AssetType >() );
    V_HierarchyNodeSmartPtr::const_iterator resultsItr = m_ResultNodes.begin();
    V_HierarchyNodeSmartPtr::const_iterator resultsEnd = m_ResultNodes.end();
    for ( int id = 0; resultsItr != resultsEnd; ++id, ++resultsItr )
    {
        tstring name = (*resultsItr)->GetName();
        tstring entityClassName = TXT( "" );
        tstring entityClassPath = TXT( "" );
        tstring zone = TXT( "" );
        tstring assetType;

        Luna::Entity* entity = Reflect::ObjectCast< Luna::Entity >( *resultsItr );
        if ( entity )
        {
            Luna::EntityAssetSet* entityClassSet = entity->GetClassSet();
            if ( entityClassSet )
            {
                entityClassPath = entityClassSet->GetName();
                Nocturnal::Path path( entityClassPath );
                entityClassName = path.Basename();

                Asset::EntityAsset* entityClass = entityClassSet->GetEntityAsset();
                if ( entityClass )
                {
                    tstring assetTypeString;
                    Asset::AssetType assetTypeEnum = entityClass->GetAssetType();
                    bool check = assetTypes->GetElementLabel( assetTypeEnum, assetTypeString );
                    if ( check )
                    {
                        assetType = assetTypeString;
                    }
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

        wxListItem zoneListItem;
        zoneListItem.SetMask( wxLIST_MASK_TEXT );
        zoneListItem.SetText( zone );
        zoneListItem.SetId( id );
        zoneListItem.SetColumn( ResultColumns::Zone );

        wxListItem assetTypeListItem;
        assetTypeListItem.SetMask( wxLIST_MASK_TEXT );
        assetTypeListItem.SetText( assetType );
        assetTypeListItem.SetId( id );
        assetTypeListItem.SetColumn( ResultColumns::AssetType );

        m_Results->InsertItem( nameListItem );
        m_Results->SetItem( entityClassNameListItem );
        m_Results->SetItem( zoneListItem );
        m_Results->SetItem( assetTypeListItem );
    }
}

bool SearchBar::CompareComponents( const Component::ComponentPtr& rhs, const Component::ComponentPtr& lhs )
{
    return ( rhs->GetClass()->m_UIName < lhs->GetClass()->m_UIName );
}

tstring SearchBar::WildcardToRegex( const tstring& str, bool partialMatch )
{
    tstring result = str;
    result = boost::regex_replace( result, tregex( TXT( "\\." ) ), TXT( "\\\\." ) );
    result = boost::regex_replace( result, tregex( TXT( "\\*" ) ), TXT( ".*" ) );
    result = boost::regex_replace( result, tregex( TXT( "\\?" ) ), TXT( "." ) );

    if ( partialMatch )
    {
        result = TXT( ".*" ) + result + TXT( ".*" );
    }
    else
    {
        result = TXT( "^" ) + result + TXT( "$" );
    }

    return result;
}

bool EntityNameCriteria::Validate( Luna::HierarchyNode* node )
{
    tsmatch matchResults; 
    return boost::regex_match( node->GetName(), matchResults, tregex( m_Value, boost::regex::icase ) );
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

    tsmatch matchResults; 
    return boost::regex_match( classSet->GetName(), matchResults, tregex( m_Value, boost::regex::icase ) );
}

bool AssetTypeCriteria::Validate( Luna::HierarchyNode* node )
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

    Asset::AssetType assetType = entityClass->GetAssetType();
    return ( assetType == m_Value );
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

bool ComponentCriteria::Validate( Luna::HierarchyNode* node )
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

    Component::V_Component::iterator itr = m_Components.begin();
    Component::V_Component::iterator end = m_Components.end();
    for ( ; itr != end; ++itr )
    {
        if ( !entityClass->ContainsComponent( (*itr)->GetType() ) )
        {
            return false;
        }
    }

    return true;
}
