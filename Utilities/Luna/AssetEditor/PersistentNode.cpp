#include "Precompile.h"
#include "PersistentNode.h"

#include "FieldFactory.h"
#include "FieldNode.h"
#include "PersistentDataFactory.h"

#include "Asset/AssetFlags.h"
#include "Inspect/ReflectClipboardData.h"
#include "Core/PropertiesManager.h"

using namespace Luna;

// Definition
LUNA_DEFINE_TYPE( Luna::PersistentNode );

///////////////////////////////////////////////////////////////////////////////
// Static initialization.
// 
void PersistentNode::InitializeType()
{
  Reflect::RegisterClass<Luna::PersistentNode>( "Luna::PersistentNode" );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void PersistentNode::CleanupType()
{
  Reflect::UnregisterClass<Luna::PersistentNode>();
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
PersistentNode::PersistentNode( Reflect::Element* data, Luna::AssetManager* assetManager )
: Luna::AssetNode( assetManager )
{
  m_PersistentData = PersistentDataFactory::GetInstance()->Create( data, assetManager );
  SetIcon( data->GetClass()->GetProperty( Asset::AssetProperties::SmallIcon ) );
  SetName( data->GetTitle() );
  m_PersistentData->AddChangedListener( ObjectChangeSignature::Delegate( this, &PersistentNode::PersistentDataChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
PersistentNode::~PersistentNode()
{
  m_PersistentData->RemoveChangedListener( ObjectChangeSignature::Delegate( this, &PersistentNode::PersistentDataChanged ) );
}

///////////////////////////////////////////////////////////////////////////////
// Do not call this function on an Luna::PersistentNode.  You should call Unpack instead.
// 
void PersistentNode::CreateChildren()
{
  Luna::FieldFactory::GetInstance()->CreateChildFieldNodes( this, m_PersistentData->GetPackage< Reflect::Element >(), m_PersistentData->GetAssetManager() );
}

///////////////////////////////////////////////////////////////////////////////
// Called when the property sheet is being populated.  Tells the enumerator
// which items to inspect when this object is selected.
// 
void PersistentNode::ConnectProperties( EnumerateElementArgs& args )
{
  args.EnumerateElement( m_PersistentData->GetPackage< Reflect::Element >() );
}

///////////////////////////////////////////////////////////////////////////////
// By default, these nodes can be moved.
// 
bool PersistentNode::CanBeMoved() const
{
  return true; // This may not be right...
}

///////////////////////////////////////////////////////////////////////////////
// By default, these nodes can be copied.
// 
bool PersistentNode::CanBeCopied() const
{
  return true;
}

///////////////////////////////////////////////////////////////////////////////
// Returns clipboard format data for this node.
// 
Inspect::ReflectClipboardDataPtr PersistentNode::GetClipboardData()
{
  return m_PersistentData->GetClipboardData();
}

///////////////////////////////////////////////////////////////////////////////
// Pass through for determining if the clipboard data is valid to paste/drop on
// this node.
// 
bool PersistentNode::CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data )
{
  return m_PersistentData->CanHandleClipboardData( data );
}

///////////////////////////////////////////////////////////////////////////////
// Pass through for copying clipoard data onto the persistent object that this 
// class wraps.
// 
bool PersistentNode::HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch )
{
  return m_PersistentData->HandleClipboardData( data, op, batch );
}

///////////////////////////////////////////////////////////////////////////////
// Callback for when the persistent data has changed.  Tries to update the name
// on this node in case the data change caused it to be invalidated.
// 
void PersistentNode::PersistentDataChanged( const ObjectChangeArgs& args )
{
  SetName( m_PersistentData->GetPackage< Reflect::Element >()->GetTitle() );
}
