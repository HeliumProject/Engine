#pragma once

#include "API.h"
#include "Common/Container/OrderedSet.h"
#include "Editor/ContextMenuGenerator.h"
#include "Core/Selectable.h"
#include "Undo/Command.h"

namespace Inspect
{
  class ReflectClipboardData;
  typedef Nocturnal::SmartPtr< ReflectClipboardData > ReflectClipboardDataPtr;
}

namespace Luna
{
  // Forwards
  class AssetClass;
  class AssetManager;
  class AssetNode;
  typedef Nocturnal::SmartPtr< Luna::AssetNode > AssetNodePtr;
  typedef Nocturnal::OrderedSet< Luna::AssetNodePtr > OS_AssetNodeSmartPtr;
  typedef std::set< Luna::AssetNodePtr > S_AssetNodeSmartPtr;
  typedef std::set< Luna::AssetNode* > S_AssetNodeDumbPtr;
  typedef std::vector< Luna::AssetNode* > V_AssetNodeDumbPtr;
  typedef std::vector< Luna::AssetNodePtr > V_AssetNodeSmartPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Argument and events for when a node's name changes.
  // 
  struct NodeNameChangeArgs
  {
    Luna::AssetNode* m_Node;
    std::string m_OldName;
    std::string m_NewName;

    NodeNameChangeArgs( Luna::AssetNode* node, const std::string& oldName, const std::string& newName )
      : m_Node( node )
      , m_OldName( oldName )
      , m_NewName( newName )
    {
    }
  };
  typedef Nocturnal::Signature< void, const NodeNameChangeArgs& > NodeNameChangeSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Argument and events for when a node's icon changes.
  // 
  struct NodeIconChangeArgs
  {
    Luna::AssetNode* m_Node;

    NodeIconChangeArgs( Luna::AssetNode* node )
      : m_Node( node )
    {
    }
  };
  typedef Nocturnal::Signature< void, const NodeIconChangeArgs& > NodeIconSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Argument and events for when the parent of a node is changed.
  // 
  struct ParentChangeArgs
  {
    Luna::AssetNode* m_Node;
    Luna::AssetNode* m_OldParent;
    Luna::AssetNode* m_NewParent;

    ParentChangeArgs( Luna::AssetNode* node, Luna::AssetNode* oldParent, Luna::AssetNode* newParent )
      : m_Node( node )
      , m_OldParent( oldParent )
      , m_NewParent( newParent )
    {
    }
  };
  typedef Nocturnal::Signature< void, const ParentChangeArgs& > ParentChangeSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Argument and events for when a child is added to or removed from this node.
  // Also, events for when a child is rearranged.
  // 
  struct ChildChangeArgs
  {
    Luna::AssetNode* m_Parent;
    Luna::AssetNode* m_Child;

    ChildChangeArgs( Luna::AssetNode* parent, Luna::AssetNode* child )
      : m_Parent( parent )
      , m_Child( child )
    {
    }
  };
  typedef Nocturnal::Signature< void, const ChildChangeArgs& > ChildChangeSignature;
  typedef Nocturnal::Signature< void, const ChildChangeArgs& > ChildRearrangeSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Enumeration of different font styles for the label of this node.
  // 
  namespace LabelStyles
  {
    enum LabelStyle
    {
      Normal = 0,
      Italic = 1 << 0,
      Bold   = 1 << 1,
    };

    inline bool HasFlag( u32 flags, LabelStyle which )
    {
      if ( which == Normal )
      {
        return flags == Normal;
      }
      return ( ( flags & which ) == which );
    }
  };
  typedef LabelStyles::LabelStyle LabelStyle;

  /////////////////////////////////////////////////////////////////////////////
  // Indicates what type of clipboard operation is being performed during 
  // drag/drop and copy/paste commands.
  // 
  namespace ClipboardOperations
  {
    enum ClipboardOperation
    {
      Move,
      Copy
    };
  }
  typedef ClipboardOperations::ClipboardOperation ClipboardOperation;

  /////////////////////////////////////////////////////////////////////////////
  // Arguments and events for when the style is changed on a node.
  // 
  struct NodeStyleChangeArgs
  {
    Luna::AssetNode* m_Node;
    u32 m_NewStyle;
    u32 m_OldStyle;

    NodeStyleChangeArgs( Luna::AssetNode* node, u32 oldStyle, u32 newStyle )
      : m_Node( node )
      , m_OldStyle( oldStyle )
      , m_NewStyle( newStyle )
    {
    }
  };
  typedef Nocturnal::Signature< void, const NodeStyleChangeArgs& > NodeStyleChangeSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Base class for all nodes that are selectable and displayed in the 
  // Asset Editor.
  // 
  class AssetNode : public Selectable
  {
  private:
    Luna::AssetManager* m_AssetManager;
    std::string m_Name;
    std::string m_Icon;
    u32 m_Style; // LabelStyle
    Luna::AssetNode* m_Parent;
    OS_AssetNodeSmartPtr m_Children;
    mutable Luna::AssetClass* m_CachedAsset;
    bool m_IsRearranging;
    ContextMenuItemSet m_ContextMenu;

  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::AssetNode, Selectable );
    static void InitializeType();
    static void CleanupType();

  public:
    AssetNode( Luna::AssetManager* assetManager );
    virtual ~AssetNode();

    inline Luna::AssetManager* GetAssetManager() const { return m_AssetManager; }

    const std::string& GetName() const;
    void SetName( const std::string& name );

    i32 GetIconIndex() const;
    const std::string& GetIcon() const;
    void SetIcon( const std::string& icon );

    u32 GetStyle() const;
    void SetStyle( u32 style );

    const OS_AssetNodeSmartPtr& GetChildren() const;
    virtual bool AddChild( Luna::AssetNodePtr child, Luna::AssetNode* beforeSibling = NULL );
    virtual bool RemoveChild( Luna::AssetNodePtr child );
    void DeleteChildren();
    virtual void CreateChildren();
    virtual bool CanBeMoved() const;
    virtual bool CanBeCopied() const;
    virtual Inspect::ReflectClipboardDataPtr GetClipboardData();
    virtual bool CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data );
    virtual bool HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch = NULL );
    virtual bool RearrangeChildren( Luna::AssetNode* child, Luna::AssetNode* beforeSibling );
    virtual bool CanAddChild( Luna::AssetNode* child ) const;
    virtual Undo::CommandPtr DeleteSelectedChildren();

    virtual Luna::AssetClass* GetAssetClass() const;
    Luna::AssetNode* GetParent() const;
    bool IsAncestor( Luna::AssetNode* node );

    ContextMenuItemSet& GetContextMenu();
    void SetContextMenu( const ContextMenuItemSet& menu );
    virtual void PreShowContextMenu();

    virtual void ActivateItem();


    // Possibly needed:
    //virtual CanBeMoved();
    //virtual CanBeDragged();
    //virtual HandleDrop( items );
    //virtual bool CanAddChild( child );

  protected:
    void AddDefaultContextMenuItems( ContextMenuItemSet& menu ) const;

  private:
    void SetParent( Luna::AssetNode* parent );

  protected:
    void ClearCachedAssetClass();

    // Events
  private:
    NodeNameChangeSignature::Event m_NameChanged;

  public:
    void AddNameChangedListener( const NodeNameChangeSignature::Delegate& listener )
    {
      m_NameChanged.Add( listener );
    }

    void RemoveNameChangedListener( const NodeNameChangeSignature::Delegate& listener )
    {
      m_NameChanged.Remove( listener );
    }

  private:
    NodeIconSignature::Event m_IconChanged;

  public:
    void AddIconChangedListener( const NodeIconSignature::Delegate& listener )
    {
      m_IconChanged.Add( listener );
    }

    void RemoveIconChangedListener( const NodeIconSignature::Delegate& listener )
    {
      m_IconChanged.Remove( listener );
    }

  private:
    NodeStyleChangeSignature::Event m_StyleChanged;

  public:
    void AddStyleChangedListener( const NodeStyleChangeSignature::Delegate& listener )
    {
      m_StyleChanged.Add( listener );
    }

    void RemoveStyleChangedListener( const NodeStyleChangeSignature::Delegate& listener )
    {
      m_StyleChanged.Remove( listener );
    }

  private:
    ParentChangeSignature::Event m_ParentChanging;

  public:
    void AddParentChangingListener( const ParentChangeSignature::Delegate& listener )
    {
      m_ParentChanging.Add( listener );
    }

    void RemoveParentChangingListener( const ParentChangeSignature::Delegate& listener )
    {
      m_ParentChanging.Remove( listener );
    }

  private:
    ParentChangeSignature::Event m_ParentChanged;

  public:
    void AddParentChangedListener( const ParentChangeSignature::Delegate& listener )
    {
      m_ParentChanged.Add( listener );
    }

    void RemoveParentChangedListener( const ParentChangeSignature::Delegate& listener )
    {
      m_ParentChanged.Remove( listener );
    }

  private:
    ChildChangeSignature::Event m_ChildAdded;

  public:
    void AddChildAddedListener( const ChildChangeSignature::Delegate& listener )
    {
      m_ChildAdded.Add( listener );
    }

    void RemoveChildAddedListener( const ChildChangeSignature::Delegate& listener )
    {
      m_ChildAdded.Remove( listener );
    }

  private:
    ChildChangeSignature::Event m_ChildRemoved;

  public:
    void AddChildRemovedListener( const ChildChangeSignature::Delegate& listener )
    {
      m_ChildRemoved.Add( listener );
    }

    void RemoveChildRemovedListener( const ChildChangeSignature::Delegate& listener )
    {
      m_ChildRemoved.Remove( listener );
    }

  private:
    ChildRearrangeSignature::Event m_ChildRearranged;

  public:
    void AddChildRearrangedListener( const ChildRearrangeSignature::Delegate& listener )
    {
      m_ChildRearranged.Add( listener );
    }

    void RemoveChildRearrangedListener( const ChildRearrangeSignature::Delegate& listener )
    {
      m_ChildRearranged.Remove( listener );
    }
  };
}
