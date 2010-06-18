#pragma once

#include "API.h"

#include "Platform/Types.h"

namespace Nocturnal
{
  enum
  {
#ifdef _WIN32
    CHUNK_MAGIC_PC = 'IGLM',      // Magic number stored at start of file to indicate it's an IG chunk file in little-endian
    CHUNK_MAGIC_HW = 'IGHW',      // Magic number stored at start of file to indicate it's an IG chunk file in big-endian
#else
    CHUNK_MAGIC_HW = 0x49474857,  // gcc compiler doesn't like multi-char character constants
#endif

    CHUNK_VERSION_128_ALIGN = 0x00000001,    // This is for backwards compatibility, we used to pad each chunk in the file to 128 bytes
    CHUNK_VERSION_16_ALIGN  = 0x00000002,    // This is the current version, where we pad each chunk in the file to 16 bytes
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
    u32     m_Magic;                        // Magic number. Identifies file as an IG chunk file
    u32     m_Version;                      // Version number. Must match loader version number for file to be loadable
    u32     m_ChunkCount;                   // The number of chunks in the file
    u32     m_HeaderPad;
  };

  struct ChunkHeader
  {
    u32     m_Type;                         // What type of data the chunk is
    u32     m_Offset;                       // The chunks offset from the head of the file
    u32     m_Size;                         // The chunks size
    u32     m_HeaderPad;
  };

#ifdef _WIN32
#pragma warning(disable:4200)
#endif

  //
  // Patches are applied at positions within the file specified through patch offsets relative the start of the file.
  //  These locations are patched by adding the start address of the file to the stored u32 at that location.
  //
  struct ChunkFilePatches
  {
    u32     m_PatchCount;                   // Number of patch offsets given
    u32     m_Patch[0];                     // Offsets to patch locations
  };

#ifdef _WIN32
#pragma warning(default:4200)
#endif

  struct ChunkFile
  {
    u32                m_FileAlignment;
    ChunkFileHeader*   m_FileHeader;
    ChunkHeader*       m_ChunkHeaders;
  };

  //
  // Loading API
  //
  FOUNDATION_API bool LoadChunkHeaders( ChunkFile& chunk_file, void* data, u32 data_size );
  FOUNDATION_API bool ParseChunkedData( ChunkFile& chunk_file, void* data, u32 data_size, void* data_to_fixup, u32 data_to_fixup_size );
  FOUNDATION_API bool ParseChunkedData( ChunkFile& chunk_file, void* data, u32 data_size );
  FOUNDATION_API bool FindChunkHeader( const ChunkFile& chunk_file, ChunkHeader*& chunk_header, u32 chunk_type );
}
