#pragma once

#include "AttributeWrapper.h"
#include "PersistentData.h"
#include "Asset/AssetClass.h"
#include "Common/Automation/Event.h"
#include "Undo/BatchCommand.h"

namespace Inspect
{ 
  class Control;
}

namespace Luna
{
  // Forwards
  struct CreatePanelArgs; 
  class AssetManager;
  class AssetClass;
  class AssetNode;
  class AssetReferenceNode;
  class ContextMenuItemSet;
  class ContextMenuArgs;
  typedef Nocturnal::SmartPtr< Luna::AssetClass > AssetClassPtr;
  typedef std::set< Luna::AssetClassPtr > S_AssetClassSmartPtr;
  typedef std::set< Luna::AssetClass* > S_AssetClassDumbPtr;
  typedef Nocturnal::SmartPtr< ContextMenuArgs > ContextMenuArgsPtr;
  typedef std::set< Luna::AssetReferenceNode* > S_AssetReferenceNodeDumbPtr;

  /////////////////////////////////////////////////////////////////////////////
  // Arguments and events for attribute existence changes on an Luna::AssetClass.
  // 
  struct AttributeExistenceArgs
  {
    Luna::AssetClass* m_Asset;
    Luna::AttributeWrapper* m_Attribute;
    
    AttributeExistenceArgs( Luna::AssetClass* asset, Luna::AttributeWrapper* attribute )
      : m_Asset( asset )
      , m_Attribute( attribute )
    {
    }
  };
  typedef Nocturnal::Signature< void, const AttributeExistenceArgs& > AttributeExistenceSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper around an Asset::AssetClass.
  // 
  class AssetClass : public Luna::PersistentData
  {
  protected:
    M_AttributeSmartPtr m_Attributes;
    mutable std::string m_Name;
    mutable Nocturnal::Path m_Path;
    S_AssetReferenceNodeDumbPtr m_References;

  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::AssetClass, Luna::PersistentData );
    static void InitializeType();
    static void CleanupType();
    static Luna::PersistentDataPtr Create( Reflect::Element* assetClass, Luna::AssetManager* manager );

  protected:
    AssetClass( Asset::AssetClass* assetClass, Luna::AssetManager* manager );
  public:
    virtual ~AssetClass();

    virtual void Pack() NOC_OVERRIDE;
    virtual void Unpack() NOC_OVERRIDE;
    virtual bool Save( std::string& error );
    bool IsBuildable() const;
    bool IsViewable() const;
    bool IsExportable() const;
    const std::string& GetName() const;
    u64 GetHash() const
    {
        return m_Path.Hash();
    }
    std::string GetFilePath();
    const Nocturnal::Path& GetPath() const
    {
        return m_Path;
    }
    virtual std::string GetIcon() const;
    const M_AttributeSmartPtr& GetAttributes();
    Luna::AttributeWrapper* FindAttribute( i32 slot );
    virtual void PopulateContextMenu( ContextMenuItemSet& menu );
    void RegisterAssetReferenceNode( Luna::AssetReferenceNode* node );
    void UnregisterAssetReferenceNode( Luna::AssetReferenceNode* node );
    const S_AssetReferenceNodeDumbPtr& GetAssetReferenceNodes() const;
    void ManageShaderGroups( const ContextMenuArgsPtr& args );
    Undo::CommandPtr AddAnimationSet( std::string& msg );
    virtual void Changed( Inspect::Control* control );
    virtual bool CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data ) NOC_OVERRIDE;
    virtual bool HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch = NULL ) NOC_OVERRIDE;
    virtual Undo::CommandPtr CopyFrom( Luna::PersistentData* src ) NOC_OVERRIDE;
    virtual void BuildFinished() {}

  protected:
    virtual Undo::CommandPtr CopyAttributesFrom( const Attribute::M_Attribute& srcAttribs );

  protected:
    // Note: These functions are not public because they do not properly
    // udpate all reference nodes in the hierarchy by themselves.  Use
    // AttributeExistenceCommand to perform these operations.
    bool AddAttribute( const Luna::AttributeWrapperPtr& attribute );
    bool RemoveAttribute( const Luna::AttributeWrapperPtr& attribute );
    friend class AttributeExistenceCommand;

  private:
    bool AddAttribute( const Luna::AttributeWrapperPtr& attribute, bool updatePackage );
    bool RemoveAttribute( const Luna::AttributeWrapperPtr& attribute, bool updatePackage );

    // Listeners
  private:
    AttributeExistenceSignature::Event m_AttributeAdded;
  public:
    void AddAttributeAddedListener( const AttributeExistenceSignature::Delegate& listener )
    {
      m_AttributeAdded.Add( listener );
    }
    void RemoveAttributeAddedListener( const AttributeExistenceSignature::Delegate& listener )
    {
      m_AttributeAdded.Remove( listener );
    }
  private:
    AttributeExistenceSignature::Event m_AttributeRemoved;
  public:
    void AddAttributeRemovedListener( const AttributeExistenceSignature::Delegate& listener )
    {
      m_AttributeRemoved.Add( listener );
    }
    void RemoveAttributeRemovedListener( const AttributeExistenceSignature::Delegate& listener )
    {
      m_AttributeRemoved.Remove( listener );
    }
  };
}
