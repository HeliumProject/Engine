#pragma once

#include "Foundation/Math/AlignedBox.h"

#include "Core/Scene/Pick.h"
#include "Core/Scene/Viewport.h"

#include "Core/API.h"
#include "Core/Scene/SceneNode.h"
#include "SceneVisitor.h"

#include "Foundation/Container/OrderedSet.h"
#include "Core/Content/NodeVisibility.h"

namespace Helium
{
    namespace Content
    {
        class HierarchyNode;
    }

    namespace Core
    {
        class Scene;
        class Transform;
        class PickVisitor;

        class PickHit;
        typedef Helium::SmartPtr< PickHit > PickHitPtr;
        typedef std::vector< PickHitPtr > V_PickHitSmartPtr;

        class Layer;

        class HierarchyNode;
        typedef Helium::SmartPtr< Core::HierarchyNode > HierarchyNodePtr;
        typedef Helium::OrderedSet< Core::HierarchyNode* > OS_HierarchyNodeDumbPtr;
        typedef std::vector< Core::HierarchyNode* > V_HierarchyNodeDumbPtr;
        typedef std::vector< HierarchyNodePtr > V_HierarchyNodeSmartPtr;

        class ManiuplatorAdapterCollection;

        struct ParentChangingArgs
        {
            ParentChangingArgs( Core::HierarchyNode* node, Core::HierarchyNode* newParent )
                : m_Node( node )
                , m_NewParent( newParent )
                , m_Veto( false )
            {

            }

            Core::HierarchyNode*    m_Node;
            Core::HierarchyNode*    m_NewParent;
            mutable bool            m_Veto;
        };

        typedef Helium::Signature< const ParentChangingArgs& > ParentChangingSignature;

        struct ParentChangedArgs
        {
            Core::HierarchyNode* m_Node;
            Core::HierarchyNode* m_OldParent;

            ParentChangedArgs( Core::HierarchyNode* node, Core::HierarchyNode* oldParent )
                : m_Node( node )
                , m_OldParent( oldParent )
            {

            }
        };

        typedef Helium::Signature< const ParentChangedArgs& >ParentChangedSignature;


        /////////////////////////////////////////////////////////////////////////////
        // A hierarchy node is a node that belongs in the scene and is represented
        // the outliner for the scene.  This node has some UI specific functionality.
        // In addition to Ancestors and Descendants, hierarchy nodes add Parents
        // and Children.
        // 
        class CORE_API HierarchyNode : public Core::SceneNode
        {
            // 
            // Member variables 
            // 

        protected:
            // computed from layers
            bool m_Visible;
            bool m_Selectable;

            // highlight state in 3d
            bool m_Highlighted;

            // when a node's parent is selected, meaning that if you move the parent, this node will also move.
            bool m_Reactive;

            // hierarchy
            tstring m_Path;
            Core::HierarchyNode* m_Parent;
            Core::HierarchyNode* m_Previous;
            Core::HierarchyNode* m_Next;
            OS_HierarchyNodeDumbPtr m_Children;

            // Cached pointers to use for switching color modes in the 3D view
            Core::Layer* m_LayerColor; 

            // bounds
            Math::AlignedBox m_ObjectBounds;
            Math::AlignedBox m_ObjectHierarchyBounds;

            // events
            ParentChangingSignature::Event m_ParentChanging;
            ParentChangedSignature::Event m_ParentChanged;


            //
            // Runtime Type Info
            //

        public:
            REFLECT_DECLARE_ABSTRACT( Core::HierarchyNode, Core::SceneNode );
            static void InitializeType();
            static void CleanupType();


            // 
            // Member functions
            // 

        public:
            HierarchyNode(Core::Scene* scene, Content::HierarchyNode* data);
            virtual ~HierarchyNode();

            // Helper
            virtual SceneNodeTypePtr CreateNodeType( Core::Scene* scene ) const HELIUM_OVERRIDE;

            // Initialize this and children
            void InitializeHierarchy();

            // Persistence managment
            virtual void Reset() HELIUM_OVERRIDE;
            virtual void Pack() HELIUM_OVERRIDE;
            virtual void Unpack() HELIUM_OVERRIDE;

            // Transient (in the process of being placed)
            virtual void SetTransient(bool isTransient) HELIUM_OVERRIDE;

            // Hidden state of this node
            virtual bool IsHidden() const;
            virtual void SetHidden(bool isHidden);

            // Live state of this node
            virtual bool IsLive() const;
            virtual void SetLive(bool isLive);

            // Visibility of this node (computed)
            virtual bool IsVisible() const HELIUM_OVERRIDE;

            // Selectability of this node (computed)
            virtual bool IsSelectable() const HELIUM_OVERRIDE;

            // Set selected state
            virtual void SetSelected(bool value) HELIUM_OVERRIDE;


            //
            // Highlight is a special visualization for when the mouse hovers over us
            //

            virtual bool IsHighlighted() const;
            virtual void SetHighlighted(bool value);


            // 
            // Reactive means that a parent of this node is selected.  It will be
            // rendered a different color in the scene view to indicate that if 
            // you move the parent this node will also be moved.
            //

            virtual bool IsReactive() const;
            virtual void SetReactive( bool value );


            //
            // Override from Core::SceneNode, this resets our (and childrens') cached path member
            //

            virtual void SetName( const tstring& value ) HELIUM_OVERRIDE;


            //
            // Path string of this node in the scene, its cached and computed as needed
            //

            const tstring& GetPath();


            //
            // Graph Connections
            //

            // the parent of this node
            Core::HierarchyNode* GetParent() const;

            // this is the entry point for attaching a node to a graph
            void SetParent(Core::HierarchyNode* value);

            // Sets only the previous pointer on this node.  To do a full rearrange,
            // set the previous and next pointers, then remove and re-add (or add for
            // the first time) the node.
            void SetPrevious( Core::HierarchyNode* value );

            // Sets only the next pointer on this node.  See comment on SetPrevious above.
            void SetNext( Core::HierarchyNode* value );

            // thie children of this node
            const OS_HierarchyNodeDumbPtr& GetChildren() const
            {
                return m_Children;
            }

            void ReverseChildren();

            // create a duplicate hierarchy
            HierarchyNodePtr Duplicate();


            //
            // Bounds
            //

            const Math::AlignedBox& GetObjectBounds() const
            {
                return m_ObjectBounds;
            }

            Math::AlignedBox GetGlobalBounds() const;

            const Math::AlignedBox& GetObjectHierarchyBounds() const
            {
                return m_ObjectHierarchyBounds;
            }

            Math::AlignedBox GetGlobalHierarchyBounds() const;


            // 
            // Events
            // 

            void AddParentChangingListener( ParentChangingSignature::Delegate listener );
            void RemoveParentChangingListener( ParentChangingSignature::Delegate listener );

            void AddParentChangedListener( ParentChangedSignature::Delegate listener );
            void RemoveParentChangedListener( ParentChangedSignature::Delegate listener );


            //
            // Hierarchy Modifiers, use by commands only
            //

        private:
            void AddChild(Core::HierarchyNode* c);
            void RemoveChild(Core::HierarchyNode* c);


            //
            // DG Sectioning
            //  Lets not do this now, we need the parent and child links to reconstruct hierarchy on undo delete and redo create recursively
            //

        protected:
            virtual void DisconnectDescendant(Core::SceneNode* descendant) HELIUM_OVERRIDE;
            virtual void ConnectDescendant(Core::SceneNode* descendant) HELIUM_OVERRIDE;

            virtual void ConnectAncestor( Core::SceneNode* ancestor ) HELIUM_OVERRIDE;
            virtual void DisconnectAncestor( Core::SceneNode* ancestor ) HELIUM_OVERRIDE;

            //
            // Transform Access
            //

        public:
            // returns the closest parent transform node (including this)
            virtual Core::Transform* GetTransform();
            virtual const Core::Transform* GetTransform() const;


            //
            // Resources
            //

            virtual void Create() HELIUM_OVERRIDE;
            virtual void Delete() HELIUM_OVERRIDE;


            //
            // Evaluate and Render
            //

        protected:
            bool ComputeVisibility() const;
            bool ComputeSelectability() const;

        public:
            // this makes the graph go dirty in both directions
            virtual u32 Dirty() HELIUM_OVERRIDE;

            // update our global bounding volume for culling
            virtual void Evaluate(GraphDirection direction) HELIUM_OVERRIDE;

        public:
            // do bounds check
            virtual bool BoundsCheck(const Math::Matrix4& instanceMatrix) const;

            // set the material on the device based upon this object's selection state
            virtual void SetMaterial( const D3DMATERIAL9& defaultMaterial ) const;

            // call VisitHierarchyNode() on the render object for each hierarhcy node recursively
            virtual TraversalAction TraverseHierarchy( HierarchyTraverser* traverser );

            // draw this object, returns whether to actually draw
            virtual void Render( RenderVisitor* render );

            // pick test this object
            virtual bool Pick(PickVisitor* pick);


            //
            // Searching
            //

            Core::HierarchyNode* Find(const tstring& targetName);

            Core::HierarchyNode* FindFromPath(tstring path);

            virtual void FindSimilar(V_HierarchyNodeDumbPtr& similar) const;
            virtual bool IsSimilar(const HierarchyNodePtr& node) const;

            virtual tstring GetDescription() const;


            //
            // Properties
            //

            // manipulator support
            virtual void ConnectManipulator(ManiuplatorAdapterCollection* collection);


            //
            // UI
            //

            // ui integration
            virtual bool ValidatePanel(const tstring& name) HELIUM_OVERRIDE;

            // creator
            static void CreatePanel(CreatePanelArgs& args);

        protected: 
            Content::NodeVisibilityPtr m_VisibilityData; 

        };
    }
}