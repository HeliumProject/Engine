#include "EditorScenePch.h"
#include "EditorScene/Scene.h"

#include <algorithm>
#include <set>
#include <cctype>

#include "Foundation/Log.h"
#include "Math/AngleAxis.h"
#include "Persist/ArchiveJson.h"
#include "Inspect/DataBinding.h"
#include "Inspect/Canvas.h"
#include "Application/Preferences.h"

#include "EditorScene/SceneSettings.h"
#include "EditorScene/SceneManifest.h"
#include "EditorScene/ParentCommand.h"
#include "EditorScene/PivotTransform.h"
#include "EditorScene/Layer.h"
#include "EditorScene/Curve.h"
#include "EditorScene/CurveControlPoint.h"
#include "EditorScene/Locator.h"

#include "Framework/WorldManager.h"

#define snprintf _snprintf

using namespace Helium;
using namespace Helium::Editor;

// TODO: Move data & serialization into SceneDefinition, drop FilePath arg, add SceneType arg
// TODO: This will become SceneProxy

Scene::Scene( Editor::Viewport* viewport, SceneDefinition &definition, SceneType type )
	: m_Type( type )
	, m_Definition( &definition )
	, m_Id( TUID::Generate() )
	, m_Progress( 0 )
	, m_Importing( false )
	, m_View( viewport )
	, m_SmartDuplicateMatrix(Matrix4::Identity)
	, m_ValidSmartDuplicateMatrix( false )
	, m_Color( 255 )
	, m_IsFocused( true )
{
	// This event delegate will cause the scene to execute and render a frame to effect the visual outcome of a selection change
	m_Selection.AddChangingListener( SelectionChangingSignature::Delegate (this, &Scene::SelectionChanging) );
	m_Selection.AddChangedListener( SelectionChangedSignature::Delegate (this, &Scene::SelectionChanged) );

	// Evaluation
	m_Graph = new Graph();

	// Setup root node
	m_Root = new PivotTransform();
	m_Root->SetOwner( this );
	m_Root->Initialize();
	m_Root->SetName( TXT( "Root" ) );
	m_Root->Evaluate( GraphDirections::Downstream );
	m_Graph->AddNode( m_Root.Ptr() );

	// All imports should default to the master root
	m_ImportRoot = m_Root.Ptr();

	m_View->GetSettingsManager()->GetSettings< ViewportSettings >()->e_Changed.Add( Reflect::ObjectChangeSignature::Delegate( this, &Scene::ViewPreferencesChanged ) );

	Load( *m_Definition );
}

Scene::~Scene()
{
	m_View->GetSettingsManager()->GetSettings< ViewportSettings >()->e_Changed.Remove( Reflect::ObjectChangeSignature::Delegate( this, &Scene::ViewPreferencesChanged ) );

	// remove selection listener
	m_Selection.RemoveChangingListener( SelectionChangingSignature::Delegate (this, &Scene::SelectionChanging) );
	m_Selection.RemoveChangedListener( SelectionChangedSignature::Delegate (this, &Scene::SelectionChanged) );

	Reset();
}

bool Scene::IsEditable()
{
	SceneEditingArgs args ( this );
	d_Editing.Invoke( args );
	return !args.m_Veto;
}

const ToolPtr& Scene::GetTool()
{
	return m_Tool;
}

void Scene::SetTool(const ToolPtr& tool)
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
		e_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Picking, SceneContexts::Normal ) );
		m_PickData = NULL;
	}
}

const Color& Scene::GetColor() const
{
	return m_Color;
}

void Scene::SetColor( const Color& color )
{
	if ( m_Color != color )
	{
		m_Color = color;
	}
}

// void Scene::ConnectDocument( Document* document )
// {
// 	document->d_Save.Set( this, &Scene::OnDocumentSave );
// 
// 	e_HasChanged.AddMethod( document, &Document::OnObjectChanged );
// }
// 
// void Scene::DisconnectDocument( const Document* document )
// {
// 	document->d_Save.Clear();
// 
// 	e_HasChanged.RemoveMethod( document, &Document::OnObjectChanged );
// }
// 
// void Scene::OnDocumentSave( const DocumentEventArgs& args )
// {
// 	const Document* document = static_cast< const Document* >( args.m_Document );
// 	HELIUM_ASSERT( document );
// 	HELIUM_ASSERT( !m_Path.empty() && document->GetPath() == m_Path )
// 
// 		args.m_Result = Serialize();
// }

bool Scene::Load( Helium::SceneDefinition& definition )
{
	if ( !m_Nodes.empty() )
	{
		HELIUM_BREAK();
		// Shouldn't happen
		Log::Error( TXT( "Scene '%s' is not empty!  You should not be trying to Load '%s'.  Do an Import instead.\n" ), *m_Definition->GetPath().ToString().GetData(), definition.GetPath().ToString().GetData() );
		return false;
	}

	if (m_Type == SceneTypes::World)
	{
		m_World = WorldManager::GetStaticInstance().CreateWorld( &definition );
		m_Slice = m_World->GetRootSlice();
		return true;
	}

	HELIUM_BREAK_MSG("Not yet supported");
	return false;
}

bool Scene::Reload()
{
	Reset();

	return Load( *m_Definition );
}

UndoCommandPtr Scene::Import( const Helium::FilePath& path, ImportAction action, uint32_t importFlags, Editor::HierarchyNode* importRoot, const Reflect::MetaClass* importReflectType )
{
	HELIUM_EDITOR_SCENE_SCOPE_TIMER( "%s", path.c_str() );

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

	e_LoadStarted.Raise( LoadArgs( this ) );

	if ( !importRoot )
	{
		importRoot = GetRoot();
	}

	//if ( action == ImportActions::Load )
	//{
	//	m_Path = path;
	//}

	// setup
	m_ImportRoot = importRoot;

	std::ostringstream str;
	str << "Loading File: " << path.c_str();
	e_StatusChanged.Raise( str.str() );

	// read data
	m_Progress = 0;
	std::vector< Reflect::ObjectPtr > elements;

	bool success = true;

	try
	{
#if REFLECT_REFACTOR
		Persist::ArchiveReaderPtr archive = Persist::GetReader( path );
		archive->e_Status.AddMethod( this, &Scene::ArchiveStatus );
		archive->Get( elements );
#endif
	}
	catch ( const Helium::Exception& exception )
	{
		Log::Error( TXT( "%s\n" ), exception.What() );
		success = false;
	}

	UndoCommandPtr command;

	if ( success )
	{
		// load and init nodes
		command = ImportSceneNodes( elements, action, importFlags, importReflectType );
	}

	m_ImportRoot = m_Root;
	e_LoadFinished.Raise( LoadArgs( this, success ) );

	return command;
}

UndoCommandPtr Scene::ImportJson( const std::string& json, uint32_t importFlags, Editor::HierarchyNode* importRoot )
{
	HELIUM_EDITOR_SCENE_SCOPE_TIMER();

	e_LoadStarted.Raise( LoadArgs( this ) );

	if ( json.empty() )
	{
		return NULL;
	}

	if ( !importRoot )
	{
		importRoot = GetRoot();
	}

	// setup
	m_ImportRoot = importRoot;

	std::ostringstream str;
	str << "Parsing JSON...";
	e_StatusChanged.Raise( str.str() );

	// read data
	m_Progress = 0;
	std::vector< Reflect::ObjectPtr > elements;

	bool success = true;

	try
	{
#if REFLECT_REFACTOR
		Persist::ArchiveXML::FromString( xml, elements );
#endif
	}
	catch ( Helium::Exception& exception )
	{
		Log::Error( TXT( "%s\n" ), exception.What() );
		success = false;
	}

	UndoCommandPtr command;

	if ( success )
	{
		// convert nodes
		command = ImportSceneNodes( elements, ImportActions::Import, importFlags );
	}

	m_ImportRoot = m_Root;
	e_LoadFinished.Raise( LoadArgs( this, success ) );

	return command;
}

void Scene::Reset()
{
	// Clear selection to free Properties API UI
	m_Selection.SetItems( OS_ObjectDumbPtr () );

	// clear Highlighted
	m_Highlighted.Clear();

	// deallocate the tool
	m_Tool = NULL;

	// Clear name cache
	m_Names.clear();

	// Reclaim resource memory
	Delete();

	// Break down entire graph
	m_Graph->Reset();

	// Clear flat hash of nodes
	m_Nodes.clear();

	// Reset root
	if ( m_Root.ReferencesObject() )
	{
		m_Root->Reset();
	}
}

UndoCommandPtr Scene::ImportSceneNodes( std::vector< Reflect::ObjectPtr >& elements, ImportAction action, uint32_t importFlags, const Reflect::MetaClass* importReflectType )
{
	HELIUM_EDITOR_SCENE_SCOPE_TIMER();

	uint64_t startTimer = Timer::GetTickCount();

	// 
	// Initialize
	// 

	m_Importing = true;
	e_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Normal, SceneContexts::Loading ) );
	e_StatusChanged.Raise( std::string( TXT("Loading Objects") ) );

	m_RemappedIDs.clear();

	// 
	// Load Elements
	// 

	BatchUndoCommandPtr command = new BatchUndoCommand ();

	V_SceneNodeSmartPtr createdNodes;
	createdNodes.reserve( elements.size() );
	{
		std::vector< Reflect::ObjectPtr >::const_iterator itr = elements.begin();
		std::vector< Reflect::ObjectPtr >::const_iterator end = elements.end();
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
			HierarchyNodePtr hierarchyNode = Reflect::SafeCast<Editor::HierarchyNode>(*itr);

			if (!hierarchyNode.ReferencesObject())
			{
				// its not a hierarchy node, just continue
				continue;
			}


			// 
			// Remap object id
			// 

			TUID childID = hierarchyNode->GetID();

			std::map< TUID, TUID >::const_iterator findChild = m_RemappedIDs.find( childID );
			if ( findChild != m_RemappedIDs.end() )
			{
				childID = findChild->second;
			}


			// 
			// Remap parent id
			// 

			TUID parentID = hierarchyNode->GetParentID();

			std::map< TUID, TUID >::const_iterator findParent = m_RemappedIDs.find( parentID );
			if ( findParent != m_RemappedIDs.end() )
			{
				parentID = findParent->second;
			}


			// 
			// Set Hierarchy
			// 

			Editor::HierarchyNode* child = NULL;
			M_SceneNodeSmartPtr::const_iterator foundChild = m_Nodes.find( childID );
			if ( foundChild != m_Nodes.end() )
			{
				child = Reflect::SafeCast< Editor::HierarchyNode >( foundChild->second );
			}

			if (child != NULL)
			{
				if (parentID != TUID::Null)
				{
					Editor::HierarchyNode* parent = NULL;

					M_SceneNodeSmartPtr::const_iterator foundParent = m_Nodes.find( parentID );
					if ( foundParent != m_Nodes.end() )
					{
						parent = Reflect::SafeCast< Editor::HierarchyNode >( foundParent->second );
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
	e_StatusChanged.Raise( std::string( TXT("Evaluating Objects...") ) );
	Evaluate(true);

	// initialize each object after initial evaluation is complete
	V_HierarchyNodeDumbPtr newNodes;
	e_StatusChanged.Raise( std::string( TXT("Initializing Objects...") ) );
	V_SceneNodeSmartPtr::const_iterator itr = createdNodes.begin();
	V_SceneNodeSmartPtr::const_iterator end = createdNodes.end();
	for ( ; itr != end; ++itr )
	{
		(*itr)->Initialize();

		if ( importFlags & ImportFlags::Select )
		{
			Editor::HierarchyNode* node = Reflect::SafeCast<Editor::HierarchyNode>( *itr );
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
			OS_ObjectDumbPtr newSelection;
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
			e_NodeAdded.Raise( NodeChangeArgs( *itr ) );
		}
	}

	// report
	std::ostringstream str;
	str.precision( 2 );
	str << "Scene Loading Complete: " << std::fixed << Timer::TicksToMilliseconds( Timer::GetTickCount() - startTimer ) / 1000.f << " seconds...";
	e_StatusChanged.Raise( str.str() );

	// done
	e_StatusChanged.Raise( std::string( TXT("Ready") ) );
	e_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Loading, SceneContexts::Normal ) );
	m_Importing = false;

	return command;
}

UndoCommandPtr Scene::ImportSceneNode( const Reflect::ObjectPtr& element, V_SceneNodeSmartPtr& createdNodes, ImportAction action, uint32_t importFlags, const Reflect::MetaClass* importReflectType )
{
	HELIUM_EDITOR_SCENE_SCOPE_TIMER( "ImportSceneNode: %s", element->GetMetaClass()->m_Name );

	SceneNodePtr sceneNode = Reflect::SafeCast< SceneNode >( element );

	if ( importReflectType == NULL )
	{
		importReflectType = Reflect::GetMetaClass< SceneNode >();
	}

	// 
	// Merging Nodes / Duplicate ID check
	// 
	bool convertNode = action == ImportActions::Load;

	if ( action == ImportActions::Import )
	{
		if ( element->IsA( importReflectType ) )
		{
			SceneNode* node = Reflect::SafeCast< SceneNode >( element );
			if ( node )
			{
				if ( ImportFlags::HasFlag( importFlags, ImportFlags::Merge ) )
				{
					M_SceneNodeSmartPtr::const_iterator found = m_Nodes.find( node->GetID() );
					if ( found != m_Nodes.end() )
					{
						Editor::SceneNode* dependNode = found->second;
						element->CopyTo( dependNode );
						dependNode->Dirty();
					}
				}
				else
				{
					// Always generate a new ID when importing and not merging
					TUID id( TUID::Generate() );
					m_RemappedIDs.insert( std::map< TUID, TUID >::value_type( node->GetID(), id ) );
					node->SetID( id );
					convertNode = true;
				}
			}
		}
	}


	// 
	// Convert Node
	// 

	if ( convertNode )
	{
		if ( sceneNode.ReferencesObject() )
		{
			// update ui
			std::ostringstream str;
			str << TXT( "Loading: " ) + sceneNode->GetName();
			e_StatusChanged.Raise( str.str() );

			// save it in the list of created nodes
			createdNodes.push_back( sceneNode );

			// add object to the scene
			return new SceneNodeExistenceCommand( ExistenceActions::Add, this, sceneNode );
		}
	}

	return NULL;
}

void Scene::ArchiveStatus( const Persist::ArchiveStatus& info )
{
	switch (info.m_State)
	{
	case Persist::ArchiveStates::ArchiveStarting:
		{
			std::string verb = info.m_Archive.GetMode() == Persist::ArchiveModes::Read ? TXT( "Opening" ) : TXT( "Saving" );
			std::string type = info.m_Archive.GetType() == Persist::ArchiveTypes::Json ? TXT( "JSON" ) : TXT( "MessagePack" );

			std::ostringstream str;
			str << verb << " " << type << " File: " << info.m_Archive.GetPath();
			e_StatusChanged.Raise( str.str() );
			break;
		}

	case Persist::ArchiveStates::ObjectProcessed:
		{
			if (info.m_Progress > m_Progress)
			{
				m_Progress = info.m_Progress;

				{
					std::string verb = info.m_Archive.GetMode() == Persist::ArchiveModes::Read ? TXT( "Opening" ) : TXT( "Saving" );

					std::ostringstream str;
					str << verb << ": " << info.m_Archive.GetPath() << " (" << m_Progress << "%)";
					e_StatusChanged.Raise( str.str() );
				}
			}

			break;
		}

	case Persist::ArchiveStates::Complete:
		{
			std::string verb = info.m_Archive.GetMode() == Persist::ArchiveModes::Read ? TXT( "Opening" ) : TXT( "Saving" );

			std::ostringstream str;
			str << "Completed " << verb << ": " << info.m_Archive.GetPath();
			e_StatusChanged.Raise( str.str() );
			break;
		}

	case Persist::ArchiveStates::PostProcessing:
		{
			std::ostringstream str;
			str << "Processing: " << info.m_Archive.GetPath();
			e_StatusChanged.Raise( str.str() );
			break;
		}

	default:
		break;
	}
}

bool Scene::Export( std::vector< Reflect::ObjectPtr >& elements, const ExportArgs& args, BatchUndoCommand* changes )
{
	bool result = true;

	{
		// before exporting anything build the manifest object that will live at the front of the file
		SceneManifestPtr manifest = new SceneManifest ();

		// the root node's global bounds will contain the extents of the scene
		manifest->m_BoundingBoxMin = m_Root->GetGlobalHierarchyBounds().minimum;
		manifest->m_BoundingBoxMax = m_Root->GetGlobalHierarchyBounds().maximum;

		// iterate over every instance
		M_SceneNodeSmartPtr::const_iterator itr = m_Nodes.begin();
		M_SceneNodeSmartPtr::const_iterator end = m_Nodes.end();
		for ( ; itr != end; ++itr )
		{
			itr->second->PopulateManifest( manifest );
		}

		elements.push_back( manifest );
	}

	// ID's of the objects that have been exported so far (to prevent exporting dupes)
	std::set<TUID> exported; 

	if ( ExportFlags::HasFlag( args.m_Flags, ExportFlags::SelectedNodes ) )
	{
		// Walk through the selection list and export each dependency node
		if ( m_Selection.GetItems().Size() > 0 )
		{
			OS_ObjectDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
			OS_ObjectDumbPtr::Iterator end = m_Selection.GetItems().End();
			for ( ; itr != end; ++itr )
			{
				Editor::SceneNode* node = Reflect::SafeCast< Editor::SceneNode >( *itr );
				if ( node )
				{
					Editor::HierarchyNode* hierarchyNode = Reflect::SafeCast< Editor::HierarchyNode >( node );

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
				M_SceneNodeSmartPtr::const_iterator itr = m_Nodes.begin();
				M_SceneNodeSmartPtr::const_iterator end = m_Nodes.end();
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

void Scene::ExportSceneNode( Editor::SceneNode* node, std::vector< Reflect::ObjectPtr >& elements, std::set<TUID>& exported, const ExportArgs& args, BatchUndoCommand* changes )
{
	// Don't export the root node
	if ( node != m_Root )
	{
		// Don't export a node if it has already been exported
		if ( exported.find( node->GetID() ) == exported.end() && !node->IsTransient() )
		{
			Editor::Transform* transformNode = Reflect::SafeCast< Editor::Transform >( node );
			if ( transformNode && !ExportFlags::HasFlag( args.m_Flags, ExportFlags::MaintainHierarchy ) )
			{
				// If we are exporting a transform node, but we are not maintaining the hierarchy,
				// we would still like to maintain the object's global position.  So, we create
				// reparent the node under the root (which maintains the node's position) and
				// store that command so that it can be undone after export completes.
				changes->Push( new ParentCommand( transformNode, GetRoot() ) );
			}

			elements.push_back( node );
			exported.insert( node->GetID() );

			// Flag to indicate whether to recursively export ancestors of this node or not
			if ( ExportFlags::HasFlag( args.m_Flags, ExportFlags::MaintainDependencies ) )
			{
				S_SceneNodeDumbPtr::const_iterator ancestorItr = node->GetAncestors().begin();
				S_SceneNodeDumbPtr::const_iterator ancestorEnd = node->GetAncestors().end();
				for ( ; ancestorItr != ancestorEnd; ++ancestorItr )
				{
					Editor::SceneNode* ancestor = *ancestorItr;

					bool skipAncestor = false;

					Editor::HierarchyNode* hierarchyNode = Reflect::SafeCast< Editor::HierarchyNode >( node );
					if ( hierarchyNode )
					{
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

void Scene::ExportHierarchyNode( Editor::HierarchyNode* node, std::vector< Reflect::ObjectPtr >& elements, std::set<TUID>& exported, const ExportArgs& args, BatchUndoCommand* changes, bool exportChildren )
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

				for ( ExportArgs::V_AlignedBox::const_iterator itr = args.m_Bounds.begin(), end = args.m_Bounds.end(); itr != end && !proceed; ++itr )
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

// bool Scene::Serialize()
// {
// 	HELIUM_ASSERT( !m_Path.empty() );
// 	return Export( m_Path, ExportFlags::Default );
// }

bool Scene::Export( const Helium::FilePath& path, const ExportArgs& args )
{
	uint64_t startTimer = Timer::GetTickCount();

	bool result = false;

	e_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Normal, SceneContexts::Saving ) );

	m_Progress = 0;

	{
		std::ostringstream str;
		str << "Preparing to save: " << path.c_str();
		e_StatusChanged.Raise( str.str() );
	}

	BatchUndoCommandPtr changes = new BatchUndoCommand();

	std::vector< Reflect::ObjectPtr > spool;
	result = Export( spool, args, changes );

	if (result)
	{
		try
		{
#if REFLECT_REFACTOR
			Persist::ArchivePtr archive = Reflect::GetArchive( path );
			archive->e_Status.AddMethod( this, &Scene::ArchiveStatus );
			archive->Open( true );
			archive->Put( spool );
			archive->Write();
			archive->Close();
#endif
		}
		catch ( Helium::Exception& ex )
		{
			Log::Error( TXT("Failed to write file %s: %s"), path.c_str(), ex.What() );
			result = false;
		}
	}

	changes->Undo();

	e_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Saving, SceneContexts::Normal ) );

	{
		std::ostringstream str;
		str.precision( 2 );
		str << "Saving Complete: " << std::fixed << Timer::TicksToMilliseconds( Timer::GetTickCount() - startTimer ) / 1000.f << " seconds...";
		e_StatusChanged.Raise( str.str() );
	}

	return result;
}

///////////////////////////////////////////////////////////////////////////////
// Exports (based upon the export arguments) either the whole scene, or the
// selected items.  The exported items are written into the xml parameter that
// is passed into this function.
// 
bool Scene::ExportJson( std::string& json, const ExportArgs& args )
{
	HELIUM_EDITOR_SCENE_SCOPE_TIMER();

	bool result = false;

	uint64_t startTimer = Timer::GetTickCount();

	e_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Normal, SceneContexts::Saving ) );

	m_Progress = 0;

	{
		std::ostringstream str;
		str << "Preparing to export";
		e_StatusChanged.Raise( str.str() );
	}

	BatchUndoCommandPtr changes = new BatchUndoCommand();

	std::vector< Reflect::ObjectPtr > spool;
	result = Export( spool, args, changes );

	if ( result && !spool.empty() )
	{
		try
		{
#if REFLECT_REFACTOR
			Persist::ArchiveXML::ToString( spool, xml );
#endif
		}
		catch ( Helium::Exception& ex )
		{
			std::ostringstream str;
			Log::Error( TXT("Failed to generate xml: %s"), ex.What() );
			result = false;
		}
	}

	changes->Undo();

	e_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Saving, SceneContexts::Normal ) );

	{
		std::ostringstream str;
		str.precision( 2 );
		str << "Export Complete: " << std::fixed << Timer::TicksToMilliseconds( Timer::GetTickCount() - startTimer ) / 1000.f << " seconds...";
		e_StatusChanged.Raise( str.str() );
	}

	return result;
}

void Scene::Rename( Editor::SceneNode* sceneNode, const std::string& newName, std::string oldName )
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
		M_NameToSceneNodeDumbPtr::iterator foundName = m_Names.find( oldName );

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

int Scene::Split( std::string& outName )
{
	int ret = -1;

	std::string name = outName.c_str();

	size_t lastNum = name.size();
	while (lastNum > 0 && isdigit(name[lastNum-1]))
	{
		lastNum--;
	}

	if ( lastNum < name.size() )
	{
		std::string numberString = name.substr(lastNum);

		// trim name
		outName = name.substr(0, lastNum);

		std::stringstream str( numberString );
		str >> ret;
	}

	return ret;
}

void Scene::SetName( Editor::SceneNode* sceneNode, const std::string& newName )
{
	// lua keywords
	static std::set<std::string> keywords;
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

	std::string realName = newName;

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
	std::string::iterator itr = realName.begin();
	std::string::iterator end = realName.end();
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
	M_NameToSceneNodeDumbPtr::iterator foundName = m_Names.find( realName );

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
		std::string result;

		// do finds while we haven't found a unique numeric version
		M_NameToSceneNodeDumbPtr::const_iterator searchItr = m_Names.end();
		M_NameToSceneNodeDumbPtr::const_iterator searchEnd = searchItr;
		do
		{
			// extract the number to ascii
			number++;
			std::ostringstream numberStr;
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
	std::pair< M_NameToSceneNodeDumbPtr::const_iterator, bool > inserted = m_Names.insert( M_NameToSceneNodeDumbPtr::value_type( realName, sceneNode ) );
	bool previouslyInserted = sceneNode == inserted.first->second && sceneNode->GetName() == realName;
	bool newlyInserted = inserted.second;
	HELIUM_ASSERT( previouslyInserted || newlyInserted );
}

void Scene::AddObject( SceneNodePtr node )
{
	HELIUM_EDITOR_SCENE_SCOPE_TIMER();

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

void Scene::RemoveObject( SceneNodePtr node )
{
	HELIUM_EDITOR_SCENE_SCOPE_TIMER();

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
	node->SetOwner( this );

	{
		HELIUM_EDITOR_SCENE_SCOPE_TIMER( "Insert in node list" );

		// this would be bad
		HELIUM_ASSERT( node->GetID() != TUID::Null );

		std::pair< M_SceneNodeSmartPtr::const_iterator, bool > inserted = m_Nodes.insert( M_SceneNodeSmartPtr::value_type( node->GetID(), node ) );
		HELIUM_ASSERT( inserted.first->second == node );
		if ( !inserted.second )
		{
			Log::Error( TXT( "Attempted to add a node with the same ID as one that already exists - %s [" ) TUID_HEX_FORMAT TXT( "].\n" ), node->GetName().c_str(), static_cast<tuid>(node->GetID()) );
			HELIUM_BREAK();
		}
	}

	{
		HELIUM_EDITOR_SCENE_SCOPE_TIMER( "Set name" );

		// check name
		SetName( node, node->GetName() );
	}

	{
		HELIUM_EDITOR_SCENE_SCOPE_TIMER( "Create" );

		// (re)creates disposable resources in object
		node->Create();
	}

	{
		HELIUM_EDITOR_SCENE_SCOPE_TIMER( "Hierarchy check" );

		Editor::HierarchyNode* hierarchyNode = Reflect::SafeCast< Editor::HierarchyNode >( node );
		if ( hierarchyNode && hierarchyNode->GetParent() == NULL )
		{
			hierarchyNode->SetParent(m_Root);
		}
	}

	{
		HELIUM_EDITOR_SCENE_SCOPE_TIMER( "Raise events if not transient" );
		if ( !node->IsTransient() && !m_Importing )
		{
			e_NodeAdded.Raise( NodeChangeArgs( node.Ptr() ) );
		}
	}
}

void Scene::RemoveSceneNode( const SceneNodePtr& node )
{
	HELIUM_EDITOR_SCENE_SCOPE_TIMER();

	if ( !node->IsTransient() )
	{
		e_NodeRemoving.Raise( NodeChangeArgs( node.Ptr() ) );
	}

	// remove shortcuts to node and children
	m_Nodes.erase( node->GetID() );

	// cleanup name
	m_Names.erase( node->GetName() );

	// destroys disposable resources in object
	node->Delete();

	if ( !node->IsTransient() )
	{
		e_NodeRemoved.Raise( NodeChangeArgs( node.Ptr() ) );
	}
}

void Scene::Execute(bool interactively)
{
	HELIUM_EDITOR_SCENE_EVALUATE_SCOPE_TIMER();

	// update data
	Evaluate();

	e_Executed.Raise( ExecuteArgs (this, interactively) );
}

void Scene::Create()
{
	for ( M_SceneNodeSmartPtr::const_iterator itr = m_Nodes.begin(), end = m_Nodes.end(); itr != end; ++itr )
	{
		Editor::SceneNode* n = itr->second;
		n->Create();
	}
}

void Scene::Delete()
{
	for ( M_SceneNodeSmartPtr::const_iterator itr = m_Nodes.begin(), end = m_Nodes.end(); itr != end; ++itr )
	{
		Editor::SceneNode* n = itr->second;
		n->Delete();
	}
}

void Scene::Render( RenderVisitor* render )
{
	HELIUM_EDITOR_SCENE_RENDER_SCOPE_TIMER();

	HierarchyRenderTraverser renderTraverser ( render );

	m_Root->TraverseHierarchy( &renderTraverser );
}

bool Scene::Pick( PickVisitor* pick ) const
{
	HELIUM_EDITOR_SCENE_SCOPE_TIMER();

	size_t hitCount = pick->GetHits().size();

	HierarchyPickTraverser pickTraverser ( pick );

	m_Root->TraverseHierarchy ( &pickTraverser );

	return pick->GetHits().size() > hitCount;
}

void Scene::Select( const SelectArgs& args )
{
	HELIUM_EDITOR_SCENE_SCOPE_TIMER();

	ClearHighlight( ClearHighlightArgs (false) );

	bool result = Pick( args.m_Pick );

	OS_ObjectDumbPtr selection;

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
				SceneNode* node = Reflect::SafeCast<SceneNode>((*itr)->GetHitObject());
				if (node)
				{
					// add it to the new selection list
					selection.Append(node);
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
				SceneNode* node = Reflect::SafeCast<SceneNode>((*itr)->GetHitObject());
				if (node)
				{
					// add it to the new selection list
					selection.Append(node);
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
			OS_ObjectDumbPtr newSelection = m_Selection.GetItems();
			OS_ObjectDumbPtr::Iterator itr = selection.Begin();
			OS_ObjectDumbPtr::Iterator end = selection.End();
			for ( ; itr != end; ++itr )
			{
				SceneNode *pSceneNode = Reflect::SafeCast<SceneNode>( *itr );
				if (pSceneNode)
				{
					if ( pSceneNode->IsSelected() )
					{
						newSelection.Remove( *itr );
					}
					else
					{
						newSelection.Append( *itr );
					}
				}
			}

			Push( m_Selection.SetItems(newSelection) );
		}

	default:
		break;
	}
}

void Scene::PickLink( const Inspect::PickLinkArgs& args )
{
	e_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Normal, SceneContexts::Picking ) );
	m_PickData = args.m_DataBinding;
}

void Scene::SelectLink( const Inspect::SelectLinkArgs& args )
{
	Editor::SceneNode* o = FindNode( args.m_ID );

	if ( o == NULL )
	{
		TUID id;
		if (id.FromString(args.m_ID))
		{
			M_SceneNodeSmartPtr::const_iterator found = m_Nodes.find( id );
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

	Editor::Layer* layer = Reflect::SafeCast<Editor::Layer>( o );

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
	std::string str;

	if ( args.m_Items.empty() )
	{
		TUID null;
		null.ToString(str);
		args.m_Items.push_back( Inspect::PopulateItem( TXT( "NULL" ), str) );
	}

	std::string suffix;

	if ( !IsFocused() )
	{
		// TODO: Get rid of STL
		std::stringstream ss;
		ss << " (" << *m_Definition->GetName() << ")";

		suffix = ss.str();
	}

	M_SceneNodeSmartPtr::const_iterator nodeItr = m_Nodes.begin();
	M_SceneNodeSmartPtr::const_iterator nodeEnd = m_Nodes.end();
	for ( ; nodeItr != nodeEnd; ++nodeItr )
	{
		nodeItr->second->GetID().ToString(str);
		args.m_Items.push_back( Inspect::PopulateItem (nodeItr->second->GetName() + suffix, str) );
	}
}

void Scene::SetHighlight(const SetHighlightArgs& args)
{
	HELIUM_EDITOR_SCENE_SCOPE_TIMER();

	ClearHighlight( ClearHighlightArgs (false) );

	if (args.m_Pick == NULL)
		return;

	OS_SceneNodeDumbPtr selection;

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
				SceneNode* node = Reflect::SafeCast<SceneNode>((*itr)->GetHitObject());
				if (node)
				{
					HELIUM_ASSERT( node->GetOwner() == this );

					// add it to the new selection list
					selection.Append(node);
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
				SceneNode* node = Reflect::SafeCast<SceneNode>((*itr)->GetHitObject());
				if (node)
				{
					HELIUM_ASSERT( node->GetOwner() == this );

					// add it to the new selection list
					selection.Append(node);
				}
			}
			break;
		}
	}

	OS_ObjectDumbPtr objects;
	for (OS_SceneNodeDumbPtr::Iterator iter = selection.Begin();
		iter != selection.End(); ++iter)
	{
		objects.Append(*iter);
	}

	if (m_Tool && !m_Tool->ValidateSelection (objects) )
	{
		return;
	}

	m_Highlighted = selection;

	Editor::HierarchyNode* first = NULL;
	OS_SceneNodeDumbPtr::Iterator itr = m_Highlighted.Begin();
	OS_SceneNodeDumbPtr::Iterator end = m_Highlighted.End();
	for ( ; itr != end; ++itr )
	{
		Editor::HierarchyNode* node = Reflect::SafeCast<Editor::HierarchyNode>(*itr);

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
		std::string status = first->GetName();
		std::string desc = first->GetDescription();

		if (!desc.empty())
		{
			status += TXT(" (") + desc + TXT( ")" );
		}

		e_StatusChanged.Raise( status );
	}
	else if (m_Highlighted.Size() > 1)
	{
		std::ostringstream str;
		str << m_Highlighted.Size() << " items";
		e_StatusChanged.Raise( str.str() );
	}
}

void Scene::ClearHighlight( const ClearHighlightArgs& args )
{
	OS_SceneNodeDumbPtr::Iterator itr = m_Highlighted.Begin();
	OS_SceneNodeDumbPtr::Iterator end = m_Highlighted.End();
	for ( ; itr != end; ++itr )
	{
		Editor::HierarchyNode* node = Reflect::SafeCast<Editor::HierarchyNode>(*itr);

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

void Scene::Evaluate(bool silent)
{
	HELIUM_EDITOR_SCENE_EVALUATE_SCOPE_TIMER();

	Editor::EvaluateResult result = m_Graph->EvaluateGraph(silent);
}

bool Scene::Push(const UndoCommandPtr& command)
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

	UndoCommandArgs args ( this, command );
	d_UndoCommand.Invoke( args );
	return true;
}

Editor::SceneNode* Scene::FindNode(const TUID& id)
{
	Editor::SceneNode* node = NULL;

	// 
	// Guid
	// 

	M_SceneNodeSmartPtr::const_iterator findGuid = m_Nodes.find( id );
	if ( findGuid != m_Nodes.end() )
	{
		node = findGuid->second;
	}
	else if ( m_RemappedIDs.size() > 0 )
	{
		// Immediately after an import, there may be some remapped IDs that 
		// can be searched to find the node.
		std::map< TUID, TUID >::const_iterator findRemap = m_RemappedIDs.find( id );
		if ( findRemap != m_RemappedIDs.end() )
		{
			M_SceneNodeSmartPtr::const_iterator findRepeat = m_Nodes.find( findRemap->second );
			if ( findRepeat != m_Nodes.end() )
			{
				node = findRepeat->second;
			}
		}
	}

	return node;
}

Editor::SceneNode* Scene::FindNode(const std::string& name)
{
	Editor::SceneNode* node = NULL;

	// 
	// Name
	// 

	M_NameToSceneNodeDumbPtr::const_iterator findName = m_Names.find( name );
	if ( findName != m_Names.end() )
	{
		node = findName->second;
	}

	return node;
}

void Scene::ChangeStatus(const std::string& status)
{
	e_StatusChanged.Raise( status );
}

void Scene::RefreshSelection()
{
	m_Selection.Refresh();
}

void Scene::PropertyChanging( const Inspect::ControlChangingArgs& args )
{
	if ( args.m_Preview )
	{
		return;
	}

	if ( args.m_Control->GetBinding() )
	{
		UndoCommandPtr command = args.m_Control->GetBinding()->GetUndoCommand();

		if ( command )
		{
			args.m_Veto = !Push( command );
			return;
		}
	}

	args.m_Veto = !IsEditable();
}

void Scene::PropertyChanged( const Inspect::ControlChangedArgs& args )
{
	Execute(false);
}

void Scene::SelectionChanging( const SelectionChangingArgs& args )
{
	HELIUM_EDITOR_SCENE_SCOPE_TIMER();

	bool allow = true;

	if (m_PickData.ReferencesObject())
	{
		if (!args.m_Selection.Empty())
		{
			Editor::SceneNode* node = Reflect::SafeCast<Editor::SceneNode>( args.m_Selection.Front() );

			if (node)
			{
				std::string str;
				node->GetID().ToString(str);

				// set the picked object ID
				Inspect::StringDataBinding* data = Inspect::CastDataBinding< Inspect::StringDataBinding, Inspect::DataBindingTypes::String >( m_PickData );
				if ( data && Push( data->GetUndoCommand() ) )
				{
					data->Set( str );
				}

				// eat the selection
				allow = false;

				// refresh the attributes
				Execute(false);
			}
		}

		e_SceneContextChanged.Raise( SceneContextChangeArgs( SceneContexts::Picking, SceneContexts::Normal ) );
		m_PickData = NULL;
	}

	args.m_Veto = !allow;
}

void Scene::SelectionChanged( const SelectionChangeArgs& args )
{
	HELIUM_EDITOR_SCENE_SCOPE_TIMER();

	m_ValidSmartDuplicateMatrix = false;

	std::ostringstream str;
	if ( args.m_Selection.Empty() )
	{
		str << "Selection cleared";
	}
	else
	{
		str << "Selected " << args.m_Selection.Size() << " objects";
	}

	e_StatusChanged.Raise( str.str() );

	Execute(false);
}

///////////////////////////////////////////////////////////////////////////////
// Helper function to return the closest ancestor that is a parent of all the
// specified hierarchy nodes.
// 
Editor::HierarchyNode* Scene::GetCommonParent( const V_HierarchyNodeDumbPtr& nodes )
{
	Editor::HierarchyNode* commonParent = NULL;

	for ( V_HierarchyNodeDumbPtr::const_iterator itr = nodes.begin(), end = nodes.end(); itr != end; ++itr )
	{
		Editor::HierarchyNode* node = *itr;
		Editor::HierarchyNode* currentParent = node->GetParent();
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
				Editor::HierarchyNode* parent = commonParent;
				Editor::HierarchyNode* searchNode = node;
				while ( !found && parent )
				{
					Editor::HierarchyNode* searchParent = searchNode;
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

	// TODO: This should probably return NULL if commonParent == m_Root
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
		Editor::HierarchyNode* outer = *outerItr;

		if ( !outer )
		{
			continue;
		}

		// are we the child of another node in the selection?
		bool childNode = false;

		// start walking up the hierarchy starting from my parent
		Editor::HierarchyNode* parent = outer->GetParent();

		// while we haven't hit the root
		while ( parent != m_Root )
		{
			// check each item in the selection, looking for a match to 'parent'
			V_HierarchyNodeDumbPtr::const_iterator innerItr = nodes.begin();
			V_HierarchyNodeDumbPtr::const_iterator innerEnd = nodes.end();
			for ( ; innerItr != innerEnd; ++innerItr )
			{
				Editor::HierarchyNode* inner = *innerItr;

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

void Scene::GetSelectionParents(OS_ObjectDumbPtr& parents)
{
	parents.Clear();

	// for each candidate item we want to test that has no other parent in the list
	OS_ObjectDumbPtr::Iterator outerItr = m_Selection.GetItems().Begin();
	OS_ObjectDumbPtr::Iterator outerEnd = m_Selection.GetItems().End();
	for ( ; outerItr != outerEnd; ++outerItr )
	{
		Editor::HierarchyNode* outer = Reflect::SafeCast< Editor::HierarchyNode > (*outerItr);

		if ( !outer )
		{
			continue;
		}

		// are we the child of another node in the selection?
		bool childNode = false;

		// start walking up the hierarchy starting from my parent
		Editor::HierarchyNode* parent = outer->GetParent();
		HELIUM_ASSERT( parent );

		// while we haven't hit the root
		while ( parent && parent != m_Root && !childNode )
		{
			// check each item in the selection, looking for a match to 'parent'
			OS_ObjectDumbPtr::Iterator innerItr = m_Selection.GetItems().Begin();
			OS_ObjectDumbPtr::Iterator innerEnd = m_Selection.GetItems().End();
			for ( ; innerItr != innerEnd; ++innerItr )
			{
				Editor::HierarchyNode* inner = Reflect::SafeCast< Editor::HierarchyNode > (*innerItr);

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
void Scene::GetFlattenedSelection(OS_ObjectDumbPtr& selection)
{
	selection.Clear();

	const OS_ObjectDumbPtr& selectedItems = m_Selection.GetItems();
	OS_ObjectDumbPtr::Iterator it = selectedItems.Begin();
	OS_ObjectDumbPtr::Iterator end = selectedItems.End();
	for ( ; it != end; ++it )
	{
		const Reflect::ObjectPtr& item = *it;

		selection.Append( item );

		Editor::PivotTransform* group = Reflect::SafeCast< Editor::PivotTransform >( item );
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
// Flattends the hierarchy of an Editor::HierarchyNode into a flat list
//
void Scene::GetFlattenedHierarchy(Editor::HierarchyNode* node, OS_HierarchyNodeDumbPtr& items)
{
	const OS_HierarchyNodeDumbPtr& children = node->GetChildren();
	OS_HierarchyNodeDumbPtr::Iterator childIt = children.Begin();
	OS_HierarchyNodeDumbPtr::Iterator childEnd = children.End();
	for ( ; childIt != childEnd; ++childIt )
	{
		items.Append( *childIt );

		Editor::HierarchyNode* group = Reflect::SafeCast< Editor::HierarchyNode >( *childIt );
		if ( group )
		{
			GetFlattenedHierarchy( group, items );
		}
	}
}

void Scene::GetSelectedTransforms( V_Matrix4& transforms )
{
	OS_ObjectDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
	OS_ObjectDumbPtr::Iterator end = m_Selection.GetItems().End();
	for (int i=0; itr != end; itr++, i++)
	{
		Editor::Transform* transform = Reflect::SafeCast< Editor::Transform >( *itr );

		if (transform)
		{
			transforms.push_back( transform->GetGlobalTransform() );
		}
	}
}

UndoCommandPtr Scene::SetSelectedTransforms( const V_Matrix4& transforms )
{
	if (m_Selection.GetItems().Empty())
	{
		return NULL;
	}

	BatchUndoCommandPtr batch = new BatchUndoCommand ();

	OS_ObjectDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
	OS_ObjectDumbPtr::Iterator end = m_Selection.GetItems().End();
	for (size_t i=0; itr != end && i<transforms.size(); itr++, i++)
	{
		Editor::Transform* transform = Reflect::SafeCast< Editor::Transform >( *itr );

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
UndoCommandPtr Scene::SetHiddenSelected( bool hidden )
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

	BatchUndoCommandPtr batch = new BatchUndoCommand ();

	OS_ObjectDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
	OS_ObjectDumbPtr::Iterator end = m_Selection.GetItems().End();
	for ( ; itr != end; ++itr )
	{
		Editor::HierarchyNode* hierarchyNode = Reflect::SafeCast<Editor::HierarchyNode>( *itr );
		if ( hierarchyNode )
		{
			if (hidden)
			{
				m_LastHidden.insert( hierarchyNode->GetID() );
			}

			PropertyUndoCommand<bool>* command = 
				new PropertyUndoCommand<bool> ( new Helium::MemberProperty<Editor::HierarchyNode, bool> (hierarchyNode, &Editor::HierarchyNode::IsHidden, &Editor::HierarchyNode::SetHidden), hidden ) ; 

			command->SetSignificant( false ); 
			batch->Push(command); 
		}
	}

	return batch->IsEmpty() ? NULL : batch;
}

UndoCommandPtr Scene::SetHiddenUnrelated( bool hidden )
{
	Log::Print( TXT( "\n o SetHiddenUnrelated( %s )\n" ), hidden ? TXT( "true" ) : TXT( "false" ) );

	if (hidden)
	{
		m_LastHidden.clear();
	}


	//
	// Bake selected root parents
	//

	OS_SceneNodeDumbPtr relatives;
	for ( OS_ObjectDumbPtr::Iterator itr = m_Selection.GetItems().Begin(), end = m_Selection.GetItems().End(); itr != end; ++itr )
	{
		Editor::HierarchyNode* hierarchyNode = Reflect::SafeCast<Editor::HierarchyNode>( *itr );
		if ( hierarchyNode )
		{
			SceneNode *pSceneNode = Reflect::AssertCast<SceneNode>( *itr );
			relatives.Append( pSceneNode );

			Editor::HierarchyNode* parent = hierarchyNode->GetParent();
			while ( parent && parent != m_Root )
			{
				relatives.Append( parent );
				parent = parent->GetParent();
			}

			HierarchyChildTraverser traverser;
			hierarchyNode->TraverseHierarchy( &traverser );

			OS_SceneNodeDumbPtr::Iterator childItr = traverser.m_Children.Begin();
			OS_SceneNodeDumbPtr::Iterator childEnd = traverser.m_Children.End();
			for ( ; childItr != childEnd; ++childItr )
			{
				relatives.Append( *childItr );
			}
		}
	}


	//
	// Set visibility of the entire scene
	//

	BatchUndoCommandPtr batch = new BatchUndoCommand ();

	for ( M_SceneNodeSmartPtr::const_iterator itr = m_Nodes.begin(), end = m_Nodes.end(); itr != end; ++itr )
	{
		Editor::HierarchyNode* node = Reflect::AssertCast<Editor::HierarchyNode>( itr->second );

		if (node && !relatives.Contains(node))
		{
			if (hidden && !node->IsHidden())
			{
				m_LastHidden.insert( node->GetID() );
			}

			PropertyUndoCommand<bool>* command = 
				new PropertyUndoCommand<bool> ( new Helium::MemberProperty<Editor::HierarchyNode, bool> (node, &Editor::HierarchyNode::IsHidden, &Editor::HierarchyNode::SetHidden), hidden ); 
			command->SetSignificant(false); 
			batch->Push( command ); 

		}
	}

	return batch->IsEmpty() ? NULL : batch;
}

UndoCommandPtr Scene::ShowLastHidden()
{
	Log::Print( TXT( "\n o ShowLastHidden()\n" ) );

	if (m_LastHidden.empty())
	{
		return NULL;
	}

	BatchUndoCommandPtr batch = new BatchUndoCommand ();

	std::set<TUID>::const_iterator itr = m_LastHidden.begin();
	std::set<TUID>::const_iterator end = m_LastHidden.end();
	for ( ; itr != end; ++itr )
	{
		Editor::HierarchyNode* hierarchyNode = Reflect::SafeCast<Editor::HierarchyNode>( FindNode( *itr ) );
		if ( hierarchyNode )
		{
			PropertyUndoCommand<bool>* command = 
				new PropertyUndoCommand<bool> ( new Helium::MemberProperty<Editor::HierarchyNode, bool> (hierarchyNode, &Editor::HierarchyNode::IsHidden, &Editor::HierarchyNode::SetHidden), false ); 

			command->SetSignificant(false); 
			batch->Push( command );
		}
	}

	m_LastHidden.clear();

	return batch->IsEmpty() ? NULL : batch;
}

UndoCommandPtr Scene::SelectSimilar()
{
	OS_ObjectDumbPtr selection;

	OS_ObjectDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
	OS_ObjectDumbPtr::Iterator end = m_Selection.GetItems().End();
	for ( ; itr != end; ++itr )
	{
		Editor::HierarchyNode* node = Reflect::SafeCast<Editor::HierarchyNode>( *itr );

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

UndoCommandPtr Scene::DeleteSelected()
{
	// since all of our children are going to be deleted with us, discard selected children from the working set
	OS_ObjectDumbPtr roots;
	GetSelectionParents(roots);

	if (roots.Empty())
	{
		return NULL;
	}

	BatchUndoCommandPtr batch = new BatchUndoCommand();

	batch->Push( m_Selection.Clear() );

	OS_ObjectDumbPtr::Iterator itr = roots.Begin();
	OS_ObjectDumbPtr::Iterator end = roots.End();
	for ( ; itr != end; ++itr )
	{
		SceneNodePtr node = Reflect::SafeCast< Editor::SceneNode >( *itr );
		if ( node )
		{
			batch->Push( new SceneNodeExistenceCommand( ExistenceActions::Remove, this, node ) ); 
		}
	}

	Execute(false);

	return batch->IsEmpty() ? NULL : batch;
}

UndoCommandPtr Scene::ParentSelected()
{
	OS_ObjectDumbPtr selection;
	GetSelectionParents( selection );

	if (selection.Empty())
	{
		return NULL;
	}

	V_HierarchyNodeDumbPtr children;

	// Go through the selection list and pull out any hierarchy nodes
	OS_ObjectDumbPtr::Iterator itr = selection.Begin();
	OS_ObjectDumbPtr::Iterator end = selection.End();
	for ( ; itr != end; ++itr )
	{
		Editor::HierarchyNode* hierarchyNode = Reflect::SafeCast< Editor::HierarchyNode >( *itr );
		if ( hierarchyNode )
		{
			children.push_back( hierarchyNode );
		}
	}

	if ( children.size() < 2 )
	{
		return NULL;
	}

	Editor::HierarchyNode* parent = children.back();
	children.resize( children.size() - 1 );

	BatchUndoCommandPtr batch = new BatchUndoCommand ();

	// Reparent the selected items under the parent node (last selected one)
	for ( V_HierarchyNodeDumbPtr::const_iterator itr = children.begin(), end = children.end(); itr != end; ++itr )
	{
		Editor::HierarchyNode* hierarchyNode = *itr;
		batch->Push( new ParentCommand( hierarchyNode, parent ) );
	}

	return batch->IsEmpty() ? NULL : batch;
}

UndoCommandPtr Scene::UnparentSelected()
{
	V_HierarchyNodeDumbPtr children;

	// Go through the selection list and pull out any hierarchy nodes
	OS_ObjectDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
	OS_ObjectDumbPtr::Iterator end = m_Selection.GetItems().End();
	for ( ; itr != end; ++itr )
	{
		Editor::HierarchyNode* hierarchyNode = Reflect::SafeCast< Editor::HierarchyNode >( *itr );
		if ( hierarchyNode )
		{
			children.push_back( hierarchyNode );
		}
	}

	if ( children.empty() )
	{
		return NULL;
	}

	BatchUndoCommandPtr batch = new BatchUndoCommand ();

	// Reparent the selected items under the root
	for ( V_HierarchyNodeDumbPtr::const_iterator itr = children.begin(), end = children.end(); itr != end; ++itr )
	{
		Editor::HierarchyNode* hierarchyNode = *itr;
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
UndoCommandPtr Scene::GroupSelected()
{
	OS_ObjectDumbPtr selection;
	GetSelectionParents( selection );

	if (selection.Empty())
	{
		return NULL;
	}

	V_HierarchyNodeDumbPtr selectedHierarchyNodes;

	// Go through the selection list and pull out any hierarchy nodes
	OS_ObjectDumbPtr::Iterator itr = selection.Begin();
	OS_ObjectDumbPtr::Iterator end = selection.End();
	for ( ; itr != end; ++itr )
	{
		Editor::HierarchyNode* hierarchyNode = Reflect::SafeCast< Editor::HierarchyNode >( *itr );
		if ( hierarchyNode )
		{
			selectedHierarchyNodes.push_back( hierarchyNode );
		}
	}

	if ( selectedHierarchyNodes.empty() )
	{
		return NULL;
	}

	BatchUndoCommandPtr batch = new BatchUndoCommand ();

	Vector3 pos;
	const Editor::Transform* transform = selectedHierarchyNodes.back()->GetTransform();
	HELIUM_ASSERT( transform );
	if ( transform )
	{
		const Matrix4& globalTransform = transform->GetGlobalTransform();
		pos.x = globalTransform.t.x;
		pos.y = globalTransform.t.y;
		pos.z = globalTransform.t.z;
	}

	// Create the new group
	Editor::PivotTransform* group = new Editor::PivotTransform();
	group->SetOwner( this );
	group->Initialize();

	// Get a decent name
	group->Rename( TXT( "group1" ) );

	// Make sure the new group is under the common parent for the selected nodes.
	group->SetParent( GetCommonParent( selectedHierarchyNodes ) );

	// This will re-compute the local components
	group->SetGlobalTransform( Matrix4 (pos) );

	// Update the object
	group->Evaluate( GraphDirections::Downstream );

	batch->Push( new SceneNodeExistenceCommand( ExistenceActions::Add, this, group ) );

	// Reparent the selected items under the new group
	for ( V_HierarchyNodeDumbPtr::const_iterator itr = selectedHierarchyNodes.begin(), end = selectedHierarchyNodes.end(); itr != end; ++itr )
	{
		Editor::HierarchyNode* hierarchyNode = *itr;
		batch->Push( new ParentCommand( hierarchyNode, group ) );
	}

	// Select the newly created group
	batch->Push( m_Selection.SetItem( group ) );

	return batch->IsEmpty() ? NULL : batch;
}

UndoCommandPtr Scene::UngroupSelected()
{
	if ( m_Selection.GetItems().Empty() )
	{
		return NULL;
	}

	BatchUndoCommandPtr batch = new BatchUndoCommand ();

	OS_ObjectDumbPtr newSelection;

	bool warn = false;
	// For each selected item
	OS_ObjectDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
	OS_ObjectDumbPtr::Iterator end = m_Selection.GetItems().End();
	for ( ; itr != end; ++itr )
	{
		// If the item is a group (pivot transform)
		Editor::SceneNode* sceneNode = Reflect::SafeCast< Editor::SceneNode >( *itr );

		if ( sceneNode && sceneNode->GetMetaClass() == Reflect::GetMetaClass<Editor::PivotTransform>() )
		{
			Editor::PivotTransform* group = Reflect::AssertCast< Editor::PivotTransform >( sceneNode );

			// If the group has children, parent the children under the group's parent (their grandparent)
			if ( group->GetChildren().Size() > 0 )
			{
				// Need to operate on a copy of the children since we are going to change the original
				// group as we iterate over it.
				OS_HierarchyNodeDumbPtr copyOfChildren = group->GetChildren();
				for ( OS_HierarchyNodeDumbPtr::Iterator itr = copyOfChildren.Begin(), end = copyOfChildren.End(); itr != end; ++itr )
				{
					Editor::HierarchyNode* child = *itr;

					// Push the parent command into the queue
					batch->Push( new ParentCommand( child, group->GetParent() ) );

					// Add the child to the new selection list
					newSelection.Append( child );
				}
			}

			// Delete the group
			batch->Push( new SceneNodeExistenceCommand( ExistenceActions::Remove, this, group ) );
		}
		else
		{
			std::string msg = TXT( "The Ungroup command only works on groups. The node '" ) + sceneNode->GetName() + TXT( "' is not a group.\n" );
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

UndoCommandPtr Scene::CenterSelected()
{
	if (m_Selection.GetItems().Empty())
	{
		return NULL;
	}

	BatchUndoCommandPtr batch = new BatchUndoCommand ();

	OS_ObjectDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
	OS_ObjectDumbPtr::Iterator end = m_Selection.GetItems().End();
	for (int i=0; itr != end; itr++, i++)
	{
		Editor::Transform* transform = Reflect::SafeCast< Editor::Transform >( *itr );

		if (transform)
		{
			batch->Push( transform->CenterTransform() );
		}
	}

	// we don't end in a selection, so execute
	Execute(false);

	return batch->IsEmpty() ? NULL : batch;
}

UndoCommandPtr Scene::DuplicateSelected()
{
	// since all of our children are going to be duplicated with us, discard selected children from the working set
	OS_ObjectDumbPtr roots;
	GetSelectionParents(roots);

	// test for nothing to do
	if (roots.Empty())
	{
		return NULL;
	}

	BatchUndoCommandPtr batch = new BatchUndoCommand ();

	OS_ObjectDumbPtr newSelection;

	OS_ObjectDumbPtr::Iterator itr = roots.Begin();
	OS_ObjectDumbPtr::Iterator end = roots.End();
	for ( ; itr != end; ++itr )
	{
		Editor::HierarchyNode* node = Reflect::SafeCast< Editor::HierarchyNode > (*itr);

		if (!node)
		{
			continue;
		}

		HierarchyNodePtr duplicate = node->Duplicate();

		newSelection.Append(duplicate);

		batch->Push( new SceneNodeExistenceCommand( ExistenceActions::Add, this, duplicate ) );

		duplicate->SetParent( node->GetParent() );

		// make sure the new nodes are initialized
		duplicate->InitializeHierarchy();
	}

	// setting the selection will invalidate the flag for having a valid smart duplicate matrix
	batch->Push( m_Selection.SetItems (newSelection) );

	// set the transform for smart duplicate, if there was only one selection
	if (roots.Size() == 1)
	{
		Editor::HierarchyNode* node = Reflect::SafeCast< Editor::HierarchyNode >(roots.Front());   
		if (node)
		{
			m_SmartDuplicateMatrix = node->GetTransform()->GetGlobalTransform();
			m_ValidSmartDuplicateMatrix = true;
		}
	}

	return batch->IsEmpty() ? NULL : batch;
}

UndoCommandPtr Scene::SmartDuplicateSelected()
{
	// if we do not have a valid duplicate matrix, then just do duplicate selected
	if (!m_ValidSmartDuplicateMatrix)
	{
		return DuplicateSelected();
	}

	// since all of our children are going to be duplicated with us, discard selected children from the working set
	OS_ObjectDumbPtr roots;
	GetSelectionParents(roots);

	// only operate on a single object!
	if (roots.Size() != 1)
	{
		return DuplicateSelected();
	}

	Editor::Transform* node = Reflect::SafeCast< Editor::Transform >(roots.Front());

	if (!node)
	{
		return DuplicateSelected();
	}

	HierarchyNodePtr duplicate = node->Duplicate();

	Editor::Transform* transform = Reflect::AssertCast<Editor::Transform>( duplicate );

	BatchUndoCommandPtr batch = new BatchUndoCommand ();

	// transform the duplicate based off of the previous duplicate matrix
	Matrix4 matrix = transform->GetGlobalTransform() * m_SmartDuplicateMatrix.Inverted() * transform->GetGlobalTransform();

	// save this for the next smart transform matrix
	m_SmartDuplicateMatrix = transform->GetGlobalTransform();

	// add the duplicate to the scene
	batch->Push( new SceneNodeExistenceCommand( ExistenceActions::Add, this, duplicate ) );

	// parent it to the original nodes' parent
	batch->Push( new ParentCommand( duplicate, node->GetParent() ) );

	// set the global transform for the duplicate object
	batch->Push( new PropertyUndoCommand<Matrix4> ( new Helium::MemberProperty<Editor::Transform, Matrix4> (transform, &Editor::Transform::GetGlobalTransform, &Editor::Transform::SetGlobalTransform), matrix ) );

	// make sure the new nodes are initialized
	duplicate->InitializeHierarchy();

	// setting the selection will invalidate the flag for having a valid smart duplicate matrix
	batch->Push( m_Selection.SetItem(duplicate) );

	// reset the flag here
	m_ValidSmartDuplicateMatrix = true;

	return batch->IsEmpty() ? NULL : batch;
}

UndoCommandPtr Scene::SnapSelectedToCamera()
{
	if (m_Selection.GetItems().Empty())
	{
		return NULL;
	}

	BatchUndoCommandPtr batch = new BatchUndoCommand ();

	Matrix4 m = Matrix4 ( AngleAxis( static_cast< float32_t >( HELIUM_PI ), Vector3::BasisY ) ) * m_View->GetCamera()->GetInverseView();

	OS_ObjectDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
	OS_ObjectDumbPtr::Iterator end = m_Selection.GetItems().End();
	for (int i=0; itr != end; itr++, i++)
	{
		Editor::Transform* transform = Reflect::SafeCast< Editor::Transform >( *itr );

		if (transform)
		{
			batch->Push( new PropertyUndoCommand<Matrix4> ( new Helium::MemberProperty<Editor::Transform, Matrix4> (transform, &Editor::Transform::GetGlobalTransform, &Editor::Transform::SetGlobalTransform), m ) );
		}
	}

	// we don't end in a selection, so execute
	Execute(false);

	return batch->IsEmpty() ? NULL : batch;
}

UndoCommandPtr Scene::SnapCameraToSelected()
{
	if (m_Selection.GetItems().Empty())
	{
		return NULL;
	}

	Editor::Transform* transform = Reflect::SafeCast< Editor::Transform >( *m_Selection.GetItems().Begin() );

	if (transform)
	{
		Matrix4 m = Matrix4 ( AngleAxis( static_cast< float32_t >( HELIUM_PI ), Vector3::BasisY ) ) * transform->GetGlobalTransform();

		m_View->GetCamera()->SetTransform( m );
	}

	// we don't end in a selection, so execute
	Execute(false);

	return NULL;
}

void Scene::FrameSelected()
{
	bool found = false;
	AlignedBox box;

	OS_ObjectDumbPtr::Iterator itr = GetSelection().GetItems().Begin();
	OS_ObjectDumbPtr::Iterator end = GetSelection().GetItems().End();
	for ( ; itr != end; ++itr )
	{
		Editor::HierarchyNode* node = Reflect::SafeCast<Editor::HierarchyNode>(*itr);
		if (node)
		{
			box.Merge(node->GetGlobalHierarchyBounds());
			found = true;
			continue;
		}

		CurveControlPoint* point = Reflect::SafeCast<CurveControlPoint>(*itr);
		if (point)
		{
			Vector3 p = point->GetPosition();
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
	Editor::Transform* first = NULL;
	Editor::Transform* second = NULL;

	OS_ObjectDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
	OS_ObjectDumbPtr::Iterator end = m_Selection.GetItems().End();
	for ( ; itr != end; ++itr )
	{
		Editor::Transform* t = Reflect::SafeCast<Editor::Transform>( *itr );

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
		std::ostringstream str;

		Vector3 v = Vector3 (first->GetGlobalTransform().t.x, first->GetGlobalTransform().t.y, first->GetGlobalTransform().t.z) -
			Vector3 (second->GetGlobalTransform().t.x, second->GetGlobalTransform().t.y, second->GetGlobalTransform().t.z);

		float distance = v.Length();

		str << first->GetName() << " is " << distance << " meters from " << second->GetName();

		e_StatusChanged.Raise( SceneStatusChangeArgs (str.str()) );
	}
	else
	{
		e_StatusChanged.Raise( SceneStatusChangeArgs( TXT( "Please select 2 placed objects and try again" ) ) );
	}
}

UndoCommandPtr Scene::PickWalkUp()
{
	if (m_Selection.GetItems().Empty())
	{
		return NULL;
	}

	OS_ObjectDumbPtr newSelection;

	OS_ObjectDumbPtr::Iterator itr = m_Selection.GetItems().Begin();
	OS_ObjectDumbPtr::Iterator end = m_Selection.GetItems().End();
	for ( ; itr != end; ++itr )
	{
		Editor::HierarchyNode* node = Reflect::SafeCast<Editor::HierarchyNode>(*itr);

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

UndoCommandPtr Scene::PickWalkDown()
{
	if (m_Selection.GetItems().Empty())
	{
		return NULL;
	}

	Editor::HierarchyNode* node = Reflect::SafeCast<Editor::HierarchyNode>(m_Selection.GetItems().Front());

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

UndoCommandPtr Scene::PickWalkSibling(bool forward)
{
	if (m_Selection.GetItems().Empty())
	{
		return NULL;
	}

	Editor::HierarchyNode* node = Reflect::SafeCast<Editor::HierarchyNode>(m_Selection.GetItems().Front());

	if (node == NULL)
	{
		return NULL;
	}

	if (node->GetParent() != NULL)
	{
		const OS_HierarchyNodeDumbPtr& children = node->GetParent()->GetChildren();

		if (!children.Empty())
		{
			typedef std::map<std::string, Editor::HierarchyNode*> M_NameToHierarchyNodeDumbPtr;

			M_NameToHierarchyNodeDumbPtr sortedChildren;
			{
				OS_HierarchyNodeDumbPtr::Iterator itr = children.Begin();
				OS_HierarchyNodeDumbPtr::Iterator end = children.End();
				for ( ; itr != end; ++itr )
				{
					sortedChildren[ (*itr)->GetName() ] = *itr;
				}
			}

			typedef std::vector<Editor::HierarchyNode*> V_HierarchyNodeDumbPtr;

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

void Scene::ViewPreferencesChanged( const Reflect::ObjectChangeArgs& args )
{
	if ( args.m_Field == m_View->GetSettingsManager()->GetSettings< ViewportSettings >()->ColorModeField() )
	{
		Execute( false );
	}
}

TUID Scene::GetRemappedID( tuid nodeId )
{
	std::map< TUID, TUID >::iterator itr = m_RemappedIDs.begin();
	std::map< TUID, TUID >::iterator itrEnd = m_RemappedIDs.end();

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
