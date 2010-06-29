#pragma once

#include "ComponentWrapper.h"
#include "PersistentData.h"
#include "Pipeline/Asset/AssetClass.h"
#include "Foundation/Automation/Event.h"
#include "Application/Undo/BatchCommand.h"

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
  struct ComponentExistenceArgs
  {
    Luna::AssetClass* m_Asset;
    Luna::ComponentWrapper* m_Component;
    
    ComponentExistenceArgs( Luna::AssetClass* asset, Luna::ComponentWrapper* component )
      : m_Asset( asset )
      , m_Component( component )
    {
    }
  };
  typedef Nocturnal::Signature< void, const ComponentExistenceArgs& > ComponentExistenceSignature;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper around an Asset::AssetClass.
  // 
  class AssetClass : public Luna::PersistentData
  {
  protected:
    M_ComponentSmartPtr m_Components;
    mutable tstring m_Name;
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
    virtual bool Save( tstring& error );
    bool IsBuildable() const;
    bool IsViewable() const;
    bool IsExportable() const;
    const tstring& GetName() const;
    u64 GetHash() const
    {
        return m_Path.Hash();
    }
    tstring GetFilePath();
    const Nocturnal::Path& GetPath() const
    {
        return m_Path;
    }
    virtual tstring GetIcon() const;
    const M_ComponentSmartPtr& GetComponents();
    Luna::ComponentWrapper* FindComponent( i32 slot );
    virtual void PopulateContextMenu( ContextMenuItemSet& menu );
    void RegisterAssetReferenceNode( Luna::AssetReferenceNode* node );
    void UnregisterAssetReferenceNode( Luna::AssetReferenceNode* node );
    const S_AssetReferenceNodeDumbPtr& GetAssetReferenceNodes() const;
    void ManageShaderGroups( const ContextMenuArgsPtr& args );
    Undo::CommandPtr AddAnimationSet( tstring& msg );
    virtual void Changed( Inspect::Control* control );
    virtual bool CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data ) NOC_OVERRIDE;
    virtual bool HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch = NULL ) NOC_OVERRIDE;
    virtual Undo::CommandPtr CopyFrom( Luna::PersistentData* src ) NOC_OVERRIDE;
    virtual void BuildFinished() {}

  protected:
    virtual Undo::CommandPtr CopyComponentsFrom( const Component::M_Component& srcComponents );

  protected:
    // Note: These functions are not public because they do not properly
    // udpate all reference nodes in the hierarchy by themselves.  Use
    // ComponentExistenceCommand to perform these operations.
    bool AddComponent( const Luna::ComponentWrapperPtr& component );
    bool RemoveComponent( const Luna::ComponentWrapperPtr& component );
    friend class ComponentExistenceCommand;

  private:
    bool AddComponent( const Luna::ComponentWrapperPtr& component, bool updatePackage );
    bool RemoveComponent( const Luna::ComponentWrapperPtr& component, bool updatePackage );

    // Listeners
  private:
    ComponentExistenceSignature::Event m_ComponentAdded;
  public:
    void AddComponentAddedListener( const ComponentExistenceSignature::Delegate& listener )
    {
      m_ComponentAdded.Add( listener );
    }
    void RemoveComponentAddedListener( const ComponentExistenceSignature::Delegate& listener )
    {
      m_ComponentAdded.Remove( listener );
    }
  private:
    ComponentExistenceSignature::Event m_ComponentRemoved;
  public:
    void AddComponentRemovedListener( const ComponentExistenceSignature::Delegate& listener )
    {
      m_ComponentRemoved.Add( listener );
    }
    void RemoveComponentRemovedListener( const ComponentExistenceSignature::Delegate& listener )
    {
      m_ComponentRemoved.Remove( listener );
    }
  };
}
