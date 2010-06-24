#include "CompressionUtilities.h" 
#include "Exceptions.h" 

#include <zlib.h> 

namespace Reflect
{
    static const u32 ZLIB_BUFFER_SIZE = 16 * 1024; 

    // helper struct to make zlib deflate initialization exception-safe: 
    //
    struct zlibOutputStream : public z_stream
    {
        zlibOutputStream()
        {
            zalloc = Z_NULL; 
            zfree  = Z_NULL; 
            opaque = Z_NULL; 

            int ok = deflateInit(this, Z_DEFAULT_COMPRESSION); 

            if( ok != Z_OK )
            {
                throw Nocturnal::Exception( TXT( "zlib deflateInit error" ) ); 
            }
        }

        ~zlibOutputStream()
        {
            deflateEnd(this); 
        }
    }; 

    // helper struct to make zlib inflate initialization exception-safe:
    // 
    struct zlibInputStream : public z_stream
    {
        zlibInputStream()
        {
            zalloc = Z_NULL; 
            zfree  = Z_NULL; 
            opaque = Z_NULL; 

            int ok = inflateInit(this); 

            if( ok != Z_OK )
            {
                throw Nocturnal::Exception( TXT( "zlib inflateInit error" ) ); 
            }
        }

        ~zlibInputStream()
        {
            inflateEnd(this); 
        }
    }; 

    // returns the size of the compressed data. 
    // 
    int CompressToStream(Reflect::Stream& reflectStream, const char* data, u32 size)
    {
        REFLECT_SCOPE_TIMER((""));

        const u32 compressBufferSize = ZLIB_BUFFER_SIZE;
        char      compressBuffer[compressBufferSize]; 

        zlibOutputStream zStream; 

        zStream.avail_in = size; 
        zStream.next_in  = (Bytef*) data; 

        int totalOut = 0; 

        // this is a pretty simple use of the zlib compressor
        // we have a fixed input size, and we compress all of that into our stack
        // allocated buffer, multiple times if need be.. 
        // 
        while(1)
        {
            zStream.avail_out = compressBufferSize; 
            zStream.next_out  = (Bytef*) &compressBuffer[0]; 

            // since we have a fixed size input, this is always Z_FINISH 
            int ret = deflate(&zStream, Z_FINISH); 

            if( ret == Z_STREAM_ERROR )
            {
                throw Nocturnal::Exception( TXT( "zlib error while compressing" ) ); 
            }

            int outputSize = compressBufferSize - zStream.avail_out; 
            reflectStream.WriteBuffer(compressBuffer, outputSize); 

            totalOut += outputSize; 

            if(ret == Z_STREAM_END)
                break; 
        }

        return totalOut; 
    }

    int DecompressFromStream(Reflect::Stream& reflectStream, int inputBytes, char* output, int outputBytes)
    {
        REFLECT_SCOPE_TIMER((""));

        const u32 fileBufferSize = ZLIB_BUFFER_SIZE;
        char      fileBuffer[fileBufferSize]; 

        zlibInputStream zStream; 

        zStream.avail_out = outputBytes; 
        zStream.next_out  = (Bytef*) output; 

        int bytesRemaining = inputBytes; 

        // again, this is pretty simple case because we know both 
        // how large the input is as well as the expected output size. 
        // 
        while(1)
        {
            int bytesRead = std::min<i32>(bytesRemaining, fileBufferSize); 

            if(bytesRead > 0)
            {
                reflectStream.ReadBuffer(&fileBuffer[0], bytesRead); 
                bytesRemaining -= bytesRead; 
            }

            int flushMode = (bytesRemaining == 0) ? Z_FINISH : Z_NO_FLUSH; 

            zStream.avail_in = bytesRead; 
            zStream.next_in  = (Bytef*) &fileBuffer[0]; 

            int ret = inflate(&zStream, flushMode); 

            // early out if we are all done
            if(ret == Z_STREAM_END)
            {
                break; 
            }

            // happens when we get to the end of the input or output
            if(ret == Z_BUF_ERROR && zStream.avail_out == 0)
            {
                throw Nocturnal::Exception( TXT( "zlib decompression overflow" ) ); 
            }

            if( ret != Z_OK )
            {
                throw Nocturnal::Exception( TXT( "zlib error while decompressing" ) ); 
            }
        }

        int bytesDecompressed = outputBytes - zStream.avail_out; 
        return bytesDecompressed; 
    }
}
