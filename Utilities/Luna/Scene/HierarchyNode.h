#pragma once

#include "Foundation/Math/AlignedBox.h"

#include "Pick.h"
#include "View.h"

#include "Luna/API.h"
#include "SceneNode.h"
#include "SceneVisitor.h"

#include "Foundation/Container/OrderedSet.h"
#include "Content/NodeVisibility.h"

namespace Content
{
  class HierarchyNode;
}

namespace Luna
{
  class Scene;
  class Transform;

  class PickVisitor;

  class PickHit;
  typedef Nocturnal::SmartPtr< PickHit > PickHitPtr;
  typedef std::vector< PickHitPtr > V_PickHitSmartPtr;

  class Layer;

  class HierarchyNode;
  typedef Nocturnal::SmartPtr< Luna::HierarchyNode > HierarchyNodePtr;
  typedef Nocturnal::OrderedSet< Luna::HierarchyNode* > OS_HierarchyNodeDumbPtr;

  typedef std::vector< Luna::HierarchyNode* > V_HierarchyNodeDumbPtr;
  typedef std::vector< HierarchyNodePtr > V_HierarchyNodeSmartPtr;


  // 
  // Parent changing event
  // 

  struct ParentChangingArgs
  {
    Luna::HierarchyNode* m_Node;
    Luna::HierarchyNode* m_NewParent;

    ParentChangingArgs( Luna::HierarchyNode* node, Luna::HierarchyNode* newParent )
      : m_Node( node )
      , m_NewParent( newParent )
    {

    }
  };

  typedef Nocturnal::Signature< bool, const ParentChangingArgs& >ParentChangingSignature;

  // 
  // Parent changed event
  // 

  struct ParentChangedArgs
  {
    Luna::HierarchyNode* m_Node;
    Luna::HierarchyNode* m_OldParent;

    ParentChangedArgs( Luna::HierarchyNode* node, Luna::HierarchyNode* oldParent )
      : m_Node( node )
      , m_OldParent( oldParent )
    {

    }
  };

  typedef Nocturnal::Signature< void, const ParentChangedArgs& >ParentChangedSignature;


  /////////////////////////////////////////////////////////////////////////////
  // A hierarchy node is a node that belongs in the scene and is represented
  // the outliner for the scene.  This node has some UI specific functionality.
  // In addition to Ancestors and Descendants, hierarchy nodes add Parents
  // and Children.
  // 
  class LUNA_SCENE_API HierarchyNode : public Luna::SceneNode
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
    std::string m_Path;
    Luna::HierarchyNode* m_Parent;
    Luna::HierarchyNode* m_Previous;
    Luna::HierarchyNode* m_Next;
    OS_HierarchyNodeDumbPtr m_Children;

    // Cached pointers to use for switching color modes in the 3D view
    Luna::Layer* m_LayerColor; 

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
    LUNA_DECLARE_TYPE( Luna::HierarchyNode, Luna::SceneNode );
    static void InitializeType();
    static void CleanupType();


    // 
    // Member functions
    // 

  public:
    HierarchyNode(Luna::Scene* scene, Content::HierarchyNode* data);
    virtual ~HierarchyNode();

    // Helper
    virtual SceneNodeTypePtr CreateNodeType( Luna::Scene* scene ) const NOC_OVERRIDE;

    // Initialize this and children
    void InitializeHierarchy();

    // Persistence managment
    virtual void Reset() NOC_OVERRIDE;
    virtual void Pack() NOC_OVERRIDE;
    virtual void Unpack() NOC_OVERRIDE;

    // Transient (in the process of being placed)
    virtual void SetTransient(bool isTransient) NOC_OVERRIDE;

    // Hidden state of this node
    virtual bool IsHidden() const;
    virtual void SetHidden(bool isHidden);

    // Live state of this node
    virtual bool IsLive() const;
    virtual void SetLive(bool isLive);

    // Visibility of this node (computed)
    virtual bool IsVisible() const NOC_OVERRIDE;

    // Selectability of this node (computed)
    virtual bool IsSelectable() const NOC_OVERRIDE;

    // Set selected state
    virtual void SetSelected(bool value) NOC_OVERRIDE;


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
    // Override from Luna::SceneNode, this resets our (and childrens') cached path member
    //

    virtual void SetName( const std::string& value ) NOC_OVERRIDE;


    //
    // Path string of this node in the scene, its cached and computed as needed
    //

    const std::string& GetPath();


    //
    // Graph Connections
    //

    // the parent of this node
    Luna::HierarchyNode* GetParent() const;

    // this is the entry point for attaching a node to a graph
    void SetParent(Luna::HierarchyNode* value);

    // Sets only the previous pointer on this node.  To do a full rearrange,
    // set the previous and next pointers, then remove and re-add (or add for
    // the first time) the node.
    void SetPrevious( Luna::HierarchyNode* value );

    // Sets only the next pointer on this node.  See comment on SetPrevious above.
    void SetNext( Luna::HierarchyNode* value );

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
    void AddChild(Luna::HierarchyNode* c);
    void RemoveChild(Luna::HierarchyNode* c);


    //
    // DG Sectioning
    //  Lets not do this now, we need the parent and child links to reconstruct hierarchy on undo delete and redo create recursively
    //

  protected:
    virtual void DisconnectDescendant(Luna::SceneNode* descendant) NOC_OVERRIDE;
    virtual void ConnectDescendant(Luna::SceneNode* descendant) NOC_OVERRIDE;

    virtual void ConnectAncestor( Luna::SceneNode* ancestor ) NOC_OVERRIDE;
    virtual void DisconnectAncestor( Luna::SceneNode* ancestor ) NOC_OVERRIDE;

    //
    // Transform Access
    //

  public:
    // returns the closest parent transform node (including this)
    virtual Luna::Transform* GetTransform();
    virtual const Luna::Transform* GetTransform() const;


    //
    // Resources
    //

    virtual void Create() NOC_OVERRIDE;
    virtual void Delete() NOC_OVERRIDE;


    //
    // Evaluate and Render
    //

  protected:
    bool ComputeVisibility() const;
    bool ComputeSelectability() const;

  public:
    // this makes the graph go dirty in both directions
    virtual u32 Dirty() NOC_OVERRIDE;

    // update our global bounding volume for culling
    virtual void Evaluate(GraphDirection direction) NOC_OVERRIDE;

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

    Luna::HierarchyNode* Find(const std::string& targetName);

    Luna::HierarchyNode* FindFromPath(std::string path);

    virtual void FindSimilar(V_HierarchyNodeDumbPtr& similar) const;
    virtual bool IsSimilar(const HierarchyNodePtr& node) const;

    virtual std::string GetDescription() const;


    //
    // Properties
    //

    // manipulator support
    virtual void ConnectManipulator(ManiuplatorAdapterCollection* collection);


    //
    // UI
    //

    // ui integration
    virtual bool ValidatePanel(const std::string& name) NOC_OVERRIDE;

    // creator
    static void CreatePanel(CreatePanelArgs& args);

  protected: 
    Content::NodeVisibilityPtr m_VisibilityData; 

  };
}
