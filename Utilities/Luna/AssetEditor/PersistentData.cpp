#include "Precompile.h"
#include "PersistentData.h"

#include "PersistentDataFactory.h"

#include "Inspect/ClipboardElementArray.h"
#include "Undo/BatchCommand.h"
#include "Undo/PropertyCommand.h"

using namespace Luna;

LUNA_DEFINE_TYPE( Luna::PersistentData );

///////////////////////////////////////////////////////////////////////////////
// Static intialization.
// 
void PersistentData::InitializeType()
{
  Reflect::RegisterClass<Luna::PersistentData>( "Luna::PersistentData" );
  PersistentDataFactory::GetInstance()->Register( Reflect::GetType< Reflect::Element >(), &Luna::PersistentData::CreatePersistentData );
}

///////////////////////////////////////////////////////////////////////////////
// Static cleanup.
// 
void PersistentData::CleanupType()
{
  Reflect::UnregisterClass<Luna::PersistentData>();
}

///////////////////////////////////////////////////////////////////////////////
// Static creator function.
// 
Luna::PersistentDataPtr PersistentData::CreatePersistentData( Reflect::Element* element, Luna::AssetManager* assetManager )
{
  return new Luna::PersistentData( element, assetManager );
}

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
PersistentData::PersistentData( Reflect::Element* element, Luna::AssetManager* assetManager )
: Luna::Persistent( element )
, m_AssetManager( assetManager )
{
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
PersistentData::~PersistentData()
{
}

///////////////////////////////////////////////////////////////////////////////
// Returns the Asset Manager for this object.
// 
Luna::AssetManager* PersistentData::GetAssetManager() const
{
  return m_AssetManager;
}

///////////////////////////////////////////////////////////////////////////////
// Wraps this package's persistent data in a clipboard data object.
// 
Inspect::ReflectClipboardDataPtr PersistentData::GetClipboardData() const
{
  const Reflect::Element* package = GetPackage< Reflect::Element >();
  Inspect::ClipboardElementArrayPtr data = new Inspect::ClipboardElementArray();
  data->SetCommonBaseTypeID( package->GetType() );
  data->Add( package );
  return data;
}

///////////////////////////////////////////////////////////////////////////////
// Derived clases should NOC_OVERRIDE this function to specify whether or not they
// can accept the clipboard data.  This function is sometimes called while items
// are being dragged, so it needs to be fast.
// 
bool PersistentData::CanHandleClipboardData( const Inspect::ReflectClipboardDataPtr& data )
{
  Inspect::ClipboardElementArray* elementArray = Reflect::ObjectCast< Inspect::ClipboardElementArray >( data );
  if ( elementArray )
  {
    if ( elementArray->GetCommonBaseTypeID() == GetPackage< Reflect::Element >()->GetType() )
    {
      if ( elementArray->m_Elements.size() == 1 )
      {
        // Clipboard contains a single item that is the same type as this one.
        return true;
      }
    }
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Derived classes should NOC_OVERRIDE this function to handle drag/drop and
// copy/paste operations.
// 
bool PersistentData::HandleClipboardData( const Inspect::ReflectClipboardDataPtr& data, ClipboardOperation op, Undo::BatchCommand* batch )
{
  if ( CanHandleClipboardData( data ) )
  {
    if ( op == ClipboardOperations::Copy )
    {
      Inspect::ClipboardElementArray* elementArray = Reflect::AssertCast< Inspect::ClipboardElementArray >( data );
      const Reflect::ElementPtr& clipboardPkg = elementArray->m_Elements.front();
      Luna::PersistentDataPtr src = PersistentDataFactory::GetInstance()->Create( clipboardPkg, m_AssetManager );
      batch->Push( CopyFrom( src ) );
      return true;
    }
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Copies the persistent data from the specified source into this object's 
// persistent data.  Returns an undoable command for the operation or NULL
// if no change occurred.
// 
Undo::CommandPtr PersistentData::CopyFrom( Luna::PersistentData* src )
{
  Reflect::Element* dstPkg = GetPackage< Reflect::Element >();
  NOC_ASSERT( dstPkg );

  Reflect::Element* srcPkg = src->GetPackage< Reflect::Element >();
  NOC_ASSERT( srcPkg );

  if ( !srcPkg->Equals( dstPkg ) )
  {
    return SnapShot( srcPkg );
  }

  return NULL;
}
