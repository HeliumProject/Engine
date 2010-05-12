#pragma once

#include "PersistentData.h"

// Forwards
namespace Attribute
{
  class AttributeBase;
  typedef Nocturnal::SmartPtr< AttributeBase > AttributePtr;
};

namespace Luna
{
  // Forwards
  class AssetClass;
  class AssetManager;
  class ContextMenuItemSet;

  class AssetNode;
  typedef Nocturnal::SmartPtr< Luna::AssetNode > AssetNodePtr;

  class AttributeWrapper;
  typedef Nocturnal::SmartPtr< Luna::AttributeWrapper > AttributeWrapperPtr;
  typedef std::map< i32, Luna::AttributeWrapperPtr > M_AttributeSmartPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Arguments and events for when an attribute is changed
  // 
  struct AttributeChangeArgs
  {
  public:
    Luna::AttributeWrapper* m_Attribute;

    AttributeChangeArgs( Luna::AttributeWrapper* attribute )
      : m_Attribute( attribute )
    {
    }
  };
  typedef Nocturnal::Signature< void, const AttributeChangeArgs& > AttributeChangeSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper around Attribute::Attribute.
  // 
  class AttributeWrapper : public Luna::PersistentData
  {
  protected:
    Luna::AssetClass* m_AssetClass;

  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::AttributeWrapper, Luna::PersistentData );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr CreateAttribute( Reflect::Element* attribute, Luna::AssetManager* assetManager );

  protected:
    AttributeWrapper( Attribute::AttributeBase* attribute, Luna::AssetManager* assetManager );
  public:
    virtual ~AttributeWrapper();
    void SetAssetClass( Luna::AssetClass* asset );
    Luna::AssetClass* GetAssetClass() const;
    virtual std::string GetName() const;
    i32 GetSlot() const;
    virtual const std::string& GetIcon() const;
    virtual void CreateChildren( Luna::AssetNode* parentNode );
    virtual void PopulateContextMenu( ContextMenuItemSet& menu );
    virtual void ConnectProperties( EnumerateElementArgs& args ); 
    virtual Undo::CommandPtr OverwriteSettings( Attribute::AttributeBase* src );

    static const std::string& GetAttributeIcon(const Attribute::AttributeBase* attribute); 

    // Listeners
  protected:
    AttributeChangeSignature::Event m_NameChanged;
  public:
    void AddNameChangedListener( const AttributeChangeSignature::Delegate& listener )
    {
      m_NameChanged.Add( listener );
    }
    void RemoveNameChangedListener( const AttributeChangeSignature::Delegate& listener )
    {
      m_NameChanged.Remove( listener );
    }

  protected:
    AttributeChangeSignature::Event m_ChildrenRefresh;
  public:
    void AddChildrenRefreshListener( const AttributeChangeSignature::Delegate& listener )
    {
      m_ChildrenRefresh.Add( listener );
    }
    void RemoveChildrenRefreshListener( const AttributeChangeSignature::Delegate& listener )
    {
      m_ChildrenRefresh.Remove( listener );
    }
  };
}
