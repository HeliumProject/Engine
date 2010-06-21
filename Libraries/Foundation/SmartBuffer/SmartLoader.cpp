#include "SmartLoader.h"

#include "Platform/Assert.h"

using namespace Nocturnal;

bool Nocturnal::LoadChunkHeaders( ChunkFile& chunk_file, void* data, u32 data_size )
{
    NOC_ASSERT_MSG( data_size >= sizeof( ChunkFileHeader ), 
        ( "Insufficient data in buffer!\n" ) );

    // 
    chunk_file.m_FileHeader = (ChunkFileHeader*)data;

    // 
    // check that the header looks valid
    NOC_ASSERT_MSG( chunk_file.m_FileHeader->m_Magic == CHUNK_MAGIC_HW,
        ( "Invalid magic number!\n" ) );

    switch ( chunk_file.m_FileHeader->m_Version )
    {
    case CHUNK_VERSION_128_ALIGN:
        chunk_file.m_FileAlignment = 128;
        break;

    case CHUNK_VERSION_16_ALIGN:
        chunk_file.m_FileAlignment = 16;
        break;

    default:
        printf("Version wrong (%x, expected %x or %x)\n", chunk_file.m_FileHeader->m_Version, CHUNK_VERSION_128_ALIGN, CHUNK_VERSION_16_ALIGN );
        return false;
    }

    // fix the pointers
    chunk_file.m_ChunkHeaders = (ChunkHeader*)( (u8*)data + sizeof( ChunkFileHeader ) );

    return true;
}

bool Nocturnal::ParseChunkedData( ChunkFile& chunk_file, void* data, u32 data_size, void* data_to_fixup, u32 data_to_fixup_size )
{
    // first try to load the headers
    if ( !LoadChunkHeaders( chunk_file, data, data_size ) )
    {
        return false;
    }

    // now fixup the pointers
    // figure out the patch address
    void* patch_addr;

    if ( chunk_file.m_FileHeader->m_ChunkCount == 0 )
    {
        // no chunks?  then both the pointer was NULL.. 
        patch_addr = 
            ( (u8*)chunk_file.m_ChunkHeaders + sizeof( chunk_file.m_ChunkHeaders ) );
    }
    else
    {
        // we use size, offset, and alignment data from the last chunk to compute the space where our fixup data is
        ChunkHeader& last_header = chunk_file.m_ChunkHeaders[ chunk_file.m_FileHeader->m_ChunkCount - 1 ];

        patch_addr = 
            (u8*)data + 
            last_header.m_Offset + 
            last_header.m_Size +
            ( ( last_header.m_Size & (chunk_file.m_FileAlignment - 1) ) != 0 ? chunk_file.m_FileAlignment - ( last_header.m_Size & (chunk_file.m_FileAlignment - 1) ) : 0 );
    }

    // all of our patches assume there is a full header on the data, so take that into account here
    u32 offset_bias = sizeof ( ChunkFileHeader ) + sizeof ( ChunkHeader ) * chunk_file.m_FileHeader->m_ChunkCount;

    // process the fixups
    ChunkFilePatches* patches = (ChunkFilePatches*)( patch_addr );
    for ( u32 patch_index = 0; patch_index < patches->m_PatchCount; ++patch_index ) 
    {
        NOC_ASSERT( (u8*)&patches[ patch_index ] >= data && (u8*)&patches->m_Patch[patch_index] < (u8*)data + data_size );
        u32 patch_offset = patches->m_Patch[patch_index];

        // make sure we can apply the bias
        if ( patch_offset < offset_bias )
        {
            NOC_ASSERT( patch_offset >= offset_bias );
            continue;
        }

        // apply the bias
        patch_offset -= offset_bias;

        // any thing pointing past the end better be invalid
        if (patch_offset >= data_to_fixup_size)
        {
            NOC_ASSERT(patch_offset == 0xFFFFFFFF);
            continue;
        }

        // find the correct pointer to fix
        u32 *pointer = (u32*)((u8*)data_to_fixup + patch_offset);
        NOC_ASSERT( (u8*)pointer >= data_to_fixup && (u8*)pointer < (u8*)data_to_fixup + data_to_fixup_size );

        // handle the null pointer case
        if ( *pointer != 0 )
        {
            NOC_ASSERT( *pointer >= offset_bias );

            // apply the bias
            *pointer -= offset_bias;

            // make sure it is in a valid range 
            NOC_ASSERT( *pointer <= data_to_fixup_size );
            *pointer += (u32)(PointerSizedUInt)(data_to_fixup);
        }
    }

    // skip past the count var
    u8* end = ((u8*)patches) + sizeof(patches->m_PatchCount);

    // if we have elements in our zero sized array
    if (patches->m_PatchCount > 0)
    {
        // skip past the elements
        end += (sizeof(i32) * patches->m_PatchCount);
    }
    else
    {
        // skip past our ghost
        end += sizeof(i32);
    }

    // this should only hit if we are not out of buffer to parse
    NOC_ASSERT( end != (u8*)data + data_size);

    // process the fixups
    patches = (ChunkFilePatches*)( end );
    NOC_ASSERT( patches->m_PatchCount == 0 );

    return true;
}   

bool Nocturnal::ParseChunkedData( ChunkFile& chunk_file, void* data, u32 data_size )
{
    // first try to load the headers
    if ( !LoadChunkHeaders( chunk_file, data, data_size ) )
    {
        return false;
    }

    // now figure out if there are any patches
    u32 data_to_fixup_size = 0;
    void *data_to_fixup = NULL;

    if ( chunk_file.m_FileHeader->m_ChunkCount )
    {
        ChunkHeader& first_header = chunk_file.m_ChunkHeaders[ 0 ];
        ChunkHeader& last_header  = chunk_file.m_ChunkHeaders[ chunk_file.m_FileHeader->m_ChunkCount - 1 ];

        data_to_fixup_size = ( last_header.m_Offset - first_header.m_Offset ) + last_header.m_Size;
        data_to_fixup      = (u8*)data + first_header.m_Offset;
    }

    return ParseChunkedData( chunk_file, data, data_size, data_to_fixup, data_to_fixup_size );
}   

bool Nocturnal::FindChunkHeader( const ChunkFile& chunk_file, ChunkHeader*& chunk_header, u32 chunk_type )
{
    NOC_ASSERT( chunk_file.m_FileHeader );

    for ( u32 index = 0; index < chunk_file.m_FileHeader->m_ChunkCount; ++index )
    {
        ChunkHeader& cheader = chunk_file.m_ChunkHeaders[index];
        if ( cheader.m_Type == chunk_type )
        {
            chunk_header = &cheader;
            return true;
        }
    }

    return false;
}
