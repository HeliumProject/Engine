#include "FoundationPch.h"
#include "SmartLoader.h"

#include "Platform/Assert.h"

#include "Foundation/Log.h"

using namespace Helium;

bool Helium::LoadChunkHeaders( ChunkFile& chunk_file, void* data, uint32_t data_size )
{
    HELIUM_ASSERT_MSG( data_size >= sizeof( ChunkFileHeader ), 
        TXT( "Insufficient data in buffer!" ) );

    // 
    chunk_file.m_FileHeader = (ChunkFileHeader*)data;

    // 
    // check that the header looks valid
    HELIUM_ASSERT_MSG( chunk_file.m_FileHeader->m_Magic == CHUNK_MAGIC_HW,
        TXT( "Invalid magic number!" ) );

    switch ( chunk_file.m_FileHeader->m_Version )
    {
    case CHUNK_VERSION_128_ALIGN:
        chunk_file.m_FileAlignment = 128;
        break;

    case CHUNK_VERSION_16_ALIGN:
        chunk_file.m_FileAlignment = 16;
        break;

    default:
        Log::Error( TXT( "Version wrong (%x, expected %x or %x)\n" ), chunk_file.m_FileHeader->m_Version, CHUNK_VERSION_128_ALIGN, CHUNK_VERSION_16_ALIGN );
        return false;
    }

    // fix the pointers
    chunk_file.m_ChunkHeaders = (ChunkHeader*)( (uint8_t*)data + sizeof( ChunkFileHeader ) );

    return true;
}

bool Helium::ParseChunkedData( ChunkFile& chunk_file, void* data, uint32_t data_size, void* data_to_fixup, uint32_t data_to_fixup_size )
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
            ( (uint8_t*)chunk_file.m_ChunkHeaders + sizeof( chunk_file.m_ChunkHeaders ) );
    }
    else
    {
        // we use size, offset, and alignment data from the last chunk to compute the space where our fixup data is
        ChunkHeader& last_header = chunk_file.m_ChunkHeaders[ chunk_file.m_FileHeader->m_ChunkCount - 1 ];

        patch_addr = 
            (uint8_t*)data + 
            last_header.m_Offset + 
            last_header.m_Size +
            ( ( last_header.m_Size & (chunk_file.m_FileAlignment - 1) ) != 0 ? chunk_file.m_FileAlignment - ( last_header.m_Size & (chunk_file.m_FileAlignment - 1) ) : 0 );
    }

    // all of our patches assume there is a full header on the data, so take that into account here
    uint32_t offset_bias = sizeof ( ChunkFileHeader ) + sizeof ( ChunkHeader ) * chunk_file.m_FileHeader->m_ChunkCount;

    // process the fixups
    ChunkFilePatches* patches = (ChunkFilePatches*)( patch_addr );
    for ( uint32_t patch_index = 0; patch_index < patches->m_PatchCount; ++patch_index ) 
    {
        HELIUM_ASSERT( (uint8_t*)&patches[ patch_index ] >= data && (uint8_t*)&patches->m_Patch[patch_index] < (uint8_t*)data + data_size );
        uint32_t patch_offset = patches->m_Patch[patch_index];

        // make sure we can apply the bias
        if ( patch_offset < offset_bias )
        {
            HELIUM_ASSERT( patch_offset >= offset_bias );
            continue;
        }

        // apply the bias
        patch_offset -= offset_bias;

        // any thing pointing past the end better be invalid
        if (patch_offset >= data_to_fixup_size)
        {
            HELIUM_ASSERT(patch_offset == 0xFFFFFFFF);
            continue;
        }

        // find the correct pointer to fix
        uint32_t *pointer = (uint32_t*)((uint8_t*)data_to_fixup + patch_offset);
        HELIUM_ASSERT( (uint8_t*)pointer >= data_to_fixup && (uint8_t*)pointer < (uint8_t*)data_to_fixup + data_to_fixup_size );

        // handle the null pointer case
        if ( *pointer != 0 )
        {
            HELIUM_ASSERT( *pointer >= offset_bias );

            // apply the bias
            *pointer -= offset_bias;

            // make sure it is in a valid range 
            HELIUM_ASSERT( *pointer <= data_to_fixup_size );
            *pointer += (uint32_t)(uintptr_t)(data_to_fixup);
        }
    }

    // skip past the count var
    uint8_t* end = ((uint8_t*)patches) + sizeof(patches->m_PatchCount);

    // if we have elements in our zero sized array
    if (patches->m_PatchCount > 0)
    {
        // skip past the elements
        end += (sizeof(int32_t) * patches->m_PatchCount);
    }
    else
    {
        // skip past our ghost
        end += sizeof(int32_t);
    }

    // this should only hit if we are not out of buffer to parse
    HELIUM_ASSERT( end != (uint8_t*)data + data_size);

    // process the fixups
    patches = (ChunkFilePatches*)( end );
    HELIUM_ASSERT( patches->m_PatchCount == 0 );

    return true;
}   

bool Helium::ParseChunkedData( ChunkFile& chunk_file, void* data, uint32_t data_size )
{
    // first try to load the headers
    if ( !LoadChunkHeaders( chunk_file, data, data_size ) )
    {
        return false;
    }

    // now figure out if there are any patches
    uint32_t data_to_fixup_size = 0;
    void *data_to_fixup = NULL;

    if ( chunk_file.m_FileHeader->m_ChunkCount )
    {
        ChunkHeader& first_header = chunk_file.m_ChunkHeaders[ 0 ];
        ChunkHeader& last_header  = chunk_file.m_ChunkHeaders[ chunk_file.m_FileHeader->m_ChunkCount - 1 ];

        data_to_fixup_size = ( last_header.m_Offset - first_header.m_Offset ) + last_header.m_Size;
        data_to_fixup      = (uint8_t*)data + first_header.m_Offset;
    }

    return ParseChunkedData( chunk_file, data, data_size, data_to_fixup, data_to_fixup_size );
}   

bool Helium::FindChunkHeader( const ChunkFile& chunk_file, ChunkHeader*& chunk_header, uint32_t chunk_type )
{
    HELIUM_ASSERT( chunk_file.m_FileHeader );

    for ( uint32_t index = 0; index < chunk_file.m_FileHeader->m_ChunkCount; ++index )
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
