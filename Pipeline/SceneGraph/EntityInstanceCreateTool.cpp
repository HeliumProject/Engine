/*#include "Precompile.h"*/
#include "EntityInstanceCreateTool.h"

#include "Foundation/Log.h"

#include "Pipeline/SceneGraph/Mesh.h"
#include "Pipeline/SceneGraph/Scene.h"
#include "Pipeline/SceneGraph/EntityInstance.h"
#include "Pipeline/SceneGraph/EntityInstanceType.h"
#include "Pipeline/SceneGraph/Pick.h"
#include "Pipeline/Asset/AssetInit.h"
#include "Pipeline/Asset/Classes/Entity.h"

#include <algorithm>

using namespace Helium;
using namespace Helium::SceneGraph;

bool EntityInstanceCreateTool::s_PointerVisible = true;
bool EntityInstanceCreateTool::s_BoundsVisible = true;
bool EntityInstanceCreateTool::s_GeometryVisible = false;
std::vector< tstring > EntityInstanceCreateTool::s_RandomEntities;

REFLECT_DEFINE_ABSTRACT(SceneGraph::EntityInstanceCreateTool);

tstring EntityRowInfo::GetListName() const
{
    const int probabilityStringSize = 32;
    tchar_t probabilityString[ probabilityStringSize + 1 ] = { 0 };

    _stprintf( probabilityString, TXT( " (%d%%)" ), (int) ( m_Probability * 100.0f ) );
    tstring listName = m_Name + probabilityString;

    return listName;
}

void EntityInstanceCreateTool::InitializeType()
{
    Reflect::RegisterClassType< SceneGraph::EntityInstanceCreateTool >( TXT( "SceneGraph::EntityInstanceCreateTool" ) );
}

void EntityInstanceCreateTool::CleanupType()
{
    Reflect::UnregisterClassType< SceneGraph::EntityInstanceCreateTool >();
}

EntityInstanceCreateTool::EntityInstanceCreateTool(SceneGraph::Scene* scene, PropertiesGenerator* generator)
: SceneGraph::CreateTool (scene, generator)
, m_RandomEntityList ( NULL )
, m_FileButton( NULL )
, m_FileButtonAdd( NULL )
{

}

EntityInstanceCreateTool::~EntityInstanceCreateTool()
{
    s_RandomEntities.clear();
    for ( V_EntityRowInfo::iterator itr = m_RandomEntityInfo.begin(), end = m_RandomEntityInfo.end(); itr != end; ++itr )
    {
        s_RandomEntities.push_back( (*itr).m_OriginalValue );
    }
}

SceneGraph::TransformPtr EntityInstanceCreateTool::CreateNode()
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

        EntityInstancePtr entityInstance = new EntityInstance ();
        entityInstance->SetOwner( m_Scene );
        entityInstance->Initialize();
        entityInstance->SetEntityPath( entityClassPath );
        entityInstance->SetPointerVisible( s_PointerVisible );
        entityInstance->SetBoundsVisible( s_BoundsVisible );
        entityInstance->SetGeometryVisible( s_GeometryVisible );
        entityInstance->Rename( entityInstance->GenerateName() );

        return entityInstance;
    }
    catch ( const Helium::Exception& ex )
    {
        Log::Error( TXT( "%s\n" ), ex.What() );
    }

    return NULL;
}

void EntityInstanceCreateTool::CreateProperties()
{
    m_Generator->PushContainer( TXT( "EntityInstance" ) );
    {
        m_Generator->PushContainer();
        {
            m_FileButton = m_Generator->AddFileDialogButton< tstring >( new Helium::MemberProperty<SceneGraph::EntityInstanceCreateTool, tstring> (this, &EntityInstanceCreateTool::GetEntityAsset, &EntityInstanceCreateTool::SetEntityAsset ) );
            m_FileButtonAdd = m_Generator->AddFileDialogButton< tstring >( new Helium::MemberProperty<SceneGraph::EntityInstanceCreateTool, tstring> (this, &EntityInstanceCreateTool::GetEntityAsset, &EntityInstanceCreateTool::AddEntityAsset ) );
            m_FileButtonAdd->a_Icon.Set( TXT( "ellipses_add" ) );

            //Inspect::Button* modifyButton = m_Generator->AddButton( Inspect::ButtonClickedSignature::Delegate( this, &EntityInstanceCreateTool::OnModify ) );
            //modifyButton->a_HelpText.Set( TXT( "Modify" ) );
            //modifyButton->a_Icon.Set( TXT( "percent" ) );

            Inspect::Button* normalizeButton = m_Generator->AddButton( Inspect::ButtonClickedSignature::Delegate( this, &EntityInstanceCreateTool::OnNormalize ) );
            normalizeButton->a_HelpText.Set( TXT( "Normalize" ) );
            normalizeButton->a_Icon.Set( TXT( "normalize" ) );

            Inspect::Button* deleteButton = m_Generator->AddButton( Inspect::ButtonClickedSignature::Delegate( this, &EntityInstanceCreateTool::OnDeleteClass ) );
            deleteButton->a_HelpText.Set( TXT( "Delete" ) );
            deleteButton->a_Icon.Set( TXT( "actions/list-remove" ) );

            Inspect::Button*  clearButton = m_Generator->AddButton( Inspect::ButtonClickedSignature::Delegate( this, &EntityInstanceCreateTool::OnClear ) );
            clearButton->a_HelpText.Set( TXT( "Clear" ) );
            clearButton->a_Icon.Set( TXT( "delete" ) );

            tstring filter;
            if ( Reflect::GetClass<Asset::Entity>()->GetProperty( Asset::AssetProperties::FileFilter, filter ) )
            {
                m_FileButton->a_Filter.Set( filter );
                m_FileButtonAdd->a_Filter.Set( filter );
            }
        }
        m_Generator->Pop();

        m_Generator->PushContainer();
        {
            m_RandomEntityList = m_Generator->AddList< tstring >( new Helium::MemberProperty<SceneGraph::EntityInstanceCreateTool, tstring > (this, &EntityInstanceCreateTool::GetRandomEntity, &EntityInstanceCreateTool::SetRandomEntity) );
            m_RandomEntityList->SetProperty( TXT( "FileFilter" ), TXT( "*.HeliumEntity" ) );
        }
        m_Generator->Pop();

        m_Generator->PushContainer();
        {
            m_Generator->AddLabel( TXT( "Show Pointer" ) );
            m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<SceneGraph::EntityInstanceCreateTool, bool> (this, &EntityInstanceCreateTool::GetPointerVisible, &EntityInstanceCreateTool::SetPointerVisible) );
        }
        m_Generator->Pop();

        m_Generator->PushContainer();
        {
            m_Generator->AddLabel( TXT( "Show Bounds" ) );
            m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<SceneGraph::EntityInstanceCreateTool, bool> (this, &EntityInstanceCreateTool::GetBoundsVisible, &EntityInstanceCreateTool::SetBoundsVisible) );
        }
        m_Generator->Pop();

        m_Generator->PushContainer();
        {
            m_Generator->AddLabel( TXT( "Show Geometry" ) );
            m_Generator->AddCheckBox<bool>( new Helium::MemberProperty<SceneGraph::EntityInstanceCreateTool, bool> (this, &EntityInstanceCreateTool::GetGeometryVisible, &EntityInstanceCreateTool::SetGeometryVisible) );
        }
        m_Generator->Pop();
    }
    m_Generator->Pop();

    for ( std::vector< tstring >::iterator itr = s_RandomEntities.begin(), end = s_RandomEntities.end(); itr != end; ++itr )
    {
        AddEntityAsset( *itr );
    }

    Base::CreateProperties();
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
    m_ClassPath.Set( value );

    V_EntityRowInfo::const_iterator itr = m_RandomEntityInfo.begin();
    V_EntityRowInfo::const_iterator end = m_RandomEntityInfo.end();
    for ( ; itr != end; ++itr )
    {
        if ( (*itr).m_ClassPath.Hash() == m_ClassPath.Hash() )
        {
            Log::Warning( TXT( "EntityInstance '%s' already exists in the random list of entities.\n" ), m_ClassPath.Filename().c_str() );
            return;
        }
    }

    EntityRowInfo rowInfo;
    rowInfo.m_ClassPath = m_ClassPath;
    rowInfo.m_Name = m_ClassPath.Filename();
    rowInfo.m_Probability = 1.0f;
    rowInfo.m_OriginalValue = value;
    m_RandomEntityInfo.push_back( rowInfo );

    m_Generator->GetContainer()->GetCanvas()->Read();

    Place(Matrix4::Identity);
}

bool EntityInstanceCreateTool::GetPointerVisible() const
{
    return s_PointerVisible;
}

void EntityInstanceCreateTool::SetPointerVisible(bool show)
{
    s_PointerVisible = show;

    Place( Matrix4::Identity );
}

bool EntityInstanceCreateTool::GetBoundsVisible() const
{
    return s_BoundsVisible;
}

void EntityInstanceCreateTool::SetBoundsVisible(bool show)
{
    s_BoundsVisible = show;

    Place( Matrix4::Identity );
}

bool EntityInstanceCreateTool::GetGeometryVisible() const
{
    return s_GeometryVisible;
}

void EntityInstanceCreateTool::SetGeometryVisible(bool show)
{
    s_GeometryVisible = show;

    Place( Matrix4::Identity );
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
#pragma TODO( "Implement this once drag-n-drop is testable (CanvasStrip is implemented)" )
    HELIUM_BREAK();
}

void EntityInstanceCreateTool::OnDeleteClass( const Inspect::ButtonClickedArgs& args )
{
    const std::set< size_t >& selectedItemIndices = m_RandomEntityList->a_SelectedItemIndices.Get();

    std::set< size_t >::const_reverse_iterator itr = selectedItemIndices.rbegin();
    std::set< size_t >::const_reverse_iterator end = selectedItemIndices.rend();
    for ( ; itr != end; ++itr )
    {
        EntityRowInfo& entityInfo = m_RandomEntityInfo[ *itr ];
        if ( m_ClassPath.Hash() == entityInfo.m_ClassPath.Hash() )
        {
            m_ClassPath.Set( TXT( "" ) );
            Place( Matrix4::Identity );
        }

        m_RandomEntityInfo.erase( m_RandomEntityInfo.begin() + *itr );
    }

    m_Generator->GetContainer()->GetCanvas()->Read();

    RefreshInstance();
}

void EntityInstanceCreateTool::OnClear( const Inspect::ButtonClickedArgs& args )
{
    m_RandomEntityInfo.clear();

    m_FileButton->WriteStringData( TXT( "" ) );
    m_FileButtonAdd->WriteStringData( TXT( "" ) );

    m_Generator->GetContainer()->GetCanvas()->Read();

    RefreshInstance();
}

void EntityInstanceCreateTool::OnNormalize( const Inspect::ButtonClickedArgs& args )
{
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

    m_Generator->GetContainer()->GetCanvas()->Read();
}

void EntityInstanceCreateTool::OnModify( const Inspect::ButtonClickedArgs& args )
{
    HELIUM_BREAK();
    m_Generator->GetContainer()->GetCanvas()->Read();
}
