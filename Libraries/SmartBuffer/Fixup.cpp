#include "Fixup.h"

using namespace Nocturnal;

Fixup::Fixup()
{

}

Fixup::~Fixup()
{
  NOC_ASSERT( m_RefCount == 0 );
}

PointerFixup::PointerFixup(u32 size)
: Fixup()
, m_HasReference ( false )
, m_Size (size)
, m_Destination ( 0, NULL )
{

}

PointerFixup::~PointerFixup()
{
  if ( m_HasReference )
  {
    NOC_ASSERT( m_Destination.second != NULL );
    m_Destination.second->DecrRefCount();

    m_HasReference = false;
  }

  m_Destination.second = NULL;
}

void PointerFixup::ChangeDestination( const SmartBuffer::Location& new_destination )
{
  bool had_ref = m_HasReference;
  SmartBuffer* old_buffer = m_Destination.second;

  m_Destination.first  = new_destination.first;
  m_Destination.second = new_destination.second.Ptr();

  m_HasReference = false;
  if ( m_Destination.second != NULL )
  {
    m_HasReference = true;
    m_Destination.second->IncrRefCount();
  }

  // we do this second in case the two pointers are the same
  if ( had_ref )
  {
    old_buffer->DecrRefCount();
  }
}

void PointerFixup::EraseIncoming( const SmartBuffer::DumbLocation& source_location ) const
{
  if ( m_Destination.second != NULL )
  {
    m_Destination.second->GetIncomingFixups().Remove( source_location );
  }
}

bool PointerFixup::DoFixup( const SmartBuffer::DumbLocation& source_location )
{
  NOC_ASSERT( source_location.second != NULL );
  NOC_ASSERT( source_location.first <= source_location.second->GetSize() );

  u32          source_offset = source_location.first;
  SmartBuffer* source_buffer = source_location.second;
  void**       source_addr   = (void**)(source_buffer->GetData() + source_offset);

  if ( m_Destination.second != NULL )
  {
    // check the destination data
    NOC_ASSERT( m_Destination.second != NULL );
    NOC_ASSERT( m_Destination.first <= m_Destination.second->GetSize() );

    // get the destination offset & buffer
    u32          destination_offset = m_Destination.first;
    SmartBuffer* destination_buffer = m_Destination.second;

    // set the correct value
    *source_addr = (void*)(destination_buffer->GetData() + destination_offset);

    // insert the fixups into our hashes
    destination_buffer->GetIncomingFixups().Append( source_location );

    // ok.. this is scary.. there is a potential for a circular reference 
    // here, so we attempt to address it
    if ( !m_HasReference )
    {
      m_HasReference = true;
      m_Destination.second->IncrRefCount();
    }
    if ( source_buffer == destination_buffer )
    {
      m_HasReference = false;
      m_Destination.second->DecrRefCount();
    }

    // everything linked
    return true;
  }

  // if we link to a missing buffer we ..
  else
  {
    // null out the pointer in the source data
    *source_addr = NULL;

    // we did not link
    return false;
  }
}

OffsetFixup::OffsetFixup( bool absolute )
: Fixup()
, m_HasReference ( false )
, m_Absolute ( absolute )
, m_Destination ( 0, NULL )
{

}

OffsetFixup::~OffsetFixup()
{
  if ( m_HasReference )
  {
    NOC_ASSERT( m_Destination.second != NULL );
    m_Destination.second->DecrRefCount();

    m_HasReference = false;
  }

  m_Destination.second = NULL;
}

void OffsetFixup::ChangeDestination( const SmartBuffer::Location& new_destination )
{
  bool had_ref = m_HasReference;
  SmartBuffer* old_buffer = m_Destination.second;

  m_Destination.first  = new_destination.first;
  m_Destination.second = new_destination.second.Ptr();

  m_HasReference = false;
  if ( m_Destination.second != NULL )
  {
    m_HasReference = true;
    m_Destination.second->IncrRefCount();
  }

  // we do this second in case the two pointers are the same
  if ( had_ref )
  {
    old_buffer->DecrRefCount();
  }
}

void OffsetFixup::EraseIncoming( const SmartBuffer::DumbLocation& source_location ) const
{
  if ( m_Destination.second != NULL )
  {
    m_Destination.second->GetIncomingFixups().Remove( source_location );
  }
}

bool OffsetFixup::DoFixup( const SmartBuffer::DumbLocation& source_location )
{
  NOC_ASSERT( source_location.second != NULL );
  NOC_ASSERT( source_location.first <= source_location.second->GetSize() );

  u32          source_offset = source_location.first;
  SmartBuffer* source_buffer = source_location.second;
  void**       source_addr   = (void**)(source_buffer->GetData() + source_offset);

  // set a temporary value
  *(i32*)source_addr = 0;

  if ( m_Destination.second != NULL )
  {
    // check the destination data
    NOC_ASSERT( m_Destination.second != NULL );
    NOC_ASSERT( m_Destination.first <= m_Destination.second->GetSize() );

    // get the destination offset & buffer
    u32          destination_offset = m_Destination.first;
    SmartBuffer* destination_buffer = m_Destination.second;

    if ( destination_buffer == source_buffer )
    {
      *(i32*)source_addr = destination_offset - source_offset;
    }

    // insert the fixups into our hashes
    destination_buffer->GetIncomingFixups().Append( source_location );

    // ok.. this is scary.. there is a potential for a circular reference 
    // here, so we attempt to address it
    if ( !m_HasReference )
    {
      m_HasReference = true;
      m_Destination.second->IncrRefCount();
    }
    if ( source_buffer == destination_buffer )
    {
      m_HasReference = false;
      m_Destination.second->DecrRefCount();
    }

    // everything linked
    return true;
  }

  // if we link to a missing buffer we ..
  else
  {
    // we did not link
    return false;
  }
}

VTableFixup::VTableFixup( u32 class_index, u32 size )
: Fixup()
, m_ClassIndex ( class_index )
, m_Size ( size )
{

}

bool VTableFixup::DoFixup( const SmartBuffer::DumbLocation& source_location )
{
  NOC_ASSERT( source_location.second != NULL );
  NOC_ASSERT( source_location.first <= source_location.second->GetSize() );

  u32          source_offset = source_location.first;
  SmartBuffer* source_buffer = source_location.second;
  void*        source_addr   = (void*)(source_buffer->GetData() + source_offset);

  // set our index into the buffer
  *(u32*)source_addr = m_ClassIndex;

  // win
  return true;
}
