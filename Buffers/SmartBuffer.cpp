#include "BuffersPch.h"
#include "SmartBuffer.h"

#include "Platform/Assert.h"
#include "Platform/Exception.h"
#include "Foundation/Log.h"

using namespace Helium;

// These are project specific, and in the order of PC, PS3
const uint32_t SmartBuffer::s_PointerSizes[ 2 ] = { 4, 4 }; // big and little endian
const bool SmartBuffer::s_BigEndian[ 2 ] = { false, true };

Fixup::Fixup()
{

}

Fixup::~Fixup()
{
    HELIUM_ASSERT( GetRefCount() == 0 );
}

PointerFixup::PointerFixup(uint32_t size)
: Fixup()
, m_HasReference ( false )
, m_Size (size)
, m_Destination ( 0, (SmartBuffer*)NULL )
{

}

PointerFixup::~PointerFixup()
{
    if ( m_HasReference )
    {
        HELIUM_ASSERT( m_Destination.second != NULL );
        m_Destination.second->DecrRefCount();

        m_HasReference = false;
    }

    m_Destination.second = NULL;
}

void PointerFixup::ChangeDestination( const BufferLocation& new_destination )
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

void PointerFixup::EraseIncoming( const DumbBufferLocation& source_location ) const
{
    if ( m_Destination.second != NULL )
    {
        m_Destination.second->GetIncomingFixups().Remove( source_location );
    }
}

bool PointerFixup::DoFixup( const DumbBufferLocation& source_location )
{
    HELIUM_ASSERT( source_location.second != NULL );
    HELIUM_ASSERT( source_location.first <= source_location.second->GetSize() );

    uint32_t          source_offset = source_location.first;
    SmartBuffer* source_buffer = source_location.second;
    void**       source_addr   = (void**)(source_buffer->GetData() + source_offset);

    if ( m_Destination.second != NULL )
    {
        // check the destination data
        HELIUM_ASSERT( m_Destination.second != NULL );
        HELIUM_ASSERT( m_Destination.first <= m_Destination.second->GetSize() );

        // get the destination offset & buffer
        uint32_t          destination_offset = m_Destination.first;
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
, m_Destination ( 0, (SmartBuffer*)NULL )
{

}

OffsetFixup::~OffsetFixup()
{
    if ( m_HasReference )
    {
        HELIUM_ASSERT( m_Destination.second != NULL );
        m_Destination.second->DecrRefCount();

        m_HasReference = false;
    }

    m_Destination.second = NULL;
}

void OffsetFixup::ChangeDestination( const BufferLocation& new_destination )
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

void OffsetFixup::EraseIncoming( const DumbBufferLocation& source_location ) const
{
    if ( m_Destination.second != NULL )
    {
        m_Destination.second->GetIncomingFixups().Remove( source_location );
    }
}

bool OffsetFixup::DoFixup( const DumbBufferLocation& source_location )
{
    HELIUM_ASSERT( source_location.second != NULL );
    HELIUM_ASSERT( source_location.first <= source_location.second->GetSize() );

    uint32_t          source_offset = source_location.first;
    SmartBuffer* source_buffer = source_location.second;
    void**       source_addr   = (void**)(source_buffer->GetData() + source_offset);

    // set a temporary value
    *(int32_t*)source_addr = 0;

    if ( m_Destination.second != NULL )
    {
        // check the destination data
        HELIUM_ASSERT( m_Destination.second != NULL );
        HELIUM_ASSERT( m_Destination.first <= m_Destination.second->GetSize() );

        // get the destination offset & buffer
        uint32_t          destination_offset = m_Destination.first;
        SmartBuffer* destination_buffer = m_Destination.second;

        if ( destination_buffer == source_buffer )
        {
            *(int32_t*)source_addr = destination_offset - source_offset;
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

VTableFixup::VTableFixup( uint32_t class_index, uint32_t size )
: Fixup()
, m_ClassIndex ( class_index )
, m_Size ( size )
{

}

bool VTableFixup::DoFixup( const DumbBufferLocation& source_location )
{
    HELIUM_ASSERT( source_location.second != NULL );
    HELIUM_ASSERT( source_location.first <= source_location.second->GetSize() );

    uint32_t          source_offset = source_location.first;
    SmartBuffer* source_buffer = source_location.second;
    void*        source_addr   = (void*)(source_buffer->GetData() + source_offset);

    // set our index into the buffer
    *(uint32_t*)source_addr = m_ClassIndex;

    // win
    return true;
}

SmartBuffer::SmartBuffer()
: m_Name( TXT( "Unknown SmartBuffer" ) )
, m_Type ( -1 )
, m_Size( 0 )
, m_MaxSize( 0 )
, m_Capacity( 0 )
, m_OwnsData( true )
, m_ByteOrder( Helium::PlatformByteOrder )
, m_Virtual( false )
, m_Data ( NULL )
{
    HELIUM_ASSERT( m_ByteOrder == ByteOrders::LittleEndian || m_ByteOrder == ByteOrders::BigEndian );
}

SmartBuffer::~SmartBuffer()
{
    HELIUM_ASSERT( GetRefCount() == 0 );
    if ( m_OwnsData && m_Data != NULL )
    {
        if (m_Virtual)
        {
            ::VirtualFree(m_Data,m_Capacity,MEM_DECOMMIT);
            ::VirtualFree(m_Data,0,MEM_RELEASE);
        }
        else
        {
            ::free( m_Data );
        }
    }

    if ( m_OwnsData )
    {
        DumbBufferLocation ptr( 0, this );
        while( !m_OutgoingFixups.empty() )
        {
            ptr.first = m_OutgoingFixups.begin()->first;
            AddFixup( ptr, NULL );
        }

        HELIUM_ASSERT( m_IncomingFixups.Empty() );
    }
}

void* SmartBuffer::operator new (size_t bytes)
{
    return ::malloc(bytes);
}

void SmartBuffer::operator delete (void *ptr, size_t bytes)
{
    ::free(ptr);
}

void SmartBuffer::Reset()
{
    HELIUM_ASSERT( m_OwnsData );

    // erase the incoming and outgoing fixups
    DumbBufferLocation ptr;
    while( !m_IncomingFixups.Empty() )
    {
        ptr = *m_IncomingFixups.Begin();
        AddFixup( ptr, NULL );
    }

    ptr.second = this;
    while( !m_OutgoingFixups.empty() )
    {
        ptr.first = m_OutgoingFixups.begin()->first;
        AddFixup( ptr, NULL );
    }

    if (m_Data)
    {
        if (m_Virtual)
        {
            // release memory pages but not the address space
            ::VirtualFree(m_Data,m_Capacity,MEM_DECOMMIT);
            ::VirtualFree(m_Data,0,MEM_RELEASE);
        }
        else
        {
            ::free(m_Data);
        }
    }

    m_Data = 0;
    m_Size = 0;  
    m_MaxSize = 0;
    m_Capacity = 0;
}

void SmartBuffer::TakeData( uint32_t& size, uint8_t*& data )
{
    HELIUM_ASSERT( m_OwnsData );
    if ( m_OwnsData )
    {
        m_OwnsData = false;

        size = m_Size;
        data = m_Data;
    }
    else
    {
        size = 0;
        data = NULL;
    }
}

void SmartBuffer::SetMaxSize(uint32_t max)
{
    HELIUM_ASSERT( m_OwnsData );
    HELIUM_ASSERT( m_Size == 0 );
    HELIUM_ASSERT( m_Capacity == 0 );

    m_MaxSize = max;
}

void SmartBuffer::SetVirtual(uint32_t size)
{
    // cannot set virtual on a buffer which is not owned or has been used
    HELIUM_ASSERT( m_OwnsData );
    HELIUM_ASSERT( m_Size == 0 );
    HELIUM_ASSERT( m_Capacity == 0 );
    HELIUM_ASSERT( m_Virtual == false );

    Reset();

    // align to 4K page boundary and reserve the address space
    size = (size+4095) & ~4095;

    // allocate
    m_Data = (uint8_t*)::VirtualAlloc(0,size,MEM_RESERVE,PAGE_READWRITE);
    if (m_Data==0)
    {
        throw Helium::Exception( TXT( "Out of virtual memory." ) );
    }

    m_Capacity = 0;
    m_Virtual = true;

    SetMaxSize(size);
}

void SmartBuffer::GrowBy(uint32_t size)
{
    HELIUM_ASSERT( m_OwnsData );

    // only grow the buffer if necessary
    if ( m_Size + size > m_Capacity )
    {
        if (m_Virtual)
        {
            if ( m_Size + size > m_MaxSize )
            {
                throw Helium::Exception( TXT( "Too much memory in virtual SmartBuffer" ) );
            }

            // calculate how may new bytes we need to allocate and align that to a page boundary
            uint32_t difference = size - (m_Capacity - m_Size);
            difference = (difference + 4095) &~ 4095;

            // Commit more storage to the end of our already commited section
            ::VirtualAlloc(m_Data+m_Capacity,difference,MEM_COMMIT,PAGE_READWRITE);
            m_Capacity += difference;
        }
        else
        {
            if ( m_MaxSize && m_Size + size > m_MaxSize )
            {
                throw Helium::Exception( TXT( "Exceeded max size of SmartBuffer (id 0x%x)" ), m_Type);
            }

            uint32_t difference = m_Size + size - m_Capacity;

            void *ptr = ::realloc( m_Data, m_Capacity + difference );
            if (ptr == NULL)
            {
                if (!m_Name.empty())
                {
                    throw Helium::Exception( TXT( "Could not allocate %d bytes for '%s' (id 0x%x)." ), difference, m_Name.c_str(), m_Type);
                }
                else
                {
                    throw Helium::Exception( TXT( "Could not allocate %d bytes (id 0x%x)." ), difference, m_Type);
                }
            }

            // we succeeded
            m_Capacity += difference;

            // set the additional memory to 0
            memset( (void*)((uint8_t*)ptr + m_Size), 0, m_Capacity - m_Size );

            // if realloc changes our ptr, we need to redo fixups
            if ( ptr != m_Data )
            {
                // cache the pointer
                m_Data = (uint8_t*)ptr;

                // Fix incoming pointers
                if ( !m_IncomingFixups.Empty() )
                {
                    S_DumbBufferLocation::Iterator itr = m_IncomingFixups.Begin();
                    S_DumbBufferLocation::Iterator end = m_IncomingFixups.End();
                    for ( ; itr != end; ++itr )
                    {
                        DumbBufferLocation& source = (*itr);

                        // get the target location from the source buffer
                        M_OffsetToFixup::iterator found = source.second->m_OutgoingFixups.find( source.first );
                        HELIUM_ASSERT( found != source.second->m_OutgoingFixups.end() );

                        // "do" fixup
                        (*found).second->DoFixup( source );
                    }
                }
            }
        }
    }
}

void SmartBuffer::Resize(uint32_t size)
{
    if ( m_OwnsData && m_Size == 0 && m_Capacity == 0 )
    {
        GrowBy( size );

        m_Size = size;
    }
    else
    {
        HELIUM_BREAK();
    }
}

void SmartBuffer::Reserve(uint32_t size)
{
    if ( m_OwnsData && m_Size == 0 && m_Capacity == 0 )
    {
        GrowBy( size );
    }
    else
    {
        HELIUM_BREAK();
    }
}

bool SmartBuffer::AdoptBuffer( const SmartBufferPtr& buffer )
{
    HELIUM_ASSERT( buffer->m_OwnsData == true );

    bool return_val = false;

    // this buffer needs to be empty before we can adopt in place
    if (m_Size == 0 )
    {
        // be sure not to leak memory
        if ( m_Capacity > 0 )
        {
            ::free( m_Data );

            m_Data = NULL;
            m_Capacity = 0;
        }

        // because we're adopting we get all the data w/o doing any copying
        m_Data = buffer->m_Data;
        m_Size = buffer->m_Size;
        m_Capacity = buffer->m_Capacity;

        // we need to take all the fixups and inherit them into this new buffer
        InheritFixups( buffer, 0 );

        // tell the other buffer that it no longer owns the data
        buffer->m_OwnsData = false;

        // success
        return_val = true;
    }

    return return_val;
}

void SmartBuffer::CollectChildren( S_SmartBufferPtr& buffers )
{
    // we don't bother collecting empty buffers
    if ( GetSize() > 0 && buffers.Append( SmartBufferPtr (this) ) )
    {
        BufferLocation destination;

        SmartBuffer::M_OffsetToFixup::const_iterator itr = m_OutgoingFixups.begin();
        SmartBuffer::M_OffsetToFixup::const_iterator end = m_OutgoingFixups.end();
        for( ; itr != end; ++itr )
        {
            if ( (*itr).second->GetDestination( destination ) )
            {
                destination.second->CollectChildren( buffers );
            }
        }
    }
}

void SmartBuffer::InheritFixups( const SmartBufferPtr& buffer, uint32_t offset )
{
    // inherit all the incoming fixups
    if ( !buffer->GetIncomingFixups().Empty() )
    {
        // first we copy the incoming fixups, since we are messing with that map 
        S_DumbBufferLocation incoming_fixup_copy = buffer->GetIncomingFixups();

        S_DumbBufferLocation::Iterator itr = incoming_fixup_copy.Begin();
        S_DumbBufferLocation::Iterator end = incoming_fixup_copy.End();
        for( ; itr != end; ++itr )
        {
            // this is the old incoming fixup data
            DumbBufferLocation source_location = (*itr);

            // get the target location from the source buffer
            M_OffsetToFixup::iterator i = source_location.second->m_OutgoingFixups.find( source_location.first );
            HELIUM_ASSERT( i != source_location.second->m_OutgoingFixups.end() );

            // get a pointer to the fixup
            FixupPtr fixup = (*i).second;

            // get the previous destination, we know this fixup must have one or else there 
            // wouldn't be an incoming entry for it
            BufferLocation old_destination;
            bool found_destination = fixup->GetDestination( old_destination );
            HELIUM_ASSERT( found_destination && old_destination.second == buffer );

            // null out the old fixup
            AddFixup( source_location, NULL );

            // now change the fixup to have the right Helium::SmartPtr
            BufferLocation new_destination (old_destination.first + offset, this);
            fixup->ChangeDestination( new_destination );

            // optimization just incase this fixup is completely internal to buffer
            if ( source_location.second == buffer.Ptr() )
            {
                source_location.first  += offset;
                source_location.second  = this;
            }

            // restore the fixup with the correct destination
            AddFixup( source_location, fixup );
        }
    }

    // inherit all the outgoing fixups from the buffer
    if ( !buffer->m_OutgoingFixups.empty() )
    {
        M_OffsetToFixup outgoing_fixups_copy = buffer->m_OutgoingFixups;

        SmartBuffer::M_OffsetToFixup::const_iterator itr = outgoing_fixups_copy.begin();
        SmartBuffer::M_OffsetToFixup::const_iterator end = outgoing_fixups_copy.end();
        for( ; itr != end; ++itr )
        {
            // get a pointer to the fixup
            FixupPtr fixup = (*itr).second;

            // first remove the old fixup
            DumbBufferLocation old_source_location ( (*itr).first, buffer );
            AddFixup( old_source_location, NULL );

            // we need to offset the previous fixup by the correct offset in the current buffer
            DumbBufferLocation new_source_location ( offset + (*itr).first, this );
            AddFixup( new_source_location, fixup );
        }
    }
}

void SmartBuffer::Dump()
{
    Log::Print( TXT("Buffer Address %x [chunk ID = %x]\n"), this, m_Type );
    Log::Print( TXT("%d outgoing\n"), m_OutgoingFixups.size() );

    // inherit all the outgoing fixups from the buffer
    {
        SmartBuffer::M_OffsetToFixup::const_iterator itr = m_OutgoingFixups.begin();
        SmartBuffer::M_OffsetToFixup::const_iterator end = m_OutgoingFixups.end();
        for( ; itr != end; ++itr )
        {
            BufferLocation destination;
            (*itr).second->GetDestination(destination);

            Log::Print( TXT("  Offset %d - points to buffer %x [ChunkID %x], offset %d\n"), (*itr).first, destination.second, destination.second->m_Type, destination.first );
        }
    }

    Log::Print( TXT("%d incoming\n"), m_IncomingFixups.Size() );

    // also inherit any incoming fixups
    {
        // first we copy the incoming fixups, since we are messing with that map 
        S_DumbBufferLocation::Iterator itr = m_IncomingFixups.Begin();
        S_DumbBufferLocation::Iterator end = m_IncomingFixups.End();
        for( ; itr != end; ++itr )
        {
            const DumbBufferLocation& source_location = (*itr);

            Log::Print( TXT("  Pointed to by buffer %x [Chunk ID = %x] (offset %d)\n"), (*itr).second, (*itr).second->m_Type, (*itr).first );
        }
    }

    // dump the info for any incomming buffers
    {
        // first we copy the incoming fixups, since we are messing with that map 
        S_DumbBufferLocation::Iterator itr = m_IncomingFixups.Begin();
        S_DumbBufferLocation::Iterator end = m_IncomingFixups.End();
        for( ; itr != end; ++itr )
        {
            (*itr).second->Dump();
        }
    }
}

bool SmartBuffer::AddFixup( const DumbBufferLocation& source, const FixupPtr& fixup )
{
    // validate some of the parameters
    HELIUM_ASSERT( source.second != NULL );
    HELIUM_ASSERT( source.second->m_OwnsData );

    // remove any previous fixup
    M_OffsetToFixup::iterator found = source.second->m_OutgoingFixups.find( source.first );
    if ( found != source.second->m_OutgoingFixups.end() )
    {
        (*found).second->EraseIncoming( source );
    }

    // 
    if ( fixup.ReferencesObject() && fixup->DoFixup( source ) )
    {
        source.second->m_OutgoingFixups[ source.first ] = fixup;
        return true;
    }
    else
    {
        source.second->m_OutgoingFixups.erase( source.first );
        return false;
    }
}

bool SmartBuffer::AddOffsetFixup( const BufferLocation& source, const BufferLocation& destination,bool absolute)
{
    // double check the source data
    HELIUM_ASSERT( source.second != NULL );
    HELIUM_ASSERT( source.first <= source.second->m_Size );

    // get the source offset, buffer, & address
    uint32_t          source_offset = source.first;
    SmartBuffer* source_buffer = source.second.Ptr();

    // find the offset within our data
    DumbBufferLocation source_location( source_offset, source_buffer );

    OffsetFixup* new_fixup = new OffsetFixup(absolute);
    new_fixup->ChangeDestination( destination );

    return AddFixup( source_location, new_fixup );
}

bool SmartBuffer::AddPointerFixup( const BufferLocation& source, const BufferLocation& destination, uint32_t size )
{
    // double check the source data
    HELIUM_ASSERT( source.second != NULL );
    HELIUM_ASSERT( source.first <= source.second->m_Size );

    // get the source offset, buffer, & address
    uint32_t          source_offset = source.first;
    SmartBuffer* source_buffer = source.second.Ptr();

    // find the offset within our data
    DumbBufferLocation source_location( source_offset, source_buffer );

    if ( size == 0 )
    {
        size = source.second->GetPlatformPtrSize();
    }

    PointerFixup* new_fixup = new PointerFixup(size);
    new_fixup->ChangeDestination( destination );

    return AddFixup( source_location, new_fixup );
}

bool SmartBuffer::AddVTableFixup( const BufferLocation& source, uint32_t class_index, uint32_t size )
{  
    // double check the source data
    HELIUM_ASSERT( source.second != NULL );
    HELIUM_ASSERT( source.first <= source.second->m_Size );

    // get the source offset, buffer, & address
    uint32_t          source_offset = source.first;
    SmartBuffer* source_buffer = source.second.Ptr();

    // find the offset within our data
    DumbBufferLocation source_location( source_offset, source_buffer );

    // 
    if ( size == 0 )
    {
        size = source.second->GetPlatformPtrSize();
    }

    VTableFixup* new_fixup = new VTableFixup(class_index, size);
    return AddFixup( source_location, new_fixup );
}

void SmartBuffer::Write(const BufferLocation& pointer,const void* src,uint32_t size)
{
    HELIUM_ASSERT( pointer.second != NULL );
    HELIUM_ASSERT( pointer.first <= pointer.second->m_Size );

    uint32_t          p_offset = pointer.first;
    SmartBuffer* p_buffer = pointer.second.Ptr();

    // the new data cannot go outside the existing buffer
    HELIUM_ASSERT( p_buffer->m_OwnsData );
    HELIUM_ASSERT( p_buffer->GetSize() >= p_offset + size );

    uint8_t* dst = p_buffer->m_Data+p_offset;
    memcpy(dst,src,size);
}

void SmartBuffer::WriteI8(const BufferLocation& pointer,int8_t val)
{
    Write(pointer,&val,sizeof(int8_t));
}

void SmartBuffer::WriteU8(const BufferLocation& pointer,uint8_t val)
{
    Write(pointer,&val,sizeof(uint8_t));
}

void SmartBuffer::WriteI16(const BufferLocation& pointer,int16_t val)
{
    val = ConvertEndian(val,pointer.second->IsPlatformBigEndian());
    Write(pointer,&val,sizeof(int16_t));
}

void SmartBuffer::WriteU16(const BufferLocation& pointer,uint16_t val)
{
    val = ConvertEndian(val,pointer.second->IsPlatformBigEndian());
    Write(pointer,&val,sizeof(uint16_t));
}

void SmartBuffer::WriteI32(const BufferLocation& pointer,int32_t val)
{
    val = ConvertEndian(val,pointer.second->IsPlatformBigEndian());
    Write(pointer,&val,sizeof(int32_t));
}

void SmartBuffer::WriteU32(const BufferLocation& pointer,uint32_t val)
{
    val = ConvertEndian(val,pointer.second->IsPlatformBigEndian());
    Write(pointer,&val,sizeof(uint32_t));
}

void SmartBuffer::WriteI64(const BufferLocation& pointer,int64_t val)
{
    val = ConvertEndian(val,pointer.second->IsPlatformBigEndian());
    Write(pointer,&val,sizeof(int64_t));
}

void SmartBuffer::WriteU64(const BufferLocation& pointer,uint64_t val)
{
    val = ConvertEndian(val,pointer.second->IsPlatformBigEndian());
    Write(pointer,&val,sizeof(uint64_t));
}

void SmartBuffer::WriteF32(const BufferLocation& pointer,float32_t val)
{
    uint32_t v = ConvertEndianFloatToU32(val,pointer.second->IsPlatformBigEndian());
    Write(pointer,&v,sizeof(v));
}

void SmartBuffer::WriteF64(const BufferLocation& pointer,float64_t val)
{
    uint64_t v = ConvertEndianDoubleToU64(val,pointer.second->IsPlatformBigEndian());
    Write(pointer,&v,sizeof(v));
}