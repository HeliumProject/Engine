#include "Precompile.h"
#include "EntityCreateTool.h"

#include "Mesh.h"
#include "Scene.h"
#include "Entity.h"
#include "EntityType.h"
#include "EntityAssetSet.h"

#include "InspectFile/FileDialogButton.h"
#include "InspectFile/FileBrowserButton.h"
#include "Pick.h"

#include "Asset/AssetInit.h"
#include "Asset/EntityAsset.h"

#include "UIToolKit/FileDialog.h"

#include "FileSystem/FileSystem.h"
#include "Finder/AssetSpecs.h"
#include "Foundation/Log.h"

#include <algorithm>

using namespace Luna;

bool EntityCreateTool::s_PointerVisible = true;
bool EntityCreateTool::s_BoundsVisible = true;
bool EntityCreateTool::s_GeometryVisible = false;
V_string EntityCreateTool::s_RandomEntities;

LUNA_DEFINE_TYPE(Luna::EntityCreateTool);

std::string EntityRowInfo::GetListName() const
{
    const int probabilityStringSize = 32;
    char probabilityString[ probabilityStringSize + 1 ] = { 0 };

    sprintf( probabilityString, " (%d%%)", (int) ( m_Probability * 100.0f ) );
    std::string listName = m_Name + probabilityString;

    return listName;
}

void EntityCreateTool::InitializeType()
{
    Reflect::RegisterClass< Luna::EntityCreateTool >( "Luna::EntityCreateTool" );
}

void EntityCreateTool::CleanupType()
{
    Reflect::UnregisterClass< Luna::EntityCreateTool >();
}

EntityCreateTool::EntityCreateTool(Luna::Scene* scene, Enumerator* enumerator)
: Luna::CreateTool (scene, enumerator)
, m_RandomEntityList ( NULL )
, m_FileButton( NULL )
, m_BrowserButton( NULL )
, m_FileButtonAdd( NULL )
, m_BrowserButtonAdd( NULL )
{

}

EntityCreateTool::~EntityCreateTool()
{
    s_RandomEntities.clear();
    for ( V_EntityRowInfo::iterator itr = m_RandomEntityInfo.begin(), end = m_RandomEntityInfo.end(); itr != end; ++itr )
    {
        s_RandomEntities.push_back( (*itr).m_OriginalValue );
    }

    if ( m_ClassPath )
    {
        delete m_ClassPath;
    }
}

Luna::TransformPtr EntityCreateTool::CreateNode()
{
    Nocturnal::Path entityClassPath;

    //
    // Create Instance Object
    //
    try
    {
        if ( m_RandomEntityInfo.size() )
        {
            float total = 0.0f;  
            V_EntityRowInfo::iterator itr = m_RandomEntityInfo.begin();
            V_EntityRowInfo::iterator end = m_RandomEntityInfo.end();
            for ( ; itr != end; ++itr )
            {
                total += (*itr).m_Probability;
            }

            float probability = total * rand() / ( (float) RAND_MAX + 1.0f );
            total = 0.0f;
            for ( itr = m_RandomEntityInfo.begin(); itr != end; ++itr )
            {
                total += (*itr).m_Probability;
                if ( probability < total )
                {
                    entityClassPath = (*itr).m_ClassPath;
                    break;
                }
            }
        }

        EntityPtr entity = new Luna::Entity (m_Scene, new Asset::Entity( entityClassPath.Get() ) );

        entity->SetPointerVisible( s_PointerVisible );
        entity->SetBoundsVisible( s_BoundsVisible );
        entity->SetGeometryVisible( s_GeometryVisible );

        entity->Rename( entity->GenerateName() );

        return entity;
    }
    catch ( const Nocturnal::Exception& ex )
    {
        Log::Error( "%s\n", ex.what() );
    }

    return NULL;
}

void EntityCreateTool::CreateProperties()
{
    m_Enumerator->PushPanel("Entity", true);
    {
        m_Enumerator->PushContainer();
        {
            m_FileButton = m_Enumerator->AddFileDialogButton< std::string >( new Nocturnal::MemberProperty<Luna::EntityCreateTool, std::string> (this, &EntityCreateTool::GetEntityAsset, &EntityCreateTool::SetEntityAsset ) );
            m_BrowserButton = m_Enumerator->AddFileBrowserButton< std::string >( new Nocturnal::MemberProperty<Luna::EntityCreateTool, std::string> (this, &EntityCreateTool::GetEntityAsset, &EntityCreateTool::SetEntityAsset ) );

            m_FileButtonAdd = m_Enumerator->AddFileDialogButton< std::string >( new Nocturnal::MemberProperty<Luna::EntityCreateTool, std::string> (this, &EntityCreateTool::GetEntityAsset, &EntityCreateTool::AddEntityAsset ) );
            m_BrowserButtonAdd = m_Enumerator->AddFileBrowserButton< std::string >( new Nocturnal::MemberProperty<Luna::EntityCreateTool, std::string> (this, &EntityCreateTool::GetEntityAsset, &EntityCreateTool::AddEntityAsset ) );

            m_FileButtonAdd->SetIcon( "ellipses_add_16.png" );
            m_BrowserButtonAdd->SetIcon( "magnify_add_16.png" );

            Inspect::Action* modifyButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityCreateTool::OnModify ) );
            modifyButton->SetToolTip( "Modify" );
            modifyButton->SetIcon( "percent_16.png" );
            modifyButton->SetClientData( this );

            Inspect::Action* normalizeButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityCreateTool::OnNormalize ) );
            normalizeButton->SetToolTip( "Normalize" );
            normalizeButton->SetIcon( "normalize_16.png" );
            normalizeButton->SetClientData( this );

            Inspect::Action* deleteButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityCreateTool::OnDeleteClass ) );
            deleteButton->SetToolTip( "Delete" );
            deleteButton->SetIcon( "remove_16.png" );
            deleteButton->SetClientData( this );

            Inspect::Action*  clearButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityCreateTool::OnClear ) );
            clearButton->SetToolTip( "Clear" );
            clearButton->SetIcon( "delete_16.png" );
            clearButton->SetClientData( this );

            std::string specName;
            if ( Reflect::GetClass<Asset::EntityAsset>()->GetProperty( Asset::AssetProperties::ModifierSpec, specName ) )
            {
                const Finder::FinderSpec* spec = Finder::GetFinderSpec( specName );

                m_FileButton->SetFilter( spec->GetDialogFilter() );
                m_BrowserButton->SetFilter( spec->GetDialogFilter() );

                m_FileButtonAdd->SetFilter( spec->GetDialogFilter() );
                m_BrowserButtonAdd->SetFilter( spec->GetDialogFilter() );
            }
        }
        m_Enumerator->Pop();

        m_Enumerator->PushContainer();
        {
            m_RandomEntityList = m_Enumerator->AddList< std::string >( new Nocturnal::MemberProperty<Luna::EntityCreateTool, std::string > (this, &EntityCreateTool::GetRandomEntity, &EntityCreateTool::SetRandomEntity) );

            Inspect::FilteredDropTarget* filteredDropTarget = new Inspect::FilteredDropTarget( &FinderSpecs::Asset::ENTITY_DECORATION );
            filteredDropTarget->AddDroppedListener( Inspect::FilteredDropTargetSignature::Delegate( this, &EntityCreateTool::OnEntityDropped ) );

            m_RandomEntityList->SetDropTarget( filteredDropTarget );
        }
        m_Enumerator->Pop();

        m_Enumerator->PushContainer();
        {
            m_Enumerator->AddLabel("Show Pointer");
            m_Enumerator->AddCheckBox<bool>( new Nocturnal::MemberProperty<Luna::EntityCreateTool, bool> (this, &EntityCreateTool::GetPointerVisible, &EntityCreateTool::SetPointerVisible) );
        }
        m_Enumerator->Pop();

        m_Enumerator->PushContainer();
        {
            m_Enumerator->AddLabel("Show Bounds");
            m_Enumerator->AddCheckBox<bool>( new Nocturnal::MemberProperty<Luna::EntityCreateTool, bool> (this, &EntityCreateTool::GetBoundsVisible, &EntityCreateTool::SetBoundsVisible) );
        }
        m_Enumerator->Pop();

        m_Enumerator->PushContainer();
        {
            m_Enumerator->AddLabel("Show Geometry");
            m_Enumerator->AddCheckBox<bool>( new Nocturnal::MemberProperty<Luna::EntityCreateTool, bool> (this, &EntityCreateTool::GetGeometryVisible, &EntityCreateTool::SetGeometryVisible) );
        }
        m_Enumerator->Pop();

        __super::CreateProperties();
    }
    m_Enumerator->Pop();

    for ( V_string::iterator itr = s_RandomEntities.begin(), end = s_RandomEntities.end(); itr != end; ++itr )
    {
        SetEntityAsset( *itr );
    }
}

std::string EntityCreateTool::GetEntityAsset() const
{
    return m_ClassPath.Get();
}

void EntityCreateTool::SetEntityAsset( const std::string& value )
{
    m_RandomEntityInfo.clear();
    AddEntityAsset( value );
}

void EntityCreateTool::AddEntityAsset( const std::string& value )
{
    std::string entityName = "";

    m_ClassPath.Set( value );

    entityName = m_ClassPath.Filename();

    V_EntityRowInfo::const_iterator itr = m_RandomEntityInfo.begin();
    V_EntityRowInfo::const_iterator end = m_RandomEntityInfo.end();
    for ( ; itr != end; ++itr )
    {
        if ( (*itr).m_ClassPath.Hash() == m_ClassPath.Hash() )
        {
            Log::Warning( "Entity '%s' already exists in the random list of entities.\n", entityName.c_str() );
            return;
        }
    }

    EntityRowInfo rowInfo;
    rowInfo.m_ClassPath = m_ClassPath;
    rowInfo.m_Probability = 1.0f;
    rowInfo.m_Name = entityName;
    rowInfo.m_OriginalValue = value;
    m_RandomEntityInfo.push_back( rowInfo );

    m_Enumerator->GetContainer()->GetCanvas()->Read();

    Place(Math::Matrix4::Identity);
}

bool EntityCreateTool::GetPointerVisible() const
{
    return s_PointerVisible;
}

void EntityCreateTool::SetPointerVisible(bool show)
{
    s_PointerVisible = show;

    Place( Math::Matrix4::Identity );
}

bool EntityCreateTool::GetBoundsVisible() const
{
    return s_BoundsVisible;
}

void EntityCreateTool::SetBoundsVisible(bool show)
{
    s_BoundsVisible = show;

    Place( Math::Matrix4::Identity );
}

bool EntityCreateTool::GetGeometryVisible() const
{
    return s_GeometryVisible;
}

void EntityCreateTool::SetGeometryVisible(bool show)
{
    s_GeometryVisible = show;

    Place( Math::Matrix4::Identity );
}

std::string EntityCreateTool::GetRandomEntity() const
{
    std::string randomEntities;
    V_EntityRowInfo::const_iterator itr = m_RandomEntityInfo.begin();
    V_EntityRowInfo::const_iterator end = m_RandomEntityInfo.end();
    for ( ; itr != end; ++itr )
    {
        if ( randomEntities.length() )
        {
            randomEntities += Reflect::s_ContainerItemDelimiter;
        }

        randomEntities += (*itr).GetListName();
    }

    return randomEntities;
}

void EntityCreateTool::SetRandomEntity( const std::string& entityName )
{
    NOC_BREAK();
}

void EntityCreateTool::OnDeleteClass( Inspect::Button* button )
{
    Luna::EntityCreateTool* thisTool = Reflect::ObjectCast< Luna::EntityCreateTool >( button->GetClientData() );
    if ( !thisTool )
    {
        Log::Error( "Invalid EntityCreateTool in OnDeleteClass()" );
        return;
    }

    const V_string& selectedItems = m_RandomEntityList->GetSelectedItems();

    V_EntityRowInfo::iterator itr = m_RandomEntityInfo.begin();
    while ( itr != m_RandomEntityInfo.end() )
    {
        std::string currentName = (*itr).GetListName();

        bool deleteItem = false;
        V_string::const_iterator itemItr = selectedItems.begin();
        V_string::const_iterator itemEnd = selectedItems.end();
        for ( ; itemItr != itemEnd; ++itemItr )
        {
            if ( *itemItr == currentName )
            {
                deleteItem = true;
                break;
            }
        }

        if ( deleteItem )
        {
            if ( m_ClassPath.Hash() == ( *itr ).m_ClassPath.Hash() )
            {
                delete m_ClassPath;
                Place( Math::Matrix4::Identity );
            }

            m_RandomEntityInfo.erase( itr );
            itr = m_RandomEntityInfo.begin();
        }
        else
        {
            ++itr;
        }
    }

    thisTool->m_Enumerator->GetContainer()->GetCanvas()->Read();

    thisTool->RefreshInstance();
}

void EntityCreateTool::OnClear( Inspect::Button* button )
{
    Luna::EntityCreateTool* thisTool = Reflect::ObjectCast< Luna::EntityCreateTool >( button->GetClientData() );
    if ( !thisTool )
    {
        Log::Error( "Invalid EntityCreateTool in OnClear()" );
        return;
    }

    m_RandomEntityInfo.clear();

    thisTool->m_FileButton->SetPath( "" );
    thisTool->m_BrowserButton->SetPath( "" );

    thisTool->m_FileButtonAdd->SetPath( "" );
    thisTool->m_BrowserButtonAdd->SetPath( "" );

    thisTool->m_Enumerator->GetContainer()->GetCanvas()->Read();

    thisTool->RefreshInstance();
}

void EntityCreateTool::OnNormalize( Inspect::Button* button )
{
    Luna::EntityCreateTool* thisTool = Reflect::ObjectCast< Luna::EntityCreateTool >( button->GetClientData() );
    if ( !thisTool )
    {
        Log::Error( "Invalid EntityCreateTool in OnNormalize()" );
        return;
    }

    float total = 0.0f;  
    V_EntityRowInfo::iterator itr = m_RandomEntityInfo.begin();
    V_EntityRowInfo::iterator end = m_RandomEntityInfo.end();
    for ( ; itr != end; ++itr )
    {
        total += (*itr).m_Probability;
    }

    for ( itr = m_RandomEntityInfo.begin(); itr != end; ++itr )
    {
        (*itr).m_Probability /= total;
    }

    thisTool->m_Enumerator->GetContainer()->GetCanvas()->Read();
}

void EntityCreateTool::OnModify( Inspect::Button* button )
{
    Luna::EntityCreateTool* thisTool = Reflect::ObjectCast< Luna::EntityCreateTool >( button->GetClientData() );
    if ( !thisTool )
    {
        Log::Error( "Invalid EntityCreateTool in OnModify()" );
        return;
    }

    M_u64 selectedHashes;

    const V_string& selectedItems = m_RandomEntityList->GetSelectedItems();
    V_string::const_iterator selectedItr = selectedItems.begin();
    V_string::const_iterator selectedEnd = selectedItems.end();
    for ( ; selectedItr != selectedEnd; ++selectedItr )
    {
        V_EntityRowInfo::iterator itr = m_RandomEntityInfo.begin();
        V_EntityRowInfo::iterator end = m_RandomEntityInfo.end();
        for ( ; itr != end; ++itr )
        {
            std::string listName = (*itr).GetListName();
            if ( listName == *selectedItr )
            {
                selectedHashes.insert( std::make_pair( (*itr).m_ClassPath.Hash(), (*itr).m_ClassPath.Hash() ) );
            }
        }
    }

    if ( selectedHashes.empty() )
    {
        Log::Warning( "No entities selected for modification in OnModify()!" );
        return;
    }

    wxTextEntryDialog dlg( NULL, "Please enter the new percentage", "Modify Percentage" );
    if ( dlg.ShowModal() != wxID_OK )
    {
        return;
    }

    std::string input = dlg.GetValue().c_str();
    if ( input.empty() )
    {
        return;
    }

    f32 newPercentage = atof( input.c_str() ) / 100.0f;
    V_EntityRowInfo::iterator itr = m_RandomEntityInfo.begin();
    V_EntityRowInfo::iterator end = m_RandomEntityInfo.end();
    for ( ; itr != end; ++itr )
    {
        if ( selectedHashes.find( (*itr).m_ClassPath.Hash() ) != selectedHashes.end() )
        {
            (*itr).m_Probability = newPercentage;
        }
    }  

    thisTool->m_Enumerator->GetContainer()->GetCanvas()->Read();
}

void EntityCreateTool::OnEntityDropped( const Inspect::FilteredDropTargetArgs& args )
{
    DropEntities( args.m_Paths, wxIsShiftDown() );
}

void EntityCreateTool::DropEntities( const V_string& entities, bool appendToList )
{
    m_Enumerator->GetContainer()->GetCanvas()->Freeze();

    if ( !appendToList )
    {
        m_RandomEntityInfo.clear();
    }

    for ( V_string::const_iterator itr = entities.begin(), end = entities.end(); itr != end; ++itr )
    {
        AddEntityAsset( *itr );
    }

    m_Enumerator->GetContainer()->GetCanvas()->Thaw();
}

