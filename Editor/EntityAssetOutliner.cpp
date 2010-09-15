#include "Precompile.h"
#include "EntityAssetOutliner.h"

#include "Editor/ArtProvider.h"

#include "Core/Scene/EntityInstance.h"
#include "Core/Scene/EntityInstanceType.h"
#include "Core/Scene/EntitySet.h"
#include "Core/Scene/Scene.h"
#include "Editor/Controls/Tree/SortTreeCtrl.h"

using namespace Helium;
using namespace Helium::Editor;
using namespace Helium::Core;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
EntityAssetOutliner::EntityAssetOutliner( Core::SceneManager* sceneManager )
: SceneOutliner( sceneManager )
, m_InvisibleRoot( NULL )
{
    m_DisplayCounts = true; 
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
EntityAssetOutliner::~EntityAssetOutliner()
{
}

///////////////////////////////////////////////////////////////////////////////
// If a valid scene is specified, iterates over all the nodes in the scene
// and adds all the entity class sets to the tree.
// 
void EntityAssetOutliner::AddEntityTypes()
{
    if ( m_CurrentScene )
    {
        if ( m_CurrentScene->GetNodeTypesByName().size() > 0 )
        {
            m_TreeCtrl->Freeze();
            bool isSortingEnabled = m_TreeCtrl->IsSortingEnabled();
            m_TreeCtrl->DisableSorting();

            // Iterate over the node types, looking for the entity types
            EntityType* entityType = NULL;
            HM_StrToSceneNodeTypeSmartPtr::const_iterator typeItr = m_CurrentScene->GetNodeTypesByName().begin();
            HM_StrToSceneNodeTypeSmartPtr::const_iterator typeEnd = m_CurrentScene->GetNodeTypesByName().end();
            for ( ; typeItr != typeEnd; ++typeItr )
            {
                entityType = Reflect::ObjectCast< Core::EntityType >( typeItr->second );
                if ( entityType )
                {
                    AddEntityType( entityType );
                }
            }

            m_TreeCtrl->EnableSorting( isSortingEnabled );
            Sort( m_InvisibleRoot );
            m_TreeCtrl->Thaw();
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Hooks up listeners to the entity type for changes and attempts to add any
// existing entity class sets to the tree.
// 
void EntityAssetOutliner::AddEntityType( Core::EntityType* entityType )
{
    // Iterate over all the entity instances and add them to the tree
    M_InstanceSetSmartPtr::const_iterator classItr = entityType->GetSets().begin();
    M_InstanceSetSmartPtr::const_iterator classEnd = entityType->GetSets().end();
    for ( ; classItr != classEnd; ++classItr )
    {
        EntitySet* set = Reflect::ObjectCast< Core::EntitySet >( classItr->second );
        if (set)
        {
            AddEntitySet( set );
        }
    }

    // Connect listeners
    entityType->AddSetAddedListener( InstanceTypeChangeSignature::Delegate ( this, &EntityAssetOutliner::SetAdded ) );
    entityType->AddSetRemovedListener( InstanceTypeChangeSignature::Delegate ( this, &EntityAssetOutliner::SetRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Unhooks listeners to the entity type and attempts to remove any existing
// entity class sets from the tree.
// 
void EntityAssetOutliner::RemoveEntityType( Core::EntityType* entityType )
{
    // Iterate over all the entity instances and add them to the tree
    M_InstanceSetSmartPtr::const_iterator classItr = entityType->GetSets().begin();
    M_InstanceSetSmartPtr::const_iterator classEnd = entityType->GetSets().end();
    for ( ; classItr != classEnd; ++classItr )
    {
        EntitySet* set = Reflect::ObjectCast< Core::EntitySet >( classItr->second );
        if (set)
        {
            RemoveEntitySet( set );
        }
    }
    // Disconnect listeners
    entityType->RemoveSetAddedListener( InstanceTypeChangeSignature::Delegate ( this, &EntityAssetOutliner::SetAdded ) );
    entityType->RemoveSetRemovedListener( InstanceTypeChangeSignature::Delegate ( this, &EntityAssetOutliner::SetRemoved ) );
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified entity class set to the tree (including the instances
// that belong to the set).
// 
void EntityAssetOutliner::AddEntitySet( Core::EntitySet* classSet )
{
    if ( m_CurrentScene )
    {
        m_TreeCtrl->Freeze();
        bool isSortingEnabled = m_TreeCtrl->IsSortingEnabled();
        m_TreeCtrl->DisableSorting();

        const i32 image = GlobalFileIconsTable().GetIconID( TXT( "folder" ) );
        wxTreeItemId classItem = AddItem( m_InvisibleRoot, 
            classSet->GetName(), 
            image, 
            new SceneOutlinerItemData( classSet ), 
            false, 
            false);

        // Connect listeners
        classSet->AddInstanceAddedListener( InstanceSetChangeSignature::Delegate ( this, &EntityAssetOutliner::EntityAdded ) );
        classSet->AddInstanceRemovedListener( InstanceSetChangeSignature::Delegate ( this, &EntityAssetOutliner::EntityRemoved ) );

        // Add all existing instances
        S_InstanceDumbPtr::const_iterator entityItr = classSet->GetInstances().begin();
        S_InstanceDumbPtr::const_iterator entityEnd = classSet->GetInstances().end();
        for ( ; entityItr != entityEnd; ++entityItr )
        {
            AddEntityInstance( Reflect::AssertCast< Core::EntityInstance >(*entityItr) );
        }

        m_TreeCtrl->EnableSorting( isSortingEnabled );
        m_TreeCtrl->SortChildren( classItem );
        m_TreeCtrl->SortChildren( m_InvisibleRoot );
        m_TreeCtrl->Thaw();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Removes the entity class set from the tree (all instances of this set should
// have already been removed).
// 
void EntityAssetOutliner::RemoveEntitySet( Core::EntitySet* classSet )
{
    M_TreeItems::const_iterator found = m_Items.find( classSet );
    if ( found != m_Items.end() )
    {
        // All instances of this class set should have already been cleared out
        HELIUM_ASSERT( !m_TreeCtrl->HasChildren( found->second ) );

        // Disconnect listeners
        classSet->RemoveInstanceAddedListener( InstanceSetChangeSignature::Delegate ( this, &EntityAssetOutliner::EntityAdded ) );
        classSet->RemoveInstanceRemovedListener( InstanceSetChangeSignature::Delegate ( this, &EntityAssetOutliner::EntityRemoved ) );

        // Actually delete the item
        DeleteItem( classSet );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Adds the specified entity to the tree.  The entity will be parented under
// its class set.
// 
void EntityAssetOutliner::AddEntityInstance( Core::EntityInstance* entityInstance )
{
    EDITOR_SCOPE_TIMER( ("") );

    // Find the tree item that corresponds to the class set that this entity
    // belongs to.
    HELIUM_ASSERT( entityInstance->GetClassSet() );
    M_TreeItems::const_iterator foundSet = m_Items.find( entityInstance->GetClassSet() );
    HELIUM_ASSERT( foundSet != m_Items.end() );
    if ( foundSet != m_Items.end() )
    {
        m_TreeCtrl->Freeze();

        // Add the entity as a child of the class set
        const wxTreeItemId& parent = foundSet->second;
        wxTreeItemId insertedItem = AddItem( parent, entityInstance->GetName(), entityInstance->GetImageIndex(), new SceneOutlinerItemData( entityInstance ), entityInstance->IsSelected() );
        m_TreeCtrl->Thaw();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Removes the specified entity instance from the tree.
// 
void EntityAssetOutliner::RemoveEntityInstance( Core::EntityInstance* entityInstance )
{
    EDITOR_SCOPE_TIMER( ("") );

    // Remove the item from the tree
    DeleteItem( entityInstance );
}

///////////////////////////////////////////////////////////////////////////////
// Creates the tree control managed by this class.  This function should only
// be called once.  The tree control is returned so that the caller can
// insert it into the UI in the proper place.
// 
SortTreeCtrl* EntityAssetOutliner::CreateTreeCtrl( wxWindow* parent, wxWindowID id )
{
    SortTreeCtrl* tree = new SortTreeCtrl( parent, id, wxDefaultPosition, wxDefaultSize, wxTR_DEFAULT_STYLE | wxNO_BORDER | wxTR_HIDE_ROOT | wxTR_EDIT_LABELS | wxTR_MULTIPLE, wxDefaultValidator, wxT( "EntityAssetOutliner" ) );
    m_InvisibleRoot = tree->AddRoot( TXT( "INVISIBLE_ROOT" ) );

    // Override dynamic GUI event handlers here
    tree->Connect( tree->GetId(), wxEVT_COMMAND_TREE_BEGIN_LABEL_EDIT, wxTreeEventHandler( EntityAssetOutliner::OnBeginLabelEdit ), NULL, this );

    return tree;
}

///////////////////////////////////////////////////////////////////////////////
// Clears out the tree.
// 
void EntityAssetOutliner::Clear()
{
    __super::Clear();

    m_TreeCtrl->DeleteChildren( m_InvisibleRoot );
}

///////////////////////////////////////////////////////////////////////////////
// Called when the base class has finished changing the current scene.  Loads
// the entity class sets into the tree control.
// 
void EntityAssetOutliner::CurrentSceneChanged( Core::Scene* oldScene )
{
    AddEntityTypes();
}

///////////////////////////////////////////////////////////////////////////////
// Connects all the required event listeners to the current scene.
// 
void EntityAssetOutliner::ConnectSceneListeners()
{
    __super::ConnectSceneListeners();

    if ( m_CurrentScene )
    {
        // Connect listeners
        m_CurrentScene->AddNodeTypeAddedListener( NodeTypeExistenceSignature::Delegate ( this, &EntityAssetOutliner::NodeTypeAdded ) );
        m_CurrentScene->AddNodeTypeRemovedListener( NodeTypeExistenceSignature::Delegate ( this, &EntityAssetOutliner::NodeTypeRemoved ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Disconnects all the registered event listeners from this class on the current
// scene.
// 
void EntityAssetOutliner::DisconnectSceneListeners()
{
    if ( m_CurrentScene )
    {
        // Disconnect listeners
        m_CurrentScene->RemoveNodeTypeAddedListener( NodeTypeExistenceSignature::Delegate ( this, &EntityAssetOutliner::NodeTypeAdded ) );
        m_CurrentScene->RemoveNodeTypeRemovedListener( NodeTypeExistenceSignature::Delegate ( this, &EntityAssetOutliner::NodeTypeRemoved ) );
    }

    __super::DisconnectSceneListeners();
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an entity set is added to an entity type.  If the set is
// an entity class set, it will be added to the tree.
// 
void EntityAssetOutliner::SetAdded( const Core::InstanceTypeChangeArgs& args )
{
    if ( args.m_InstanceSet->HasType( Reflect::GetType< Core::EntitySet >() ) )
    {
        AddEntitySet( Reflect::DangerousCast< Core::EntitySet >( args.m_InstanceSet ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an entity set is removed from an entity type.  If the set
// is an entity class set, it will be removed from the tree.
// 
void EntityAssetOutliner::SetRemoved( const Core::InstanceTypeChangeArgs& args )
{
    if ( args.m_InstanceSet->HasType( Reflect::GetType< Core::EntitySet >() ) )
    {
        RemoveEntitySet( Reflect::DangerousCast< Core::EntitySet >( args.m_InstanceSet ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an entity is added to the scene.  Updates the tree control.
// 
void EntityAssetOutliner::EntityAdded( const Core::InstanceSetChangeArgs& args )
{
    AddEntityInstance( Reflect::AssertCast< Core::EntityInstance >(args.m_Instance) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when an entity is removed from the scene.  Updates the tree 
// control.
// 
void EntityAssetOutliner::EntityRemoved( const Core::InstanceSetChangeArgs& args )
{
    RemoveEntityInstance( Reflect::AssertCast< Core::EntityInstance >(args.m_Instance) );
}

///////////////////////////////////////////////////////////////////////////////
// Callback when a node type is added to the scene.  If the node type is an
// Editor::EntityType, this class starts tracking it.
// 
void EntityAssetOutliner::NodeTypeAdded( const Core::NodeTypeExistenceArgs& args )
{
    if ( args.m_NodeType->HasType( Reflect::GetType< Core::EntityType >() ) )
    {
        AddEntityType( Reflect::DangerousCast< Core::EntityType >( args.m_NodeType ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Callback when a node type is removed from the scene.  If the node type is
// an Editor::EntityType, disconnect from it.
// 
void EntityAssetOutliner::NodeTypeRemoved( const Core::NodeTypeExistenceArgs& args )
{
    if ( args.m_NodeType->HasType( Reflect::GetType< Core::EntityType >() ) )
    {
        RemoveEntityType( Reflect::DangerousCast< Core::EntityType >( args.m_NodeType ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// GUI callback for when the user tries to edit an item in the tree.  Vetos the
// event if the item is not allowed to be renamed.
// 
void EntityAssetOutliner::OnBeginLabelEdit( wxTreeEvent& args )
{
    Reflect::Object* found = GetTreeItemData( args.GetItem() )->GetObject();

    // If a valid Object was not found, or if the the object is not
    // an entity node, we won't allow it's name to be changed.
    if ( !found || !found->HasType( Reflect::GetType< Core::EntityInstance >() ) )
    {
        args.Veto();
    }
}
