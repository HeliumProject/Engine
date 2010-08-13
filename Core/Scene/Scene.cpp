/*#include "Precompile.h"*/
#include "Core/Scene/Scene.h"

#include <algorithm>
#include <hash_set>
#include <cctype>

#include "Foundation/Container/Insert.h" 
#include "Foundation/Reflect/ArchiveXML.h"
#include "Foundation/Reflect/Version.h"
#include "Foundation/Log.h"

#include "Application/Preferences.h"
#include "Core/Asset/Classes/Entity.h"
#include "Core/Asset/Manifests/SceneManifest.h"

#include "Core/Content/ContentScene.h"
#include "Core/Content/ContentVersion.h"
#include "Core/Content/Nodes/ContentJointTransform.h"
#include "Core/Content/Nodes/ContentPivotTransform.h"
#include "Core/Content/Nodes/ContentMesh.h"
#include "Core/Content/Nodes/ContentShader.h"
#include "Core/Content/Nodes/ContentSkin.h"
#include "Core/Content/Nodes/ContentCurve.h"
#include "Core/Content/Nodes/ContentVolume.h"

#include "Application/Inspect/Data/Data.h"
#include "Application/Inspect/Controls/Canvas.h"
#include "Application/Undo/PropertyCommand.h"
#include "SceneGraph.h"
#include "Statistics.h"

#include "Core/Scene/SceneNodeType.h"
#include "HierarchyNodeType.h"

#include "ScenePreferences.h"

#include "ParentCommand.h"

#include "PivotTransform.h"
#include "JointTransform.h"
#include "Layer.h"
#include "Core/Scene/Mesh.h"
#include "Shader.h"
#include "Skin.h"
#include "Curve.h"
#include "Core/Scene/Point.h"
#include "InstanceType.h"
#include "EntityInstance.h"
#include "EntityInstanceType.h"
#include "EntitySet.h"
#include "Volume.h"
#include "Locator.h"
#include "Light.h"
#include "SpotLight.h"
#include "PointLight.h"
#include "DirectionalLight.h"
#include "AmbientLight.h"
#include "Foundation/String/Utilities.h"
#include "Foundation/Math/AngleAxis.h"

#include "Foundation/Log.h"

#define snprintf _snprintf

using namespace Helium;
using namespace Helium::Math;
using namespace Helium::Core;

Scene::Scene( Core::Viewport* viewport, const Helium::Path& path )
: m_Path( path )
, m_Id( TUID::Generate() )
, m_Progress( 0 )
, m_Importing( false )
, m_View( viewport )
, m_SmartDuplicateMatrix(Math::Matrix4::Identity)
, m_ValidSmartDuplicateMatrix( false )
, m_Color( 255 )
, m_IsFocused( false )
{
    LoadVisibility(); 

    // Mark the scene as needing to be saved when a command is added to the undo stack
    m_UndoQueue.AddCommandPushedListener( Undo::QueueChangeSignature::Delegate ( this, &Scene::UndoQueueCommandPushed ) );
    m_UndoQueue.AddUndoingListener( Undo::QueueChangingSignature::Delegate ( this, &Scene::UndoingOrRedoing ) );
    m_UndoQueue.AddRedoingListener( Undo::QueueChangingSignature::Delegate ( this, &Scene::UndoingOrRedoing ) );

    // This event delegate will cause the scene to execute and render a frame to effect the visual outcome of a selection change
    m_Selection.AddChangingListener( SelectionChangingSignature::Delegate (this, &Scene::SelectionChanging) );
    m_Selection.AddChangedListener( SelectionChangedSignature::Delegate (this, &Scene::SelectionChanged) );

    // Evaluation
    m_Graph = new SceneGraph();

    // Setup root node
    m_Root = new Core::PivotTransform( this );
    m_Root->SetName( TXT( "Root" ) );
    m_Root->Evaluate( GraphDirections::Downstream );
    m_Graph->AddNode( m_Root.Ptr() );

    // All imports should default to the master root
    m_ImportRoot = m_Root.Ptr();

    // we listen to our own events because there a couple ways to add nodes and get this event
    AddNodeAddedListener( NodeChangeSignature::Delegate(this, &Scene::OnSceneNodeAdded )); 
    AddNodeRemovedListener( NodeChangeSignature::Delegate(this, &Scene::OnSceneNodeRemoved )); 
#if SCENE_REFACTOR
    wxGetApp().GetPreferences()->GetViewportPreferences()->AddChangedListener( Reflect::ElementChangeSignature::Delegate( this, &Scene::ViewPreferencesChanged ) );
#endif
}

Scene::~Scene()
{
#if SCENE_REFACTOR
    wxGetApp().GetPreferences()->GetViewportPreferences()->RemoveChangedListener( Reflect::ElementChangeSignature::Delegate( this, &Scene::ViewPreferencesChanged ) );
#endif

    // remove our own listeners 
    RemoveNodeAddedListener( NodeChangeSignature::Delegate(this, &Scene::OnSceneNodeAdded )); 
    RemoveNodeRemovedListener( NodeChangeSignature::Delegate(this, &Scene::OnSceneNodeRemoved )); 

    // remove undo listener
    m_UndoQueue.RemoveCommandPushedListener( Undo::QueueChangeSignature::Delegate ( this, &Scene::UndoQueueCommandPushed ) );
    m_UndoQueue.RemoveUndoingListener( Undo::QueueChangingSignature::Delegate ( this, &Scene::UndoingOrRedoing ) );
    m_UndoQueue.RemoveRedoingListener( Undo::QueueChangingSignature::Delegate ( this, &Scene::UndoingOrRedoing ) );

    // remove selection listener
    m_Selection.RemoveChangingListener( SelectionChangingSignature::Delegate (this, &Scene::SelectionChanging) );
    m_Selection.RemoveChangedListener( SelectionChangedSignature::Delegate (this, &Scene::SelectionChanged) );

    Reset();
}

bool Scene::IsEditable()
{
    if ( m_EditingDelegate.Invoke( SceneEditingArgs( this ) ) )
    {
        return true;
    }

    return false;
}

const Helium::Path& Scene::GetPath() const
{
    return m_Path;
}

const LToolPtr& Scene::GetTool()
{
    return m_Tool;
}

void Scene::SetTool(const LToolPtr& tool)
{
    if (m_Tool.ReferencesObject())
    {
        // clean up predecessor
        m_Tool->Cleanup();
    }

    // reset
    m_Tool = NULL;

    if (tool && tool->Initialize())
    {
        // set the tool
        m_Tool = tool;
    }

    // connect it to the view
    m_View->SetTool(m_Tool);

    // cancel pick, if any
    if (m_PickData.ReferencesObject())
    {
        m_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Picking, SceneContexts::Normal ) );
        m_PickData = NULL;
    }
}

const Math::Color3& Scene::GetColor() const
{
    return m_Color;
}

void Scene::SetColor( const Math::Color3& color )
{
    if ( m_Color != color )
    {
        m_Color = color;
    }
}

bool Scene::Reload()
{
    Reset();

    return Load( m_Path );
}

bool Scene::Load( const Helium::Path& path )
{
    if ( !m_Nodes.empty() )
    {
        HELIUM_BREAK();
        // Shouldn't happen
        Log::Error( TXT( "Scene '%s' is not empty!  You should not be trying to Load '%s'.  Do an Import instead.\n" ), m_Path.c_str(), path.c_str() );
        return false;
    }

    return Import( path, ImportActions::Load, NULL ).ReferencesObject();
}

Undo::CommandPtr Scene::Import( const Helium::Path& path, ImportAction action, u32 importFlags, Core::HierarchyNode* importRoot, i32 importReflectType )
{
    CORE_SCOPE_TIMER( ( "%s", path.c_str() ) );

    if ( action != ImportActions::Load )
    {
        if( !IsEditable() )
        {
            return NULL;
        }
    }

    if ( path.empty() )
    {
        return NULL;
    }

    m_LoadStarted.Raise( LoadArgs( this ) );

    if ( !importRoot )
    {
        importRoot = GetRoot();
    }

    if ( action == ImportActions::Load )
    {
        m_Path = path;

        LoadVisibility(); 
    }

    // setup
    m_ImportRoot = importRoot;

    tostringstream str;
    str << "Loading File: " << path.c_str();
    m_StatusChanged.Raise( str.str() );

    // read data
    m_Progress = 0;
    Reflect::V_Element elements;

    bool success = true;


    Content::ScenePtr scene = new Content::Scene();

    try
    {
        scene->Load( path, elements, this );
    }
    catch ( const Helium::Exception& exception )
    {
        Log::Error( TXT( "%s\n" ), exception.What() );
        success = false;
    }

    Undo::CommandPtr command;

    if ( success )
    {
        // load and init nodes
        command = ImportSceneNodes( elements, action, importFlags, importReflectType );
    }

    m_ImportRoot = m_Root;
    m_LoadFinished.Raise( LoadArgs( this, success ) );

    return command;
}

Undo::CommandPtr Scene::ImportXML( const tstring& xml, u32 importFlags, Core::HierarchyNode* importRoot )
{
    CORE_SCOPE_TIMER( ("") );

    m_LoadStarted.Raise( LoadArgs( this ) );

    if ( xml.empty() )
    {
        return NULL;
    }

    if ( !importRoot )
    {
        importRoot = GetRoot();
    }

    // setup
    m_ImportRoot = importRoot;

    tostringstream str;
    str << "Parsing XML...";
    m_StatusChanged.Raise( str.str() );

    // read data
    m_Progress = 0;
    Reflect::V_Element elements;

    bool success = true;

    Content::ScenePtr scene = new Content::Scene();

    try
    {
        scene->LoadXML( xml, elements, this );
    }
    catch ( Helium::Exception& exception )
    {
        Log::Error( TXT( "%s\n" ), exception.What() );
        success = false;
    }

    Undo::CommandPtr command;

    if ( success )
    {
        // convert nodes
        command = ImportSceneNodes( elements, ImportActions::Import, importFlags );
    }

    m_ImportRoot = m_Root;
    m_LoadFinished.Raise( LoadArgs( this, success ) );

    return command;
}

SceneNodePtr Scene::CreateNode( Content::SceneNode* data )
{
    SceneNodePtr createdNode;

    if ( data->HasType( Reflect::GetType<Asset::Entity>() ) )
    {
        createdNode = new Core::Entity( this, Reflect::DangerousCast< Asset::EntityInstance >( data ) );
    }
    else if ( data->HasType( Reflect::GetType<Content::Volume>() ) )
    {
        createdNode = new Core::Volume( this, Reflect::DangerousCast< Content::Volume >( data ) );
    }
    else if ( data->HasType( Reflect::GetType<Content::Locator>() ) )
    {
        createdNode = new Core::Locator( this, Reflect::DangerousCast< Content::Locator >( data ) );
    }
    else if ( data->HasType( Reflect::GetType<Content::DirectionalLight>() ) )
    {
        createdNode = new Core::DirectionalLight( this, Reflect::DangerousCast< Content::DirectionalLight >( data ) );
    }
    else if ( data->HasType( Reflect::GetType<Content::SpotLight>() ) )
    {
        createdNode = new Core::SpotLight( this, Reflect::DangerousCast< Content::SpotLight >( data ) );
    }
    else if ( data->HasType( Reflect::GetType<Content::PointLight>() ) )
    {
        createdNode = new Core::PointLight( this, Reflect::DangerousCast< Content::PointLight >( data ) );
    }
    else if ( data->HasType( Reflect::GetType<Content::AmbientLight>() ) )
    {
        createdNode = new Core::AmbientLight( this, Reflect::DangerousCast< Content::AmbientLight >( data ) );
    }
    else if ( data->HasType( Reflect::GetType<Content::Shader>() ) )
    {
        createdNode = new Core::Shader( this, Reflect::DangerousCast< Content::Shader >( data ) );
    }
    else if ( data->HasType( Reflect::GetType<Content::Skin>() ) )
    {
        createdNode = new Core::Skin( this, Reflect::DangerousCast< Content::Skin >( data ) );
    }
    else if ( data->HasType( Reflect::GetType<Content::Mesh>() ) )
    {
        createdNode = new Core::Mesh( this, Reflect::DangerousCast< Content::Mesh >( data ) );
    }
    else if ( data->HasType( Reflect::GetType<Content::Layer>() ) )
    {
        createdNode = new Core::Layer( this, Reflect::DangerousCast< Content::Layer >( data ) );
    }
    else if ( data->HasType( Reflect::GetType<Content::Curve>() ) )
    {
        createdNode = new Core::Curve( this, Reflect::DangerousCast< Content::Curve >( data ) );
    }
    else if ( data->HasType( Reflect::GetType<Content::PivotTransform>() ) )
    {
        createdNode = new Core::PivotTransform( this, Reflect::DangerousCast< Content::PivotTransform >( data ) );
    }
    else if ( data->HasType( Reflect::GetType<Content::JointTransform>() ) )
    {
        createdNode = new Core::JointTransform( this, Reflect::DangerousCast< Content::JointTransform >( data ) );
    }
    else if ( data->HasType( Reflect::GetType<Content::Point>() ) )
    {
        createdNode = new Core::Point( this, Reflect::DangerousCast< Content::Point >( data ) );
    }

    if ( createdNode.ReferencesObject() )
    {
        // post-create logic
        createdNode->Unpack();
    }
    else
    {
        Log::Error( TXT( "Unable to create node for data of type '%s'.\n" ), data->GetClass()->m_UIName.c_str() );
        HELIUM_BREAK();
    }

    return createdNode;
}

void Scene::Reset()
{
    // Clear selection to free Properties API UI
    m_Selection.SetItems( OS_SelectableDumbPtr () );

    // clear Highlighted
    m_Highlighted.Clear();

    // deallocate the tool
    m_Tool = NULL;

    // Clear undo queue
    m_UndoQueue.Reset();

    // Clear name cache
    m_Names.clear();

    // Reclaim resource memory
    Delete();

    // Break down entire graph
    m_Graph->Reset();

    // Clear flat hash of nodes
    m_Nodes.clear();

    // Flush graph objects, frees all objects (for now)
    HM_StrToSceneNodeTypeSmartPtr::const_iterator tItr = m_NodeTypesByName.begin();
    HM_StrToSceneNodeTypeSmartPtr::const_iterator tEnd = m_NodeTypesByName.end();
    for ( ; tItr != tEnd; ++tItr )
    {
        SceneNodeTypePtr nodeType = tItr->second;
        nodeType->Reset();
    }
    m_NodeTypesByName.clear();
    m_NodeTypesByType.clear();

    // Reset root
    if ( m_Root.ReferencesObject() )
    {
        m_Root->Reset();
    }
}

Undo::CommandPtr Scene::ImportSceneNodes( Reflect::V_Element& elements, ImportAction action, u32 importFlags, i32 importReflectType )
{
    CORE_SCOPE_TIMER( ("") );

    u64 startTimer = Helium::TimerGetClock();

    // 
    // Initialize
    // 

    m_Importing = true;
    m_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Normal, SceneContexts::Loading ) );
    m_StatusChanged.Raise( tstring( TXT("Loading Objects") ) );

    m_RemappedIDs.clear();

    // 
    // Load Elements
    // 

    Undo::BatchCommandPtr command = new Undo::BatchCommand ();

    V_SceneNodeSmartPtr createdNodes;
    createdNodes.reserve( elements.size() );
    {
        Reflect::V_Element::const_iterator itr = elements.begin();
        Reflect::V_Element::const_iterator end = elements.end();
        for ( ; itr != end; ++itr )
        {
            command->Push( ImportSceneNode( *itr, createdNodes, action, importFlags, importReflectType ) );
        }
    }

    // 
    // Build Hierarchy
    // 

    {
        V_SceneNodeSmartPtr::iterator itr = createdNodes.begin();
        V_SceneNodeSmartPtr::iterator end = createdNodes.end();
        for ( ; itr != end; ++itr )
        {
            HierarchyNodePtr hierarchyNode = Reflect::ObjectCast<Core::HierarchyNode>(*itr);

            if (!hierarchyNode.ReferencesObject())
            {
                // its not a hierarchy node, just continue
                continue;
            }


            // 
            // Remap object id
            // 

            TUID childID = hierarchyNode->GetID();

            HM_TUID::const_iterator findChild = m_RemappedIDs.find( childID );
            if ( findChild != m_RemappedIDs.end() )
            {
                childID = findChild->second;
            }


            // 
            // Remap parent id
            // 

            TUID parentID = hierarchyNode->GetPackage<Content::HierarchyNode>()->m_ParentID;

            HM_TUID::const_iterator findParent = m_RemappedIDs.find( parentID );
            if ( findParent != m_RemappedIDs.end() )
            {
                parentID = findParent->second;
            }


            // 
            // Set Hierarchy
            // 

            Core::HierarchyNode* child = NULL;
            HM_SceneNodeDumbPtr::const_iterator foundChild = m_Nodes.find( childID );
            if ( foundChild != m_Nodes.end() )
            {
                child = Reflect::ObjectCast< Core::HierarchyNode >( foundChild->second );
            }

            if (child != NULL)
            {
                if (parentID != TUID::Null)
                {
                    Core::HierarchyNode* parent = NULL;

                    HM_SceneNodeDumbPtr::const_iterator foundParent = m_Nodes.find( parentID );
                    if ( foundParent != m_Nodes.end() )
                    {
                        parent = Reflect::ObjectCast< Core::HierarchyNode >( foundParent->second );
                    }

                    if (parent != NULL)
                    {
                        child->SetParent( parent );
                    }
                    else
                    {
                        child->SetParent( m_ImportRoot );
                    }
                }
                else
                {
                    child->SetParent( m_ImportRoot );
                }
            }
        }
    }

    // 
    // Process new data
    // 

    // evaluate the graph to build global transforms
    m_StatusChanged.Raise( tstring( TXT("Evaluating Objects...") ) );
    Evaluate(true);

    // initialize each object after initial evaluation is complete
    V_HierarchyNodeDumbPtr newNodes;
    m_StatusChanged.Raise( tstring( TXT("Initializing Objects...") ) );
    V_SceneNodeSmartPtr::const_iterator itr = createdNodes.begin();
    V_SceneNodeSmartPtr::const_iterator end = createdNodes.end();
    for ( ; itr != end; ++itr )
    {
        (*itr)->Initialize();

        if ( importFlags & ImportFlags::Select )
        {
            Core::HierarchyNode* node = Reflect::ObjectCast<Core::HierarchyNode>( *itr );
            if ( node )
            {
                newNodes.push_back( node );
            }
        }
    }

    // select imported if the flag is set
    if ( importFlags & ImportFlags::Select )
    {
        V_HierarchyNodeDumbPtr newParents;
        GetCommonParents( newNodes, newParents );

        if ( !newParents.empty() )
        {
            OS_SelectableDumbPtr newSelection;
            V_HierarchyNodeDumbPtr::const_iterator itr = newParents.begin();
            V_HierarchyNodeDumbPtr::const_iterator end = newParents.end();
            for ( ; itr != end; ++itr )
            {
                newSelection.Append( *itr );
            }

            command->Push( m_Selection.SetItems( newSelection ) );
        }
        else
        {
            HELIUM_BREAK();
        }
    }

    // cleanup
    m_RemappedIDs.clear();

    // raise node added events (our nodes are ready to go now)
    {
        V_SceneNodeSmartPtr::iterator itr = createdNodes.begin();
        V_SceneNodeSmartPtr::iterator end = createdNodes.end();
        for ( ; itr != end; ++itr )
        {
            m_NodeAdded.Raise( NodeChangeArgs( *itr ) );
        }
    }

    // report
    tostringstream str;
    str.precision( 2 );
    str << "Scene Loading Complete: " << std::fixed << Helium::CyclesToMillis( Helium::TimerGetClock() - startTimer ) / 1000.f << " seconds...";
    m_StatusChanged.Raise( str.str() );

    // done
    m_StatusChanged.Raise( tstring( TXT("Ready") ) );
    m_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Loading, SceneContexts::Normal ) );
    m_Importing = false;

    return command;
}

Undo::CommandPtr Scene::ImportSceneNode( const Reflect::ElementPtr& element, V_SceneNodeSmartPtr& createdNodes, ImportAction action, u32 importFlags, i32 importReflectType )
{
    CORE_SCOPE_TIMER( ("ImportSceneNode: %s", element->GetClass()->m_ShortName.c_str()) );

    SceneNodePtr createdNode = NULL;

    if( importReflectType == Reflect::ReservedTypes::Invalid )
        importReflectType = Reflect::GetType< Content::SceneNode >();

    // 
    // Merging Nodes / Duplicate ID check
    // 
    bool convertNode = action == ImportActions::Load;

    if ( action == ImportActions::Import )
    {
        if ( element->HasType( importReflectType ) )
        {
            Content::SceneNode* node = Reflect::DangerousCast< Content::SceneNode >( element );

            if ( ImportFlags::HasFlag( importFlags, ImportFlags::Merge ) )
            {
                HM_SceneNodeDumbPtr::const_iterator find = m_Nodes.find( node->m_ID );
                if ( find != m_Nodes.end() )
                {
                    Core::SceneNode* dependNode = find->second;
                    element->CopyTo( dependNode->GetPackage< Reflect::Element >() );
                    dependNode->Unpack();
                    dependNode->Dirty();
                }    
            }
            else
            {
                // Always generate a new ID when importing and not merging
                TUID id( TUID::Generate() );
                m_RemappedIDs.insert( HM_TUID::value_type( node->m_ID, id ) );
                node->m_ID = id;
                convertNode = true;
            }
        }
    }


    // 
    // Convert Node
    // 

    if ( convertNode )
    {
        if ( element->HasType( Reflect::GetType<Content::SceneNode>() ) )
        {
            createdNode = CreateNode( Reflect::DangerousCast< Content::SceneNode >( element ) );
        }

        if ( createdNode.ReferencesObject() )
        {
            // update ui
            tostringstream str;
            str << TXT( "Loading: " ) + createdNode->GetName();
            m_StatusChanged.Raise( str.str() );

            // save it in the list of created nodes
            createdNodes.push_back( createdNode );

            // add object to the scene
            return new SceneNodeExistenceCommand( Undo::ExistenceActions::Add, this, createdNode );
        }
    }

    return NULL;
}

void Scene::ArchiveStatus(Reflect::StatusInfo& info)
{
    switch (info.m_Action)
    {
    case Reflect::ArchiveStates::ArchiveStarting:
        {
            tstring verb = info.m_Archive.GetMode() == Reflect::ArchiveModes::Read ? TXT( "Opening" ) : TXT( "Saving" );
            tstring type = info.m_Archive.GetType() == Reflect::ArchiveTypes::XML ? TXT( "XML" ) : TXT( "Binary" );

            tostringstream str;
            str << verb << " " << type << " File: " << info.m_Archive.GetPath();
            m_StatusChanged.Raise( str.str() );
            break;
        }

    case Reflect::ArchiveStates::ElementProcessed:
        {
            if (info.m_Progress > m_Progress)
            {
                m_Progress = info.m_Progress;

                {
                    tstring verb = info.m_Archive.GetMode() == Reflect::ArchiveModes::Read ? TXT( "Opening" ) : TXT( "Saving" );

                    tostringstream str;
                    str << verb << ": " << info.m_Archive.GetPath() << " (" << m_Progress << "%)";
                    m_StatusChanged.Raise( str.str() );
                }
            }

            break;
        }

    case Reflect::ArchiveStates::Complete:
        {
            tstring verb = info.m_Archive.GetMode() == Reflect::ArchiveModes::Read ? TXT( "Opening" ) : TXT( "Saving" );

            tostringstream str;
            str << "Completed " << verb << ": " << info.m_Archive.GetPath();
            m_StatusChanged.Raise( str.str() );
            break;
        }

    case Reflect::ArchiveStates::PostProcessing:
        {
            tostringstream str;
            str << "Processing: " << info.m_Archive.GetPath();
            m_StatusChanged.Raise( str.str() );
            break;
        }
    }
}

void Scene::ArchiveException(Reflect::ExceptionInfo& info)
{
#pragma TODO( "Sub default assets?" )
}

bool Scene::Export( Reflect::V_Element& elements, const ExportArgs& args, Undo::BatchCommand* changes )
{
    bool result = true;

    {
        // before exporting anything build the manifest object that will live at the front of the file
        Asset::SceneManifestPtr manifest = new Asset::SceneManifest ();

        // the root node's global bounds will contain the extents of the scene
        manifest->m_BoundingBoxMin = m_Root->GetGlobalHierarchyBounds().minimum;
        manifest->m_BoundingBoxMax = m_Root->GetGlobalHierarchyBounds().maximum;

        // iterate over the types' containers
        HMS_TypeToSceneNodeTypeDumbPtr::const_iterator typesItr = m_NodeTypesByType.begin();
        HMS_TypeToSceneNodeTypeDumbPtr::const_iterator typesEnd = m_NodeTypesByType.end();
        for ( ; typesItr != typesEnd; ++typesItr )
        {
            // iterate over every instance of the type
            S_SceneNodeTypeDumbPtr::const_iterator typeItr = typesItr->second.begin();
            S_SceneNodeTypeDumbPtr::const_iterator typeEnd = typesItr->second.end();
            for ( ; typeItr != typeEnd; ++typeItr )
            {
                (*typeItr)->PopulateManifest( manifest );
            }
        }

        elements.push_back( manifest );
    }

    // ID's of the objects that have been exported so far (to prevent exporting dupes)
    S_TUID exported; 

    if ( ExportFlags::HasFlag( args.m_Flags, ExportFlags::SelectedNodes ) )
    {
        // Walk through the selection list and export each dependency node
        if ( m_Selection.GetItems().Size() > 0 )
        {
            OS_SelectableDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
            OS_SelectableDumbPtr::Iterator end = m_Selection.GetItems().End();
            for ( ; itr != end; ++itr )
            {
                Core::SceneNode* node = Reflect::ObjectCast< Core::SceneNode >( *itr );
                if ( node )
                {
                    Core::HierarchyNode* hierarchyNode = Reflect::ObjectCast< Core::HierarchyNode >( node );

                    if ( hierarchyNode && ExportFlags::HasFlag( args.m_Flags, ExportFlags::MaintainHierarchy ) )
                    {
                        ExportHierarchyNode( hierarchyNode, elements, exported, args, changes );
                    }
                    else
                    {
                        if ( args.m_Bounds.empty() )
                        {
                            ExportSceneNode( node, elements, exported, args, changes );
                        }
                    }
                }
            }
        }
        else
        {
            Log::Warning( TXT( "Nothing is selected, there is nothing to export\n" ) );
        }
    }
    else
    {
        // Walk through all the dependency nodes, adding each one to the list of elements
        if ( m_Nodes.size() > 0 )
        {
            // Export the entire hierarchy, in order
            OS_HierarchyNodeDumbPtr::Iterator childItr = m_Root->GetChildren().Begin();
            OS_HierarchyNodeDumbPtr::Iterator childEnd = m_Root->GetChildren().End();
            for ( ; childItr != childEnd; ++childItr )
            {
                ExportHierarchyNode( *childItr, elements, exported, args, changes, true );
            }

            if ( args.m_Bounds.empty() )
            {
                // Export the rest of the dependency nodes
                HM_SceneNodeDumbPtr::const_iterator itr = m_Nodes.begin();
                HM_SceneNodeDumbPtr::const_iterator end = m_Nodes.end();
                for ( ; itr != end; ++itr )
                {
                    ExportSceneNode( itr->second, elements, exported, args, changes );
                }
            }
        }
        else
        {
            Log::Warning( TXT( "Scene is empty, there's nothing to save!\n" ) );
        }
    }

    return result;
}

void Scene::ExportSceneNode( Core::SceneNode* node, Reflect::V_Element& elements, S_TUID& exported, const ExportArgs& args, Undo::BatchCommand* changes )
{
    // Don't export the root node
    if ( node != m_Root )
    {
        // Don't export a node if it has already been exported
        if ( exported.find( node->GetID() ) == exported.end() && !node->IsTransient() )
        {
            if ( node->HasType( Reflect::GetType< Core::Transform >() ) && !ExportFlags::HasFlag( args.m_Flags, ExportFlags::MaintainHierarchy ) )
            {
                // If we are exporting a transform node, but we are not maintaining the hierarchy,
                // we would still like to maintain the object's global position.  So, we create
                // reparent the node under the root (which maintains the node's position) and
                // store that command so that it can be undone after export completes.
                Core::Transform* transformNode = Reflect::DangerousCast< Core::Transform >( node );
                changes->Push( new ParentCommand( transformNode, GetRoot() ) );
            }

            node->Pack();
            elements.push_back( node->GetPackage() );
            exported.insert( node->GetID() );

            // Flag to indicate whether to recursively export ancestors of this node or not
            if ( ExportFlags::HasFlag( args.m_Flags, ExportFlags::MaintainDependencies ) )
            {
                S_SceneNodeDumbPtr::const_iterator ancestorItr = node->GetAncestors().begin();
                S_SceneNodeDumbPtr::const_iterator ancestorEnd = node->GetAncestors().end();
                for ( ; ancestorItr != ancestorEnd; ++ancestorItr )
                {
                    Core::SceneNode* ancestor = *ancestorItr;

                    bool skipAncestor = false;

                    // Check to see if the ancestor is a hierarchy node
                    if ( node->HasType( Reflect::GetType<Core::HierarchyNode>() ) )
                    {
                        Core::HierarchyNode* hierarchyNode = Reflect::DangerousCast< Core::HierarchyNode >( node );
                        if ( hierarchyNode->GetParent() && hierarchyNode->GetParent() == ancestor )
                        {
                            // Hierarchy is exported separately, so ignore this ancestor if it is a parent of this node
                            skipAncestor = true;
                        }
                    }

                    if ( !skipAncestor )
                    {
                        ExportSceneNode( ancestor, elements, exported, args, changes );
                    }
                }
            }
        }
    }
}

void Scene::ExportHierarchyNode( Core::HierarchyNode* node, Reflect::V_Element& elements, S_TUID& exported, const ExportArgs& args, Undo::BatchCommand* changes, bool exportChildren )
{
    // Export parents first
    if ( node->GetParent() != m_Root )
    {
        ExportHierarchyNode( node->GetParent(), elements, exported, args, changes, false );
    }

    // If this child has not been exported yet (and is not transient)
    if ( exported.find( node->GetID() ) == exported.end() )
    {
        // Transient nodes and their children will not be exported
        if ( !node->IsTransient() )
        {
            bool proceed = true;

            if ( !args.m_Bounds.empty() )
            {
                proceed = false;

                for ( V_AlignedBox::const_iterator itr = args.m_Bounds.begin(), end = args.m_Bounds.end(); itr != end && !proceed; ++itr )
                {
                    proceed = itr->IntersectsBox( node->GetGlobalBounds() );
                }
            }

            if ( proceed )
            {
                // Do the export work for this node
                ExportSceneNode( node, elements, exported, args, changes );
            }

            if ( exportChildren )
            {
                // Export all the children of the specified node, recursively.
                OS_HierarchyNodeDumbPtr::Iterator childItr = node->GetChildren().Begin();
                OS_HierarchyNodeDumbPtr::Iterator childEnd = node->GetChildren().End();
                for ( ; childItr != childEnd; ++childItr )
                {
                    ExportHierarchyNode( *childItr, elements, exported, args, changes, true );
                }
            }
        }
    }
}

bool Scene::Save()
{
    HELIUM_ASSERT( !m_Path.empty() );
    return Export( m_Path, ExportFlags::Default );
}

bool Scene::Export( const Helium::Path& path, const ExportArgs& args )
{
    u64 startTimer = Helium::TimerGetClock();

    bool result = false;

    m_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Normal, SceneContexts::Saving ) );

    m_Progress = 0;

    {
        tostringstream str;
        str << "Preparing to save: " << path.c_str();
        m_StatusChanged.Raise( str.str() );
    }

    Undo::BatchCommandPtr changes = new Undo::BatchCommand();

    Reflect::V_Element spool;
    result = Export( spool, args, changes );

    if (result)
    {
        try
        {
            Reflect::Archive::ToFile( spool, path.Get(), new Content::ContentVersion (), this );
        }
        catch ( Helium::Exception& ex )
        {
            tostringstream str;
            str << "Failed to write file " << path.c_str() << ": " << ex.What();
            wxMessageBox( str.str(), TXT( "Error" ), wxOK|wxCENTRE|wxICON_ERROR );
            result = false;
        }
    }

    changes->Undo();

    m_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Saving, SceneContexts::Normal ) );

    {
        tostringstream str;
        str.precision( 2 );
        str << "Saving Complete: " << std::fixed << Helium::CyclesToMillis( Helium::TimerGetClock() - startTimer ) / 1000.f << " seconds...";
        m_StatusChanged.Raise( str.str() );
    }

    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Exports (based upon the export arguments) either the whole scene, or the
// selected items.  The exported items are written into the xml parameter that
// is passed into this function.
// 
bool Scene::ExportXML( tstring& xml, const ExportArgs& args )
{
    CORE_SCOPE_TIMER( ("") );

    bool result = false;

    u64 startTimer = Helium::TimerGetClock();

    m_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Normal, SceneContexts::Saving ) );

    m_Progress = 0;

    {
        tostringstream str;
        str << "Preparing to export";
        m_StatusChanged.Raise( str.str() );
    }

    Undo::BatchCommandPtr changes = new Undo::BatchCommand();

    Reflect::V_Element spool;
    result = Export( spool, args, changes );

    if ( result && !spool.empty() )
    {
        try
        {
            Reflect::ArchiveXML::ToString( spool, xml, this );
        }
        catch ( Helium::Exception& ex )
        {
            tostringstream str;
            str << "Failed to generate xml: " << ex.What();
            wxMessageBox( str.str(), TXT( "Error" ), wxOK|wxCENTRE|wxICON_ERROR );
            result = false;
        }
    }

    changes->Undo();

    m_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Saving, SceneContexts::Normal ) );

    {
        tostringstream str;
        str.precision( 2 );
        str << "Export Complete: " << std::fixed << Helium::CyclesToMillis( Helium::TimerGetClock() - startTimer ) / 1000.f << " seconds...";
        m_StatusChanged.Raise( str.str() );
    }

    return result;
}

int Scene::Split( tstring& outName )
{
    int ret = -1;

    tstring name = outName.c_str();

    size_t lastNum = name.size();
    while (lastNum > 0 && isdigit(name[lastNum-1]))
    {
        lastNum--;
    }

    if ( lastNum < 0 || lastNum >= name.size() )
    {
        // we have no digit to split on
    }
    else
    {
        tstring numberString = name.substr(lastNum);

        // trim name
        outName = name.substr(0, lastNum);

        ret = _tstoi(numberString.c_str());
    }

    return ret;
}

void Scene::SetName( Core::SceneNode* sceneNode, const tstring& newName )
{
    // lua keywords
    static stdext::hash_set<tstring, NameHasher> keywords;
    if (keywords.empty())
    {
        keywords.insert( TXT( "and" ) );
        keywords.insert( TXT( "break" ) );
        keywords.insert( TXT( "do" ) );
        keywords.insert( TXT( "else" ) );
        keywords.insert( TXT( "elseif" ) );
        keywords.insert( TXT( "end" ) );
        keywords.insert( TXT( "false" ) );
        keywords.insert( TXT( "for" ) );
        keywords.insert( TXT( "function" ) );
        keywords.insert( TXT( "if" ) );
        keywords.insert( TXT( "in" ) );
        keywords.insert( TXT( "local" ) );
        keywords.insert( TXT( "nil" ) );
        keywords.insert( TXT( "not" ) );
        keywords.insert( TXT( "or" ) );
        keywords.insert( TXT( "repeat" ) );
        keywords.insert( TXT( "return" ) );
        keywords.insert( TXT( "then" ) );
        keywords.insert( TXT( "true" ) );
        keywords.insert( TXT( "until" ) );
        keywords.insert( TXT( "while" ) );
    }

    tstring realName = newName;

    // handle the no-name case
    if (realName.empty() || keywords.find(realName) != keywords.end())
    {
        realName = sceneNode->GenerateName();
    }

    // handle invalid name (starts with numeral)
    while ( !realName.empty() && isdigit(*realName.begin()) )
    {
        realName.erase(0, 1);
    }

    // handle invalid name (contains invalid characters)
    bool inSpace = false;
    tstring::iterator itr = realName.begin();
    tstring::iterator end = realName.end();
    while ( itr != end )
    {
        if ( !isdigit(*itr) && !isalpha(*itr) && *itr != '_' )
        {
            // check for spaced region
            inSpace = *itr == ' ';

            // erase the invalid character
            itr = realName.erase( itr );
            end = realName.end();

            // we are at the next character
            continue;
        }

        if ( inSpace )
        {
            // camelCase
            *itr = toupper(*itr);

            // stop space
            inSpace = false;
        }

        ++itr;
    }

    // check to see if this name is unique
    HM_NameToSceneNodeDumbPtr::iterator foundName = m_Names.find( realName );

    // if this name is used *BY ANOTHER OBJECT*
    if ( foundName != m_Names.end() && foundName->second != sceneNode )
    {
        // split the name and number apart
        int number = Split( realName );

        // why would this happen?
        if ( number < 0 )
        {
            number = 0;
        }

        // the result of numeric uniquification
        tstring result;

        // do finds while we haven't found a unique numeric version
        HM_NameToSceneNodeDumbPtr::const_iterator searchItr = m_Names.end();
        HM_NameToSceneNodeDumbPtr::const_iterator searchEnd = searchItr;
        do
        {
            // extract the number to ascii
            number++;
            tostringstream numberStr;
            numberStr << number;

            // build the new name to try
            result = realName + numberStr.str();

            // see if its taken
            searchItr = m_Names.find( result );
        }
        while ( searchItr != searchEnd ); // continue until we find a name not taken

        // set the numeric result
        realName = result;
    }

    // set our new name into the object
    sceneNode->SetName( realName );

    // mark it taken
    Helium::Insert<HM_NameToSceneNodeDumbPtr>::Result inserted = m_Names.insert( HM_NameToSceneNodeDumbPtr::value_type( realName, sceneNode ) );
    bool previouslyInserted = sceneNode == inserted.first->second && sceneNode->GetName() == realName;
    bool newlyInserted = inserted.second;
    HELIUM_ASSERT( previouslyInserted || newlyInserted );
}

void Scene::Rename( Core::SceneNode* sceneNode, const tstring& newName, tstring oldName )
{
    if ( oldName.empty() )
    {
        oldName = sceneNode->GetName();
    }

    // special case the root
    if ( sceneNode == m_Root.Ptr() )
    {
        // roots NEVER change name
        sceneNode->Rename( oldName );
    }
    else
    {
        // find our name
        HM_NameToSceneNodeDumbPtr::iterator foundName = m_Names.find( oldName );

        // if we found it, *AND ITS OUR OBJECT*
        if ( foundName != m_Names.end() && foundName->second == sceneNode )
        {
            // erase it
            m_Names.erase( oldName );
        }

        // check it for uniqueness and set it
        SetName( sceneNode, newName );
    }
}

void Scene::AddNodeType(const SceneNodeTypePtr& nodeType)
{
    // insert into the map by name
    m_NodeTypesByName.insert( HM_StrToSceneNodeTypeSmartPtr::value_type( nodeType->GetName(), nodeType ) );

    // insert into the map by compile time type id
    Helium::Insert<HMS_TypeToSceneNodeTypeDumbPtr>::Result typeSet = m_NodeTypesByType.insert( HMS_TypeToSceneNodeTypeDumbPtr::value_type( nodeType->GetInstanceType(), S_SceneNodeTypeDumbPtr() ) );
    typeSet.first->second.insert( nodeType );

    // insert it into the types for its bases
    const Reflect::Class* type = Reflect::Registry::GetInstance()->GetClass( nodeType->GetInstanceType() );
    for ( ; type->m_TypeID != Reflect::GetType<Core::SceneNode>(); type = Reflect::Registry::GetInstance()->GetClass( type->m_Base ) )
    {
        Helium::Insert<HMS_TypeToSceneNodeTypeDumbPtr>::Result baseTypeSet = m_NodeTypesByType.insert( HMS_TypeToSceneNodeTypeDumbPtr::value_type( type->m_TypeID, S_SceneNodeTypeDumbPtr() ) );
        baseTypeSet.first->second.insert( nodeType );
    }

    m_NodeTypeCreated.Raise( nodeType.Ptr() );
}

void Scene::RemoveNodeType(const SceneNodeTypePtr& nodeType)
{
    // erase from the map by name
    m_NodeTypesByName.erase( nodeType->GetName() );

    // erase from the map by compile time type id
    HMS_TypeToSceneNodeTypeDumbPtr::iterator typeSet = m_NodeTypesByType.find( nodeType->GetInstanceType() );
    typeSet->second.erase( nodeType );

    // we can conditional here to save time becase if our exact type set is not empty some other type stil must exist in the base class sets
    //  otherwise if we are the last type removed then we must check each base type to see if we are the last base type to be removed
    if (typeSet->second.empty())
    {
        m_NodeTypesByType.erase( typeSet );

        const Reflect::Class* type = Reflect::Registry::GetInstance()->GetClass( nodeType->GetInstanceType() );
        for ( ; type->m_TypeID != Reflect::GetType<Core::SceneNode>(); type = Reflect::Registry::GetInstance()->GetClass( type->m_Base ) )
        {
            HMS_TypeToSceneNodeTypeDumbPtr::iterator baseTypeSet = m_NodeTypesByType.find( type->m_TypeID );

            // don't remove it until we are the last one, each base type is probably shared with other derivations
            if (baseTypeSet->second.empty())
            {
                m_NodeTypesByType.erase( baseTypeSet );
            }
        }
    }

    m_NodeTypeDeleted.Raise( nodeType.Ptr() );
}

void Scene::AddObject( const SceneNodePtr& node )
{
    CORE_SCOPE_TIMER( ("") );

    ClearHighlight( ClearHighlightArgs (false) );

    // (re)insert node into graph, this restores deleted downstream graph connections
    V_SceneNodeDumbPtr insertedNodes;
    insertedNodes.push_back( node );
    node->Insert( m_Graph, insertedNodes );

    V_SceneNodeDumbPtr::const_iterator itr = insertedNodes.begin();
    V_SceneNodeDumbPtr::const_iterator end = insertedNodes.end();
    for ( ; itr != end; ++itr )
    {
        AddSceneNode( *itr );
    }
}

void Scene::RemoveObject( const SceneNodePtr& node )
{
    CORE_SCOPE_TIMER( ("") );

    ClearHighlight( ClearHighlightArgs (false) );

    // prune node branch from the DG and Hierarchy
    V_SceneNodeDumbPtr prunedNodes;
    node->Prune( prunedNodes );
    prunedNodes.push_back( node );

    V_SceneNodeDumbPtr::const_iterator itr = prunedNodes.begin();
    V_SceneNodeDumbPtr::const_iterator end = prunedNodes.end();
    for ( ; itr != end; ++itr )
    {
        RemoveSceneNode( *itr );
    }
}

void Scene::AddSceneNode( const SceneNodePtr& node )
{
    {
        CORE_SCOPE_TIMER( ("Insert in node list") );

        // this would be bad
        HELIUM_ASSERT( node->GetID() != TUID::Null );

        Helium::Insert<HM_SceneNodeDumbPtr>::Result inserted = m_Nodes.insert( HM_SceneNodeDumbPtr::value_type( node->GetID(), node ) );
        HELIUM_ASSERT( inserted.first->second == node );
        if ( !inserted.second )
        {
            Log::Error( TXT( "Attempted to add a node with the same ID as one that already exists - %s [" ) TUID_HEX_FORMAT TXT( "].\n" ), node->GetName().c_str(), node->GetID() );
            HELIUM_BREAK();
        }
    }

    {

        Core::SceneNodeType* t = node->GetNodeType();
        if ( t == NULL )
        {
            CORE_SCOPE_TIMER( ("Deduce node type") );
            t = node->DeduceNodeType();
        }

        {
            CORE_SCOPE_TIMER( ("Add instance") );
            t->AddInstance(node);
        }
    }


    {
        CORE_SCOPE_TIMER( ("Set name") );

        // check name
        SetName( node, node->GetName() );
    }

    {
        CORE_SCOPE_TIMER( ("Create") );
        // (re)creates disposable resources in object
        node->Create();
    }

    {
        CORE_SCOPE_TIMER( ("Hierarchy check") );
        if ( node->HasType( Reflect::GetType<Core::HierarchyNode>() ) )
        {
            Core::HierarchyNode* hierarchyNode = Reflect::DangerousCast< Core::HierarchyNode >( node );

            if (hierarchyNode->GetParent() == NULL)
            {
                hierarchyNode->SetParent(m_Root);
            }
        }
    }

    {
        CORE_SCOPE_TIMER( ("Raise events if not transient") );
        if ( !node->IsTransient() && !m_Importing )
        {
            m_NodeAdded.Raise( NodeChangeArgs( node.Ptr() ) );
        }
    }
}

void Scene::RemoveSceneNode( const SceneNodePtr& node )
{
    CORE_SCOPE_TIMER( ("") );

    if ( !node->IsTransient() )
    {
        m_NodeRemoving.Raise( NodeChangeArgs( node.Ptr() ) );
    }

    // remove shortcuts to node and children
    m_Nodes.erase( node->GetID() );

    Core::SceneNodeType* t = node->GetNodeType();
    if ( t == NULL )
    {
        t = node->DeduceNodeType();
    }

    t->RemoveInstance( node );

    // cleanup name
    m_Names.erase( node->GetName() );

    // destroys disposable resources in object
    node->Delete();

    if ( !node->IsTransient() )
    {
        m_NodeRemoved.Raise( NodeChangeArgs( node.Ptr() ) );
    }
}

void Scene::OnSceneNodeAdded( const NodeChangeArgs& args )
{
    m_VisibilityDB->ActivateNode( args.m_Node->GetID() ); 
}

void Scene::OnSceneNodeRemoved( const NodeChangeArgs& args )
{
    m_VisibilityDB->DeactivateNode( args.m_Node->GetID() ); 
}

void Scene::Evaluate(bool silent)
{
    CORE_EVALUATE_SCOPE_TIMER( ("") );

    Core::EvaluateResult result = m_Graph->EvaluateGraph(silent);

    Statistics* stats = m_View->GetStatistics();
    stats->m_EvaluateTime += result.m_TotalTime;
    stats->m_NodeCount += result.m_NodeCount;
}

void Scene::Execute(bool interactively)
{
    CORE_EVALUATE_SCOPE_TIMER( ("") );

    // update data
    Evaluate();

#if SCENE_REFACTOR
    // invalidate the view
    m_View->Refresh();

    if (interactively)
    {
        // paint 3d view
        m_View->Update();
    }
#endif

    m_Executed.Raise( ExecuteArgs (this, interactively) );
}

void Scene::Create()
{
    for each ( HM_StrToSceneNodeTypeSmartPtr::value_type valType in m_NodeTypesByName )
    {
        const SceneNodeTypePtr& t = valType.second;
        if ( t->HasType( Reflect::GetType<Core::HierarchyNodeType>() ) )
        {
            Core::HierarchyNodeType* h = Reflect::DangerousCast< Core::HierarchyNodeType >( t );
            h->Create();
        }
    }

    for each ( HM_SceneNodeDumbPtr::value_type valType in m_Nodes )
    {
        Core::SceneNode* n = valType.second;
        n->Create();
    }
}

void Scene::Delete()
{
    for each ( HM_StrToSceneNodeTypeSmartPtr::value_type dependNodeType in m_NodeTypesByName )
    {
        const SceneNodeTypePtr& t = dependNodeType.second;
        if ( t->HasType( Reflect::GetType<Core::HierarchyNodeType>() ) )
        {
            Core::HierarchyNodeType* h = Reflect::DangerousCast< Core::HierarchyNodeType >( t );

            h->Delete();
        }
    }

    for each ( HM_SceneNodeDumbPtr::value_type dependNode in m_Nodes )
    {
        Core::SceneNode* n = dependNode.second;
        n->Delete();
    }
}

void Scene::Render( RenderVisitor* render ) const
{
    {
        CORE_RENDER_SCOPE_TIMER( ("") );

        HierarchyRenderTraverser renderTraverser ( render );

        m_Root->TraverseHierarchy( &renderTraverser );
    }
}

bool Scene::Pick( PickVisitor* pick ) const
{
    size_t hitCount = pick->GetHits().size();

    {
        CORE_SCOPE_TIMER( ("") );

        HierarchyPickTraverser pickTraverser ( pick );

        m_Root->TraverseHierarchy ( &pickTraverser );
    }

    return pick->GetHits().size() > hitCount;
}

void Scene::Select( const SelectArgs& args )
{
    CORE_SCOPE_TIMER( ("") );

    ClearHighlight( ClearHighlightArgs (false) );

    bool result = Pick( args.m_Pick );

    OS_SelectableDumbPtr selection;

    switch (args.m_Target)
    {
    case SelectionTargetModes::Single:
        {
            V_PickHitSmartPtr sorted;
            PickHit::Sort( m_View->GetCamera(), args.m_Pick->GetHits(), sorted, PickSortTypes::Intersection);

            V_PickHitSmartPtr::const_iterator itr = sorted.begin();
            V_PickHitSmartPtr::const_iterator end = sorted.end();
            for ( ; itr != end; ++itr )
            {
                Selectable* selectable = Reflect::ObjectCast<Selectable>((*itr)->GetObject());
                if (selectable)
                {
                    // add it to the new selection list
                    selection.Append(selectable);
                    break;
                }
            }
            break;
        }

    case SelectionTargetModes::Multiple:
        {
            V_PickHitSmartPtr::const_iterator itr = args.m_Pick->GetHits().begin();
            V_PickHitSmartPtr::const_iterator end = args.m_Pick->GetHits().end();
            for ( ; itr != end; ++itr )
            {
                Selectable* selectable = Reflect::ObjectCast<Selectable>((*itr)->GetObject());
                if (selectable)
                {
                    // add it to the new selection list
                    selection.Append(selectable);
                }
            }
            break;
        }
    }

    if (m_Tool && !m_Tool->ValidateSelection (selection))
    {
        return;
    }

    switch(args.m_Mode)
    {
    case SelectionModes::Replace:
        {
            Push( m_Selection.SetItems(selection) );
            break;
        }

    case SelectionModes::Add:
        {
            Push( m_Selection.AddItems(selection) );
            break;
        }

    case SelectionModes::Remove:
        {
            Push( m_Selection.RemoveItems (selection) );
            break;
        }

    case SelectionModes::Toggle:
        {
            OS_SelectableDumbPtr newSelection = m_Selection.GetItems();
            OS_SelectableDumbPtr::Iterator itr = selection.Begin();
            OS_SelectableDumbPtr::Iterator end = selection.End();
            for ( ; itr != end; ++itr )
            {
                if ( (*itr)->IsSelected() )
                {
                    newSelection.Remove( *itr );
                }
                else
                {
                    newSelection.Append( *itr );
                }
            }

            Push( m_Selection.SetItems(newSelection) );
        }
    }
}

void Scene::PickLink( const Inspect::PickLinkArgs& args )
{
    m_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Normal, SceneContexts::Picking ) );
    m_PickData = args.m_Data;
}

void Scene::SelectLink( const Inspect::SelectLinkArgs& args )
{
    Core::SceneNode* o = FindNode( args.m_ID );

    if ( o == NULL )
    {
        TUID id;
        if (id.FromString(args.m_ID))
        {
            HM_SceneNodeDumbPtr::const_iterator found = m_Nodes.find( id );
            if (found != m_Nodes.end())
            {
                o = found->second;
            }
        }
    }

    if ( o == NULL )
    {
        return;
    }

    Core::Layer* layer = Reflect::ObjectCast<Core::Layer>( o );

    if ( layer )
    {
        m_Selection.SetItems( layer->GetMembers() );
    }
    else
    {
        m_Selection.SetItem( o );
    }
}

void Scene::PopulateLink( Inspect::PopulateLinkArgs& args )
{
    tstring str;

    if ( args.m_Items.empty() )
    {
        TUID null;
        null.ToString(str);
        args.m_Items.push_back( Inspect::Item( TXT( "NULL" ), str) );
    }

    tstring suffix;

    if ( !IsFocused() )
    {
        suffix = TXT( " (" ) + m_Path.Basename() + TXT( ")" );
    }

    //
    // Map engine internal type to luna internal type
    //

    i32 typeID = Reflect::GetType<Core::SceneNode>();

    HMS_TypeToSceneNodeTypeDumbPtr::const_iterator found = m_NodeTypesByType.find( typeID );
    if (found != m_NodeTypesByType.end())
    {
        S_SceneNodeTypeDumbPtr::const_iterator itr = found->second.begin();
        S_SceneNodeTypeDumbPtr::const_iterator end = found->second.end();
        for ( ; itr != end; ++itr )
        {
            HM_SceneNodeSmartPtr::const_iterator nodeItr = (*itr)->GetInstances().begin();
            HM_SceneNodeSmartPtr::const_iterator nodeEnd = (*itr)->GetInstances().end();
            for ( ; nodeItr != nodeEnd; ++nodeItr )
            {
                nodeItr->second->GetID().ToString(str);
                args.m_Items.push_back( Inspect::Item (nodeItr->second->GetName() + suffix, str) );
            }
        }
    }
}

void Scene::SetHighlight(const SetHighlightArgs& args)
{
    CORE_SCOPE_TIMER( ("") );

    ClearHighlight( ClearHighlightArgs (false) );

    if (args.m_Pick == NULL)
        return;

    OS_SelectableDumbPtr selection;

    bool result = Pick( args.m_Pick );

    switch (args.m_Target)
    {
    case SelectionTargetModes::Single:
        {
            V_PickHitSmartPtr sorted;
            PickHit::Sort( m_View->GetCamera(), args.m_Pick->GetHits(), sorted, PickSortTypes::Intersection);

            V_PickHitSmartPtr::const_iterator itr = sorted.begin();
            V_PickHitSmartPtr::const_iterator end = sorted.end();
            for ( ; itr != end; ++itr )
            {
                Selectable* selectable = Reflect::ObjectCast<Selectable>((*itr)->GetObject());
                if (selectable)
                {
                    // add it to the new selection list
                    selection.Append(selectable);
                    break;
                }

#ifdef HELIUM_ASSERT_ENABLED
                SceneNode* node = Reflect::ObjectCast<SceneNode>( (*itr)->GetObject() );
                if (node)
                {
                    HELIUM_ASSERT( node->GetOwner() == this );
                }
#endif
            }
            break;
        }

    case SelectionTargetModes::Multiple:
        {
            V_PickHitSmartPtr::const_iterator itr = args.m_Pick->GetHits().begin();
            V_PickHitSmartPtr::const_iterator end = args.m_Pick->GetHits().end();
            for ( ; itr != end; ++itr )
            {
                Selectable* selectable = Reflect::ObjectCast<Selectable>((*itr)->GetObject());
                if (selectable)
                {
                    // add it to the new selection list
                    selection.Append(selectable);
                }

#ifdef HELIUM_ASSERT_ENABLED
                SceneNode* node = Reflect::ObjectCast<SceneNode>( (*itr)->GetObject() );
                if (node)
                {
                    HELIUM_ASSERT( node->GetOwner() == this );
                }
#endif
            }
            break;
        }
    }

    if (m_Tool && !m_Tool->ValidateSelection (selection) )
    {
        return;
    }

    m_Highlighted = selection;

    Core::HierarchyNode* first = NULL;
    OS_SelectableDumbPtr::Iterator itr = m_Highlighted.Begin();
    OS_SelectableDumbPtr::Iterator end = m_Highlighted.End();
    for ( ; itr != end; ++itr )
    {
        Core::HierarchyNode* node = Reflect::ObjectCast<Core::HierarchyNode>(*itr);

        if (node)
        {
            if ( !first )
            {
                first = node;
            }

            node->SetHighlighted(true);
        }
    }

    if (m_Highlighted.Size() == 1)
    {
        tstring status = first->GetName();
        tstring desc = first->GetDescription();

        if (!desc.empty())
        {
            status += TXT(" (") + desc + TXT( ")" );
        }

        m_StatusChanged.Raise( status );
    }
    else if (m_Highlighted.Size() > 1)
    {
        tostringstream str;
        str << m_Highlighted.Size() << " items";
        m_StatusChanged.Raise( str.str() );
    }
}

void Scene::ClearHighlight( const ClearHighlightArgs& args )
{
    OS_SelectableDumbPtr::Iterator itr = m_Highlighted.Begin();
    OS_SelectableDumbPtr::Iterator end = m_Highlighted.End();
    for ( ; itr != end; ++itr )
    {
        Core::HierarchyNode* node = Reflect::ObjectCast<Core::HierarchyNode>(*itr);

        if (node)
        {
            node->SetHighlighted (false);
        }
    }

    m_Highlighted.Clear();

    if ( args.m_Update )
    {
        Execute(false);
    }
}

bool Scene::Push(const Undo::CommandPtr& command)
{
    if (!command.ReferencesObject())
    {
        // allow the null change
        return true;
    }

    if (command->IsSignificant() && !IsEditable())
    {
        // we are significant and not editable, abort operation
        command->Undo();

        // we aborted, restore state if necessary
        return false;
    }

#pragma TODO( "Raise an event, don't reach up in to the app..." )
#if SCENE_REFACTOR
    wxGetApp().GetFrame()->Push( command );
#endif

    // change committed
    return true;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the undo queue is about to undo or redo a command.  Makes
// sure that the scene is editable before allowing the operation to proceed.
// 
bool Scene::UndoingOrRedoing( const Undo::QueueChangeArgs& args )
{
    bool allow = true;
    if ( args.m_Command->IsSignificant() )
    {
        allow = IsEditable();
    }
    return allow;
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when a command is pushed on the undo queue.  If the command was
// significant, the scene will be marked as dirty (needs to be saved).
// 
void Scene::UndoQueueCommandPushed( const Undo::QueueChangeArgs& args )
{
    if ( args.m_Command->IsSignificant() )
    {
#pragma TODO( "Raise an event so the SceneDocument knows this file has been modified" )
        //m_File->SetModified( true );
    }
}

Core::SceneNode* Scene::FindNode(const TUID& id)
{
    Core::SceneNode* node = NULL;

    // 
    // Guid
    // 

    HM_SceneNodeDumbPtr::const_iterator findGuid = m_Nodes.find( id );
    if ( findGuid != m_Nodes.end() )
    {
        node = findGuid->second;
    }
    else if ( m_RemappedIDs.size() > 0 )
    {
        // Immediately after an import, there may be some remapped IDs that 
        // can be searched to find the node.
        HM_TUID::const_iterator findRemap = m_RemappedIDs.find( id );
        if ( findRemap != m_RemappedIDs.end() )
        {
            HM_SceneNodeDumbPtr::const_iterator findRepeat = m_Nodes.find( findRemap->second );
            if ( findRepeat != m_Nodes.end() )
            {
                node = findRepeat->second;
            }
        }
    }

    return node;
}

Core::SceneNode* Scene::FindNode(const tstring& name)
{
    Core::SceneNode* node = NULL;

    // 
    // Name
    // 

    HM_NameToSceneNodeDumbPtr::const_iterator findName = m_Names.find( name );
    if ( findName != m_Names.end() )
    {
        node = findName->second;
    }

    return node;
}

void Scene::ChangeStatus(const tstring& status)
{
    m_StatusChanged.Raise( status );
}

void Scene::RefreshSelection()
{
    m_Selection.Refresh();
}

bool Scene::PropertyChanging( const Inspect::ChangingArgs& args )
{
    if ( args.m_Preview )
    {
        return true;
    }

    if ( args.m_Control->GetData().ReferencesObject() )
    {
        Undo::CommandPtr command = args.m_Control->GetData()->GetUndoCommand();

        if ( command )
        {
            return Push( command );
        }
    }

    return IsEditable();
}

void Scene::PropertyChanged( const Inspect::ChangeArgs& args )
{
    Execute(false);
}

bool Scene::SelectionChanging(const OS_SelectableDumbPtr& selection)
{
    CORE_SCOPE_TIMER( ("") );

    bool result = true;

    if (m_PickData.ReferencesObject())
    {
        if (!selection.Empty())
        {
            Core::SceneNode* node = Reflect::ObjectCast<Core::SceneNode>( selection.Front() );

            if (node)
            {
                tstring str;
                node->GetID().ToString(str);

                // set the picked object ID
                Inspect::StringData* data = Inspect::CastData< Inspect::StringData, Inspect::DataTypes::String >( m_PickData );
                if ( data && Push( data->GetUndoCommand() ) )
                {
                    data->Set( str );
                }

                // eat the selection
                result = false;

                // refresh the attributes
                Execute(false);
            }
        }

        m_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Picking, SceneContexts::Normal ) );
        m_PickData = NULL;
    }

    return result;
}

void Scene::SelectionChanged(const OS_SelectableDumbPtr& selection)
{
    CORE_SCOPE_TIMER( ("") );

    m_ValidSmartDuplicateMatrix = false;

    tostringstream str;
    if ( selection.Empty() )
    {
        str << "Selection cleared";
    }
    else
    {
        str << "Selected " << selection.Size() << " objects";
    }

    m_StatusChanged.Raise( str.str() );

    Execute(false);
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to return the closest ancestor that is a parent of all the
// specified hierarchy nodes.
// 
Core::HierarchyNode* Scene::GetCommonParent( const V_HierarchyNodeDumbPtr& nodes )
{
    Core::HierarchyNode* commonParent = NULL;

    for each ( Core::HierarchyNode* node in nodes )
    {
        Core::HierarchyNode* currentParent = node->GetParent();
        if ( !commonParent )
        {
            // First time through the loop, just use the current parent
            commonParent = currentParent;
        }
        else
        {
            // Only need to do additional work if the current parent does not match
            if ( currentParent != commonParent )
            {
                // Iterate up the hierarchy looking for the first node that is an ancestor
                // to the "common parent" so far and the current node.
                bool found = false;
                Core::HierarchyNode* parent = commonParent;
                Core::HierarchyNode* searchNode = node;
                while ( !found && parent )
                {
                    Core::HierarchyNode* searchParent = searchNode;
                    while ( !found && searchParent )
                    {
                        if ( searchParent == parent )
                        {
                            commonParent = parent;
                            found = true;
                        }

                        searchParent = searchParent->GetParent();
                    }
                    parent = parent->GetParent();
                }
            }
        }
    }

#pragma TODO("This should probably return NULL if commonParent == m_Root")
    return commonParent;
}

void Scene::GetCommonParents( const V_HierarchyNodeDumbPtr& nodes, V_HierarchyNodeDumbPtr& parents )
{
    parents.clear();

    // for each candidate item we want to test that has no other parent in the list
    V_HierarchyNodeDumbPtr::const_iterator outerItr = nodes.begin();
    V_HierarchyNodeDumbPtr::const_iterator outerEnd = nodes.end();
    for ( ; outerItr != outerEnd; ++outerItr )
    {
        Core::HierarchyNode* outer = *outerItr;

        if ( !outer )
        {
            continue;
        }

        // are we the child of another node in the selection?
        bool childNode = false;

        // start walking up the hierarchy starting from my parent
        Core::HierarchyNode* parent = outer->GetParent();

        // while we haven't hit the root
        while ( parent != m_Root )
        {
            // check each item in the selection, looking for a match to 'parent'
            V_HierarchyNodeDumbPtr::const_iterator innerItr = nodes.begin();
            V_HierarchyNodeDumbPtr::const_iterator innerEnd = nodes.end();
            for ( ; innerItr != innerEnd; ++innerItr )
            {
                Core::HierarchyNode* inner = *innerItr;

                if ( !inner )
                {
                    continue;
                }

                // if we found our parent
                if ( parent == inner )
                {
                    // our outer node is a child of another selection node
                    childNode = true;
                }
            }

            // walk up the hierarchy
            HELIUM_ASSERT (parent);
            parent = parent->GetParent();
        }

        // if we did not find a parent in the selection
        if ( !childNode )
        {
            // add it to the list of parent nodes
            parents.push_back( *outerItr );
        }
    }
}

void Scene::GetSelectionParents(OS_SelectableDumbPtr& parents)
{
    parents.Clear();

    // for each candidate item we want to test that has no other parent in the list
    OS_SelectableDumbPtr::Iterator outerItr = m_Selection.GetItems().Begin();
    OS_SelectableDumbPtr::Iterator outerEnd = m_Selection.GetItems().End();
    for ( ; outerItr != outerEnd; ++outerItr )
    {
        Core::HierarchyNode* outer = Reflect::ObjectCast< Core::HierarchyNode > (*outerItr);

        if ( !outer )
        {
            continue;
        }

        // are we the child of another node in the selection?
        bool childNode = false;

        // start walking up the hierarchy starting from my parent
        Core::HierarchyNode* parent = outer->GetParent();
        HELIUM_ASSERT( parent );

        // while we haven't hit the root
        while ( parent && parent != m_Root && !childNode )
        {
            // check each item in the selection, looking for a match to 'parent'
            OS_SelectableDumbPtr::Iterator innerItr = m_Selection.GetItems().Begin();
            OS_SelectableDumbPtr::Iterator innerEnd = m_Selection.GetItems().End();
            for ( ; innerItr != innerEnd; ++innerItr )
            {
                Core::HierarchyNode* inner = Reflect::ObjectCast< Core::HierarchyNode > (*innerItr);

                if ( !inner )
                {
                    continue;
                }

                // if we found our parent
                if ( parent == inner )
                {
                    // our outer node is a child of another selection node
                    childNode = true;
                    break;
                }
            }

            // walk up the hierarchy
            if ( parent )
            {
                parent = parent->GetParent();
            }
            else
            {
                HELIUM_BREAK();
            }
        }

        // if we did not find a parent in the selection
        if ( !childNode )
        {
            // add it to the list of parent nodes
            parents.Append( *outerItr );
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over the selection and flattens any selected groups
//
void Scene::GetFlattenedSelection(OS_SelectableDumbPtr& selection)
{
    selection.Clear();

    const OS_SelectableDumbPtr& selectedItems = m_Selection.GetItems();
    OS_SelectableDumbPtr::Iterator it = selectedItems.Begin();
    OS_SelectableDumbPtr::Iterator end = selectedItems.End();
    for ( ; it != end; ++it )
    {
        const LSelectablePtr& selectable = *it;

        selection.Append( selectable );

        Core::PivotTransform* group = Reflect::ObjectCast< Core::PivotTransform >( selectable );
        if ( group )
        {
            OS_HierarchyNodeDumbPtr items;
            GetFlattenedHierarchy( group, items );

            OS_HierarchyNodeDumbPtr::Iterator itemIt = items.Begin();
            OS_HierarchyNodeDumbPtr::Iterator itemEnd = items.End();
            for ( ; itemIt != itemEnd; ++itemIt )
            {
                selection.Append( *itemIt );
            }
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
// Flattends the hierarchy of an Core::HierarchyNode into a flat list
//
void Scene::GetFlattenedHierarchy(Core::HierarchyNode* node, OS_HierarchyNodeDumbPtr& items)
{
    const OS_HierarchyNodeDumbPtr& children = node->GetChildren();
    OS_HierarchyNodeDumbPtr::Iterator childIt = children.Begin();
    OS_HierarchyNodeDumbPtr::Iterator childEnd = children.End();
    for ( ; childIt != childEnd; ++childIt )
    {
        items.Append( *childIt );

        Core::HierarchyNode* group = Reflect::ObjectCast< Core::HierarchyNode >( *childIt );
        if ( group )
        {
            GetFlattenedHierarchy( group, items );
        }
    }
}

void Scene::GetSelectedTransforms( Math::V_Matrix4& transforms )
{
    OS_SelectableDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
    OS_SelectableDumbPtr::Iterator end = m_Selection.GetItems().End();
    for (int i=0; itr != end; itr++, i++)
    {
        Core::Transform* transform = Reflect::ObjectCast< Core::Transform >( *itr );

        if (transform)
        {
            transforms.push_back( transform->GetGlobalTransform() );
        }
    }
}

Undo::CommandPtr Scene::SetSelectedTransforms( const Math::V_Matrix4& transforms )
{
    if (m_Selection.GetItems().Empty())
    {
        return NULL;
    }

    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    OS_SelectableDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
    OS_SelectableDumbPtr::Iterator end = m_Selection.GetItems().End();
    for (size_t i=0; itr != end && i<transforms.size(); itr++, i++)
    {
        Core::Transform* transform = Reflect::ObjectCast< Core::Transform >( *itr );

        if (transform)
        {
            batch->Push( transform->SnapShot() );

            transform->SetGlobalTransform( transforms[i] );
        }
    }

    // we don't end in a selection, so execute
    Execute(false);

    return batch->IsEmpty() ? NULL : batch;
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over all the currently selected items and sets their hidden flag
// to match the indicated value.  These operations add undoable commands to the
// undo queue.  Call BeginMultiCommand before calling this function if you want
// to batch all of the commands into one operation.
// 
Undo::CommandPtr Scene::SetHiddenSelected( bool hidden )
{
    if (m_Selection.GetItems().Empty())
    {
        return NULL;
    }

    Log::Print( TXT( "\n o SetHiddenSelected( %s )\n" ), hidden ? TXT( "true" ) : TXT( "false" ) );

    if (hidden)
    {
        m_LastHidden.clear();
    }

    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    OS_SelectableDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
    OS_SelectableDumbPtr::Iterator end = m_Selection.GetItems().End();
    for ( ; itr != end; ++itr )
    {
        Core::HierarchyNode* hierarchyNode = Reflect::ObjectCast<Core::HierarchyNode>( *itr );
        if ( hierarchyNode )
        {
            if (hidden)
            {
                m_LastHidden.insert( hierarchyNode->GetID() );
            }

            Undo::PropertyCommand<bool>* command = 
                new Undo::PropertyCommand<bool> ( new Helium::MemberProperty<Core::HierarchyNode, bool> (hierarchyNode, &Core::HierarchyNode::IsHidden, &Core::HierarchyNode::SetHidden), hidden ) ; 

            command->SetSignificant( false ); 
            batch->Push(command); 
        }
    }

    return batch->IsEmpty() ? NULL : batch;
}

Undo::CommandPtr Scene::SetHiddenUnrelated( bool hidden )
{
    Log::Print( TXT( "\n o SetHiddenUnrelated( %s )\n" ), hidden ? TXT( "true" ) : TXT( "false" ) );

    if (hidden)
    {
        m_LastHidden.clear();
    }


    //
    // Bake selected root parents
    //

    OS_SelectableDumbPtr relatives;
    OS_SelectableDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
    OS_SelectableDumbPtr::Iterator end = m_Selection.GetItems().End();
    for ( ; itr != end; ++itr )
    {
        Core::HierarchyNode* hierarchyNode = Reflect::ObjectCast<Core::HierarchyNode>( *itr );
        if ( hierarchyNode )
        {
            relatives.Append( *itr );

            Core::HierarchyNode* parent = hierarchyNode->GetParent();
            while ( parent && parent != m_Root )
            {
                relatives.Append( parent );

                parent = parent->GetParent();
            }

            HierarchyChildTraverser traverser;
            hierarchyNode->TraverseHierarchy( &traverser );

            OS_SelectableDumbPtr::Iterator childItr = traverser.m_Children.Begin();
            OS_SelectableDumbPtr::Iterator childEnd = traverser.m_Children.End();
            for ( ; childItr != childEnd; ++childItr )
            {
                relatives.Append( *childItr );
            }
        }
    }


    //
    // Set visibility of the entire scene
    //

    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    {
        HMS_TypeToSceneNodeTypeDumbPtr::const_iterator found = m_NodeTypesByType.find( Reflect::GetType<Core::HierarchyNode>() );

        if (found != m_NodeTypesByType.end())
        {
            S_SceneNodeTypeDumbPtr::const_iterator typeItr = found->second.begin();
            S_SceneNodeTypeDumbPtr::const_iterator typeEnd = found->second.end();
            for ( ; typeItr != typeEnd; ++typeItr )
            {
                Core::HierarchyNodeType* type = Reflect::AssertCast<Core::HierarchyNodeType>( *typeItr );

                if (type)
                {
                    HM_SceneNodeSmartPtr::const_iterator itr = type->GetInstances().begin();
                    HM_SceneNodeSmartPtr::const_iterator end = type->GetInstances().end();
                    for ( ; itr != end; ++itr )
                    {
                        Core::HierarchyNode* node = Reflect::AssertCast<Core::HierarchyNode>( itr->second );

                        if (node && !relatives.Contains(node))
                        {
                            if (hidden && !node->IsHidden())
                            {
                                m_LastHidden.insert( node->GetID() );
                            }

                            Undo::PropertyCommand<bool>* command = 
                                new Undo::PropertyCommand<bool> ( new Helium::MemberProperty<Core::HierarchyNode, bool> (node, &Core::HierarchyNode::IsHidden, &Core::HierarchyNode::SetHidden), hidden ); 
                            command->SetSignificant(false); 
                            batch->Push( command ); 

                        }
                    }
                }
            }
        }
    }

    return batch->IsEmpty() ? NULL : batch;
}

///////////////////////////////////////////////////////////////////////////////
// Iterates over all the currently selected items and sets each one's visibility
// of thier referenced geometry to match the indicated value.
// 
Undo::CommandPtr Scene::SetGeometryShown( bool shown, bool selected )
{
    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    HM_SceneNodeDumbPtr::const_iterator itr = m_Nodes.begin();
    HM_SceneNodeDumbPtr::const_iterator end = m_Nodes.end();
    for ( ; itr != end; ++itr )
    {
        Core::Entity* entity = Reflect::ObjectCast< Core::Entity >( itr->second );
        if ( entity && entity->IsSelected() == selected )
        {
            Undo::PropertyCommand<bool>* command = 
                new Undo::PropertyCommand<bool> ( new Helium::MemberProperty<Core::Entity, bool> (entity, &Core::Entity::IsGeometryVisible, &Core::Entity::SetGeometryVisible), shown ); 

            command->SetSignificant(false); 
            batch->Push( command );
        }
    }

    return batch->IsEmpty() ? NULL : batch;
}

Undo::CommandPtr Scene::ShowLastHidden()
{
    Log::Print( TXT( "\n o ShowLastHidden()\n" ) );

    if (m_LastHidden.empty())
    {
        return NULL;
    }

    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    std::set<TUID>::const_iterator itr = m_LastHidden.begin();
    std::set<TUID>::const_iterator end = m_LastHidden.end();
    for ( ; itr != end; ++itr )
    {
        Core::HierarchyNode* hierarchyNode = Reflect::ObjectCast<Core::HierarchyNode>( FindNode( *itr ) );
        if ( hierarchyNode )
        {
            Undo::PropertyCommand<bool>* command = 
                new Undo::PropertyCommand<bool> ( new Helium::MemberProperty<Core::HierarchyNode, bool> (hierarchyNode, &Core::HierarchyNode::IsHidden, &Core::HierarchyNode::SetHidden), false ); 

            command->SetSignificant(false); 
            batch->Push( command );
        }
    }

    m_LastHidden.clear();

    return batch->IsEmpty() ? NULL : batch;
}

Undo::CommandPtr Scene::SelectSimilar()
{
    OS_SelectableDumbPtr selection;

    OS_SelectableDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
    OS_SelectableDumbPtr::Iterator end = m_Selection.GetItems().End();
    for ( ; itr != end; ++itr )
    {
        Core::HierarchyNode* node = Reflect::ObjectCast<Core::HierarchyNode>( *itr );

        if (node)
        {
            V_HierarchyNodeDumbPtr similar;
            node->FindSimilar( similar );

            V_HierarchyNodeDumbPtr::const_iterator similarItr = similar.begin();
            V_HierarchyNodeDumbPtr::const_iterator similarEnd = similar.end();
            for ( ; similarItr != similarEnd; ++similarItr )
            {
                selection.Append( *similarItr );
            }
        }
    }

    return m_Selection.SetItems(selection);
}

Undo::CommandPtr Scene::DeleteSelected()
{
    // since all of our children are going to be deleted with us, discard selected children from the working set
    OS_SelectableDumbPtr roots;
    GetSelectionParents(roots);

    if (roots.Empty())
    {
        return NULL;
    }

    Undo::BatchCommandPtr batch = new Undo::BatchCommand();

    batch->Push( m_Selection.Clear() );

    OS_SelectableDumbPtr::Iterator itr = roots.Begin();
    OS_SelectableDumbPtr::Iterator end = roots.End();
    for ( ; itr != end; ++itr )
    {
        SceneNodePtr node = Reflect::ObjectCast< Core::SceneNode >( *itr );
        if ( node )
        {
            batch->Push( new SceneNodeExistenceCommand( Undo::ExistenceActions::Remove, this, node ) ); 
        }
    }

    Execute(false);

    return batch->IsEmpty() ? NULL : batch;
}

Undo::CommandPtr Scene::ParentSelected()
{
    OS_SelectableDumbPtr selection;
    GetSelectionParents( selection );

    if (selection.Empty())
    {
        return NULL;
    }

    V_HierarchyNodeDumbPtr children;

    // Go through the selection list and pull out any hierarchy nodes
    OS_SelectableDumbPtr::Iterator itr = selection.Begin();
    OS_SelectableDumbPtr::Iterator end = selection.End();
    for ( ; itr != end; ++itr )
    {
        Core::HierarchyNode* hierarchyNode = Reflect::ObjectCast< Core::HierarchyNode >( *itr );
        if ( hierarchyNode )
        {
            children.push_back( hierarchyNode );
        }
    }

    if ( children.size() < 2 )
    {
        return NULL;
    }

    Core::HierarchyNode* parent = children.back();
    children.resize( children.size() - 1 );

    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    // Reparent the selected items under the parent node (last selected one)
    for each (Core::HierarchyNode* hierarchyNode in children)
    {
        batch->Push( new ParentCommand( hierarchyNode, parent ) );
    }

    return batch->IsEmpty() ? NULL : batch;
}

Undo::CommandPtr Scene::UnparentSelected()
{
    V_HierarchyNodeDumbPtr children;

    // Go through the selection list and pull out any hierarchy nodes
    OS_SelectableDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
    OS_SelectableDumbPtr::Iterator end = m_Selection.GetItems().End();
    for ( ; itr != end; ++itr )
    {
        Core::HierarchyNode* hierarchyNode = Reflect::ObjectCast< Core::HierarchyNode >( *itr );
        if ( hierarchyNode )
        {
            children.push_back( hierarchyNode );
        }
    }

    if ( children.empty() )
    {
        return NULL;
    }

    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    // Reparent the selected items under the root
    for each (Core::HierarchyNode* hierarchyNode in children)
    {
        if ( hierarchyNode->GetParent() != m_Root )
        {
            batch->Push( new ParentCommand( hierarchyNode, m_Root ) );
        }
    }

    // we don't end in a selection, so execute
    Execute(false);

    return batch->IsEmpty() ? NULL : batch;
}

///////////////////////////////////////////////////////////////////////////////
// Groups all the currently selected items into a new group, under the common
// ancestor for the selection.
// 
Undo::CommandPtr Scene::GroupSelected()
{
    OS_SelectableDumbPtr selection;
    GetSelectionParents( selection );

    if (selection.Empty())
    {
        return NULL;
    }

    V_HierarchyNodeDumbPtr selectedHierarchyNodes;

    // Go through the selection list and pull out any hierarchy nodes
    OS_SelectableDumbPtr::Iterator itr = selection.Begin();
    OS_SelectableDumbPtr::Iterator end = selection.End();
    for ( ; itr != end; ++itr )
    {
        Core::HierarchyNode* hierarchyNode = Reflect::ObjectCast< Core::HierarchyNode >( *itr );
        if ( hierarchyNode )
        {
            selectedHierarchyNodes.push_back( hierarchyNode );
        }
    }

    if ( selectedHierarchyNodes.empty() )
    {
        return NULL;
    }

    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    Math::Vector3 pos;
    const Core::Transform* transform = selectedHierarchyNodes.back()->GetTransform();
    HELIUM_ASSERT( transform );
    if ( transform )
    {
        const Math::Matrix4& globalTransform = transform->GetGlobalTransform();
        pos.x = globalTransform.t.x;
        pos.y = globalTransform.t.y;
        pos.z = globalTransform.t.z;
    }

    // Create the new group
    Core::PivotTransform* group = new Core::PivotTransform( this );

    // Get a decent name
    group->Rename( TXT( "group1" ) );

    // Make sure the new group is under the common parent for the selected nodes.
    group->SetParent( GetCommonParent( selectedHierarchyNodes ) );

    // This will re-compute the local components
    group->SetGlobalTransform( Matrix4 (pos) );

    // Update the object
    group->Evaluate( GraphDirections::Downstream );

    batch->Push( new SceneNodeExistenceCommand( Undo::ExistenceActions::Add, this, group ) );

    // Reparent the selected items under the new group
    for each (Core::HierarchyNode* hierarchyNode in selectedHierarchyNodes)
    {
        batch->Push( new ParentCommand( hierarchyNode, group ) );
    }

    // Select the newly created group
    batch->Push( m_Selection.SetItem( group ) );

    return batch->IsEmpty() ? NULL : batch;
}

Undo::CommandPtr Scene::UngroupSelected()
{
    if ( m_Selection.GetItems().Empty() )
    {
        return NULL;
    }

    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    OS_SelectableDumbPtr newSelection;

    bool warn = false;
    // For each selected item
    OS_SelectableDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
    OS_SelectableDumbPtr::Iterator end = m_Selection.GetItems().End();
    for ( ; itr != end; ++itr )
    {
        // If the item is a group (pivot transform)
        Core::SceneNode* sceneNode = Reflect::ObjectCast< Core::SceneNode >( *itr );

        if ( sceneNode && sceneNode->GetType() == Reflect::GetType<Core::PivotTransform>() )
        {
            Core::PivotTransform* group = Reflect::AssertCast< Core::PivotTransform >( sceneNode );

            // If the group has children, parent the children under the group's parent (their grandparent)
            if ( group->GetChildren().Size() > 0 )
            {
                // Need to operate on a copy of the children since we are going to change the original
                // group as we iterate over it.
                OS_HierarchyNodeDumbPtr copyOfChildren = group->GetChildren();
                for ( OS_HierarchyNodeDumbPtr::Iterator itr = copyOfChildren.Begin(), end = copyOfChildren.End(); itr != end; ++itr )
                {
                    Core::HierarchyNode* child = *itr;

                    // Push the parent command into the queue
                    batch->Push( new ParentCommand( child, group->GetParent() ) );

                    // Add the child to the new selection list
                    newSelection.Append( child );
                }
            }

            // Delete the group
            batch->Push( new SceneNodeExistenceCommand( Undo::ExistenceActions::Remove, this, group ) );
        }
        else
        {
            tstring msg = TXT( "The Ungroup command only works on groups. The node '" ) + sceneNode->GetName() + TXT( "' is not a group.\n" );
            Log::Warning( msg.c_str() );
            warn = true;
        }
    }

    // Change the selection
    batch->Push( m_Selection.SetItems( newSelection ) );

    if ( warn )
    {
        ChangeStatus( TXT( "The Ungroup command only works on groups.  See output window for more information." ) );
    }

    return batch->IsEmpty() ? NULL : batch;
}

Undo::CommandPtr Scene::CenterSelected()
{
    if (m_Selection.GetItems().Empty())
    {
        return NULL;
    }

    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    OS_SelectableDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
    OS_SelectableDumbPtr::Iterator end = m_Selection.GetItems().End();
    for (int i=0; itr != end; itr++, i++)
    {
        Core::Transform* transform = Reflect::ObjectCast< Core::Transform >( *itr );

        if (transform)
        {
            batch->Push( transform->CenterTransform() );
        }
    }

    // we don't end in a selection, so execute
    Execute(false);

    return batch->IsEmpty() ? NULL : batch;
}

Undo::CommandPtr Scene::DuplicateSelected()
{
    // since all of our children are going to be duplicated with us, discard selected children from the working set
    OS_SelectableDumbPtr roots;
    GetSelectionParents(roots);

    // test for nothing to do
    if (roots.Empty())
    {
        return NULL;
    }

    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    OS_SelectableDumbPtr newSelection;

    OS_SelectableDumbPtr::Iterator itr = roots.Begin();
    OS_SelectableDumbPtr::Iterator end = roots.End();
    for ( ; itr != end; ++itr )
    {
        Core::HierarchyNode* node = Reflect::ObjectCast< Core::HierarchyNode > (*itr);

        if (!node)
        {
            continue;
        }

        HierarchyNodePtr duplicate = node->Duplicate();

        newSelection.Append(duplicate);

        batch->Push( new SceneNodeExistenceCommand( Undo::ExistenceActions::Add, this, duplicate ) );

        duplicate->SetParent( node->GetParent() );

        // make sure the new nodes are initialized
        duplicate->InitializeHierarchy();
    }

    // setting the selection will invalidate the flag for having a valid smart duplicate matrix
    batch->Push( m_Selection.SetItems (newSelection) );

    // set the transform for smart duplicate, if there was only one selection
    if (roots.Size() == 1)
    {
        Core::HierarchyNode* node = Reflect::ObjectCast< Core::HierarchyNode >(roots.Front());   
        if (node)
        {
            m_SmartDuplicateMatrix = node->GetTransform()->GetGlobalTransform();
            m_ValidSmartDuplicateMatrix = true;
        }
    }

    return batch->IsEmpty() ? NULL : batch;
}

Undo::CommandPtr Scene::SmartDuplicateSelected()
{
    // if we do not have a valid duplicate matrix, then just do duplicate selected
    if (!m_ValidSmartDuplicateMatrix)
    {
        return DuplicateSelected();
    }

    // since all of our children are going to be duplicated with us, discard selected children from the working set
    OS_SelectableDumbPtr roots;
    GetSelectionParents(roots);

    // only operate on a single object!
    if (roots.Size() != 1)
    {
        return DuplicateSelected();
    }

    Core::Transform* node = Reflect::ObjectCast< Core::Transform >(roots.Front());

    if (!node)
    {
        return DuplicateSelected();
    }

    HierarchyNodePtr duplicate = node->Duplicate();

    Core::Transform* transform = Reflect::AssertCast<Core::Transform>( duplicate );

    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    // transform the duplicate based off of the previous duplicate matrix
    Math::Matrix4 matrix = transform->GetGlobalTransform() * m_SmartDuplicateMatrix.Inverted() * transform->GetGlobalTransform();

    // save this for the next smart transform matrix
    m_SmartDuplicateMatrix = transform->GetGlobalTransform();

    // add the duplicate to the scene
    batch->Push( new SceneNodeExistenceCommand( Undo::ExistenceActions::Add, this, duplicate ) );

    // parent it to the original nodes' parent
    batch->Push( new ParentCommand( duplicate, node->GetParent() ) );

    // set the global transform for the duplicate object
    batch->Push( new Undo::PropertyCommand<Math::Matrix4> ( new Helium::MemberProperty<Core::Transform, Math::Matrix4> (transform, &Core::Transform::GetGlobalTransform, &Core::Transform::SetGlobalTransform), matrix ) );

    // make sure the new nodes are initialized
    duplicate->InitializeHierarchy();

    // setting the selection will invalidate the flag for having a valid smart duplicate matrix
    batch->Push( m_Selection.SetItem(duplicate) );

    // reset the flag here
    m_ValidSmartDuplicateMatrix = true;

    return batch->IsEmpty() ? NULL : batch;
}

Undo::CommandPtr Scene::SnapSelectedToCamera()
{
    if (m_Selection.GetItems().Empty())
    {
        return NULL;
    }

    Undo::BatchCommandPtr batch = new Undo::BatchCommand ();

    Matrix4 m = Matrix4 ( AngleAxis( Math::Pi, Vector3::BasisY ) ) * m_View->GetCamera()->GetInverseView();

    OS_SelectableDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
    OS_SelectableDumbPtr::Iterator end = m_Selection.GetItems().End();
    for (int i=0; itr != end; itr++, i++)
    {
        Core::Transform* transform = Reflect::ObjectCast< Core::Transform >( *itr );

        if (transform)
        {
            batch->Push( new Undo::PropertyCommand<Math::Matrix4> ( new Helium::MemberProperty<Core::Transform, Math::Matrix4> (transform, &Core::Transform::GetGlobalTransform, &Core::Transform::SetGlobalTransform), m ) );
        }
    }

    // we don't end in a selection, so execute
    Execute(false);

    return batch->IsEmpty() ? NULL : batch;
}

Undo::CommandPtr Scene::SnapCameraToSelected()
{
    if (m_Selection.GetItems().Empty())
    {
        return NULL;
    }

    Core::Transform* transform = Reflect::ObjectCast< Core::Transform >( *m_Selection.GetItems().Begin() );

    if (transform)
    {
        Matrix4 m = Matrix4 ( AngleAxis( Math::Pi, Vector3::BasisY ) ) * transform->GetGlobalTransform();

        m_View->GetCamera()->SetTransform( m );
    }

    // we don't end in a selection, so execute
    Execute(false);

    return NULL;
}

void Scene::FrameSelected()
{
    bool found = false;
    Math::AlignedBox box;

    OS_SelectableDumbPtr::Iterator itr = GetSelection().GetItems().Begin();
    OS_SelectableDumbPtr::Iterator end = GetSelection().GetItems().End();
    for ( ; itr != end; ++itr )
    {
        Core::HierarchyNode* node = Reflect::ObjectCast<Core::HierarchyNode>(*itr);
        if (node)
        {
            box.Merge(node->GetGlobalHierarchyBounds());
            found = true;
            continue;
        }

        Core::Point* point = Reflect::ObjectCast<Core::Point>(*itr);
        if (point)
        {
            Math::Vector3 p = point->GetPosition();
            point->GetTransform()->GetGlobalTransform().TransformVertex(p);
            box.Merge(p);
            found = true;
            continue;
        }
    }

    if (found)
    {
        m_View->UpdateCameraHistory();    // we want the previous state before the move
        m_View->GetCamera()->Frame(box);

        Execute(false);
    }
}

void Scene::MeasureDistance()
{
    Core::Transform* first = NULL;
    Core::Transform* second = NULL;

    OS_SelectableDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
    OS_SelectableDumbPtr::Iterator end = m_Selection.GetItems().End();
    for ( ; itr != end; ++itr )
    {
        Core::Transform* t = Reflect::ObjectCast<Core::Transform>( *itr );

        if (t)
        {
            if (!first)
            {
                first = t;
                continue;
            }

            if (!second)
            {
                second = t;
                break;
            }
        }
    }

    if (first && second)
    {
        tostringstream str;

        Vector3 v = Vector3 (first->GetGlobalTransform().t.x, first->GetGlobalTransform().t.y, first->GetGlobalTransform().t.z) -
            Vector3 (second->GetGlobalTransform().t.x, second->GetGlobalTransform().t.y, second->GetGlobalTransform().t.z);

        float distance = v.Length();

        str << first->GetName() << " is " << distance << " meters from " << second->GetName();

        m_StatusChanged.Raise( SceneStatusChangeArgs (str.str()) );
    }
    else
    {
        m_StatusChanged.Raise( SceneStatusChangeArgs( TXT( "Please select 2 placed objects and try again" ) ) );
    }
}

Undo::CommandPtr Scene::PickWalkUp()
{
    if (m_Selection.GetItems().Empty())
    {
        return NULL;
    }

    OS_SelectableDumbPtr newSelection;

    OS_SelectableDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
    OS_SelectableDumbPtr::Iterator end = m_Selection.GetItems().End();
    for ( ; itr != end; ++itr )
    {
        Core::HierarchyNode* node = Reflect::ObjectCast<Core::HierarchyNode>(*itr);

        if (node == NULL)
        {
            return NULL;
        }

        if (node->GetParent() != m_Root)
        {
            newSelection.Append( node->GetParent() );
        }
    }

    if (!newSelection.Empty())
    {
        return m_Selection.SetItems( newSelection );
    }
    else
    {
        return NULL;
    }
}

Undo::CommandPtr Scene::PickWalkDown()
{
    if (m_Selection.GetItems().Empty())
    {
        return NULL;
    }

    Core::HierarchyNode* node = Reflect::ObjectCast<Core::HierarchyNode>(m_Selection.GetItems().Front());

    if (node == NULL)
    {
        return NULL;
    }

    if (!node->GetChildren().Empty())
    {
        return m_Selection.SetItem( node->GetChildren().Front() );
    }
    else
    {
        return NULL;
    }
}

Undo::CommandPtr Scene::PickWalkSibling(bool forward)
{
    if (m_Selection.GetItems().Empty())
    {
        return NULL;
    }

    Core::HierarchyNode* node = Reflect::ObjectCast<Core::HierarchyNode>(m_Selection.GetItems().Front());

    if (node == NULL)
    {
        return NULL;
    }

    if (node->GetParent() != NULL)
    {
        const OS_HierarchyNodeDumbPtr& children = node->GetParent()->GetChildren();

        if (!children.Empty())
        {
            typedef std::map<tstring, Core::HierarchyNode*> M_NameToHierarchyNodeDumbPtr;

            M_NameToHierarchyNodeDumbPtr sortedChildren;
            {
                OS_HierarchyNodeDumbPtr::Iterator itr = children.Begin();
                OS_HierarchyNodeDumbPtr::Iterator end = children.End();
                for ( ; itr != end; ++itr )
                {
                    sortedChildren[ (*itr)->GetName() ] = *itr;
                }
            }

            typedef std::vector<Core::HierarchyNode*> V_HierarchyNodeDumbPtr;

            size_t index = -1;
            V_HierarchyNodeDumbPtr indexedChildren;
            {
                M_NameToHierarchyNodeDumbPtr::const_iterator itr = sortedChildren.begin();
                M_NameToHierarchyNodeDumbPtr::const_iterator end = sortedChildren.end();
                for ( size_t i=0; itr != end; ++itr, ++i )
                {
                    if ( itr->second == node )
                    {
                        index = i;
                    }

                    indexedChildren.push_back( itr->second );
                }
            }

            return m_Selection.SetItem( indexedChildren[ ( forward?(index+1):(index-1) ) % indexedChildren.size() ] );
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        return NULL;
    }
}

void Scene::ViewPreferencesChanged( const Reflect::ElementChangeArgs& args )
{
#if SCENE_REFACTOR
    if ( args.m_Field == wxGetApp().GetPreferences()->GetViewportPreferences()->ColorModeField() )
    {
        Execute( false );
    }
#endif
}

Content::NodeVisibilityPtr Scene::GetVisibility(tuid nodeId)
{
    HELIUM_ASSERT(m_VisibilityDB); 

    tuid fallbackId = GetRemappedID(nodeId);

    return m_VisibilityDB->GetVisibility(nodeId, fallbackId); 

}

bool Scene::GetVisibilityFile(tstring& filename)
{
    tchar buffer[1024]; 
    _sntprintf(buffer, 1024, TXT( "Visibility/" ) TUID_HEX_FORMAT TXT( ".vis.nrb" ), m_Path.Hash() ); 

    Helium::Path prefsDir;
    if ( !Application::GetPreferencesDirectory( prefsDir ) )
    {
        return false;
    }

    Helium::Path filePath( prefsDir.Get() + tstring(buffer) );
    filename = filePath.Get();

    return true; 
}

void Scene::LoadVisibility()
{
    // attempt to load up our visibility file...
    tstring filename; 
    if( GetVisibilityFile(filename) && Helium::Path(filename).Exists() )
    {
        m_VisibilityDB = Reflect::Archive::FromFile<Content::SceneVisibility>(filename); 
    }
    else
    {
        m_VisibilityDB = new Content::SceneVisibility(); 
    }

#if SCENE_REFACTOR
    m_VisibilityDB->SetNodeDefaults( wxGetApp().GetPreferences()->GetScenePreferences()->GetDefaultNodeVisibility() ); 
#endif
}

void Scene::SaveVisibility()
{
    tstring filename; 
    if(m_VisibilityDB && GetVisibilityFile(filename) )
    {
        Helium::Path ( filename ).MakePath();
        Reflect::Archive::ToFile(m_VisibilityDB, filename); 
    }
}

TUID Scene::GetRemappedID( tuid nodeId )
{
    HM_TUID::iterator itr = m_RemappedIDs.begin();
    HM_TUID::iterator itrEnd = m_RemappedIDs.end();

    while( itr != itrEnd )
    {
        if( nodeId == (*itr).second )
        {
            return (*itr).first;
        }
        itr++;
    }

    return TUID::Null;
}
