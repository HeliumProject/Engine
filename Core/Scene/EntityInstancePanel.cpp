/*#include "Precompile.h"*/
#include "EntityInstancePanel.h"
#include "EntitySet.h"

#include "Application/Inspect/Controls/InspectValue.h"
#include "Application/Inspect/Controls/InspectButton.h"

#include "Core/Asset/AssetInit.h"
#include "Foundation/Component/ComponentHandle.h"
#include "Core/Asset/AssetClass.h"
#include "Foundation/Log.h"
#include "Platform/Process.h"
#include "Volume.h"

#include "Light.h"

#include "Core/Scene/Scene.h"
#include "Core/Scene/SceneManager.h"

using namespace Helium;
using namespace Helium::Reflect;
using namespace Helium::Asset;
using namespace Helium::Core;

EntityPanel::EntityPanel(PropertiesGenerator* generator, const OS_SelectableDumbPtr& selection)
: InstancePanel (generator, selection)
, m_EntityPath( NULL )
{
    m_Expanded = true;
    m_Name = TXT( "Entity" );

    m_Generator->PushContainer();
    {
        m_Generator->AddLabel( TXT( "Class Path" ) );

        m_EntityPath = m_Generator->AddValue<Core::Entity, tstring>( m_Selection, &Core::Entity::GetEntityAssetPath, &Core::Entity::SetEntityAssetPath );
        m_EntityPath->AddBoundDataChangingListener( Inspect::ChangingSignature::Delegate ( this, &EntityPanel::OnEntityAssetChanging ) );
        m_EntityPath->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &EntityPanel::OnEntityAssetChanged ) );

        Inspect::FileDialogButton* fileButton = m_Generator->AddFileDialogButton<Core::Entity, tstring>( m_Selection, &Core::Entity::GetEntityAssetPath, &Core::Entity::SetEntityAssetPath );
        fileButton->AddBoundDataChangingListener( Inspect::ChangingSignature::Delegate ( this, &EntityPanel::OnEntityAssetChanging ) );
        fileButton->AddBoundDataChangedListener( Inspect::ChangedSignature::Delegate ( this, &EntityPanel::OnEntityAssetChanged ) );

        Inspect::FileBrowserButton* browserButton = m_Generator->AddFileBrowserButton<Core::Entity, tstring>( m_Selection, &Core::Entity::GetEntityAssetPath, &Core::Entity::SetEntityAssetPath );
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

#ifdef INSPECT_REFACTOR
        Inspect::FilteredDropTarget* filteredDropTarget = new Inspect::FilteredDropTarget( filter );
        filteredDropTarget->AddDroppedListener( Inspect::FilteredDropTargetSignature::Delegate( this, &EntityPanel::OnEntityAssetDrop ) );
        m_EntityPath->SetDropTarget( filteredDropTarget );
#endif
    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    {
        m_Generator->AddLabel( TXT( "Class Buttons" ) );

        Inspect::Button* refreshButton = m_Generator->AddButton( Inspect::ButtonSignature::Delegate( this, &EntityPanel::OnEntityAssetRefresh ) );
        refreshButton->SetIcon( TXT( "actions/view-refresh" ) );
        refreshButton->SetToolTip( TXT( "Refresh" ) );

        bool singular = m_Selection.Size() == 1;

        Inspect::Button* lunaButton = m_Generator->AddButton( Inspect::ButtonSignature::Delegate( this, &EntityPanel::OnEntityAssetEditAsset ) );
        lunaButton->SetIcon( TXT( "asset_editor" ) );
        lunaButton->SetToolTip( TXT( "Edit this entity class in Editor's Asset Editor" ) );

        Inspect::Button* mayaButton = m_Generator->AddButton( Inspect::ButtonSignature::Delegate( this, &EntityPanel::OnEntityAssetEditArt ) );
        mayaButton->SetIcon( TXT( "maya" ) );
        mayaButton->SetEnabled( singular );
        mayaButton->SetToolTip( TXT( "Edit this entity class's art in Maya" ) );
    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    {
        m_Generator->AddLabel( TXT( "Show Pointer" ) );
        m_Generator->AddCheckBox<Core::Entity, bool>( m_Selection, 
            &Core::Entity::IsPointerVisible, 
            &Core::Entity::SetPointerVisible, false );
    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    {
        m_Generator->AddLabel( TXT( "Show Bounds" ) );
        m_Generator->AddCheckBox<Core::Entity, bool>( m_Selection, 
            &Core::Entity::IsBoundsVisible, 
            &Core::Entity::SetBoundsVisible, false );
    }
    m_Generator->Pop();

    m_Generator->PushContainer();
    {
        m_Generator->AddLabel( TXT( "Show Geometry" ) );
        m_Generator->AddCheckBox<Core::Entity, bool>( m_Selection, 
            &Core::Entity::IsGeometryVisible, 
            &Core::Entity::SetGeometryVisible, false );
    }
    m_Generator->Pop();
}

///////////////////////////////////////////////////////////////////////////////
// Validation function for the controls that change the Entity Class field.
// Returns true if the new value for the Entity Class field can be resolved to
// a file TUID.
//
bool EntityPanel::OnEntityAssetChanging( const Inspect::ControlChangingArgs& args )
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

void EntityPanel::OnEntityAssetChanged( const Inspect::ControlChangedArgs& args )
{
}

void EntityPanel::OnEntityAssetRefresh( Inspect::Button* button )
{
    Core::Scene* scene = NULL;

    // when we refresh, reload the common class set information in case
    // we did something like reexport an art class, while luna is still opened
    std::set< EntitySet* > reloadQueue;     // entities we want to reload

    std::set< tstring > files;

    OS_SelectableDumbPtr::Iterator selectionIter = m_Selection.Begin();
    OS_SelectableDumbPtr::Iterator selectionEnd = m_Selection.End();
    for (; selectionIter != selectionEnd; ++selectionIter )
    {
        Core::Entity* entity = Reflect::ObjectCast< Core::Entity >( *selectionIter );

        if ( !scene )
        {
            Core::SceneNode* node = Reflect::ObjectCast< Core::SceneNode >( *selectionIter );
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
                Core::Scene* nestedScene = entity->GetNestedScene(entity->GetOwner()->GetViewport()->GetGeometryMode());

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

    std::set< EntitySet* >::iterator itr = reloadQueue.begin();
    std::set< EntitySet* >::iterator end = reloadQueue.end();

    while( itr != end )
    {
        EntitySet* entClassSet = ObjectCast< EntitySet >( *itr );

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
        Core::Entity* entity = Reflect::ObjectCast< Core::Entity >( *selectionIter );
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
        m_EntityPath->WriteStringData( args.m_Paths[ 0 ] );
    }
}