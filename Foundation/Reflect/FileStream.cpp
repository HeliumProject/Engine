#include "FileStream.h" 
#include "Exceptions.h"
#include "Foundation/Profile.h" 

#include <fstream>
#include <strstream>

using namespace Reflect;

FileStream::FileStream(const tstring& filename, bool write)
: m_Filename(filename)
, m_OpenForWrite(write)
{

}

FileStream::~FileStream()
{

}

void FileStream::Open()
{
    // deal with the mode bits.. 
    int fmode = std::ios_base::binary;
    if (m_OpenForWrite)
    {
        fmode |= std::ios_base::in | std::ios_base::out | std::ios_base::trunc;
    }
    else
    {
        fmode |= std::ios_base::in;
    }

    std::fstream* fstream = new std::fstream(); 
    fstream->open(m_Filename.c_str(), fmode);

    if (!fstream->is_open())
    {
        delete fstream;
        throw Reflect::StreamException( TXT( "Unable to open '%s' for %s" ) , m_Filename.c_str(), m_OpenForWrite ? "write" : "read");
    }

    m_Stream    = fstream; 
    m_OwnStream = true; 
}

void FileStream::Close()
{
    std::fstream* fstream = static_cast<std::fstream*>(GetInternal());
    fstream->close();

    if (fstream->is_open())
    {
        throw Reflect::StreamException( TXT( "Unable to close '%s' after %s" ), m_Filename.c_str(), m_OpenForWrite ? "write" : "read");
    }
    // parent destructor will clean up the internal stream
    // 
}
