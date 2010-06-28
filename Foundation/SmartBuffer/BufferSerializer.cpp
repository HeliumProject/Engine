#include "BufferSerializer.h"
#include "SmartBuffer.h"
#include "ObjectBuffer.h"
#include "BasicBuffer.h"
#include "Fixup.h"

#include "Foundation/Log.h"
#include "Foundation/SmartBuffer/SmartLoader.h"

#include <iostream>
#include <sstream>
#include <fstream>

using namespace Nocturnal;

#ifdef PROFILE_ACCUMULATION
Profile::Accumulator BufferSerializer::s_ReadAccum ( "BufferSerializer Data Read" );
Profile::Accumulator BufferSerializer::s_WriteAccum ( "BufferSerializer Data Write" );
#endif

BufferSerializer::BufferSerializer()
: m_Platform( DEFAULT_PLATFORM )
{
}

BufferSerializer::BufferSerializer( BufferPlatform platform )
: m_Platform( platform )
{

}

BasicBufferPtr BufferSerializer::CreateBasic( u32 type, bool track )
{
    BasicBufferPtr return_val = new BasicBuffer();

    return_val->SetType( type );
    return_val->SetPlatform( m_Platform );

    // we don't track anonymous buffers because they could be empty or not pointed to from a non-anonymous buffer
    //  however, we do allow non-anonymous buffers to not be tracked in case they were allocated but not written to
    //  take, for instance some code that may or may not write data to a non-anonymous buffer, it could choose not
    //  to have the buffer be tracked in the serializer, and it would be discarded if it was not pointed to
    if ( type != 0xffffffff && track )
    {
        AddBuffer( return_val );
    }

    return return_val;
}

void BufferSerializer::AddBuffer( const SmartBufferPtr& buffer )
{
    m_Buffers.Append( buffer );
}

void BufferSerializer::AddBuffers( const BufferSerializer& serializer )
{
    S_SmartBufferPtr::Iterator itr = serializer.begin();
    S_SmartBufferPtr::Iterator end = serializer.end();
    for ( ; itr != end; ++itr )
    {
        AddBuffer( *itr );
    }
}

// we pad when we write to 16 bytes
const u32 BF_ALIGN = 16;
const u32 BF_ALIGN_MINUS_ONE = BF_ALIGN - 1;
const tchar BF_PAD_STR[BF_ALIGN+1] = TXT( "PAD0PAD1PAD2PAD3" );

struct P_Fixup
{
    u32             source_offset;
    u32             target_offset;

    u32             source_chunk_offset;
    SmartBufferPtr target_chunk_buffer;
};

u32 BufferSerializer::ComputeSize() const
{
    u32 computed_size = 0;
    bool align        = m_Platform == BufferPlatforms::Power32;

    // make a unique list of contained buffers
    S_SmartBufferPtr buffers;
    {
        S_SmartBufferPtr::Iterator itr = m_Buffers.Begin();
        S_SmartBufferPtr::Iterator end = m_Buffers.End();
        for ( ; itr != end; ++itr )
        {
            (*itr)->CollectChildren( buffers );
        }
    }

    // we have a file header
    computed_size += sizeof ( ChunkFileHeader );

    // 
    u32 num_chunks = (u32)buffers.Size();

    if ( num_chunks == 0 )
    {
        computed_size += sizeof( num_chunks );
    }
    else
    {
        // and N chunk headers
        computed_size += (u32)(buffers.Size() * sizeof( ChunkHeader ));
    }

    {
        u32 num_fixups_32 = 0;
        u32 num_fixups_64 = 0;

        S_SmartBufferPtr::Iterator itr = buffers.Begin();
        S_SmartBufferPtr::Iterator end = buffers.End();
        for ( ; itr != end; ++itr )
        {
            // the size of each buffer
            u32 buffer_size = (*itr)->GetSize();
            computed_size += buffer_size;

            // align to BF_ALIGN?
            if ( align )
            {
                computed_size += (buffer_size & BF_ALIGN_MINUS_ONE) != 0 ? BF_ALIGN - ( buffer_size & BF_ALIGN_MINUS_ONE ) : 0; 
            }

            SmartBuffer::M_OffsetToFixup::const_iterator of_itr = (*itr)->GetOutgoingFixups().begin();
            SmartBuffer::M_OffsetToFixup::const_iterator of_end = (*itr)->GetOutgoingFixups().end();
            for ( ; of_itr != of_end; ++of_itr )
            {
                if( (*of_itr).second->GetType() == FixupTypes::Pointer )
                {
                    PointerFixup* fixup = static_cast<PointerFixup*>( (*of_itr).second.Ptr() );

                    if (fixup->GetSize() == 4)
                    {
                        num_fixups_32++;
                    }
                    else
                    {
                        num_fixups_64++;
                    }
                }
            }
        }

        // the number of fixups
        computed_size += sizeof( num_fixups_32 );
        computed_size += sizeof( num_fixups_64 );

        //
        if ( num_fixups_32 == 0 )
        {
            computed_size += sizeof( u32 );
        }
        else
        {
            computed_size += num_fixups_32 * sizeof ( u32 );
        }

        if ( num_fixups_64 == 0 )
        {
            computed_size += sizeof( u32 );
        }
        else
        {
            computed_size += num_fixups_64 * sizeof ( u32 );
        }
    }

    return computed_size;
}


bool BufferSerializer::WriteToFile( const tchar* filename ) const
{
    bool return_val = false;

    tchar print[512];
    _sntprintf(print, sizeof(print) / sizeof( tchar ), TXT( "Writing '%s'" ), filename);
#pragma TODO("Profiler support for wide strings")
    PROFILE_SCOPE_ACCUM_VERBOSE(s_WriteAccum, ""/*print*/);
    Log::Debug( TXT( "%s\n" ), print);

    tofstream strm( filename, std::ios::out | std::ios::binary );
    if ( strm.is_open() )
    {
        if ( WriteToStream( strm ) )
        {
            return_val = true;
        }
        strm.close();
    }

    return return_val;
}

bool BufferSerializer::WriteToStream( tostream& strm ) const
{
    bool swizzle = m_Platform == BufferPlatforms::Power32;
    bool align   = m_Platform == BufferPlatforms::Power32;

    // track data for when we need to fixup
    typedef std::map< SmartBufferPtr, u32 > M_BuffU32;
    typedef std::vector< P_Fixup > V_Fixup;

    M_BuffU32 buffer_to_offset_map;

    V_Fixup fixup_32;
    V_Fixup fixup_64;

    // make a unique list of contained buffers
    S_SmartBufferPtr buffers;
    {
        S_SmartBufferPtr::Iterator itr = m_Buffers.Begin();
        S_SmartBufferPtr::Iterator end = m_Buffers.End();

        for ( ; itr != end; ++itr )
        {
            (*itr)->CollectChildren( buffers );
        }
    }

    // write a ChunkFileHeader
    {
        ChunkFileHeader file_header;
        memset( &file_header, 0, sizeof( ChunkFileHeader ) );

        file_header.m_Magic      = ConvertEndian( swizzle ? CHUNK_MAGIC_HW : CHUNK_MAGIC_PC, swizzle );
        file_header.m_Version    = ConvertEndian( CHUNK_VERSION_16_ALIGN, swizzle );
        file_header.m_ChunkCount = ConvertEndian( (u32)buffers.Size(), swizzle );

        strm.write( (const tchar*)&file_header, sizeof( ChunkFileHeader ) );
    }

    // how many chunks?
    u32 num_chunks = (u32)buffers.Size();

    // write a header for each chunk
    if ( num_chunks == 0 )
    {
        strm.write( (const tchar*)&num_chunks, sizeof( num_chunks ) );
    }
    else
    {
        // where are the buffers going to start? 
        u32 buffer_offset = ( num_chunks * sizeof( ChunkHeader ) ) + sizeof( ChunkFileHeader );

        S_SmartBufferPtr::Iterator itr = buffers.Begin();
        S_SmartBufferPtr::Iterator end = buffers.End();
        for ( u32 chunk_index = 0; itr != end; ++itr, ++chunk_index )
        {
            // a little sanity check
            NOC_ASSERT( (*itr)->GetPlatform() == m_Platform );
            NOC_ASSERT( (*itr)->GetSize() > 0 );

            // better not have a map for this id already
            bool inserted = buffer_to_offset_map.insert( M_BuffU32::value_type( (*itr), buffer_offset ) ).second;
            NOC_ASSERT( inserted );

            ChunkHeader chunk_header;
            memset( &chunk_header, 0, sizeof( ChunkHeader ) );

            chunk_header.m_Type   = ConvertEndian( (*itr)->GetType(), swizzle );
            chunk_header.m_Offset = ConvertEndian( buffer_offset, swizzle );
            chunk_header.m_Size   = ConvertEndian( (*itr)->GetSize(), swizzle );

            // move the offset
            buffer_offset += (*itr)->GetSize();

            // align to BF_ALIGN?
            if ( align && ((*itr)->GetSize() & BF_ALIGN_MINUS_ONE) != 0 )
            {
                buffer_offset += BF_ALIGN - ( (*itr)->GetSize() & BF_ALIGN_MINUS_ONE );
            }

            strm.write( (const tchar*)&chunk_header, sizeof( ChunkHeader ) );
        }
    }

    {
        S_SmartBufferPtr::Iterator itr = buffers.Begin();
        S_SmartBufferPtr::Iterator end = buffers.End();
        for ( u32 chunk_index = 0; itr != end; ++itr, ++chunk_index )
        {
            // go back and write the offset
            u32 chunk_start_loc = (u32)strm.tellp();

            // write the buffer
            u32 buffer_size = (*itr)->GetSize();
            const u8* buffer_data = (*itr)->GetData();
            strm.write( (const tchar*)buffer_data, buffer_size );

            //  align to boundary...
            if ( align && (buffer_size & BF_ALIGN_MINUS_ONE) != 0 )
            {
                u32 pad = BF_ALIGN - (buffer_size & BF_ALIGN_MINUS_ONE);
                strm.write( BF_PAD_STR, pad );
            }

            // store all the outgoing fixups and process them at the end..
            P_Fixup fix;
            fix.source_chunk_offset = chunk_start_loc;

            SmartBuffer::Location destination;

            SmartBuffer::M_OffsetToFixup::const_iterator of_itr = (*itr)->GetOutgoingFixups().begin();
            SmartBuffer::M_OffsetToFixup::const_iterator of_end = (*itr)->GetOutgoingFixups().end();
            for ( ; of_itr != of_end; ++of_itr )
            {
                if( (*of_itr).second->GetType() == FixupTypes::Pointer )
                {
                    PointerFixup* fixup = static_cast<PointerFixup*>( (*of_itr).second.Ptr() );
                    fixup->GetDestination( destination );

                    fix.source_offset       = (*of_itr).first;
                    fix.target_offset       = destination.first;
                    fix.target_chunk_buffer = destination.second;
                    if (fixup->GetSize() == 4)
                    {
                        fixup_32.push_back( fix );
                    }
                    else
                    {
                        fixup_64.push_back( fix );
                    }
                }
            }
        }
    }

    u32 test = (u32)strm.tellp();

    // write the number fixups
    u32 num_fixups_32 = (u32)fixup_32.size();
    u32 num_fixups_32_swizzled = ConvertEndian( num_fixups_32, swizzle );
    strm.write( (const tchar*)&num_fixups_32_swizzled, sizeof( num_fixups_32_swizzled ) );

    // now the fixups themselvs
    if ( num_fixups_32 == 0 )
    {
        strm.write( (const tchar*)&num_fixups_32, sizeof( num_fixups_32 ) );
    }
    else
    {
        V_Fixup::const_iterator itr = fixup_32.begin();
        V_Fixup::const_iterator end  = fixup_32.end();
        for( ; itr != end; ++itr )
        {
            M_BuffU32::const_iterator target_chunk_offset = buffer_to_offset_map.find( (*itr).target_chunk_buffer );
            NOC_ASSERT( target_chunk_offset != buffer_to_offset_map.end() );

            u32 curr_offset   = (u32)strm.tellp();
            u32 target_offset = ( (*target_chunk_offset).second + (*itr).target_offset );
            u32 source_offset = ( (*itr).source_chunk_offset + (*itr).source_offset );

            u32 target_offset_swizzled = ConvertEndian( target_offset, swizzle );
            u32 source_offset_swizzled = ConvertEndian( source_offset, swizzle );

            strm.seekp( source_offset );
            strm.write( (const tchar*)&target_offset_swizzled, sizeof( target_offset_swizzled ) );
            strm.seekp( curr_offset );
            strm.write( (const tchar*)&source_offset_swizzled, sizeof( source_offset_swizzled ) );
        }
    }

    // write the number fixups
    u32 num_fixups_64 = (u32)fixup_64.size();
    u32 num_fixups_64_swizzled = ConvertEndian( num_fixups_64, swizzle );
    strm.write( (const tchar*)&num_fixups_64_swizzled, sizeof( num_fixups_64_swizzled ) );

    if ( num_fixups_64 == 0 )
    {
        strm.write( (const tchar*)&num_fixups_64, sizeof( num_fixups_64 ) );
    }
    else
    {
        V_Fixup::const_iterator itr = fixup_64.begin();
        V_Fixup::const_iterator end  = fixup_64.end();
        for( ; itr != end; ++itr )
        {
            M_BuffU32::const_iterator target_chunk_offset = buffer_to_offset_map.find( (*itr).target_chunk_buffer );
            NOC_ASSERT( target_chunk_offset != buffer_to_offset_map.end() );

            u32 curr_offset   = (u32)strm.tellp();
            u32 target_offset = ( (*target_chunk_offset).second + (*itr).target_offset );
            u32 source_offset = ( (*itr).source_chunk_offset + (*itr).source_offset );

            u32 target_offset_swizzled = ConvertEndian( target_offset, swizzle );
            u32 source_offset_swizzled = ConvertEndian( source_offset+4, swizzle );
            u32 pad = 0x0;

            // an 8 byte pointer, but still uses a 4-byte pointer fixup runtime
            strm.seekp( source_offset );
            strm.write( (const tchar*)&pad, sizeof(pad) );
            strm.write( (const tchar*)&target_offset_swizzled, sizeof( target_offset_swizzled ) );
            strm.seekp( curr_offset );
            strm.write( (const tchar*)&source_offset_swizzled, sizeof( source_offset_swizzled ) );
        }
    }

    return true;
}

bool BufferSerializer::ReadFromFile( const tchar* filename )
{
    bool return_val = false;

    tchar print[512];
    _sntprintf(print, sizeof(print) / sizeof( tchar ), TXT( "Reading '%s'" ), filename);
#pragma TODO("Profiler support for wide strings")
    PROFILE_SCOPE_ACCUM_VERBOSE(s_ReadAccum, ""/*print*/);
    Log::Debug( TXT( "%s\n" ), print);

    tifstream strm( filename, std::ios::in | std::ios::binary );
    if ( strm.is_open() )
    {
        if ( ReadFromStream( strm ) )
        {
            return_val = true;
        }
        strm.close();
    }

    return return_val;
}

bool BufferSerializer::ReadFromStream( tistream& strm ) 
{
    //u32 starting_offset = strm.tellp();

    // read a ChunkFileHeader
    ChunkFileHeader file_header;
    strm.read( (tchar*)&file_header, sizeof( ChunkFileHeader ) );

    u32 test1 = (u32)strm.tellg();

    bool swizzle = false;

    // check that the header looks valid
    if ( file_header.m_Magic != CHUNK_MAGIC_PC )
    {
        u32 magic = ConvertEndian(file_header.m_Magic, true);

        if ( magic == CHUNK_MAGIC_HW )
        {
            swizzle = true;
        }
        else
        {
            return false;
        }
    }

    file_header.m_Version = ConvertEndian(file_header.m_Version, swizzle);
    file_header.m_ChunkCount = ConvertEndian(file_header.m_ChunkCount, swizzle);

    u32 alignment;
    switch( file_header.m_Version )
    {
    case CHUNK_VERSION_128_ALIGN:
        alignment = 128;
        break;

    case CHUNK_VERSION_16_ALIGN:
        alignment = 16;
        break;

    default:
        return false;
    }

    // 
    std::vector< ChunkHeader > chunk_headers( file_header.m_ChunkCount );
    std::vector< SmartBufferPtr > chunks( file_header.m_ChunkCount );
    std::map< u32, u32, std::greater<u32> > chunk_map;

    // read each chunk header
    for ( u32 chunk_index = 0; chunk_index < file_header.m_ChunkCount; ++chunk_index )
    {
        strm.read( (tchar*)&chunk_headers[ chunk_index ], sizeof( ChunkHeader ) );
        chunk_headers[ chunk_index ].m_Type = ConvertEndian(chunk_headers[ chunk_index ].m_Type, swizzle);
        chunk_headers[ chunk_index ].m_Offset = ConvertEndian(chunk_headers[ chunk_index ].m_Offset, swizzle);
        chunk_headers[ chunk_index ].m_Size = ConvertEndian(chunk_headers[ chunk_index ].m_Size, swizzle);
    }

    // create and read each chunk
    for ( u32 chunk_index = 0; chunk_index < file_header.m_ChunkCount; ++chunk_index )
    {
        const ChunkHeader& header = chunk_headers[ chunk_index ];
        SmartBufferPtr buffer = new SmartBuffer();
        buffer->SetPlatform( swizzle ? BufferPlatforms::Power32 : BufferPlatforms::x86 );

        //NOC_ASSERT( header.m_Offset == ( strm.tellp() - starting_offset ) );
        chunks[ chunk_index ] = buffer;

        buffer->Resize( header.m_Size );
        buffer->SetType( header.m_Type );
        strm.read( (tchar*)buffer->GetData(), buffer->GetSize() );

        NOC_ASSERT( chunk_map.find( header.m_Offset ) == chunk_map.end() );
        chunk_map[ header.m_Offset ] = chunk_index;

        //  is the buffer aligned?
        if ( swizzle && (buffer->GetSize() & ( alignment - 1 )) != 0 )
        {
            u32 pad = alignment - (buffer->GetSize() & ( alignment - 1 ) );
            strm.seekg( pad, std::ios_base::cur );
        }
    }

    u32 test = (u32)strm.tellg();

    // read the fixups
    u32 num_fixups;
    strm.read( (tchar*)&num_fixups, sizeof( num_fixups ) );
    num_fixups = ConvertEndian(num_fixups, swizzle);

    for ( u32 fixup_index = 0; fixup_index < num_fixups; ++fixup_index )
    {
        u32 source_offset;
        strm.read( (tchar*)&source_offset, sizeof( source_offset ) );
        source_offset = ConvertEndian(source_offset, swizzle);

        // figure out which buffer this fixup starts in
        std::map< u32, u32, std::greater<u32> >::iterator source_itr = chunk_map.lower_bound( source_offset );
        NOC_ASSERT( source_itr != chunk_map.end() );

        // get the necessary source data
        const ChunkHeader& source_header = chunk_headers[ (*source_itr).second ];
        SmartBufferPtr source_buffer = chunks[ (*source_itr).second ];

        u32* source_data = (u32*)(source_buffer->GetData() + ( source_offset - source_header.m_Offset ) );
        u32  dest_offset = *source_data;
        dest_offset = ConvertEndian(dest_offset, swizzle);

        // figure out which buffer this fixup ends in
        std::map< u32, u32, std::greater<u32> >::iterator dest_itr = chunk_map.lower_bound( dest_offset );
        NOC_ASSERT( dest_itr != chunk_map.end() );

        // get the necessary dest data
        const ChunkHeader& dest_header = chunk_headers[ (*dest_itr).second ];
        SmartBufferPtr dest_buffer = chunks[ (*dest_itr).second ];

        // do the fixup
        SmartBuffer::AddPointerFixup( source_buffer->GetOffsetLocation( source_offset - source_header.m_Offset ),
            dest_buffer->GetOffsetLocation  ( dest_offset   - dest_header.m_Offset ), 4 );
    }

    // read the fixups
    u32 num_fixups_64;
    strm.read( (tchar*)&num_fixups_64, sizeof( num_fixups_64 ) );
    num_fixups_64 = ConvertEndian(num_fixups_64, swizzle);

    if (!strm.eof())
    {
        for ( u32 fixup_index = 0; fixup_index < num_fixups_64; ++fixup_index )
        {
            u32 source_offset;
            strm.read( (tchar*)&source_offset, sizeof( source_offset ) );
            source_offset = ConvertEndian(source_offset, swizzle);

            // figure out which buffer this fixup starts in
            std::map< u32, u32, std::greater<u32> >::iterator source_itr = chunk_map.lower_bound( source_offset );
            NOC_ASSERT( source_itr != chunk_map.end() );

            // get the necessary source data
            const ChunkHeader& source_header = chunk_headers[ (*source_itr).second ];
            SmartBufferPtr source_buffer = chunks[ (*source_itr).second ];

            u32* source_data = (u32*)(source_buffer->GetData() + ( source_offset - source_header.m_Offset ) );
            u32  dest_offset = *source_data;
            dest_offset = ConvertEndian(dest_offset, swizzle);

            // figure out which buffer this fixup ends in
            std::map< u32, u32, std::greater<u32> >::iterator dest_itr = chunk_map.lower_bound( dest_offset );
            NOC_ASSERT( dest_itr != chunk_map.end() );

            // get the necessary dest data
            const ChunkHeader& dest_header = chunk_headers[ (*dest_itr).second ];
            SmartBufferPtr dest_buffer = chunks[ (*dest_itr).second ];

            // do the fixup
            SmartBuffer::AddPointerFixup( source_buffer->GetOffsetLocation( source_offset - source_header.m_Offset - 4),
                dest_buffer->GetOffsetLocation  ( dest_offset   - dest_header.m_Offset ), 8 );
        }
    }

    // store the chunks
    m_Buffers.Clear();

    {
        V_SmartBufferPtr::const_iterator itr = chunks.begin();
        V_SmartBufferPtr::const_iterator end = chunks.end();
        for ( ; itr != end; ++itr )
        {
            m_Buffers.Append( *itr );
        }
    }

    return true;
}
