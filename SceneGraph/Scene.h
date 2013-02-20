#pragma once

#include "Application/DocumentManager.h"
#include "Foundation/Event.h"
#include "Foundation/TUID.h"
#include "Application/UndoQueue.h"

#include "Reflect/Archive.h"
#include "Reflect/Version.h"

#include "Inspect/DataBinding.h"
#include "Inspect/Canvas.h"

#include "SceneGraph/API.h"
#include "SceneGraph/Selection.h"
#include "SceneGraph/PropertiesGenerator.h"

#include "Pick.h"
#include "Tool.h"
#include "SceneNode.h"
#include "Graph.h"
#include "Transform.h"

namespace Helium
{
    namespace SceneGraph
    {
        class Layer;
        class PickVisitor;
        struct SceneChangeArgs;
    }

    namespace SceneGraph
    {
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

            static bool HasFlag( uint32_t flags, ExportFlag singleFlag )
            {
                return ( flags & singleFlag ) == singleFlag;
            }
        }
        typedef ExportFlags::ExportFlag ExportFlag;

        typedef std::vector< AlignedBox > V_AlignedBox;
        struct ExportArgs
        {
            uint32_t m_Flags;
            V_AlignedBox m_Bounds;

            ExportArgs()
                : m_Flags ( ExportFlags::Default )
            {

            }

            ExportArgs( uint32_t flags )
                : m_Flags ( flags )
            {

            }
        };


        typedef Helium::Signature< const struct ResolveSceneArgs& > ResolveSceneSignature;
        typedef Helium::Signature< const struct ReleaseSceneArgs& > ReleaseSceneSignature;
        typedef Helium::Signature< const struct SceneEditingArgs& > SceneEditingSignature;
        typedef Helium::Signature< const struct SceneStatusChangeArgs& > SceneStatusChangeSignature;
        typedef Helium::Signature< const struct SceneContextChangeArgs& > SceneContextChangedSignature;
        typedef Helium::Signature< const struct NodeChangeArgs& > NodeChangeSignature;
        typedef Helium::Signature< const struct LoadArgs& > LoadSignature;
        typedef Helium::Signature< const struct ExecuteArgs& > ExecuteSignature;
        typedef Helium::Signature< const struct UndoCommandArgs& > UndoCommandSignature; 

        // Hashing class for storing UIDs as keys to a hash_map.
        class NameHasher : public stdext::hash_compare< tstring >
        {
        public:
            size_t operator( )( const tstring& str ) const
            {
                return stdext::hash_compare< tstring >::operator()( str );
            }

            bool operator( )( const tstring& str1, const tstring& str2 ) const
            {
                return CaseInsensitiveCompareString(str1.c_str(), str2.c_str()) < 0;
            }
        };

        typedef stdext::hash_map< tstring, SceneGraph::SceneNode*, NameHasher > HM_NameToSceneNodeDumbPtr;

        class HELIUM_SCENE_GRAPH_API Scene : public Reflect::Object
        {
        public:
            // Differentiate between replacing the entire contents of a file, and bringing in new ones
            class ImportActions
            {
            public:
                // Mutually exclusive operations
                enum ImportAction
                {
                    Load,   // Scene must be empty, we are bringing in all the nodes
                    Import, // Leave whatever nodes are currently in the scene and bring in new ones
                };
            };
            typedef ImportActions::ImportAction ImportAction;

            // Options specific to an import action
            class ImportFlags
            {
            public:
                enum ImportFlag
                {
                    None = 0,

                    Merge   = 1 << 1, // If a node matches one being imported, replace node in this scene with the new one
                    Select  = 1 << 2, // Select the imported nodes so the user can easily identify them

                    Default = None,
                };

                static bool HasFlag( uint32_t flags, ImportFlag singleFlag )
                {
                    return ( flags & singleFlag ) == singleFlag;
                }
            };
            typedef ImportFlags::ImportFlag ImportFlag;

            Scene( SceneGraph::Viewport* viewport, const Helium::FilePath& path );
            ~Scene();

            Helium::TUID GetId() const
            {
                return m_Id;
            }

            // is this the current scene in the editor?
            void SetFocused( bool focused )
            {
                m_IsFocused = focused;
            }
            bool IsFocused()
            {
                return m_IsFocused;
            }


            // is this scene able to be edited?
            bool IsEditable();

            // FilePath to the file that this scene is currently editing
            const Helium::FilePath& GetPath() const
            {
                return m_Path;
            }
            void SetPath( const FilePath& path )
            {
                m_Path = path;
            }

            // get the current tool in use in this scene
            const ToolPtr& GetTool();
            void SetTool(const ToolPtr& tool);

            // support for zone color
            const Color3& GetColor() const;
            void SetColor( const Color3& color );

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
            const OS_SceneNodeDumbPtr& GetHighlighted() const
            {
                return m_Highlighted;
            }


            //
            // Database
            //  These comprise the most basic data structures that hold references to the objects in the scene
            //

            // the scene root node (synthetic)
            const SceneGraph::TransformPtr& GetRoot() const
            {
                return m_Root;
            }

            // the dependency graph for ordered and efficient execution
            const SceneGraphPtr& GetGraph() const
            {
                return m_Graph;
            }

            // the nodes in the scene
            const HM_SceneNodeSmartPtr& GetNodes() const
            {
                return m_Nodes;
            }

            SceneGraph::SceneNode* Find( const tstring& name ) const; 

            SceneGraph::SceneNode* Get( const Helium::TUID& uid ) const
            {
                HM_SceneNodeSmartPtr::const_iterator it = m_Nodes.find( uid );

                if ( it != m_Nodes.end() )
                {
                    return it->second;
                }

                return NULL;
            }


            template< class T >
            T* Get( const Helium::TUID &uid ) const
            {
                return Reflect::SafeCast< T >( Get( uid ) );
            }

            template< class T >
            void GetAll( std::vector< T* >& objects, bool (*filterFunc )( SceneNode* ) = NULL ) const
            {
                HM_SceneNodeDumbPtr::const_iterator itor = m_Nodes.begin();
                HM_SceneNodeDumbPtr::const_iterator end  = m_Nodes.end();
                for( ; itor != end; ++itor )
                {
                    if( itor->second->HasType( Reflect::GetClass< T >() ) )
                    {
                        if ( !filterFunc || (*filterFunc)( itor->second ) )
                        {
                            objects.push_back( Reflect::AssertCast< T >( itor->second ) );
                        }
                    }
                }
            }

            template< class T >
            void GetAllPackages( std::vector< T* >& objects, int32_t attributeType = -1, bool pack = false ) const
            {
                HM_SceneNodeDumbPtr::const_iterator itor = m_Nodes.begin();
                HM_SceneNodeDumbPtr::const_iterator end  = m_Nodes.end();
                for( ; itor != end; ++itor )
                {
                    T* contentObject = Reflect::SafeCast<T>( itor->second->GetPackage() );

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
            SceneGraph::Viewport* GetViewport() const
            {
                return m_View;
            }

            //
            // Document
            //

        public:
            void ConnectDocument( Document* document );
            void DisconnectDocument( const Document* document );

            mutable DocumentObjectChangedSignature::Event e_HasChanged;

        private:
            // Callback for when a document is saved.
            void OnDocumentSave( const DocumentEventArgs& args );


            //
            // Load
            //

        public:
            // Open a whole scene, replacing the current one.
            bool Reload();
            bool Load( const Helium::FilePath& path ); 

            // Import data into this scene, possibly merging with existing nodes.
            UndoCommandPtr Import( const Helium::FilePath& path, ImportAction action = ImportActions::Import, uint32_t importFlags = ImportFlags::None, SceneGraph::HierarchyNode* parent = NULL, const Reflect::Class* importReflectType = NULL );
            UndoCommandPtr ImportXML( const tstring& xml, uint32_t importFlags = ImportFlags::None, SceneGraph::HierarchyNode* parent = NULL );
            UndoCommandPtr ImportSceneNodes( std::vector< Reflect::ObjectPtr >& elements, ImportAction action, uint32_t importFlags, const Reflect::Class* importReflectType = NULL );

        private:
            // loading helpers
            void Reset();

            UndoCommandPtr ImportSceneNode( const Reflect::ObjectPtr& element, V_SceneNodeSmartPtr& createdNodes, ImportAction action, uint32_t importFlags, const Reflect::Class* importReflectType = NULL  );

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
            // for status and error reporting from Reflect::Archive events
            void ArchiveStatus( const Reflect::ArchiveStatus& info );

            // Saves this scene to its current file location. 
            // (get and change the scene editor file to switch the destination)
            bool Serialize();

            // Save nodes to a file or to an xml string buffer.  Do not change the file
            // that this scene is pointing at.  Optionally export the entire scene or
            // just selected nodes.  Optionally maintain hiearchy or dependencies.
            bool Export( const FilePath& path, const ExportArgs& args );
            bool ExportXML( tstring& xml, const ExportArgs& args );
            bool Export( std::vector< Reflect::ObjectPtr >& elements, const ExportArgs& args, BatchUndoCommand* changes );

        private:
            // saving helpers
            void ExportSceneNode( SceneGraph::SceneNode* node, std::vector< Reflect::ObjectPtr >& elements, Helium::S_TUID& exported, const ExportArgs& args, BatchUndoCommand* changes );
            void ExportHierarchyNode( SceneGraph::HierarchyNode* node, std::vector< Reflect::ObjectPtr >& elements, Helium::S_TUID& exported, const ExportArgs& args, BatchUndoCommand* changes, bool exportChildren = true );


            //
            // Naming
            //  Each object in Editor has a unique name, these interfaces ensure that
            //  We use unique names because a lot of our drop down combo boxes need to refer to truly unique objects
            //

        private:
            // split the number portion of the name out
            int Split( tstring& outName );

            // fully validating setter for a node's name
            void SetName( SceneGraph::SceneNode* sceneNode, const tstring& newName );

        public:
            // entry point for other objects to request their name to be changed
            void Rename( SceneGraph::SceneNode* sceneNode, const tstring& newName, tstring oldName = TXT( "" ) );


            //
            // Object managment
            //  Add/remove objects during load and save, create and delete, as well as undo and redo
            //

        public:
            // insert a node into the scene
            void AddObject( SceneNodePtr node );
            void RemoveObject( SceneNodePtr node );

        protected:
            // nitty gritty helpers for AddObject/RemoveObject
            void AddSceneNode( const SceneNodePtr& node );
            void RemoveSceneNode( const SceneNodePtr& node );

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
            void Render( RenderVisitor* render );
            bool Pick( PickVisitor* pick ) const;

            // selection and highlight setup
            void Select( const SelectArgs& args );
            void SetHighlight( const SetHighlightArgs& args );
            void ClearHighlight( const ClearHighlightArgs& args );


            //
            // Undo/Redo support
            //

            bool Push(const UndoCommandPtr& command);

        protected:
            void UndoingOrRedoing( const UndoQueueChangingArgs& args );
            void UndoQueueCommandPushed( const UndoQueueChangeArgs& args );

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
            SceneGraph::SceneNode* FindNode( const Helium::TUID& id );
            SceneGraph::SceneNode* FindNode( const tstring& name );

            // raise event
            void ChangeStatus(const tstring& status);
            void RefreshSelection();

            // callbacks when important events occur
            void PropertyChanging( const Inspect::ControlChangingArgs& args );
            void PropertyChanged( const Inspect::ControlChangedArgs& args );
            void SelectionChanging( const SelectionChangingArgs& args );
            void SelectionChanged( const SelectionChangeArgs& args );
            void CurrentSceneChanging( const SceneChangeArgs& args );
            void CurrentSceneChanged( const SceneChangeArgs& args );

            //
            // Change and Scene managment
            //  Additional routines that perform work mandated by the UI should go here (at the end of the cpp file)
            //

            SceneGraph::HierarchyNode* GetCommonParent( const V_HierarchyNodeDumbPtr& nodes );
            void GetCommonParents( const V_HierarchyNodeDumbPtr& nodes, V_HierarchyNodeDumbPtr& parents );
            void GetSelectionParents(OS_SceneNodeDumbPtr& parents);

            void GetFlattenedSelection(OS_SceneNodeDumbPtr& selection);
            void GetFlattenedHierarchy(SceneGraph::HierarchyNode* node, OS_HierarchyNodeDumbPtr& items);

            void GetSelectedTransforms( V_Matrix4& transforms );
            UndoCommandPtr SetSelectedTransforms( const V_Matrix4& transforms );

            UndoCommandPtr SetHiddenSelected( bool hidden );
            UndoCommandPtr SetHiddenUnrelated( bool hidden );
            UndoCommandPtr ShowLastHidden();
            UndoCommandPtr SelectSimilar();
            UndoCommandPtr DeleteSelected();
            UndoCommandPtr ParentSelected();
            UndoCommandPtr UnparentSelected();
            UndoCommandPtr GroupSelected();
            UndoCommandPtr UngroupSelected();
            UndoCommandPtr CenterSelected();
            UndoCommandPtr DuplicateSelected();
            UndoCommandPtr SmartDuplicateSelected();
            UndoCommandPtr SnapSelectedToCamera();
            UndoCommandPtr SnapCameraToSelected();

            void FrameSelected();

            void MeasureDistance();

            UndoCommandPtr PickWalkUp();
            UndoCommandPtr PickWalkDown();
            UndoCommandPtr PickWalkSibling(bool forward);

        private:
            void ViewPreferencesChanged( const Reflect::ObjectChangeArgs& args );

            //
            // Events
            //

        public:
            UndoCommandSignature::Delegate d_UndoCommand;
            ResolveSceneSignature::Delegate d_ResolveScene;
            ReleaseSceneSignature::Delegate d_ReleaseScene;
            SceneEditingSignature::Delegate d_Editing;
            SceneStatusChangeSignature::Event e_StatusChanged;
            SceneContextChangedSignature::Event e_SceneContextChanged;
            NodeChangeSignature::Event e_NodeAdded;
            NodeChangeSignature::Event e_NodeRemoved;
            NodeChangeSignature::Event e_NodeRemoving;
            LoadSignature::Event e_LoadStarted;
            LoadSignature::Event e_LoadFinished;
            ExecuteSignature::Event e_Executed;

        private:
            Helium::FilePath m_Path;
            Helium::TUID m_Id;

            // load
            int32_t m_Progress;
            Helium::HM_TUID m_RemappedIDs;
            SceneGraph::HierarchyNode* m_ImportRoot;
            bool m_Importing;

            // scene data
            SceneGraph::TransformPtr m_Root;

            // gives us ordered evaluation
            SceneGraphPtr m_Graph;

            // container for nodes sorted by uid
            HM_SceneNodeSmartPtr m_Nodes;

            // container for nodes sorted by name
            HM_NameToSceneNodeDumbPtr m_Names;

            // selection of this scene
            Selection m_Selection;

            // highlighted items of this scene
            OS_SceneNodeDumbPtr m_Highlighted;

            // data for handling picks
            Inspect::DataBindingPtr m_PickData;

            // the 3d view control
            SceneGraph::Viewport* m_View;

            // the tool in use by this scene
            ToolPtr m_Tool;

            // offset matrix for smart duplicate
            Matrix4 m_SmartDuplicateMatrix;

            // flag that the smart duplicate matrix is valid
            bool m_ValidSmartDuplicateMatrix;

            // the set of last hidden
            std::set<Helium::TUID> m_LastHidden;

            // set by the zone that this scene belongs to, and used for
            // the 3D view's "color modes"
            Color3 m_Color;

            bool m_IsFocused;
        };

        typedef Helium::StrongPtr< SceneGraph::Scene > ScenePtr;
        typedef std::set< ScenePtr > S_SceneSmartPtr;
        typedef std::map< tstring, ScenePtr > M_SceneSmartPtr;
        typedef std::map< Scene*, int32_t > M_AllocScene;

        /////////////////////////////////////////////////////////////////////////////
        // Command for adding and removing nodes from a scene.
        // 
        class SceneNodeExistenceCommand : public ExistenceUndoCommand< SceneNodePtr >
        {
        public:
            SceneNodeExistenceCommand( ExistenceAction action, SceneGraph::Scene* scene, const SceneNodePtr& node, bool redo = true )
                : ExistenceUndoCommand( action, node,
                                    Delegate< SceneNodePtr >( scene, &SceneGraph::Scene::AddObject ),
                                    Delegate< SceneNodePtr >( scene, &SceneGraph::Scene::RemoveObject ),
                                    redo )
            {
            }
        };

        /////////////////////////////////////////////////////////////////////////////
        // Command for importing nodes into the scene
        // 
        class SceneImportCommand : public UndoCommand
        {
        public:
            SceneImportCommand( SceneGraph::Scene* scene, const Helium::FilePath& path, Scene::ImportAction importAction = Scene::ImportActions::Import, uint32_t importFlags = Scene::ImportFlags::None, SceneGraph::HierarchyNode* importRoot = NULL, const Reflect::Class* importReflectType = NULL )
                : m_Scene( scene )
                , m_Path( path )
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
                        m_UndoCommand = m_Scene->Import( m_Path, m_ImportAction, m_ImportFlags, m_ImportRoot, m_ImportReflectType );
                    }
                }
            }

        private:
            SceneGraph::Scene*          m_Scene;
            Helium::FilePath                m_Path;
            Scene::ImportAction         m_ImportAction;
            uint32_t                    m_ImportFlags;
            SceneGraph::HierarchyNode*  m_ImportRoot;
            UndoCommandPtr            m_UndoCommand;
            const Reflect::Class*       m_ImportReflectType;
        };


        /////////////////////////////////////////////////////////////////////////////
        // Command for selecting nodes in the scene
        // 
        class SceneSelectCommand : public UndoCommand
        {

        public:

            SceneSelectCommand( SceneGraph::Scene* scene, OS_SceneNodeDumbPtr& selection  ) 
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
            SceneGraph::Scene* m_Scene;
            OS_SceneNodeDumbPtr m_Selection;
            OS_SceneNodeDumbPtr m_OldSelection;
        };
    }
}

#include "Scene.inl"