#include "Precompile.h"
#include "EntityCreateTool.h"

#include "Mesh.h"
#include "Scene.h"
#include "Entity.h"
#include "EntityType.h"
#include "EntityAssetSet.h"

#include "Application/Inspect/File/FileDialogButton.h"
#include "Application/Inspect/File/FileBrowserButton.h"
#include "Pick.h"

#include "Pipeline/Asset/AssetInit.h"
#include "Pipeline/Asset/Classes/Entity.h"

#include "Application/UI/FileDialog.h"

#include "Foundation/Log.h"

#include <algorithm>

using namespace Luna;

bool EntityCreateTool::s_PointerVisible = true;
bool EntityCreateTool::s_BoundsVisible = true;
bool EntityCreateTool::s_GeometryVisible = false;
std::vector< tstring > EntityCreateTool::s_RandomEntities;

LUNA_DEFINE_TYPE(Luna::EntityCreateTool);

tstring EntityRowInfo::GetListName() const
{
    const int probabilityStringSize = 32;
    tchar probabilityString[ probabilityStringSize + 1 ] = { 0 };

    _stprintf( probabilityString, TXT( " (%d%%)" ), (int) ( m_Probability * 100.0f ) );
    tstring listName = m_Name + probabilityString;

    return listName;
}

void EntityCreateTool::InitializeType()
{
    Reflect::RegisterClass< Luna::EntityCreateTool >( TXT( "Luna::EntityCreateTool" ) );
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

        EntityPtr entity = new Luna::Entity (m_Scene, new Asset::EntityInstance( entityClassPath.Get() ) );

        entity->SetPointerVisible( s_PointerVisible );
        entity->SetBoundsVisible( s_BoundsVisible );
        entity->SetGeometryVisible( s_GeometryVisible );

        entity->Rename( entity->GenerateName() );

        return entity;
    }
    catch ( const Nocturnal::Exception& ex )
    {
        Log::Error( TXT( "%s\n" ), ex.What() );
    }

    return NULL;
}

void EntityCreateTool::CreateProperties()
{
    m_Enumerator->PushPanel( TXT( "Entity" ), true);
    {
        m_Enumerator->PushContainer();
        {
            m_FileButton = m_Enumerator->AddFileDialogButton< tstring >( new Nocturnal::MemberProperty<Luna::EntityCreateTool, tstring> (this, &EntityCreateTool::GetEntityAsset, &EntityCreateTool::SetEntityAsset ) );
            m_BrowserButton = m_Enumerator->AddFileBrowserButton< tstring >( new Nocturnal::MemberProperty<Luna::EntityCreateTool, tstring> (this, &EntityCreateTool::GetEntityAsset, &EntityCreateTool::SetEntityAsset ) );

            m_FileButtonAdd = m_Enumerator->AddFileDialogButton< tstring >( new Nocturnal::MemberProperty<Luna::EntityCreateTool, tstring> (this, &EntityCreateTool::GetEntityAsset, &EntityCreateTool::AddEntityAsset ) );
            m_BrowserButtonAdd = m_Enumerator->AddFileBrowserButton< tstring >( new Nocturnal::MemberProperty<Luna::EntityCreateTool, tstring> (this, &EntityCreateTool::GetEntityAsset, &EntityCreateTool::AddEntityAsset ) );

            m_FileButtonAdd->SetIcon( TXT( "ellipses_add.png" ) );
            m_BrowserButtonAdd->SetIcon( TXT( "magnify_add.png" ) );

            Inspect::Action* modifyButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityCreateTool::OnModify ) );
            modifyButton->SetToolTip( TXT( "Modify" ) );
            modifyButton->SetIcon( TXT( "percent.png" ) );
            modifyButton->SetClientData( this );

            Inspect::Action* normalizeButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityCreateTool::OnNormalize ) );
            normalizeButton->SetToolTip( TXT( "Normalize" ) );
            normalizeButton->SetIcon( TXT( "normalize.png" ) );
            normalizeButton->SetClientData( this );

            Inspect::Action* deleteButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityCreateTool::OnDeleteClass ) );
            deleteButton->SetToolTip( TXT( "Delete" ) );
            deleteButton->SetIcon( TXT( "actions/list-remove.png" ) );
            deleteButton->SetClientData( this );

            Inspect::Action*  clearButton = m_Enumerator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityCreateTool::OnClear ) );
            clearButton->SetToolTip( TXT( "Clear" ) );
            clearButton->SetIcon( TXT( "delete.png" ) );
            clearButton->SetClientData( this );

            tstring filter;
            if ( Reflect::GetClass<Asset::Entity>()->GetProperty( Asset::AssetProperties::FileFilter, filter ) )
            {
                m_FileButton->SetFilter( filter );
                m_BrowserButton->SetFilter( filter );

                m_FileButtonAdd->SetFilter( filter );
                m_BrowserButtonAdd->SetFilter( filter );
            }
        }
        m_Enumerator->Pop();

        m_Enumerator->PushContainer();
        {
            m_RandomEntityList = m_Enumerator->AddList< tstring >( new Nocturnal::MemberProperty<Luna::EntityCreateTool, tstring > (this, &EntityCreateTool::GetRandomEntity, &EntityCreateTool::SetRandomEntity) );

            Inspect::FilteredDropTarget* filteredDropTarget = new Inspect::FilteredDropTarget( TXT( "*.entity.*" ) );
            filteredDropTarget->AddDroppedListener( Inspect::FilteredDropTargetSignature::Delegate( this, &EntityCreateTool::OnEntityDropped ) );

            m_RandomEntityList->SetDropTarget( filteredDropTarget );
        }
        m_Enumerator->Pop();

        m_Enumerator->PushContainer();
        {
            m_Enumerator->AddLabel( TXT( "Show Pointer" ) );
            m_Enumerator->AddCheckBox<bool>( new Nocturnal::MemberProperty<Luna::EntityCreateTool, bool> (this, &EntityCreateTool::GetPointerVisible, &EntityCreateTool::SetPointerVisible) );
        }
        m_Enumerator->Pop();

        m_Enumerator->PushContainer();
        {
            m_Enumerator->AddLabel( TXT( "Show Bounds" ) );
            m_Enumerator->AddCheckBox<bool>( new Nocturnal::MemberProperty<Luna::EntityCreateTool, bool> (this, &EntityCreateTool::GetBoundsVisible, &EntityCreateTool::SetBoundsVisible) );
        }
        m_Enumerator->Pop();

        m_Enumerator->PushContainer();
        {
            m_Enumerator->AddLabel( TXT( "Show Geometry" ) );
            m_Enumerator->AddCheckBox<bool>( new Nocturnal::MemberProperty<Luna::EntityCreateTool, bool> (this, &EntityCreateTool::GetGeometryVisible, &EntityCreateTool::SetGeometryVisible) );
        }
        m_Enumerator->Pop();

        __super::CreateProperties();
    }
    m_Enumerator->Pop();

    for ( std::vector< tstring >::iterator itr = s_RandomEntities.begin(), end = s_RandomEntities.end(); itr != end; ++itr )
    {
        SetEntityAsset( *itr );
    }
}

tstring EntityCreateTool::GetEntityAsset() const
{
    return m_ClassPath.Get();
}

void EntityCreateTool::SetEntityAsset( const tstring& value )
{
    m_RandomEntityInfo.clear();
    AddEntityAsset( value );
}

void EntityCreateTool::AddEntityAsset( const tstring& value )
{
    tstring entityName = TXT( "" );

    m_ClassPath.Set( value );

    entityName = m_ClassPath.Filename();

    V_EntityRowInfo::const_iterator itr = m_RandomEntityInfo.begin();
    V_EntityRowInfo::const_iterator end = m_RandomEntityInfo.end();
    for ( ; itr != end; ++itr )
    {
        if ( (*itr).m_ClassPath.Hash() == m_ClassPath.Hash() )
        {
            Log::Warning( TXT( "Entity '%s' already exists in the random list of entities.\n" ), entityName.c_str() );
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

tstring EntityCreateTool::GetRandomEntity() const
{
    tstring randomEntities;
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

void EntityCreateTool::SetRandomEntity( const tstring& entityName )
{
    NOC_BREAK();
}

void EntityCreateTool::OnDeleteClass( Inspect::Button* button )
{
    Luna::EntityCreateTool* thisTool = Reflect::ObjectCast< Luna::EntityCreateTool >( button->GetClientData() );
    if ( !thisTool )
    {
        Log::Error( TXT( "Invalid EntityCreateTool in OnDeleteClass()" ) );
        return;
    }

    const std::vector< tstring >& selectedItems = m_RandomEntityList->GetSelectedItems();

    V_EntityRowInfo::iterator itr = m_RandomEntityInfo.begin();
    while ( itr != m_RandomEntityInfo.end() )
    {
        tstring currentName = (*itr).GetListName();

        bool deleteItem = false;
        std::vector< tstring >::const_iterator itemItr = selectedItems.begin();
        std::vector< tstring >::const_iterator itemEnd = selectedItems.end();
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
        Log::Error( TXT( "Invalid EntityCreateTool in OnClear()" ) );
        return;
    }

    m_RandomEntityInfo.clear();

    thisTool->m_FileButton->SetPath( TXT( "" ) );
    thisTool->m_BrowserButton->SetPath( TXT( "" ) );

    thisTool->m_FileButtonAdd->SetPath( TXT( "" ) );
    thisTool->m_BrowserButtonAdd->SetPath( TXT( "" ) );

    thisTool->m_Enumerator->GetContainer()->GetCanvas()->Read();

    thisTool->RefreshInstance();
}

void EntityCreateTool::OnNormalize( Inspect::Button* button )
{
    Luna::EntityCreateTool* thisTool = Reflect::ObjectCast< Luna::EntityCreateTool >( button->GetClientData() );
    if ( !thisTool )
    {
        Log::Error( TXT( "Invalid EntityCreateTool in OnNormalize()" ) );
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
        Log::Error( TXT( "Invalid EntityCreateTool in OnModify()" ) );
        return;
    }

    std::map< u64, u64 > selectedHashes;

    const std::vector< tstring >& selectedItems = m_RandomEntityList->GetSelectedItems();
    std::vector< tstring >::const_iterator selectedItr = selectedItems.begin();
    std::vector< tstring >::const_iterator selectedEnd = selectedItems.end();
    for ( ; selectedItr != selectedEnd; ++selectedItr )
    {
        V_EntityRowInfo::iterator itr = m_RandomEntityInfo.begin();
        V_EntityRowInfo::iterator end = m_RandomEntityInfo.end();
        for ( ; itr != end; ++itr )
        {
            tstring listName = (*itr).GetListName();
            if ( listName == *selectedItr )
            {
                selectedHashes.insert( std::make_pair( (*itr).m_ClassPath.Hash(), (*itr).m_ClassPath.Hash() ) );
            }
        }
    }

    if ( selectedHashes.empty() )
    {
        Log::Warning( TXT( "No entities selected for modification in OnModify()!" ) );
        return;
    }

    wxTextEntryDialog dlg( NULL, wxT( "Please enter the new percentage" ), wxT( "Modify Percentage" ) );
    if ( dlg.ShowModal() != wxID_OK )
    {
        return;
    }

    tstring input = dlg.GetValue().c_str();
    if ( input.empty() )
    {
        return;
    }

    f32 newPercentage = _tstof( input.c_str() ) / 100.0f;
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

void EntityCreateTool::DropEntities( const std::vector< tstring >& entities, bool appendToList )
{
    m_Enumerator->GetContainer()->GetCanvas()->Freeze();

    if ( !appendToList )
    {
        m_RandomEntityInfo.clear();
    }

    for ( std::vector< tstring >::const_iterator itr = entities.begin(), end = entities.end(); itr != end; ++itr )
    {
        AddEntityAsset( *itr );
    }

    m_Enumerator->GetContainer()->GetCanvas()->Thaw();
}

