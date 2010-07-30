#pragma once

#include <hash_map>

#include "Foundation/Reflect/Element.h"

#include "Persistent.h"
#include "Selection.h"

#include "PropertiesGenerator.h"

#include "Editor/API.h"

namespace Helium
{
    namespace Content
    {
        class SceneNode;
    }

    namespace Asset
    {
        class SceneManifest;
    }

    namespace Editor
    {
        class Scene;
        class SceneGraph;

        class SceneNode;
        typedef Helium::SmartPtr< Editor::SceneNode > SceneNodePtr;

        class SceneNodeType;
        typedef Helium::SmartPtr< Editor::SceneNodeType > SceneNodeTypePtr;

        typedef std::vector< Editor::SceneNode* > V_SceneNodeDumbPtr;
        typedef std::vector< SceneNodePtr > V_SceneNodeSmartPtr;

        typedef std::set< Editor::SceneNode* > S_SceneNodeDumbPtr;
        typedef std::set< SceneNodePtr > S_SceneNodeSmartPtr;

        typedef stdext::hash_map< Helium::TUID, Editor::SceneNode*, Helium::TUIDHasher > HM_SceneNodeDumbPtr;
        typedef stdext::hash_map< Helium::TUID, SceneNodePtr, Helium::TUIDHasher > HM_SceneNodeSmartPtr;


        // 
        // Hashing class for storing UIDs as keys to a hash_map.
        // 

        class NameHasher : public stdext::hash_compare< tstring >
        {
        public:
            size_t operator( )( const tstring& str ) const
            {
                return __super::operator()( str );
            }

            bool operator( )( const tstring& str1, const tstring& str2 ) const
            {
                return _tcsicmp(str1.c_str(), str2.c_str()) < 0;
            }
        };

        typedef stdext::hash_map< tstring, Editor::SceneNode*, NameHasher > HM_NameToSceneNodeDumbPtr;
        typedef stdext::hash_map< tstring, SceneNodePtr, NameHasher > HM_NameToSceneNodeSmartPtr;


        // 
        // Name change event
        // 

        struct SceneNodeChangeArgs
        {
            Editor::SceneNode* m_Node;

            SceneNodeChangeArgs( Editor::SceneNode* node )
                : m_Node( node )
            {

            }
        };

        typedef Helium::Signature< void, const SceneNodeChangeArgs& > SceneNodeChangeSignature;


        //
        // Enumerates the different states that a node can be in with regards to its evaluation in the graph
        //

        namespace GraphDirections
        {
            enum GraphDirection
            {
                Downstream,
                Upstream,
                Count,
            };
        }

        typedef GraphDirections::GraphDirection GraphDirection;

        namespace NodeStates
        {
            enum NodeState
            {
                Evaluating,
                Clean,
                Dirty,
                Count,
            };
        }

        typedef NodeStates::NodeState NodeState;


        //
        // Scene Node
        //  Editor application object that can have a dependencies on other objects.
        //   o Has an evaluate function that does work that is dependent on other objects being clean.
        //   o Dependencies of this object are traversed by SceneGraph to compute the order of evaluation.
        //

        class LUNA_SCENE_API SceneNode HELIUM_ABSTRACT : public Persistent
        {
            //
            // Members
            //

        protected:
            // our initialization state
            bool m_IsInitialized;

            // our current state
            NodeState m_NodeStates[ GraphDirections::Count ];

            // the type we are an instance of
            Editor::SceneNodeType* m_NodeType;

            // the graph that evaluates us
            SceneGraph* m_Graph;

            // The scene that owns us
            Editor::Scene* m_Scene;

            // ancestors are Dependency Nodes that are evaluated before this Node
            S_SceneNodeDumbPtr m_Ancestors;

            // descendants are Dependency Nodes that are evaluated after this Node
            S_SceneNodeSmartPtr m_Descendants;

            // Transient nodes are not really part of the scene and will not be serialized.
            // See Editor::CreateTool for more information.
            bool m_IsTransient;

        private:
            // data cached for evaluation
            u32 m_VisitedID;


            //
            // Runtime Type Info
            //

        public:
            EDITOR_DECLARE_TYPE( Editor::SceneNode, Persistent );
            static void InitializeType();
            static void CleanupType();


            //
            // Implementation
            //

            SceneNode( Editor::Scene* scene, Content::SceneNode* data );
            virtual ~SceneNode();


            //
            // Get/Set state
            //

            SceneNode::NodeState GetNodeState(GraphDirection direction) const
            {
                return m_NodeStates[direction];
            }

            void SetNodeState(GraphDirection direction, NodeState value)
            {
                m_NodeStates[direction] = value;
            }

            Editor::SceneNodeType* GetNodeType() const
            {
                return m_NodeType;
            }

            void SetNodeType(Editor::SceneNodeType* type)
            {
                m_NodeType = type;
            }

            SceneGraph* GetGraph()
            {
                return m_Graph;
            }

            const SceneGraph* GetGraph() const
            {
                return m_Graph;
            }

            void SetGraph(SceneGraph* value)
            {
                m_Graph = value;
            }

            Editor::Scene* GetScene()
            {
                return m_Scene;
            }

            const Editor::Scene* GetScene() const
            {
                return m_Scene;
            }

            virtual bool IsTransient() const
            {
                return m_IsTransient;
            }

            virtual void SetTransient( bool isTransient )
            {
                m_IsTransient = isTransient;
            }

            S_SceneNodeDumbPtr& GetAncestors()
            {
                return m_Ancestors;
            }

            const S_SceneNodeDumbPtr& GetAncestors() const
            {
                return m_Ancestors;
            }

            S_SceneNodeSmartPtr& GetDescendants()
            {
                return m_Descendants;
            }

            const S_SceneNodeSmartPtr& GetDescendants() const
            {
                return m_Descendants;
            }


            // 
            // Visibility indicates that the item shows up in the 3D view (assuming
            // that additional options are enabled, such as the pointer or bounds).
            // Should be overridden in derived classes to accurately determine this.
            // Derived classes should raise the m_VisibilityChanged event if they 
            // make a change to the node's visibility so that it it accurately
            // Reflected throughout the UI.
            // 
            virtual bool IsVisible() const
            {
                return true;
            }

        protected:
            SceneNodeChangeSignature::Event m_VisibilityChanged;

        public:
            void AddVisibilityChangedListener( const SceneNodeChangeSignature::Delegate& listener )
            {
                m_VisibilityChanged.Add( listener );
            }

            void RemoveVisibilityChangedListener( const SceneNodeChangeSignature::Delegate& listener )
            {
                m_VisibilityChanged.Remove( listener );
            }

            //
            // ID and Name, every node has a unique name
            //

        public:
            virtual const Helium::TUID& SceneNode::GetID() const;
            virtual void SceneNode::SetID(const Helium::TUID& id);

            virtual tstring GenerateName() const;
            virtual const tstring& GetName() const;
            virtual void SetName(const tstring& newName);

            virtual bool UseGivenName() const;
            bool UseAutoName() const
            {
                return !UseGivenName();
            }

            virtual void SetUseGivenName(bool use);
            void SetUseAutoName(bool use)
            {
                SetUseGivenName(!use);
            }

            virtual void SetGivenName(const tstring& newName);
            virtual void Rename(const tstring& newName);


            //
            // Manifest Generation
            //

            virtual void PopulateManifest( Asset::SceneManifest* manifest ) const;


            //
            // VisitedID tracks which eval traversal we were last visited on
            //

            u32 GetVisitedID() const
            {
                return m_VisitedID;
            }

            void SetVisitedID(u32 id)
            {
                m_VisitedID = id;
            }


            //
            // Node management
            //

            // Reset object state, used to decomposing dependency graph
            virtual void Reset();

            // Callback after new scene data is loaded, and initial evaluation is complete
            virtual void Initialize();

            // Check for initialization state
            bool IsInitialized()
            {
                return m_IsInitialized;
            }


            //
            // These protected functions provide a way to extend the logic of an atomic
            //  operation on a dependency
            //

        protected:
            virtual void ConnectDescendant( Editor::SceneNode* descendant );
            virtual void DisconnectDescendant( Editor::SceneNode* descendant );

            virtual void ConnectAncestor( Editor::SceneNode* ancestor );
            virtual void DisconnectAncestor( Editor::SceneNode* ancestor );

        public:
            ///////////////////////////////////////////////////////////////////////////
            // Returns true if the attribute specified by the template parameter is in
            // this collection.
            // 
            template < class T >
            bool HasAttribute() const
            {
                const Component::ComponentCollection* pkg = GetPackage< Component::ComponentCollection >();
                return ( pkg->GetAttribute( Reflect::GetType< T >() ).ReferencesObject() );
            }

            ///////////////////////////////////////////////////////////////////////////
            // Adds or removes the specified attribute from this collection.
            // 
            template < class T >
            void SetAttribute( bool enable )
            {
                Component::ComponentCollection* pkg = GetPackage< Component::ComponentCollection >(); 
                if ( enable )
                {
                    // This will create a new attribute or enable an existing one.
                    Component::ComponentEditor< T > editor( pkg );
                    editor.Commit();
                }
                else
                {
                    pkg->RemoveAttribute( Reflect::GetType< T >() );
                }
            }

            //
            // These public functions ensure that the dependency is created bidirectionally and that
            //  the dependent object is added to the graph and classified
            //

        public:
            void CreateDependency(Editor::SceneNode* ancestor);
            void RemoveDependency(Editor::SceneNode* ancestor);


            //
            // Graph seggregation and merging
            //
            // Insert reconnects trimmed decendant connections from all non-pruned dependent graph nodes
            //  Note this uses the preserved ancestor connections for each pruned node
            //
            // Prune trims the decendant connection from all dependent graph nodes
            //  Note this preserves the ancestor connection for each pruned node so we can do insert easily
            //

        public:
            virtual void Insert( SceneGraph* graph, V_SceneNodeDumbPtr& insertedNodes );
            virtual void Prune( V_SceneNodeDumbPtr& prunedNodes );


            //
            // Evaluate
            //

        protected:
            // entry point from the graph
            virtual void DoEvaluate(GraphDirection direction); friend SceneGraph;

        public:
            // Makes us Evaluate() on next graph evaluation
            virtual u32 Dirty();

            // overridable method for derived classes
            virtual void Evaluate(GraphDirection direction);


            //
            // Type system allows us to collect instances of objects into type collectors at runtime
            //

        public:
            // the image index for use in the UI (in the scene outliner for example)
            virtual i32 GetImageIndex() const;

            // the user interface name for this compile-time class, used to matching object's type
            virtual tstring GetApplicationTypeName() const;

            // creates our node type object (we don't have a configured type when we call this)
            virtual SceneNodeTypePtr CreateNodeType( Editor::Scene* scene ) const;

            // transition to another node type
            virtual void ChangeNodeType( Editor::SceneNodeType* type );

            // take a stab at deducing what node type you should be
            virtual Editor::SceneNodeType* DeduceNodeType();

            // check that you are a member of the type that best suits you
            virtual void CheckNodeType();


            //
            // Scene nodes use resources, allow them to be created and deleted on demand
            //

        public:
            // call to create state resources
            virtual void Create();

            // call to delete state resources
            virtual void Delete();

            // Make object dirty and iterate world
            virtual void Execute(bool interactively);


            //
            // UI
            //

        public:
            // validates named panel types usable by this instance
            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;

            // create the panel from the selection
            static void CreatePanel(CreatePanelArgs& args);

            // membership property
            tstring GetMembership() const;
            void SetMembership( const tstring& layers );


            //
            // Events
            //

        private:
            SceneNodeChangeSignature::Event m_NameChanging;
        public:
            void AddNameChangingListener( const SceneNodeChangeSignature::Delegate& listener )
            {
                m_NameChanging.Add( listener );
            }
            void RemoveNameChangingListener( const SceneNodeChangeSignature::Delegate& listener )
            {
                m_NameChanging.Remove( listener );
            }

        private:
            SceneNodeChangeSignature::Event m_NameChanged;
        public:
            void AddNameChangedListener( const SceneNodeChangeSignature::Delegate& listener )
            {
                m_NameChanged.Add( listener );
            }
            void RemoveNameChangedListener( const SceneNodeChangeSignature::Delegate& listener )
            {
                m_NameChanged.Remove( listener );
            }
        };
    }
}