#include "Stream.h" 

#include "Foundation/Profile.h" 
#include "Exceptions.h"

using namespace Reflect;

Profile::Accumulator g_StreamWrite( TXT( "Reflect Stream Write" ) );
Profile::Accumulator g_StreamRead( TXT( "Reflect Stream Read" ) ); 

Stream::Stream()
: m_Stream(NULL)
, m_OwnStream(false)
{

}

Stream::Stream(std::iostream* stream, bool ownStream) : 
m_Stream(stream), 
m_OwnStream(ownStream)
{

}

Stream::~Stream()
{
    if(m_OwnStream)
    {
        delete m_Stream; 
        m_Stream    = NULL; 
        m_OwnStream = false; 
    }
}

void Stream::Open()
{
    // no op
}

void Stream::Close()
{
    // no op
}

void Stream::SkipWhitespace()
{
    *m_Stream >> std::ws; 
}

Stream& Stream::SeekWrite(std::streamoff offset, std::ios_base::seekdir dir)
{
    m_Stream->seekp(offset, dir); 
    return *this; 
}

Stream& Stream::SeekRead(std::streamoff offset, std::ios_base::seekdir dir)
{
    m_Stream->seekg(offset, dir); 
    return *this; 
}

std::streampos Stream::TellRead()
{
    return m_Stream->tellg(); 
}

std::streampos Stream::TellWrite()
{
    return m_Stream->tellp(); 
}

std::streamsize Stream::BytesRead() const
{
    return m_Stream->gcount(); 
}

Stream& Stream::WriteBuffer(const void* t, std::streamsize size)
{
    PROFILE_SCOPE_ACCUM(g_StreamWrite); 

    m_Stream->write((const char*)t, size); 

    if (m_Stream->fail())
    {
        throw Reflect::StreamException( TXT( "General write failure") ); 
    }

    return *this; 
}

Stream& Stream::ReadBuffer(void* t, std::streamsize size)
{
    PROFILE_SCOPE_ACCUM(g_StreamRead); 

    m_Stream->read((char*)t, size); 

    if (m_Stream->fail() && !m_Stream->eof())
    {
        throw Reflect::StreamException( TXT( "General read failure" ) ); 
    }

    return *this; 
}

bool Stream::Fail(void)
{
    return m_Stream->fail(); 
}

bool Stream::Done()
{
    return m_Stream->eof(); 
}

void Stream::Clear()
{
    m_Stream->clear(); 
}

Stream& Stream::Flush()
{
    m_Stream->flush(); 
    return *this; 
}

void Stream::SetPrecision(int p)
{
    m_Stream->precision(p); 
}

std::streamsize Stream::BytesAvailable()
{
    return m_Stream->rdbuf()->in_avail();
}