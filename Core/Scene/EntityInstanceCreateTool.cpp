/*#include "Precompile.h"*/
#include "EntityInstanceCreateTool.h"

#include "Foundation/Log.h"

#include "Application/Inspect/Interpreters/File/FileDialogButton.h"
#include "Application/Inspect/Interpreters/File/FileBrowserButton.h"
#include "Application/UI/FileDialog.h"

#include "Core/Scene/Mesh.h"
#include "Core/Scene/Scene.h"
#include "Core/Scene/EntityInstance.h"
#include "Core/Scene/EntityInstanceType.h"
#include "Core/Scene/EntitySet.h"
#include "Core/Scene/Pick.h"
#include "Core/Asset/AssetInit.h"
#include "Core/Asset/Classes/Entity.h"

#include <algorithm>

using namespace Helium;
using namespace Helium::Core;

bool EntityInstanceCreateTool::s_PointerVisible = true;
bool EntityInstanceCreateTool::s_BoundsVisible = true;
bool EntityInstanceCreateTool::s_GeometryVisible = false;
std::vector< tstring > EntityInstanceCreateTool::s_RandomEntities;

SCENE_DEFINE_TYPE(Core::EntityInstanceCreateTool);

tstring EntityRowInfo::GetListName() const
{
    const int probabilityStringSize = 32;
    tchar probabilityString[ probabilityStringSize + 1 ] = { 0 };

    _stprintf( probabilityString, TXT( " (%d%%)" ), (int) ( m_Probability * 100.0f ) );
    tstring listName = m_Name + probabilityString;

    return listName;
}

void EntityInstanceCreateTool::InitializeType()
{
    Reflect::RegisterClassType< Core::EntityInstanceCreateTool >( TXT( "Core::EntityInstanceCreateTool" ) );
}

void EntityInstanceCreateTool::CleanupType()
{
    Reflect::UnregisterClassType< Core::EntityInstanceCreateTool >();
}

EntityInstanceCreateTool::EntityInstanceCreateTool(Core::Scene* scene, PropertiesGenerator* generator)
: Core::CreateTool (scene, generator)
, m_RandomEntityList ( NULL )
, m_FileButton( NULL )
, m_BrowserButton( NULL )
, m_FileButtonAdd( NULL )
, m_BrowserButtonAdd( NULL )
{

}

EntityInstanceCreateTool::~EntityInstanceCreateTool()
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

Core::TransformPtr EntityInstanceCreateTool::CreateNode()
{
    Helium::Path entityClassPath;

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

        EntityPtr entity = new Core::Entity (m_Scene, new Asset::EntityInstance( entityClassPath.Get() ) );

        entity->SetPointerVisible( s_PointerVisible );
        entity->SetBoundsVisible( s_BoundsVisible );
        entity->SetGeometryVisible( s_GeometryVisible );

        entity->Rename( entity->GenerateName() );

        return entity;
    }
    catch ( const Helium::Exception& ex )
    {
        Log::Error( TXT( "%s\n" ), ex.What() );
    }

    return NULL;
}

void EntityInstanceCreateTool::CreateProperties()
{
    m_Generator->PushPanel( TXT( "Entity" ), true);
    {
        m_Generator->PushContainer();
        {
            m_FileButton = m_Generator->AddFileDialogButton< tstring >( new Helium::MemberProperty<Core::EntityInstanceCreateTool, tstring> (this, &EntityInstanceCreateTool::GetEntityAsset, &EntityInstanceCreateTool::SetEntityAsset ) );
            m_BrowserButton = m_Generator->AddFileBrowserButton< tstring >( new Helium::MemberProperty<Core::EntityInstanceCreateTool, tstring> (this, &EntityInstanceCreateTool::GetEntityAsset, &EntityInstanceCreateTool::SetEntityAsset ) );

            m_FileButtonAdd = m_Generator->AddFileDialogButton< tstring >( new Helium::MemberProperty<Core::EntityInstanceCreateTool, tstring> (this, &EntityInstanceCreateTool::GetEntityAsset, &EntityInstanceCreateTool::AddEntityAsset ) );
            m_BrowserButtonAdd = m_Generator->AddFileBrowserButton< tstring >( new Helium::MemberProperty<Core::EntityInstanceCreateTool, tstring> (this, &EntityInstanceCreateTool::GetEntityAsset, &EntityInstanceCreateTool::AddEntityAsset ) );

            m_FileButtonAdd->SetIcon( TXT( "ellipses_add" ) );
            m_BrowserButtonAdd->SetIcon( TXT( "magnify_add" ) );

            Inspect::Button* modifyButton = m_Generator->AddButton( Inspect::ButtonSignature::Delegate( this, &EntityInstanceCreateTool::OnModify ) );
            modifyButton->SetToolTip( TXT( "Modify" ) );
            modifyButton->SetIcon( TXT( "percent" ) );
            modifyButton->SetInterpreterClientData( this );

            Inspect::Button* normalizeButton = m_Generator->AddButton( Inspect::ButtonSignature::Delegate( this, &EntityInstanceCreateTool::OnNormalize ) );
            normalizeButton->SetToolTip( TXT( "Normalize" ) );
            normalizeButton->SetIcon( TXT( "normalize" ) );
            normalizeButton->SetInterpreterClientData( this );

            Inspect::Button* deleteButton = m_Generator->AddButton( Inspect::ButtonSignature::Delegate( this, &EntityInstanceCreateTool::OnDeleteClass ) );
            deleteButton->SetToolTip( TXT( "Delete" ) );
            deleteButton->SetIcon( TXT( "actions/list-remove" ) );
            deleteButton->SetInterpreterClientData( this );

            Inspect::Button*  clearButton = m_Generator->AddButton( Inspect::ButtonSignature::Delegate( this, &EntityInstanceCreateTool::OnClear ) );
            clearButton->SetToolTip( TXT( "Clear" ) );
            clearButton->SetIcon( TXT( "delete" ) );
            clearButton->SetInterpreterClientData( this );

            tstring filter;
            if ( Reflect::GetClass<Asset::Entity>()->GetProperty( Asset::AssetProperties::FileFilter, filter ) )
            {
                m_FileButton->SetFilter( filter );
                m_BrowserButton->SetFilter( filter );

                m_FileButtonAdd->SetFilter( filter );
                m_BrowserButtonAdd->SetFilter( filter );
            }
        }
        m_Generator->Pop();

        m_Generator->PushContainer();
        {
            m_RandomEntityList = m_Generator->AddList< tstring >( new Helium::MemberProperty<Core::EntityInstanceCreateTool, tstring > (this, &EntityInstanceCreateTool::GetRandomEntity, &EntityInstanceCreateTool::SetRandomEntity) );

#ifdef INSPECT_REFACTOR
            Inspect::FilteredDropTarget* filteredDropTarget = new Inspect::FilteredDropTarget( TXT( "*.entity.*" ) );
            filteredDropTarget->AddDroppedListener( Inspect::FilteredDropTargetSignature::Delegate( this, &EntityInstanceCreateTool::OnEntityDropped ) );
            m_RandomEntityList->SetDropTarget( filteredDropTarget );
#endif
        }
        m_Generator->Pop();

        m_Generator->PushContainer();
        {
            m_Generator->AddLabel( TXT( "Show Pointer" ) );
            m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<Core::EntityInstanceCreateTool, bool> (this, &EntityInstanceCreateTool::GetPointerVisible, &EntityInstanceCreateTool::SetPointerVisible) );
        }
        m_Generator->Pop();

        m_Generator->PushContainer();
        {
            m_Generator->AddLabel( TXT( "Show Bounds" ) );
            m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<Core::EntityInstanceCreateTool, bool> (this, &EntityInstanceCreateTool::GetBoundsVisible, &EntityInstanceCreateTool::SetBoundsVisible) );
        }
        m_Generator->Pop();

        m_Generator->PushContainer();
        {
            m_Generator->AddLabel( TXT( "Show Geometry" ) );
            m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<Core::EntityInstanceCreateTool, bool> (this, &EntityInstanceCreateTool::GetGeometryVisible, &EntityInstanceCreateTool::SetGeometryVisible) );
        }
        m_Generator->Pop();

        __super::CreateProperties();
    }
    m_Generator->Pop();

    for ( std::vector< tstring >::iterator itr = s_RandomEntities.begin(), end = s_RandomEntities.end(); itr != end; ++itr )
    {
        SetEntityAsset( *itr );
    }
}

tstring EntityInstanceCreateTool::GetEntityAsset() const
{
    return m_ClassPath.Get();
}

void EntityInstanceCreateTool::SetEntityAsset( const tstring& value )
{
    m_RandomEntityInfo.clear();
    AddEntityAsset( value );
}

void EntityInstanceCreateTool::AddEntityAsset( const tstring& value )
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

    m_Generator->GetContainer()->GetCanvas()->Read();

    Place(Math::Matrix4::Identity);
}

bool EntityInstanceCreateTool::GetPointerVisible() const
{
    return s_PointerVisible;
}

void EntityInstanceCreateTool::SetPointerVisible(bool show)
{
    s_PointerVisible = show;

    Place( Math::Matrix4::Identity );
}

bool EntityInstanceCreateTool::GetBoundsVisible() const
{
    return s_BoundsVisible;
}

void EntityInstanceCreateTool::SetBoundsVisible(bool show)
{
    s_BoundsVisible = show;

    Place( Math::Matrix4::Identity );
}

bool EntityInstanceCreateTool::GetGeometryVisible() const
{
    return s_GeometryVisible;
}

void EntityInstanceCreateTool::SetGeometryVisible(bool show)
{
    s_GeometryVisible = show;

    Place( Math::Matrix4::Identity );
}

tstring EntityInstanceCreateTool::GetRandomEntity() const
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

void EntityInstanceCreateTool::SetRandomEntity( const tstring& entityName )
{
    HELIUM_BREAK();
}

void EntityInstanceCreateTool::OnDeleteClass( Inspect::Button* button )
{
    Core::EntityInstanceCreateTool* thisTool = Reflect::ObjectCast< Core::EntityInstanceCreateTool >( button->GetInterpreterClientData() );
    if ( !thisTool )
    {
        Log::Error( TXT( "Invalid EntityInstanceCreateTool in OnDeleteClass()" ) );
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

    thisTool->m_Generator->GetContainer()->GetCanvas()->Read();

    thisTool->RefreshInstance();
}

void EntityInstanceCreateTool::OnClear( Inspect::Button* button )
{
    Core::EntityInstanceCreateTool* thisTool = Reflect::ObjectCast< Core::EntityInstanceCreateTool >( button->GetInterpreterClientData() );
    if ( !thisTool )
    {
        Log::Error( TXT( "Invalid EntityInstanceCreateTool in OnClear()" ) );
        return;
    }

    m_RandomEntityInfo.clear();

    thisTool->m_FileButton->SetPath( TXT( "" ) );
    thisTool->m_BrowserButton->SetPath( TXT( "" ) );

    thisTool->m_FileButtonAdd->SetPath( TXT( "" ) );
    thisTool->m_BrowserButtonAdd->SetPath( TXT( "" ) );

    thisTool->m_Generator->GetContainer()->GetCanvas()->Read();

    thisTool->RefreshInstance();
}

void EntityInstanceCreateTool::OnNormalize( Inspect::Button* button )
{
    Core::EntityInstanceCreateTool* thisTool = Reflect::ObjectCast< Core::EntityInstanceCreateTool >( button->GetInterpreterClientData() );
    if ( !thisTool )
    {
        Log::Error( TXT( "Invalid EntityInstanceCreateTool in OnNormalize()" ) );
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

    thisTool->m_Generator->GetContainer()->GetCanvas()->Read();
}

void EntityInstanceCreateTool::OnModify( Inspect::Button* button )
{
    Core::EntityInstanceCreateTool* thisTool = Reflect::ObjectCast< Core::EntityInstanceCreateTool >( button->GetInterpreterClientData() );
    if ( !thisTool )
    {
        Log::Error( TXT( "Invalid EntityInstanceCreateTool in OnModify()" ) );
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

    wxTextEntryDialog dlg( NULL, TXT( "Please enter the new percentage" ), TXT( "Modify Percentage" ) );
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

    thisTool->m_Generator->GetContainer()->GetCanvas()->Read();
}

void EntityInstanceCreateTool::OnEntityDropped( const Inspect::FilteredDropTargetArgs& args )
{
    DropEntities( args.m_Paths, false );
}

void EntityInstanceCreateTool::DropEntities( const std::vector< tstring >& entities, bool appendToList )
{
    m_Generator->GetContainer()->GetCanvas()->Freeze();

    if ( !appendToList )
    {
        m_RandomEntityInfo.clear();
    }

    for ( std::vector< tstring >::const_iterator itr = entities.begin(), end = entities.end(); itr != end; ++itr )
    {
        AddEntityAsset( *itr );
    }

    m_Generator->GetContainer()->GetCanvas()->Thaw();
}

