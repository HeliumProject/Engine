#pragma once

#include "Editor/API.h"

#include "Foundation/Automation/Event.h"
#include "Foundation/TUID.h"
#include "Foundation/Reflect/Archive.h"
#include "Foundation/Reflect/Version.h"

#include "Selection.h"

#include "Application/Inspect/Data/Data.h"
#include "Application/Inspect/Controls/Canvas.h"
#include "PropertiesGenerator.h"

#include "Application/Undo/ExistenceCommand.h"
#include "Application/Undo/Queue.h"

#include "SceneGraph.h"

#include "Pick.h"
#include "Tool.h"

#include "SceneDocument.h"
#include "SceneNodeType.h"
#include "SceneNode.h"

#include "Transform.h"

#include "Pipeline/Content/NodeVisibility.h"
#include "Pipeline/Content/SceneVisibility.h"

namespace Content
{
  class SceneNode;
}

namespace Editor
{
  // 
  // Forwards
  // 

  class Layer;
  class SceneManager;
  class PickVisitor;
  struct SceneChangeArgs;


  // 
  // Import options
  // 

  // Differentiate between replacing the entire contents of a file, and bringing in new ones
  namespace ImportActions
  {
    // Mutually exclusive operations
    enum ImportAction
    {
      Load,   // Scene must be empty, we are bringing in all the nodes
      Import, // Leave whatever nodes are currently in the scene and bring in new ones
    };
  }
  typedef ImportActions::ImportAction ImportAction;

  // Options specific to an import action
  namespace ImportFlags
  {
    enum ImportFlag
    {
      None = 0,

      Merge   = 1 << 1, // If a node matches one being imported, replace node in this scene with the new one
      Select  = 1 << 2, // Select the imported nodes so the user can easily identify them

      Default = None,
    };

    static bool HasFlag( u32 flags, ImportFlag singleFlag )
    {
      return ( flags & singleFlag ) == singleFlag;
    }
  }
  typedef ImportFlags::ImportFlag ImportFlag;

  // 
  // Export options
  //

  namespace ExportFlags
  {
    enum ExportFlag
    {
      None = 0,

      SelectedNodes         = 1 << 1, // Only export nodes that are selected
      MaintainHierarchy     = 1 << 2, // Include Parents and children
      MaintainDependencies  = 1 << 3, // Include layers, etc...

      Default = MaintainHierarchy | MaintainDependencies,
    };

    static bool HasFlag( u32 flags, ExportFlag singleFlag )
    {
      return ( flags & singleFlag ) == singleFlag;
    }
  }
  typedef ExportFlags::ExportFlag ExportFlag;

  typedef std::vector< Math::AlignedBox > V_AlignedBox;

  struct ExportArgs
  {
    u32 m_Flags;
    V_AlignedBox m_Bounds;

    ExportArgs()
      : m_Flags ( ExportFlags::Default )
    {

    }

    ExportArgs( u32 flags )
      : m_Flags ( flags )
    {

    }
  };


  //
  // Sometimes we need to update the window title or status bar for keeping the user up to date
  //

  // update the title to show loading progress (in percent)
  struct TitleChangeArgs
  {
    const tstring& m_Title;

    TitleChangeArgs ( const tstring& title )
      : m_Title (title)
    {

    }
  };
  typedef Helium::Signature< void, const TitleChangeArgs& > TitleChangeSignature;

  // update the status bar of the frame of this instance of the scene editor
  struct SceneStatusChangeArgs
  {
    const tstring& m_Status;

    SceneStatusChangeArgs ( const tstring& status )
      : m_Status (status)
    {

    }
  };
  typedef Helium::Signature< void, const SceneStatusChangeArgs& > SceneStatusChangeSignature;

    namespace SceneContexts
    {
        enum SceneContext
        {
            None,
            Normal,
            Loading,
            Saving,
            Picking,
        };
    }
    typedef SceneContexts::SceneContext SceneContext;

  struct SceneContextChangeArgs
  {
      SceneContext m_OldContext;
      SceneContext m_NewContext;

    SceneContextChangeArgs( SceneContext oldContext, SceneContext newContext )
      : m_OldContext( oldContext )
      , m_NewContext( newContext )
    {
    }
  };
  typedef Helium::Signature< void, const SceneContextChangeArgs& > SceneContextChangedSignature;


  //
  // Some scene data directly correllates with UI, and we need to fire events when the UI needs updating
  //

  // arguments and delegates for when a node is changed (in this case, added to or removed from the scene)
  struct NodeChangeArgs
  {
    Editor::SceneNode* m_Node;

    NodeChangeArgs( Editor::SceneNode* node )
      : m_Node( node )
    {

    }
  };
  typedef Helium::Signature< void, const NodeChangeArgs& > NodeChangeSignature;

  // node types appear in the objects window in the UI, this event helps keep it up to date
  struct NodeTypeExistenceArgs
  {
    Editor::SceneNodeType* m_NodeType;

    NodeTypeExistenceArgs( Editor::SceneNodeType* nodeType )
      : m_NodeType (nodeType)
    {

    }
  };
  typedef Helium::Signature< void, const NodeTypeExistenceArgs& > NodeTypeExistenceSignature;

  // event for loading a scene.
  struct LoadArgs
  {
    Editor::Scene* m_Scene;
    bool m_Success; // Only valid for finished loading events

    LoadArgs( Editor::Scene* scene, bool loadedOk = false )
      : m_Scene( scene )
      , m_Success( loadedOk )
    {

    }
  };
  typedef Helium::Signature< void, const LoadArgs& > LoadSignature;

  // event for loading a scene.
  struct ExecuteArgs
  {
    Editor::Scene* m_Scene;
    bool m_Interactively;

      ExecuteArgs( Editor::Scene* scene, bool interactively )
      : m_Scene( scene )
      , m_Interactively( interactively )
    {

    }
  };
  typedef Helium::Signature< void, const ExecuteArgs& > ExecuteSignature;


  //
  // This manages all the objects in a scene (typically Reflected in a file on disk)
  //  Scenes are primarily used as partitions of a level, and as a point of nesting for nested instanced types
  //  Scene implements the ObjectManager interface to facilitate undo/redo for the creation and deletion of its objects
  //  Scene implements the StatusHandler interface to update the UI as its loads data through Reflect (its means of serialzation)
  //  Scene instances are reference counted and owned by the scene manager
  //

  class Transform;
  typedef std::map< Helium::TUID, const Editor::Transform* > M_TransformConstDumbPtr;

  class LUNA_SCENE_API Scene : public Object, public Reflect::StatusHandler
  {
    //
    // Members
    //

  private:
    // file
    SceneDocumentPtr m_File;

    // id
    Helium::TUID m_Id;

    // load
    i32 m_Progress;
    Helium::HM_TUID m_RemappedIDs;
    Editor::HierarchyNode* m_ImportRoot;
    bool m_Importing;

    // scene data
    Editor::TransformPtr m_Root;

    // gives us ordered evaluation
    SceneGraphPtr m_Graph;

    // container for nodes sorted by uid
    HM_SceneNodeDumbPtr m_Nodes;

    // container for nodes sorted by name
    HM_NameToSceneNodeDumbPtr m_Names;

    // container for node types (collect instances by runtime type)
    HM_StrToSceneNodeTypeSmartPtr m_NodeTypesByName;

    // references to the node types by the compile time type id
    HMS_TypeToSceneNodeTypeDumbPtr m_NodeTypesByType;

    // selection of this scene
    Selection m_Selection;

    // highlighted items of this scene
    OS_SelectableDumbPtr m_Highlighted;

    // data for handling picks
    Inspect::DataPtr m_PickData;

    // holds undoable data
    Undo::Queue m_UndoQueue;

    // the 3d view control
    Editor::Viewport* m_View;

    // the manager for this class
    Editor::SceneManager* m_Manager;

    // the tool in use by this scene
    LToolPtr m_Tool;

    // offset matrix for smart duplicate
    Math::Matrix4 m_SmartDuplicateMatrix;

    // flag that the smart duplicate matrix is valid
    bool m_ValidSmartDuplicateMatrix;

    // the set of last hidden
    std::set<Helium::TUID> m_LastHidden;

    // set by the zone that this scene belongs to, and used for 
    // the 3D view's "color modes"
    Math::Color3 m_Color;


    //
    // Constructor
    //

  public:
      Scene( Editor::Viewport* viewport, Editor::SceneManager* manager, const SceneDocumentPtr& file );
    ~Scene();

    Helium::TUID GetId() const
    {
        return m_Id;
    }

    // is this the current scene in the editor?
    bool IsCurrent();

    // is this scene able to be edited?
    bool IsEditable();

    // allows access to the scene editor
    Editor::SceneManager* GetManager() const
    {
      return m_Manager;
    }

    // sigh, people really should only use this if they know they need to
    Undo::Queue& GetUndoQueue()
    {
      return m_UndoQueue;
    }

    // Path to the file that this scene is currently editing
    const tstring& GetFileName() const;
    tstring GetFullPath() const;
    SceneDocument* GetSceneDocument() const;

    // get the current tool in use in this scene
    const LToolPtr& GetTool();
    void SetTool(const LToolPtr& tool);

    // support for zone color
    const Math::Color3& GetColor() const;
    void SetColor( const Math::Color3& color );

    //
    // Selection
    //  These are are PER-SCENE, so they can be utilized by objects in the scene or tools
    //

    // some code wants to know if there is anything selected during undo/redo, use this
    bool HasSelection() const
    {
      return !m_Selection.GetItems().Empty();
    }

    // allows external people to modify selection
    Selection& GetSelection()
    {
      // right now we don't use the selection when undoing/redoing
      //  and would like to keep it that way so we can clear it without undo support
      //  use the above prototypes to avoid this issue
      HELIUM_ASSERT( !m_UndoQueue.IsActive() );

      return m_Selection;
    }

    // add/remove access to the selection changing event (callable while the undo queue is active)
    void AddSelectionChangingListener( const SelectionChangingSignature::Delegate& listener ) const
    {
      m_Selection.AddChangingListener( listener );
    }
    void RemoveSelectionChangingListener( const SelectionChangingSignature::Delegate& listener ) const
    {
      m_Selection.RemoveChangingListener( listener );
    }

    // add/remove access to the selection change event (callable while the undo queue is active)
    void AddSelectionChangedListener( const SelectionChangedSignature::Delegate& listener ) const
    {
      m_Selection.AddChangedListener( listener );
    }
    void RemoveSelectionChangedListener( const SelectionChangedSignature::Delegate& listener ) const
    {
      m_Selection.RemoveChangedListener( listener );
    }

    // some code wants to know if there is anything selected during undo/redo, use this
    bool HasHighlighted() const
    {
      return !m_Highlighted.Empty();
    }

    // allows external people to modify selection
    const OS_SelectableDumbPtr& GetHighlighted() const
    {
      // right now we don't use the selection when undoing/redoing
      //  and would like to keep it that way so we can clear it without undo support
      HELIUM_ASSERT( !m_UndoQueue.IsActive() );

      return m_Highlighted;
    }


    //
    // Database
    //  These comprise the most basic data structures that hold references to the objects in the scene
    //

    // the scene root node (synthetic)
    const Editor::TransformPtr& GetRoot() const
    {
      return m_Root;
    }

    // the dependency graph for ordered and efficient execution
    const SceneGraphPtr& GetGraph() const
    {
      return m_Graph;
    }

    // the nodes in the scene
    const HM_SceneNodeDumbPtr& GetNodes() const
    {
      return m_Nodes;
    }

    // the node types of the scene
    const HM_StrToSceneNodeTypeSmartPtr& GetNodeTypesByName() const
    {
      return m_NodeTypesByName;
    }

    // the node types of the scene
    const HMS_TypeToSceneNodeTypeDumbPtr& GetNodeTypesByType() const
    {
      return m_NodeTypesByType;
    }

    Editor::SceneNode* Find( const tstring& name ) const; 

    Editor::SceneNode* Get( const Helium::TUID& uid ) const
    {
      HM_SceneNodeDumbPtr::const_iterator it = m_Nodes.find( uid );

      if ( it != m_Nodes.end() )
      {
        return it->second;
      }

      return NULL;
    }


    template< class T >
    T* Get( const Helium::TUID &uid ) const
    {
      return Reflect::ObjectCast< T >( Get( uid ) );
    }

    template< class T >
    void GetAll( std::vector< T* >& objects, bool (*filterFunc )( SceneNode* ) = NULL ) const
    {
      HM_SceneNodeDumbPtr::const_iterator itor = m_Nodes.begin();
      HM_SceneNodeDumbPtr::const_iterator end  = m_Nodes.end();
      for( ; itor != end; ++itor )
      {
        if( itor->second->HasType( Reflect::GetType< T >() ) )
        {
          if( !filterFunc )
          {
            objects.push_back( Reflect::AssertCast< T >( itor->second ) );
          }
          else
          {
            if( (*filterFunc)( itor->second ) )
              objects.push_back( Reflect::AssertCast< T >( itor->second ) );
          }
        }
      }
      /*
      HMS_TypeToSceneNodeTypeDumbPtr::const_iterator found = GetNodeTypesByType().find( Reflect::GetType<T>() );
      if ( found != GetNodeTypesByType().end() )
      {
        S_SceneNodeTypeDumbPtr::const_iterator typeItr = found->second.begin();
        S_SceneNodeTypeDumbPtr::const_iterator typeEnd = found->second.end();
        for ( ; typeItr != typeEnd; ++typeItr )
        {
          Editor::SceneNodeType* nodeType = *typeItr;
          HM_SceneNodeSmartPtr::const_iterator instItr = nodeType->GetInstances().begin();
          HM_SceneNodeSmartPtr::const_iterator instEnd = nodeType->GetInstances().end();
          for ( ; instItr != instEnd; ++instItr )
          {
            objects.push_back( Reflect::AssertCast< T >( instItr->second ) );
          }
        }
      }
      */
    }

    template< class T >
    void GetAllPackages( std::vector< T* >& objects, i32 attributeType = -1, bool pack = false ) const
    {
      HM_SceneNodeDumbPtr::const_iterator itor = m_Nodes.begin();
      HM_SceneNodeDumbPtr::const_iterator end  = m_Nodes.end();
      for( ; itor != end; ++itor )
      {
        T* contentObject = Reflect::ObjectCast<T>( itor->second->GetPackage() );
        
        if (contentObject == NULL)
        {
          continue;
        }
        
        if( attributeType == -1 || contentObject->GetAttribute( attributeType ).ReferencesObject() )
        {
          if( pack )
          {
            itor->second->Pack();
          }

          objects.push_back(contentObject);
        }       
      }
    }


    //
    // GUI Elements
    //  Sometimes direct references to GUI are necessary (like the 3d view)
    //

    // the 3d view to use for drawing this scene
    Editor::Viewport* GetViewport() const
    {
      return m_View;
    }

    /// @brief function that looks up or creates a visibility entry in our database
    /// Factory function that returns the visibility settings for the ID passed in.
    /// If that ID does not have visibility settings, assigns and returns the default settings 
    /// @param ndoeId The ID of the node whos visibility settings we want
    /// @return The visibility settings
    Content::NodeVisibilityPtr GetVisibility(tuid nodeId); 

    bool GetVisibilityFile(tstring& filePath); 
    void LoadVisibility(); 
    void SaveVisibility(); 

  private: 
    Content::SceneVisibilityPtr m_VisibilityDB; 


    //
    // Load
    //

  public:
    // Open a whole scene, replacing the current one.
    bool Reload();
    bool LoadFile( const tstring& file ); 

    // Import data into this scene, possibly merging with existing nodes.
    Undo::CommandPtr ImportFile( const tstring& file, ImportAction action = ImportActions::Import, u32 importFlags = ImportFlags::None, Editor::HierarchyNode* parent = NULL, i32 importReflectType = Reflect::ReservedTypes::Invalid );
    Undo::CommandPtr ImportXML( const tstring& xml, u32 importFlags = ImportFlags::None, Editor::HierarchyNode* parent = NULL );
    SceneNodePtr CreateNode( Content::SceneNode* data );
    Undo::CommandPtr ImportSceneNodes( Reflect::V_Element& elements, ImportAction action, u32 importFlags, i32 importReflectType = Reflect::ReservedTypes::Invalid );

  private:
    // loading helpers
    void Reset();
    
    Undo::CommandPtr ImportSceneNode( const Reflect::ElementPtr& element, V_SceneNodeSmartPtr& createdNodes, ImportAction action, u32 importFlags, i32 importReflectType = Reflect::ReservedTypes::Invalid  );

    // Reflect::Archive overrides for status and error reporting
    void ArchiveStatus(Reflect::StatusInfo& info)HELIUM_OVERRIDE;
    void ArchiveException(Reflect::ExceptionInfo& info) HELIUM_OVERRIDE;

    /// @brief If this node has been remapped from another node, return the source nodes ID
    /// When we copy elements, we give them a new UniqueID. If we need information related
    /// to the original node, we need a way to gather the id of the original node.
    /// @param nodeId the id of the copied node
    /// @return returns the id of the source node if it exists. If not it returns Helium::TUID::Null
    Helium::TUID  GetRemappedID( tuid nodeId );

    //
    // Save
    //

  public:
    // Saves this scene to its current file location. 
    // (get and change the scene editor file to switch the destination)
    bool Save();

    // Save nodes to a file or to an xml string buffer.  Do not change the file
    // that this scene is pointing at.  Optionally export the entire scene or
    // just selected nodes.  Optionally maintain hiearchy or dependencies.
    bool ExportFile( const tstring& file, const ExportArgs& args );
    bool ExportXML( tstring& xml, const ExportArgs& args );
    bool Export( Reflect::V_Element& elements, const ExportArgs& args, Undo::BatchCommand* changes );

  private:
    // saving helpers
    void ExportSceneNode( Editor::SceneNode* node, Reflect::V_Element& elements, Helium::S_TUID& exported, const ExportArgs& args, Undo::BatchCommand* changes );
    void ExportHierarchyNode( Editor::HierarchyNode* node, Reflect::V_Element& elements, Helium::S_TUID& exported, const ExportArgs& args, Undo::BatchCommand* changes, bool exportChildren = true );


    //
    // Naming
    //  Each object in Editor has a unique name, these interfaces ensure that
    //  We use unique names because a lot of our drop down combo boxes need to refer to truly unique objects
    //

  private:
    // split the number portion of the name out
    int Split( tstring& outName );

    // fully validating setter for a node's name
    void SetName( Editor::SceneNode* sceneNode, const tstring& newName );

  public:
    // entry point for other objects to request their name to be changed
    void Rename( Editor::SceneNode* sceneNode, const tstring& newName, tstring oldName = TXT( "" ) );


    //
    // Object managment
    //  Add/remove objects during load and save, create and delete, as well as undo and redo
    //

  public:
    // add a new type to the scene
    void AddNodeType(const SceneNodeTypePtr& nodeType);
    void RemoveNodeType(const SceneNodeTypePtr& nodeType);

    // insert a node into the scene
    void AddObject( const SceneNodePtr& node );
    void RemoveObject( const SceneNodePtr& node );

  protected:
    // nitty gritty helpers for AddObject/RemoveObject
    void AddSceneNode( const SceneNodePtr& node );
    void RemoveSceneNode( const SceneNodePtr& node );

    // handle our own events
    void OnSceneNodeAdded( const NodeChangeArgs& args ); 
    void OnSceneNodeRemoved( const NodeChangeArgs& args ); 

    //
    // Evaluation, Rendering, and Picking
    //  Update object states, render viewports, and handle iewport intersection and selection
    //

  private:
    // evaluate dependency graph
    void Evaluate( bool silent = false );

  public:
    // pump evaluation and refresh view
    void Execute( bool interactively );

    // allocate/cleanup resources
    void Create();
    void Delete();

    // core render and pick visitation entry points
    void Render( RenderVisitor* render ) const;
    bool Pick( PickVisitor* pick ) const;

    // selection and highlight setup
    void Select( const SelectArgs& args );
    void SetHighlight( const SetHighlightArgs& args );
    void ClearHighlight( const ClearHighlightArgs& args );


    //
    // Undo/Redo support
    //

    bool Push(const Undo::CommandPtr& command);

  protected:
    bool UndoingOrRedoing( const Undo::QueueChangeArgs& args );
    void UndoQueueCommandPushed( const Undo::QueueChangeArgs& args );

    //
    // Query interfaces
    //  Simple Queries that find objects and interfaces to advanced find/selection
    //

  public:
    // pick will inject the guid of the object selected next into this data
    void PickLink( const Inspect::PickLinkArgs& args );

    // select will cause a selection to occur for the object referenced by the key
    void SelectLink( const Inspect::SelectLinkArgs& args );

    // populate will populate all the items in a scene that match the specified type
    void PopulateLink( Inspect::PopulateLinkArgs& args );

    // find/search for an object by different criteria
    Editor::SceneNode* FindNode( const Helium::TUID& id );
    Editor::SceneNode* FindNode( const tstring& name );

    // raise event
    void ChangeStatus(const tstring& status);
    void RefreshSelection();

    // callbacks when important events occur
    bool PropertyChanging( const Inspect::ChangingArgs& args );
    void PropertyChanged( const Inspect::ChangeArgs& args );
    bool SelectionChanging( const OS_SelectableDumbPtr& selection );
    void SelectionChanged( const OS_SelectableDumbPtr& selection );
    void CurrentSceneChanging( const SceneChangeArgs& args );
    void CurrentSceneChanged( const SceneChangeArgs& args );

    //
    // Change and Scene managment
    //  Additional routines that perform work mandated by the UI should go here (at the end of the cpp file)
    //

    Editor::HierarchyNode* GetCommonParent( const V_HierarchyNodeDumbPtr& nodes );
    void GetCommonParents( const V_HierarchyNodeDumbPtr& nodes, V_HierarchyNodeDumbPtr& parents );
    void GetSelectionParents(OS_SelectableDumbPtr& parents);

    void GetFlattenedSelection(OS_SelectableDumbPtr& selection);
    void GetFlattenedHierarchy(Editor::HierarchyNode* node, OS_HierarchyNodeDumbPtr& items);

    void GetSelectedTransforms( Math::V_Matrix4& transforms );
    Undo::CommandPtr SetSelectedTransforms( const Math::V_Matrix4& transforms );

    Undo::CommandPtr SetHiddenSelected( bool hidden );
    Undo::CommandPtr SetHiddenUnrelated( bool hidden );
    Undo::CommandPtr SetGeometryShown( bool shown, bool selected );
    Undo::CommandPtr ShowLastHidden();
    Undo::CommandPtr SelectSimilar();
    Undo::CommandPtr DeleteSelected();
    Undo::CommandPtr ParentSelected();
    Undo::CommandPtr UnparentSelected();
    Undo::CommandPtr GroupSelected();
    Undo::CommandPtr UngroupSelected();
    Undo::CommandPtr CenterSelected();
    Undo::CommandPtr DuplicateSelected();
    Undo::CommandPtr SmartDuplicateSelected();
    Undo::CommandPtr SnapSelectedToCamera();
    Undo::CommandPtr SnapCameraToSelected();

    void FrameSelected();
    void MeasureDistance();
    Undo::CommandPtr PickWalkUp();
    Undo::CommandPtr PickWalkDown();
    Undo::CommandPtr PickWalkSibling(bool forward);

  private:
    void ViewPreferencesChanged( const Reflect::ElementChangeArgs& args );

    //
    // Events
    //

  private:
    SceneStatusChangeSignature::Event m_StatusChanged;
  public:
    void AddStatusChangedListener( const SceneStatusChangeSignature::Delegate& listener )
    {
      m_StatusChanged.Add( listener );
    }
    void RemoveStatusChangedListener( const SceneStatusChangeSignature::Delegate& listener )
    {
      m_StatusChanged.Remove( listener );
    }

  private:
      SceneContextChangedSignature::Event m_SceneContextChanged;
  public:
    void AddSceneContextChangedListener( const SceneContextChangedSignature::Delegate& listener )
    {
      m_SceneContextChanged.Add( listener );
    }
    void RemoveSceneContextChangedListener( const SceneContextChangedSignature::Delegate& listener )
    {
      m_SceneContextChanged.Remove( listener );
    }

  private:
    NodeTypeExistenceSignature::Event m_NodeTypeCreated;
  public:
    void AddNodeTypeAddedListener( const NodeTypeExistenceSignature::Delegate& listener )
    {
      m_NodeTypeCreated.Add( listener );
    }
    void RemoveNodeTypeAddedListener( const NodeTypeExistenceSignature::Delegate& listener )
    {
      m_NodeTypeCreated.Remove( listener );
    }

  private:
    NodeTypeExistenceSignature::Event m_NodeTypeDeleted;
  public:
    void AddNodeTypeRemovedListener( const NodeTypeExistenceSignature::Delegate& listener )
    {
      m_NodeTypeDeleted.Add( listener );
    }
    void RemoveNodeTypeRemovedListener( const NodeTypeExistenceSignature::Delegate& listener )
    {
      m_NodeTypeDeleted.Remove( listener );
    }

  private:
    NodeChangeSignature::Event m_NodeAdded;
  public:
    void AddNodeAddedListener( const NodeChangeSignature::Delegate& listener )
    {
      m_NodeAdded.Add( listener );
    }
    void RemoveNodeAddedListener( const NodeChangeSignature::Delegate& listener )
    {
      m_NodeAdded.Remove( listener );
    }

  private:
    NodeChangeSignature::Event m_NodeRemoved;
  public:
    void AddNodeRemovedListener( const NodeChangeSignature::Delegate& listener )
    {
      m_NodeRemoved.Add( listener );
    }
    void RemoveNodeRemovedListener( const NodeChangeSignature::Delegate& listener )
    {
      m_NodeRemoved.Remove( listener );
    }

  private:
      NodeChangeSignature::Event m_NodeRemoving;
  public:
    void AddNodeRemovingListener( const NodeChangeSignature::Delegate& listener )
    {
      m_NodeRemoving.Add( listener );
    }
    void RemoveNodeRemovingListener( const NodeChangeSignature::Delegate& listener )
    {
      m_NodeRemoving.Remove( listener );
    }

  private:
    LoadSignature::Event m_LoadStarted;
  public:
    void AddLoadStartedListener( const LoadSignature::Delegate& listener )
    {
      m_LoadStarted.Add( listener );
    }
    void RemoveLoadStartedListener( const LoadSignature::Delegate& listener )
    {
      m_LoadStarted.Remove( listener );
    }

  private:
    LoadSignature::Event m_LoadFinished;
  public:
    void AddLoadFinishedListener( const LoadSignature::Delegate& listener )
    {
      m_LoadFinished.Add( listener );
    }
    void RemoveLoadFinishedListener( const LoadSignature::Delegate& listener )
    {
      m_LoadFinished.Remove( listener );
    }

  private:
    ExecuteSignature::Event m_Executed;
  public:
    void AddExecutedListener( const ExecuteSignature::Delegate& listener )
    {
      m_Executed.Add( listener );
    }
    void RemoveExecutedListener( const ExecuteSignature::Delegate& listener )
    {
      m_Executed.Remove( listener );
    }
  };

  typedef Helium::SmartPtr< Editor::Scene > ScenePtr;
  typedef std::set< ScenePtr > S_SceneSmartPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Command for adding and removing nodes from a scene.
  // 
  class SceneNodeExistenceCommand : public Undo::ExistenceCommand
  {
  public:
    SceneNodeExistenceCommand( Undo::ExistenceAction action, Editor::Scene* scene, const SceneNodePtr& node, bool redo = true )
      : Undo::ExistenceCommand( action, new Undo::MemberFunctionConstRef< Editor::Scene, SceneNodePtr >( scene, node, &Editor::Scene::AddObject ), new Undo::MemberFunctionConstRef< Editor::Scene, SceneNodePtr >( scene, node, &Editor::Scene::RemoveObject ), redo )
    {
    }
  };

  /////////////////////////////////////////////////////////////////////////////
  // Command for importing nodes into the scene
  // 
  class SceneImportCommand : public Undo::Command
  {
  public:
    SceneImportCommand( Editor::Scene* scene, const tstring& sceneFilePath, ImportAction importAction = ImportActions::Import, u32 importFlags = ImportFlags::None, Editor::HierarchyNode* importRoot = NULL, i32 importReflectType = Reflect::ReservedTypes::Invalid )
      : m_Scene( scene )
      , m_SceneFilePath( sceneFilePath )
      , m_ImportAction( importAction )
      , m_ImportFlags( importFlags )
      , m_ImportRoot( importRoot )
      , m_ImportReflectType( importReflectType )
    { 
    }

    virtual void Undo() HELIUM_OVERRIDE
    {
      if ( m_UndoCommand )
      {
        m_UndoCommand->Undo();
      }
    }

    virtual void Redo() HELIUM_OVERRIDE
    {
      if ( m_Scene )
      {
        if ( m_UndoCommand )
        {
          m_UndoCommand->Redo();
        }
        else
        {
          m_UndoCommand = m_Scene->ImportFile( m_SceneFilePath, m_ImportAction, m_ImportFlags, m_ImportRoot, m_ImportReflectType );
        }
      }
    }

  private:
    Editor::Scene*           m_Scene;
    tstring       m_SceneFilePath;
    ImportAction      m_ImportAction;
    u32               m_ImportFlags;
    Editor::HierarchyNode*   m_ImportRoot;
    Undo::CommandPtr  m_UndoCommand;
    i32  m_ImportReflectType;
  };


   /////////////////////////////////////////////////////////////////////////////
  // Command for selecting nodes in the scene
  // 
  class SceneSelectCommand : public Undo::Command
  {

  public:

    SceneSelectCommand( Editor::Scene* scene, OS_SelectableDumbPtr& selection  ) 
      : m_Scene( scene )
      , m_Selection( selection )
    { 
      if( m_Scene )
      {
        m_OldSelection = m_Scene->GetSelection().GetItems();
      }
    }

    virtual void Undo() HELIUM_OVERRIDE
    {
      if ( m_Scene )
      {
        m_Scene->GetSelection().SetItems( m_OldSelection );
      }
    }

    virtual void Redo() HELIUM_OVERRIDE
    {
      if ( m_Scene )
      {
        m_Scene->GetSelection().SetItems( m_Selection );
      }
    }

  private:

     Editor::Scene* m_Scene;
     OS_SelectableDumbPtr m_Selection;
     OS_SelectableDumbPtr m_OldSelection;

  };
}
