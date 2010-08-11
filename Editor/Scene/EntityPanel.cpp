#include "Precompile.h"
#include "EntityPanel.h"
#include "EntityAssetSet.h"

#include "Editor/App.h"

#include "Application/Inspect/Controls/Value.h"
#include "Application/Inspect/Controls/InspectButton.h"

#include "Core/Asset/AssetInit.h"
#include "Foundation/Component/ComponentHandle.h"
#include "Core/Asset/AssetClass.h"
#include "Foundation/Log.h"
#include "Platform/Process.h"
#include "Volume.h"

#include "Light.h"

#include "Editor/Scene/Scene.h"
#include "Editor/Scene/SceneManager.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Asset;
using namespace Helium::Editor;

template <class T>
bool SelectionHasAttribute(const OS_SelectableDumbPtr& selection)
{
    OS_SelectableDumbPtr::Iterator itr = selection.Begin();
    OS_SelectableDumbPtr::Iterator end = selection.End();
    for ( ; itr != end; ++itr )
    {
        Editor::SceneNode* node = Reflect::ObjectCast< Editor::SceneNode >( *itr );

        if (!node)
        {
            return false;
        }

        Content::SceneNode* packageNode = node->GetPackage<Content::SceneNode>();

        if (!packageNode)
        {
            return false;
        }

        Component::ComponentViewer<T> attr ( packageNode );

        if (!attr.Valid())
        {
            return false;
        }
    }

    return true;
}

template <class T>
bool SelectionHasSameAttribute(const OS_SelectableDumbPtr& selection, Component::ComponentViewer< T >& attribute)
{
    OS_SelectableDumbPtr::Iterator itr = selection.Begin();
    OS_SelectableDumbPtr::Iterator end = selection.End();
    for ( ; itr != end; ++itr )
    {
        Editor::SceneNode* node = Reflect::ObjectCast< Editor::SceneNode >( *itr );

        if (!node)
        {
            return false;
        }

        Content::SceneNode* packageNode = node->GetPackage<Content::SceneNode>();

        if (!packageNode)
        {
            return false;
        }

        if ( itr == selection.Begin() )
        {
            attribute.Viewport( packageNode );
            if ( !attribute.Valid() )
            {
                return false;
            }
        }
        else
        {
            Component::ComponentViewer< T > nextAttribute = Component::ComponentViewer< T >( packageNode );
            if ( !nextAttribute.Valid() || ( attribute.operator->() != nextAttribute.operator->() ) )
            {
                return false;
            }
        }
    }

    return true;
}

EntityPanel::EntityPanel(PropertiesGenerator* generator, const OS_SelectableDumbPtr& selection)
: InstancePanel (generator, selection)
, m_LightingPanel ( NULL )
, m_DrawDistance ( NULL )
, m_UpdateDistance ( NULL )
, m_ShaderGroup ( NULL )
, m_Attenuation( NULL )
, m_FarShadowFadeout( NULL )
, m_CastsBakedShadows( NULL )
, m_DoBakedShadowCollisions( NULL )
, m_BakedShadowAABBExt ( NULL )
, m_BakedShadowMergeGroups( NULL )
, m_HighResShadowMap( NULL )
, m_SegmentEnabler ( NULL )
, m_ShaderGroupEnabler ( NULL )
, m_FarShadowFadeoutEnabler( NULL )
, m_CastsBakedShadowsEnabler( NULL )
, m_DoBakedShadowCollisionsEnabler( NULL )
, m_BakedShadowAABBExtEnabler( NULL )
, m_BakedShadowMergeGroupsEnabler( NULL )
, m_HighResShadowMapEnabler( NULL )
{
    m_Expanded = true;
    m_Text = TXT( "Entity" );

    OS_SelectableDumbPtr::Iterator itr = m_Selection.Begin();
    OS_SelectableDumbPtr::Iterator end = m_Selection.End();
    for ( ; itr != end; ++itr )
    {
        Editor::Entity* entity = Reflect::AssertCast< Editor::Entity >( *itr );
        if ( entity && entity->GetClassSet() )
        {
            entity->GetClassSet()->AddClassLoadedListener( EntityAssetSetChangeSignature::Delegate ( this, &EntityPanel::EntityAssetReloaded ) );
            m_Entities.push_back( entity );
        }
    }

    m_SegmentEnabler = new ControlEnabler ();
    m_ShaderGroupEnabler = new ControlEnabler ();
    m_FarShadowFadeoutEnabler = new ControlEnabler ();
    m_CastsBakedShadowsEnabler = new ControlEnabler ();
    m_DoBakedShadowCollisionsEnabler = new ControlEnabler ();
    m_BakedShadowAABBExtEnabler = new ControlEnabler ();
    m_BakedShadowMergeGroupsEnabler = new ControlEnabler ();
    m_HighResShadowMapEnabler = new ControlEnabler ();
}

EntityPanel::~EntityPanel()
{
    V_EntitySmartPtr::const_iterator itr = m_Entities.begin();
    V_EntitySmartPtr::const_iterator end = m_Entities.end();
    for ( ; itr != end; ++itr )
    {
        Editor::Entity* entity = *itr;
        if ( entity->GetClassSet() )
        {
            entity->GetClassSet()->RemoveClassLoadedListener( EntityAssetSetChangeSignature::Delegate ( this, &EntityPanel::EntityAssetReloaded ) );
        }
    }

    delete m_SegmentEnabler;
    delete m_ShaderGroupEnabler;
    delete m_FarShadowFadeoutEnabler;
    delete m_CastsBakedShadowsEnabler;
    delete m_DoBakedShadowCollisionsEnabler;
    delete m_BakedShadowAABBExtEnabler;
    delete m_BakedShadowMergeGroupsEnabler;
    delete m_HighResShadowMapEnabler;
}

void EntityPanel::CreateClassPath()
{
    m_Generator->PushContainer();
    {
        m_Generator->AddLabel( TXT( "Class Path" ) );

        m_TextBox = m_Generator->AddValue<Editor::Entity, tstring>( m_Selection, &Editor::Entity::GetEntityAssetPath, &Editor::Entity::SetEntityAssetPath );
        m_TextBox->AddBoundDataChangingListener( Inspect::ChangingSignature::Delegate ( this, &EntityPanel::OnEntityAssetChanging ) );
        m_TextBox->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &EntityPanel::OnEntityAssetChanged ) );

        Inspect::FileDialogButton* fileButton = m_Generator->AddFileDialogButton<Editor::Entity, tstring>( m_Selection, &Editor::Entity::GetEntityAssetPath, &Editor::Entity::SetEntityAssetPath );
        fileButton->AddBoundDataChangingListener( Inspect::ChangingSignature::Delegate ( this, &EntityPanel::OnEntityAssetChanging ) );
        fileButton->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &EntityPanel::OnEntityAssetChanged ) );

        Inspect::FileBrowserButton* browserButton = m_Generator->AddFileBrowserButton<Editor::Entity, tstring>( m_Selection, &Editor::Entity::GetEntityAssetPath, &Editor::Entity::SetEntityAssetPath );
        browserButton->AddBoundDataChangingListener( Inspect::ChangingSignature::Delegate ( this, &EntityPanel::OnEntityAssetChanging ) );
        browserButton->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &EntityPanel::OnEntityAssetChanged ) );

        tstring filter;
        Reflect::GetClass<Asset::Entity>()->GetProperty( Asset::AssetProperties::FileFilter, filter );

        if ( !filter.empty() )
        {
            fileButton->SetFilter( filter );
            browserButton->SetFilter( filter );
        }
        else
        {
            // There's a problem, better disable the button
            fileButton->SetEnabled( false );
            browserButton->SetEnabled( false );
        }

        Inspect::FilteredDropTarget* filteredDropTarget = new Inspect::FilteredDropTarget( filter );
        filteredDropTarget->AddDroppedListener( Inspect::FilteredDropTargetSignature::Delegate( this, &EntityPanel::OnEntityAssetDrop ) );
        m_TextBox->SetDropTarget( filteredDropTarget );
    }
    m_Generator->Pop();

}

void EntityPanel::CreateClassActions()
{
    m_Generator->PushContainer();
    {
        m_Generator->AddLabel( TXT( "Class Actions" ) );

        Inspect::Action* refreshButton = m_Generator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnEntityAssetRefresh ) );
        refreshButton->SetIcon( TXT( "actions/view-refresh" ) );
        refreshButton->SetToolTip( TXT( "Refresh" ) );

        bool singular = m_Selection.Size() == 1;

        Inspect::Action* lunaButton = m_Generator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnEntityAssetEditAsset ) );
        lunaButton->SetIcon( TXT( "asset_editor" ) );
        lunaButton->SetToolTip( TXT( "Edit this entity class in Editor's Asset Editor" ) );

        Inspect::Action* mayaButton = m_Generator->AddAction( Inspect::ActionSignature::Delegate( this, &EntityPanel::OnEntityAssetEditArt ) );
        mayaButton->SetIcon( TXT( "maya" ) );
        mayaButton->SetEnabled( singular );
        mayaButton->SetToolTip( TXT( "Edit this entity class's art in Maya" ) );
    }
    m_Generator->Pop();
}

void EntityPanel::CreateShowFlags()
{
    m_Generator->PushContainer();
    {
        m_Generator->AddLabel( TXT( "Show Pointer" ) );
        m_Generator->AddCheckBox<Editor::Entity, bool>( m_Selection, 
            &Editor::Entity::IsPointerVisible, 
            &Editor::Entity::SetPointerVisible, false );
    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    {
        m_Generator->AddLabel( TXT( "Show Bounds" ) );
        m_Generator->AddCheckBox<Editor::Entity, bool>( m_Selection, 
            &Editor::Entity::IsBoundsVisible, 
            &Editor::Entity::SetBoundsVisible, false );
    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    {
        m_Generator->AddLabel( TXT( "Show Geometry" ) );
        m_Generator->AddCheckBox<Editor::Entity, bool>( m_Selection, 
            &Editor::Entity::IsGeometryVisible, 
            &Editor::Entity::SetGeometryVisible, false );
    }
    m_Generator->Pop();
}



void EntityPanel::Create()
{
    CreateClassPath(); 
    CreateClassActions(); 

    CreateShowFlags(); 
    CreateAppearanceFlags();

    Inspect::Panel::Create();
}


///////////////////////////////////////////////////////////////////////////////
// Validation function for the controls that change the Entity Class field.
// Returns true if the new value for the Entity Class field can be resolved to
// a file TUID.
//
bool EntityPanel::OnEntityAssetChanging( const Inspect::ChangingArgs& args )
{
    bool result = false;

    tstring newValue;
    Reflect::Serializer::GetValue(args.m_NewValue, newValue);

    Helium::Path path( newValue );
    if ( path.Exists() )
    {
        // Make sure the file has a reflect extension
        std::set< tstring > extensions;
        Reflect::Archive::GetExtensions( extensions );
        if ( extensions.find( path.Extension() ) != extensions.end() )
        {
            result = true;
        }
    }

    if ( !result )
    {
        // Message to the user that the value is not correct.
        wxMessageBox( TXT( "Invalid Entity Class specified!" ), TXT( "Error" ), wxOK | wxCENTER | wxICON_ERROR, GetWindow() );
    }

    return result;
}

void EntityPanel::OnEntityAssetChanged( const Inspect::ChangeArgs& args )
{
}

void EntityPanel::OnEntityAssetRefresh( Inspect::Button* button )
{
    Editor::Scene* scene = NULL;

    // when we refresh, reload the common class set information in case
    // we did something like reexport an art class, while luna is still opened
    std::set< EntityAssetSet* > reloadQueue;     // entities we want to reload

    std::set< tstring > files;

    OS_SelectableDumbPtr::Iterator selectionIter = m_Selection.Begin();
    OS_SelectableDumbPtr::Iterator selectionEnd = m_Selection.End();
    for (; selectionIter != selectionEnd; ++selectionIter )
    {
        Editor::Entity* entity = Reflect::ObjectCast< Editor::Entity >( *selectionIter );

        if ( !scene )
        {
            Editor::SceneNode* node = Reflect::ObjectCast< Editor::SceneNode >( *selectionIter );
            scene = node->GetOwner();
        }

        if( entity->GetClassSet( ) )
        {
            reloadQueue.insert( entity->GetClassSet( ) );
        }

        if (entity->IsGeometryVisible())
        {
            for ( int i=0; i<GeometryModes::Count; i++ )
            {
                Editor::Scene* nestedScene = entity->GetNestedScene(entity->GetOwner()->GetViewport()->GetGeometryMode());

                if (nestedScene)
                {
                    if ( files.insert( nestedScene->GetPath().Get() ).second )
                    {
                        // Only reload the scene if we haven't already done so during this iteration.
                        nestedScene->Reload();
                    }
                }
            }
        }
    }

    std::set< EntityAssetSet* >::iterator itr = reloadQueue.begin();
    std::set< EntityAssetSet* >::iterator end = reloadQueue.end();

    while( itr != end )
    {
        EntityAssetSet* entClassSet = ObjectCast< EntityAssetSet >( *itr );

        if( entClassSet )
        {
            entClassSet->LoadAssetClass( );
        }

        itr++;
    }

    if (scene)
    {
        scene->Execute(false);
    }
}

void EntityPanel::OnEntityAssetEditAsset( Inspect::Button* button )
{
    std::set< tstring > files;
    OS_SelectableDumbPtr::Iterator selectionIter = m_Selection.Begin();
    OS_SelectableDumbPtr::Iterator selectionEnd = m_Selection.End();
    for ( ; selectionIter != selectionEnd; ++selectionIter )
    {
        Editor::Entity* entity = Reflect::ObjectCast< Editor::Entity >( *selectionIter );
        if ( entity )
        {
            tstring fileToEdit = entity->GetEntityAssetPath();
            if ( !fileToEdit.empty() )
            {
                files.insert( fileToEdit );
            }
        }
    }

    std::set< tstring >::const_iterator fileItr = files.begin();
    std::set< tstring >::const_iterator fileEnd = files.end();
    for ( ; fileItr != fileEnd; ++fileItr )
    {
#pragma TODO( "Open the file for edit" )
        HELIUM_BREAK();
    }
}

void EntityPanel::OnEntityAssetEditArt( Inspect::Button* button )
{
}

void EntityPanel::OnEntityAssetDrop( const Inspect::FilteredDropTargetArgs& args )
{
    if ( args.m_Paths.size() )
    {
        m_TextBox->WriteData( args.m_Paths[ 0 ] );
    }
}


///////////////////////////////////////////////////////////////////////////////
// Callback for when one of the selected entity class sets is reloaed. 
// 
void EntityPanel::EntityAssetReloaded( const EntityAssetSetChangeArgs& args )
{
    Read();
}

