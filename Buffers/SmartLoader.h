#pragma once

#include "API.h"

#include "Platform/Types.h"

namespace Helium
{
    enum
    {
#ifdef HELIUM_CC_CL
        CHUNK_MAGIC_LITTLE_ENDIAN   = 'HeLE',       // Magic number stored at start of file to indicate it's an helium chunk file in little-endian
        CHUNK_MAGIC_BIG_ENDIAN      = 'HeBE',       // Magic number stored at start of file to indicate it's an helium chunk file in big-endian
#else
        CHUNK_MAGIC_LITTLE_ENDIAN   = 0x48654C45,   // gcc compiler doesn't like multi-tchar_t character constants
        CHUNK_MAGIC_BIG_ENDIAN      = 0x48654245,   // gcc compiler doesn't like multi-tchar_t character constants
#endif

        CHUNK_VERSION_128_ALIGN     = 0x00000001,   // This is for backwards compatibility, we used to pad each chunk in the file to 128 bytes
        CHUNK_VERSION_16_ALIGN      = 0x00000002,   // This is the current version, where we pad each chunk in the file to 16 bytes
    };

    //
    // Chunk data file consists of:
    //   - ChunkFileHeader, 
    //   - ChunkHeaders
    //   - Chunks
    //   - ChunkFilePatches
    //
    struct ChunkFileHeader 
    {
        uint32_t     m_Magic;                        // Magic number. Identifies file as an helium chunk file
        uint32_t     m_Version;                      // Version number. Must match loader version number for file to be loadable
        uint32_t     m_ChunkCount;                   // The number of chunks in the file
        uint32_t     m_HeaderPad;
    };

    struct ChunkHeader
    {
        uint32_t     m_Type;                         // What type of data the chunk is
        uint32_t     m_Offset;                       // The chunks offset from the head of the file
        uint32_t     m_Size;                         // The chunks size
        uint32_t     m_HeaderPad;
    };

#ifdef HELIUM_CC_CL
# pragma warning(disable:4200)
#endif

    //
    // Patches are applied at positions within the file specified through patch offsets relative the start of the file.
    //  These locations are patched by adding the start address of the file to the stored uint32_t at that location.
    //
    struct ChunkFilePatches
    {
        uint32_t     m_PatchCount;                   // Number of patch offsets given
        uint32_t     m_Patch[0];                     // Offsets to patch locations
    };

#ifdef HELIUM_CC_CL
# pragma warning(default:4200)
#endif

    struct ChunkFile
    {
        uint32_t                m_FileAlignment;
        ChunkFileHeader*   m_FileHeader;
        ChunkHeader*       m_ChunkHeaders;
    };

    //
    // Loading API
    //
    HELIUM_BUFFERS_API bool LoadChunkHeaders( ChunkFile& chunk_file, void* data, uint32_t data_size );
    HELIUM_BUFFERS_API bool ParseChunkedData( ChunkFile& chunk_file, void* data, uint32_t data_size, void* data_to_fixup, uint32_t data_to_fixup_size );
    HELIUM_BUFFERS_API bool ParseChunkedData( ChunkFile& chunk_file, void* data, uint32_t data_size );
    HELIUM_BUFFERS_API bool FindChunkHeader( const ChunkFile& chunk_file, ChunkHeader*& chunk_header, uint32_t chunk_type );
}
