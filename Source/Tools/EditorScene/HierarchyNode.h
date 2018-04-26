#pragma once

#include "Math/AlignedBox.h"
#include "Application/OrderedSet.h"

#include "EditorScene/API.h"
#include "EditorScene/Pick.h"
#include "EditorScene/SceneNode.h"
#include "EditorScene/SceneVisitor.h"
#include "EditorScene/Viewport.h"

namespace Helium
{
	namespace Editor
	{
		class Scene;
		class Transform;
		class Layer;

		class PickVisitor;
		class PickHit;
		typedef Helium::SmartPtr< PickHit > PickHitPtr;
		typedef std::vector< PickHitPtr > V_PickHitSmartPtr;

		class HierarchyNode;
		typedef Helium::StrongPtr< HierarchyNode > HierarchyNodePtr;

		typedef std::vector< HierarchyNode* > V_HierarchyNodeDumbPtr;
		typedef std::vector< HierarchyNodePtr > V_HierarchyNodeSmartPtr;

		typedef std::set< HierarchyNode* > S_HierarchyNodeDumbPtr;
		typedef std::set< HierarchyNodePtr > S_HierarchyNodeSmartPtr;

		typedef Helium::OrderedSet< HierarchyNode* > OS_HierarchyNodeDumbPtr;
		typedef Helium::OrderedSet< HierarchyNodePtr > OS_HierarchyNodeSmartPtr;

		class ManiuplatorAdapterCollection;

		struct ParentChangingArgs
		{
			ParentChangingArgs( Editor::HierarchyNode* node, Editor::HierarchyNode* newParent )
				: m_Node( node )
				, m_NewParent( newParent )
				, m_Veto( false )
			{

			}

			Editor::HierarchyNode*    m_Node;
			Editor::HierarchyNode*    m_NewParent;
			mutable bool            m_Veto;
		};
		typedef Helium::Signature< const ParentChangingArgs& > ParentChangingSignature;

		struct ParentChangedArgs
		{
			Editor::HierarchyNode* m_Node;
			Editor::HierarchyNode* m_OldParent;

			ParentChangedArgs( Editor::HierarchyNode* node, Editor::HierarchyNode* oldParent )
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
		class HELIUM_EDITOR_SCENE_API HierarchyNode : public SceneNode
		{
		public:
			HELIUM_DECLARE_ABSTRACT( Editor::HierarchyNode, Editor::SceneNode );
			static void PopulateMetaType( Reflect::MetaStruct& comp );

		public:
			HierarchyNode();
			~HierarchyNode();

			// Initialize this and children
			void InitializeHierarchy();

			// Persistence managment
			virtual void Reset() override;

			// Transient (in the process of being placed)
			virtual void SetTransient(bool isTransient) override;

			// Hidden state of this node
			virtual bool IsHidden() const;
			virtual void SetHidden(bool isHidden);

			// Live state of this node
			virtual bool IsLive() const;
			virtual void SetLive(bool isLive);

			// Visibility of this node (computed)
			virtual bool IsVisible() const override;

			// Selectability of this node (computed)
			virtual bool IsSelectable() const override;

			// Set selected state
			virtual void SetSelected(bool value) override;

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
			// Override from Editor::SceneNode, this resets our (and childrens') cached path member
			//

			virtual void SetName( const std::string& value ) override;

			//
			// FilePath string of this node in the scene, its cached and computed as needed
			//

			const std::string& GetPath();

			//
			// Graph Connections
			//

			// the parent of this node
			TUID GetParentID() const
			{
				return m_ParentID;
			}
			Editor::HierarchyNode* GetParent() const;

			// this is the entry point for attaching a node to a graph
			void SetParent(Editor::HierarchyNode* value);

			// Sets only the previous pointer on this node.  To do a full rearrange,
			// set the previous and next pointers, then remove and re-add (or add for
			// the first time) the node.
			void SetPrevious( Editor::HierarchyNode* value );

			// Sets only the next pointer on this node.  See comment on SetPrevious above.
			void SetNext( Editor::HierarchyNode* value );

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

			const AlignedBox& GetObjectBounds() const
			{
				return m_ObjectBounds;
			}

			AlignedBox GetGlobalBounds() const;

			const AlignedBox& GetObjectHierarchyBounds() const
			{
				return m_ObjectHierarchyBounds;
			}

			AlignedBox GetGlobalHierarchyBounds() const;

			//
			// Hierarchy Modifiers, use by commands only
			//

		private:
			void AddChild(Editor::HierarchyNode* c);
			void RemoveChild(Editor::HierarchyNode* c);

			//
			// DG Sectioning
			//  Lets not do this now, we need the parent and child links to reconstruct hierarchy on undo delete and redo create recursively
			//

		protected:
			virtual void DisconnectDescendant(Editor::SceneNode* descendant) override;
			virtual void ConnectDescendant(Editor::SceneNode* descendant) override;

			virtual void ConnectAncestor( Editor::SceneNode* ancestor ) override;
			virtual void DisconnectAncestor( Editor::SceneNode* ancestor ) override;

			//
			// Transform Access
			//

		public:
			// returns the closest parent transform node (including this)
			virtual Editor::Transform* GetTransform();
			virtual const Editor::Transform* GetTransform() const;

			//
			// Resources
			//

			virtual void Create() override;
			virtual void Delete() override;

			//
			// Evaluate and Render
			//

		protected:
			bool ComputeVisibility() const;
			bool ComputeSelectability() const;

		public:
			// this makes the graph go dirty in both directions
			virtual uint32_t Dirty() override;

			// update our global bounding volume for culling
			virtual void Evaluate(GraphDirection direction) override;

		public:
			// do bounds check
			virtual bool BoundsCheck(const Matrix4& instanceMatrix) const;

			// get the material color based upon this object's selection state
			virtual Helium::Color GetMaterialColor( Helium::Color defaultMaterial ) const;

			// call VisitHierarchyNode() on the render object for each hierarhcy node recursively
			virtual TraversalAction TraverseHierarchy( HierarchyTraverser* traverser );

			// draw this object, returns whether to actually draw
			virtual void Render( RenderVisitor* render );

			// pick test this object
			virtual bool Pick(PickVisitor* pick);

			//
			// Searching
			//

			Editor::HierarchyNode* Find(const std::string& targetName);

			Editor::HierarchyNode* FindFromPath(std::string path);

			virtual void FindSimilar(V_HierarchyNodeDumbPtr& similar) const;
			virtual bool IsSimilar(const HierarchyNodePtr& node) const;

			virtual std::string GetDescription() const;

			//
			// Properties
			//

			// manipulator support
			virtual void ConnectManipulator(ManiuplatorAdapterCollection* collection);

			// 
			// Events
			// 

		protected:
			ParentChangingSignature::Event m_ParentChanging;
		public:
			void AddParentChangingListener( ParentChangingSignature::Delegate listener )
			{
				m_ParentChanging.Add( listener );
			}
			void RemoveParentChangingListener( ParentChangingSignature::Delegate listener )
			{
				m_ParentChanging.Remove( listener );
			}

		protected:
			ParentChangedSignature::Event m_ParentChanged;
		public:
			void AddParentChangedListener( ParentChangedSignature::Delegate listener )
			{
				m_ParentChanged.Add( listener );
			}
			void RemoveParentChangedListener( ParentChangedSignature::Delegate listener )
			{
				m_ParentChanged.Remove( listener );
			}

		protected: 
			// Reflected
			Helium::TUID                m_ParentID;                 // The ID of the parent node
			bool                        m_Hidden;                   // The hidden state
			bool                        m_Live;                     // The live state

			// Non-reflected
			bool                        m_Visible;                  // computed from layers
			bool                        m_Selectable;               // computed from layers
			bool                        m_Highlighted;              // highlight state in 3d
			bool                        m_Reactive;                 // when a node's parent is selected, meaning that if you move the parent, this node will also move.
			std::string                     m_Path;
			HierarchyNode*              m_Parent;
			HierarchyNode*              m_Previous;
			HierarchyNode*              m_Next;
			OS_HierarchyNodeDumbPtr     m_Children;
			Layer*                      m_LayerColor;               // cached pointers to use for switching color modes in the 3D view
			AlignedBox            m_ObjectBounds;             // bounds
			AlignedBox            m_ObjectHierarchyBounds;
		};
	}
}