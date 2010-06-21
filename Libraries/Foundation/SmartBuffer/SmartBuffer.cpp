#include "SmartBuffer.h"
#include "Fixup.h"

#include "Platform/Assert.h"
#include "Foundation/Exception.h"
#include "Foundation/Log.h"
#include "Platform/Windows/Windows.h"

using namespace Nocturnal;

// These are project specific, and in the order of PC, PS3
const u32 SmartBuffer::s_PointerSizes[ BufferPlatforms::Count ] = { 4, 4 };
const bool SmartBuffer::s_BigEndian[ BufferPlatforms::Count ] = { false, true };

// For profiling memory usage
Profile::MemoryPoolHandle SmartBuffer::s_ObjectPool;
Profile::MemoryPoolHandle SmartBuffer::s_DataPool;

SmartBuffer::SmartBuffer()
: m_Name( "Unknown SmartBuffer" )
, m_Type ( -1 )
, m_Size( 0 )
, m_MaxSize( 0 )
, m_Capacity( 0 )
, m_OwnsData( true )
, m_Platform( DEFAULT_PLATFORM )
, m_Virtual( false )
, m_Data ( NULL )
{
    NOC_ASSERT( m_Platform >= 0 && m_Platform < BufferPlatforms::Count );
}

SmartBuffer::~SmartBuffer()
{
    NOC_ASSERT( m_RefCount == 0 );
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

        Profile::Memory::Deallocate( s_DataPool, m_Capacity );
    }

    if ( m_OwnsData )
    {
        DumbLocation ptr( 0, this );
        while( !m_OutgoingFixups.empty() )
        {
            ptr.first = m_OutgoingFixups.begin()->first;
            AddFixup( ptr, NULL );
        }

        NOC_ASSERT( m_IncomingFixups.Empty() );
    }
}

void* SmartBuffer::operator new (size_t bytes)
{
    static bool initialized = false;
    if (!initialized)
    {
        s_ObjectPool = Profile::Memory::CreatePool("SmartBuffer Objects");
        s_DataPool = Profile::Memory::CreatePool("SmartBuffer Data");
        initialized = true;
    }

    Profile::Memory::Allocate( s_ObjectPool, (u32)bytes );

    return ::malloc(bytes);
}

void SmartBuffer::operator delete (void *ptr, size_t bytes)
{
    ::free(ptr);

    Profile::Memory::Deallocate( s_ObjectPool, (u32)bytes );
}

void SmartBuffer::Reset()
{
    NOC_ASSERT( m_OwnsData );

    // erase the incoming and outgoing fixups
    DumbLocation ptr;
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

        Profile::Memory::Deallocate( s_DataPool, m_Capacity );
    }

    m_Data = 0;
    m_Size = 0;  
    m_MaxSize = 0;
    m_Capacity = 0;
}

void SmartBuffer::TakeData( u32& size, u8*& data )
{
    NOC_ASSERT( m_OwnsData );
    if ( m_OwnsData )
    {
        m_OwnsData = false;

        size = m_Size;
        data = m_Data;

        Profile::Memory::Deallocate( s_DataPool, m_Capacity );
    }
    else
    {
        size = 0;
        data = NULL;
    }
}

void SmartBuffer::SetMaxSize(u32 max)
{
    NOC_ASSERT( m_OwnsData );
    NOC_ASSERT( m_Size == 0 );
    NOC_ASSERT( m_Capacity == 0 );

    m_MaxSize = max;
}

void SmartBuffer::SetVirtual(u32 size)
{
    // cannot set virtual on a buffer which is not owned or has been used
    NOC_ASSERT( m_OwnsData );
    NOC_ASSERT( m_Size == 0 );
    NOC_ASSERT( m_Capacity == 0 );
    NOC_ASSERT( m_Virtual == false );

    Reset();

    // align to 4K page boundary and reserve the address space
    size = (size+4095) & ~4095;

    // allocate
    m_Data = (u8*)::VirtualAlloc(0,size,MEM_RESERVE,PAGE_READWRITE);
    if (m_Data==0)
    {
        throw Nocturnal::Exception("Out of virtual memory.");
    }

    Profile::Memory::Allocate( s_DataPool, size );

    m_Capacity = 0;
    m_Virtual = true;

    SetMaxSize(size);
}

void SmartBuffer::GrowBy(u32 size)
{
    NOC_ASSERT( m_OwnsData );

    // only grow the buffer if necessary
    if ( m_Size + size > m_Capacity )
    {
        if (m_Virtual)
        {
            if ( m_Size + size > m_MaxSize )
            {
                throw Nocturnal::Exception("Too much memory in virtual SmartBuffer");
            }

            // calculate how may new bytes we need to allocate and align that to a page boundary
            u32 difference = size - (m_Capacity - m_Size);
            difference = (difference + 4095) &~ 4095;

            // profiler keeps the allocation count so deallocate our old size and allocate our new total
            Profile::Memory::Deallocate( s_DataPool, m_Capacity );
            Profile::Memory::Allocate( s_DataPool, m_Capacity + difference );

            // Commit more storage to the end of our already commited section
            ::VirtualAlloc(m_Data+m_Capacity,difference,MEM_COMMIT,PAGE_READWRITE);
            m_Capacity += difference;
        }
        else
        {
            if ( m_MaxSize && m_Size + size > m_MaxSize )
            {
                throw Nocturnal::Exception("Exceeded max size of SmartBuffer (id 0x%x)", m_Type);
            }

            u32 difference = m_Size + size - m_Capacity;

            // profiler keeps the allocation count so deallocate our old size and allocate our new total
            Profile::Memory::Deallocate( s_DataPool, m_Capacity );
            Profile::Memory::Allocate( s_DataPool, m_Capacity + difference );

            void *ptr = ::realloc( m_Data, m_Capacity + difference );
            if (ptr == NULL)
            {
                if (!m_Name.empty())
                {
                    throw Nocturnal::Exception("Could not allocate %d bytes for '%s' (id 0x%x).", difference, m_Name.c_str(), m_Type);
                }
                else
                {
                    throw Nocturnal::Exception("Could not allocate %d bytes (id 0x%x).", difference, m_Type);
                }
            }

            // we succeeded
            m_Capacity += difference;

            // set the additional memory to 0
            memset( (void*)((u8*)ptr + m_Size), 0, m_Capacity - m_Size );

            // if realloc changes our ptr, we need to redo fixups
            if ( ptr != m_Data )
            {
                // cache the pointer
                m_Data = (u8*)ptr;

                // Fix incoming pointers
                if ( !m_IncomingFixups.Empty() )
                {
                    S_DumbLocation::Iterator itr = m_IncomingFixups.Begin();
                    S_DumbLocation::Iterator end = m_IncomingFixups.End();
                    for ( ; itr != end; ++itr )
                    {
                        DumbLocation& source = (*itr);

                        // get the target location from the source buffer
                        M_OffsetToFixup::iterator found = source.second->m_OutgoingFixups.find( source.first );
                        NOC_ASSERT( found != source.second->m_OutgoingFixups.end() );

                        // "do" fixup
                        (*found).second->DoFixup( source );
                    }
                }
            }
        }
    }
}

void SmartBuffer::Resize(u32 size)
{
    if ( m_OwnsData && m_Size == 0 && m_Capacity == 0 )
    {
        GrowBy( size );

        m_Size = size;
    }
    else
    {
        NOC_BREAK();
    }
}

void SmartBuffer::Reserve(u32 size)
{
    if ( m_OwnsData && m_Size == 0 && m_Capacity == 0 )
    {
        GrowBy( size );
    }
    else
    {
        NOC_BREAK();
    }
}

bool SmartBuffer::AdoptBuffer( const SmartBufferPtr& buffer )
{
    NOC_ASSERT( buffer->m_OwnsData == true );

    bool return_val = false;

    // this buffer needs to be empty before we can adopt in place
    if (m_Size == 0 )
    {
        // be sure not to leak memory
        if ( m_Capacity > 0 )
        {
            ::free( m_Data );

            Profile::Memory::Deallocate( s_DataPool, m_Capacity );

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
        SmartBuffer::Location destination;

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

void SmartBuffer::InheritFixups( const SmartBufferPtr& buffer, u32 offset )
{
    // inherit all the incoming fixups
    if ( !buffer->GetIncomingFixups().Empty() )
    {
        // first we copy the incoming fixups, since we are messing with that map 
        S_DumbLocation incoming_fixup_copy = buffer->GetIncomingFixups();

        SmartBuffer::S_DumbLocation::Iterator itr = incoming_fixup_copy.Begin();
        SmartBuffer::S_DumbLocation::Iterator end = incoming_fixup_copy.End();
        for( ; itr != end; ++itr )
        {
            // this is the old incoming fixup data
            DumbLocation source_location = (*itr);

            // get the target location from the source buffer
            M_OffsetToFixup::iterator i = source_location.second->m_OutgoingFixups.find( source_location.first );
            NOC_ASSERT( i != source_location.second->m_OutgoingFixups.end() );

            // get a pointer to the fixup
            FixupPtr fixup = (*i).second;

            // get the previous destination, we know this fixup must have one or else there 
            // wouldn't be an incoming entry for it
            Location old_destination;
            bool found_destination = fixup->GetDestination( old_destination );
            NOC_ASSERT( found_destination && old_destination.second == buffer );

            // null out the old fixup
            AddFixup( source_location, NULL );

            // now change the fixup to have the right Nocturnal::SmartPtr
            Location new_destination (old_destination.first + offset, this);
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
            DumbLocation old_source_location ( (*itr).first, buffer );
            AddFixup( old_source_location, NULL );

            // we need to offset the previous fixup by the correct offset in the current buffer
            DumbLocation new_source_location ( offset + (*itr).first, this );
            AddFixup( new_source_location, fixup );
        }
    }
}

void SmartBuffer::Dump()
{
#if 0
    Log::Print("Buffer Address %x [chunk ID = %x]\n",this,m_Type);
    Log::Print("%d outgoing\n",m_OutgoingFixups.size());

    // inherit all the outgoing fixups from the buffer
    {
        SmartBuffer::M_OffsetToFixup::const_iterator itr = m_OutgoingFixups.begin();
        SmartBuffer::M_OffsetToFixup::const_iterator end = m_OutgoingFixups.end();
        for( ; itr != end; ++itr )
        {
            SmartBuffer::Location destination;
            (*itr).second->GetDestination(destination);

            Log::Print("  Offset %d - points to buffer %x [ChunkID %x], offset %d\n",(*itr).first,destination.second,destination.second->m_Type,destination.first);
        }
    }

    Log::Print("%d incoming\n",m_IncomingFixups.size());

    // also inherit any incoming fixups
    {
        // first we copy the incoming fixups, since we are messing with that map 
        SmartBuffer::S_DumbLocation::Iterator itr = m_IncomingFixups.begin();
        SmartBuffer::S_DumbLocation::Iterator end = m_IncomingFixups.end();
        for( ; itr != end; ++itr )
        {
            const DumbLocation& source_location = (*itr);

            Log::Print("  Pointed to by buffer %x [Chunk ID = %x] (offset %d)\n",(*itr).second,(*itr).second->m_Type,(*itr).first );
        }
    }

    // dump the info for any incomming buffers
    {
        // first we copy the incoming fixups, since we are messing with that map 
        SmartBuffer::S_DumbLocation::Iterator itr = m_IncomingFixups.begin();
        SmartBuffer::S_DumbLocation::Iterator end = m_IncomingFixups.end();
        for( ; itr != end; ++itr )
        {
            (*itr).second->Dump();
        }
    }
#endif
}

bool SmartBuffer::AddFixup( const DumbLocation& source, const FixupPtr& fixup )
{
    // validate some of the parameters
    NOC_ASSERT( source.second != NULL );
    NOC_ASSERT( source.second->m_OwnsData );

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

bool SmartBuffer::AddOffsetFixup( const Location& source, const Location& destination,bool absolute)
{
    // double check the source data
    NOC_ASSERT( source.second != NULL );
    NOC_ASSERT( source.first <= source.second->m_Size );

    // get the source offset, buffer, & address
    u32          source_offset = source.first;
    SmartBuffer* source_buffer = source.second.Ptr();

    // find the offset within our data
    DumbLocation source_location( source_offset, source_buffer );

    OffsetFixup* new_fixup = new OffsetFixup(absolute);
    new_fixup->ChangeDestination( destination );

    return AddFixup( source_location, new_fixup );
}

bool SmartBuffer::AddPointerFixup( const Location& source, const Location& destination, u32 size )
{
    // double check the source data
    NOC_ASSERT( source.second != NULL );
    NOC_ASSERT( source.first <= source.second->m_Size );

    // get the source offset, buffer, & address
    u32          source_offset = source.first;
    SmartBuffer* source_buffer = source.second.Ptr();

    // find the offset within our data
    DumbLocation source_location( source_offset, source_buffer );

    if ( size == 0 )
    {
        size = source.second->GetPlatformPtrSize();
    }

    PointerFixup* new_fixup = new PointerFixup(size);
    new_fixup->ChangeDestination( destination );

    return AddFixup( source_location, new_fixup );
}

bool SmartBuffer::AddVTableFixup( const Location& source, u32 class_index, u32 size )
{  
    // double check the source data
    NOC_ASSERT( source.second != NULL );
    NOC_ASSERT( source.first <= source.second->m_Size );

    // get the source offset, buffer, & address
    u32          source_offset = source.first;
    SmartBuffer* source_buffer = source.second.Ptr();

    // find the offset within our data
    DumbLocation source_location( source_offset, source_buffer );

    // 
    if ( size == 0 )
    {
        size = source.second->GetPlatformPtrSize();
    }

    VTableFixup* new_fixup = new VTableFixup(class_index, size);
    return AddFixup( source_location, new_fixup );
}

void SmartBuffer::Write(const Location& pointer,const void* src,u32 size)
{
    NOC_ASSERT( pointer.second != NULL );
    NOC_ASSERT( pointer.first <= pointer.second->m_Size );

    u32          p_offset = pointer.first;
    SmartBuffer* p_buffer = pointer.second.Ptr();

    // the new data cannot go outside the existing buffer
    NOC_ASSERT( p_buffer->m_OwnsData );
    NOC_ASSERT( p_buffer->GetSize() >= p_offset + size );

    u8* dst = p_buffer->m_Data+p_offset;
    memcpy(dst,src,size);
}

void SmartBuffer::WriteI8(const Location& pointer,i8 val)
{
    Write(pointer,&val,sizeof(i8));
}

void SmartBuffer::WriteU8(const Location& pointer,u8 val)
{
    Write(pointer,&val,sizeof(u8));
}

void SmartBuffer::WriteI16(const Location& pointer,i16 val)
{
    val = ConvertEndian(val,pointer.second->IsPlatformBigEndian());
    Write(pointer,&val,sizeof(i16));
}

void SmartBuffer::WriteU16(const Location& pointer,u16 val)
{
    val = ConvertEndian(val,pointer.second->IsPlatformBigEndian());
    Write(pointer,&val,sizeof(u16));
}

void SmartBuffer::WriteI32(const Location& pointer,i32 val)
{
    val = ConvertEndian(val,pointer.second->IsPlatformBigEndian());
    Write(pointer,&val,sizeof(i32));
}

void SmartBuffer::WriteU32(const Location& pointer,u32 val)
{
    val = ConvertEndian(val,pointer.second->IsPlatformBigEndian());
    Write(pointer,&val,sizeof(u32));
}

void SmartBuffer::WriteI64(const Location& pointer,i64 val)
{
    val = ConvertEndian(val,pointer.second->IsPlatformBigEndian());
    Write(pointer,&val,sizeof(i64));
}

void SmartBuffer::WriteU64(const Location& pointer,u64 val)
{
    val = ConvertEndian(val,pointer.second->IsPlatformBigEndian());
    Write(pointer,&val,sizeof(u64));
}

void SmartBuffer::WriteF32(const Location& pointer,f32 val)
{
    u32 v = ConvertEndianFloatToU32(val,pointer.second->IsPlatformBigEndian());
    Write(pointer,&v,sizeof(v));
}

void SmartBuffer::WriteF64(const Location& pointer,f64 val)
{
    u64 v = ConvertEndianDoubleToU64(val,pointer.second->IsPlatformBigEndian());
    Write(pointer,&v,sizeof(v));
}