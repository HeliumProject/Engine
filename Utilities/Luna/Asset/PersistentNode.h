#pragma once

#include "AssetNode.h"
#include "PersistentData.h"

namespace Luna
{
  // Forwards
  class AssetManager;

  /////////////////////////////////////////////////////////////////////////////
  // Wrapper for nodes that contain Luna::Persistent data.
  // 
  class PersistentNode : public Luna::AssetNode
  {
  protected:
    Luna::PersistentDataPtr m_PersistentData;

  public:
    // Runtime Type Info
    LUNA_DECLARE_TYPE( Luna::PersistentNode, Luna::AssetNode );
    static void InitializeType();
    static void CleanupType();

  public:
    PersistentNode( Reflect::Element* data, Luna::AssetManager* assetManager );
    virtual ~PersistentNode();
    virtual void CreateChildren() NOC_OVERRIDE;
    virtual void ConnectProperties( EnumerateElementArgs& args ) NOC_OVERRIDE;
    virtual bool CanBeMoved() const NOC_OVERRIDE;
    virtual bool CanBeCopied() const NOC_OVERRIDE;
    virtual Inspect::ReflectClipboardDataPtr GetClipboardData() NOC_OVERRIDE;
    virtual bool CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data ) NOC_OVERRIDE;
    virtual bool HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch = NULL ) NOC_OVERRIDE;

  private:
    void PersistentDataChanged( const ObjectChangeArgs& args );

  public:
    // Templated getter functions for the persistent data backing this node
    template< class T >
    T* GetPersistentData()
    {
      return Reflect::ObjectCast< T >( m_PersistentData );
    }

    template< class T >
    const T* GetPersistentData() const
    {
      return Reflect::ConstObjectCast< T >( m_PersistentData );
    }
  };
  typedef Nocturnal::SmartPtr< Luna::PersistentNode > PersistentNodePtr;
}
