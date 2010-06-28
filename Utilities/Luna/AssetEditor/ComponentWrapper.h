#pragma once

#include "PersistentData.h"

// Forwards
namespace Component
{
  class ComponentBase;
  typedef Nocturnal::SmartPtr< ComponentBase > ComponentPtr;
};

namespace Luna
{
  // Forwards
  class AssetClass;
  class AssetManager;
  class ContextMenuItemSet;

  class AssetNode;
  typedef Nocturnal::SmartPtr< Luna::AssetNode > AssetNodePtr;

  class ComponentWrapper;
  typedef Nocturnal::SmartPtr< Luna::ComponentWrapper > ComponentWrapperPtr;
  typedef std::map< i32, Luna::ComponentWrapperPtr > M_ComponentSmartPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Arguments and events for when an attribute is changed
  // 
  struct ComponentChangeArgs
  {
  public:
    Luna::ComponentWrapper* m_Component;

    ComponentChangeArgs( Luna::ComponentWrapper* attribute )
      : m_Component( attribute )
    {
    }
  };
  typedef Nocturnal::Signature< void, const ComponentChangeArgs& > ComponentChangeSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper around Component::Component.
  // 
  class ComponentWrapper : public Luna::PersistentData
  {
  protected:
    Luna::AssetClass* m_AssetClass;

  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::ComponentWrapper, Luna::PersistentData );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr CreateComponent( Reflect::Element* attribute, Luna::AssetManager* assetManager );

  protected:
    ComponentWrapper( Component::ComponentBase* attribute, Luna::AssetManager* assetManager );
  public:
    virtual ~ComponentWrapper();
    void SetAssetClass( Luna::AssetClass* asset );
    Luna::AssetClass* GetAssetClass() const;
    virtual tstring GetName() const;
    i32 GetSlot() const;
    virtual const tstring& GetIcon() const;
    virtual void CreateChildren( Luna::AssetNode* parentNode );
    virtual void PopulateContextMenu( ContextMenuItemSet& menu );
    virtual void ConnectProperties( EnumerateElementArgs& args ); 
    virtual Undo::CommandPtr OverwriteSettings( Component::ComponentBase* src );

    static const tstring& GetComponentIcon(const Component::ComponentBase* attribute); 

    // Listeners
  protected:
    ComponentChangeSignature::Event m_NameChanged;
  public:
    void AddNameChangedListener( const ComponentChangeSignature::Delegate& listener )
    {
      m_NameChanged.Add( listener );
    }
    void RemoveNameChangedListener( const ComponentChangeSignature::Delegate& listener )
    {
      m_NameChanged.Remove( listener );
    }

  protected:
    ComponentChangeSignature::Event m_ChildrenRefresh;
  public:
    void AddChildrenRefreshListener( const ComponentChangeSignature::Delegate& listener )
    {
      m_ChildrenRefresh.Add( listener );
    }
    void RemoveChildrenRefreshListener( const ComponentChangeSignature::Delegate& listener )
    {
      m_ChildrenRefresh.Remove( listener );
    }
  };
}
