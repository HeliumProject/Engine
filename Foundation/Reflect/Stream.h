#pragma once

#include "API.h"  
#include "Exceptions.h"

#include "Platform/Assert.h"

namespace Reflect
{
    extern Profile::Accumulator g_StreamWrite;
    extern Profile::Accumulator g_StreamRead; 

    //
    // Stream object, read and write data to/from a buffer
    //

    template< class C >
    class Stream : public Nocturnal::RefCountBase< Stream< C > >
    {
    public: 
        Stream()
            : m_Stream( NULL )
            , m_OwnStream( false )
        {

        }

        Stream( std::basic_iostream< C, std::char_traits<C> >* stream, bool ownStream = false )
            : m_Stream( stream )
            , m_OwnStream( ownStream )
        {

        }

        virtual ~Stream()
        {
            if ( m_OwnStream )
            {
                delete m_Stream; 
                m_Stream    = NULL; 
                m_OwnStream = false; 
            }
        }

        virtual void Open()
        {

        }

        virtual void Close()
        {

        }

        void Clear()
        {
            m_Stream->clear(); 
        }

        void SetPrecision(int p)
        {
            m_Stream->precision(p); 
        }

        void SkipWhitespace()
        {
            *m_Stream >> std::ws; 
        }

        std::streampos TellRead()
        {
            return m_Stream->tellg(); 
        }

        std::streampos TellWrite()
        {
            return m_Stream->tellp(); 
        }

        std::streamsize BytesRead()
        {
            return m_Stream->gcount(); 
        }

        std::streamsize BytesAvailable()
        {
            return m_Stream->rdbuf()->in_avail();
        }

        Stream& SeekRead(std::streamoff offset, std::ios_base::seekdir dir)
        {
            m_Stream->seekg(offset, dir); 
            return *this; 
        }

        Stream& SeekWrite(std::streamoff offset, std::ios_base::seekdir dir)
        {
            m_Stream->seekp(offset, dir); 
            return *this; 
        }

        Stream& ReadBuffer(void* t, std::streamsize size)
        {
            PROFILE_SCOPE_ACCUM(g_StreamRead); 

            m_Stream->read((char*)t, size); 

            if (m_Stream->fail() && !m_Stream->eof())
            {
                throw Reflect::StreamException( TXT( "General read failure" ) ); 
            }

            return *this; 
        }

        template <typename T>
        inline Stream& Read(T* ptr)
        {
            return ReadBuffer( ptr, sizeof(T)); 
        }

        Stream& WriteBuffer(const void* t, std::streamsize size)
        {
            PROFILE_SCOPE_ACCUM(g_StreamWrite); 

            m_Stream->write((const char*)t, size); 

            if (m_Stream->fail())
            {
                throw Reflect::StreamException( TXT( "General write failure") ); 
            }

            return *this; 
        }

        template <typename T>
        inline Stream& Write(const T* ptr)
        {
            return WriteBuffer( ptr, sizeof(T)); 
        }
    
        Stream& Flush()
        {
            m_Stream->flush(); 
            return *this; 
        }

        bool Fail()
        {
            return m_Stream->fail(); 
        }

        bool Done()
        {
            return m_Stream->eof(); 
        }

        std::basic_iostream< C, std::char_traits<C> >& GetInternal()
        {
            return *m_Stream;
        } 

    protected: 
        std::basic_iostream< C, std::char_traits<C> >*  m_Stream; 
        bool                                            m_OwnStream; 
    };

    template <class T, class C>
    Stream<C>& operator>>(Stream<C>& stream, T& val)
    {
        stream.GetInternal() >> val;

        if(stream.Fail() && !stream.Done())
        {
            throw Reflect::StreamException( TXT( "General read failure" ) ); 
        }

        return stream; 
    }

    template <class T, class C>
    Stream<C>& operator<<(Stream<C>& stream, const T& val)
    {
        stream.GetInternal() << val; 

        if(stream.Fail())
        {
            throw Reflect::StreamException( TXT( "General write failure" ) ); 
        }

        return stream; 
    }

    typedef Stream<char> CharStream;
    typedef Stream<wchar_t> WCharStream;
    typedef Stream<tchar> TCharStream;

    // pointer declaration. 
    typedef Nocturnal::SmartPtr< Stream<char> >     CharStreamPtr; 
    typedef Nocturnal::SmartPtr< Stream<wchar_t> >  WCharStreamPtr; 
    typedef Nocturnal::SmartPtr< Stream<tchar> >    TCharStreamPtr;

    //
    // FileStream, a stream object backed by file data
    //

    template< class C >
    class FileStream : public Stream< C >
    {
    public: 
        FileStream(const tstring& filename, bool write)
            : m_Filename(filename)
            , m_OpenForWrite(write)
        {

        }

        ~FileStream()
        {

        }

        virtual void Open() NOC_OVERRIDE
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

            std::basic_fstream< C, std::char_traits< C > >* fstream = new std::basic_fstream< C, std::char_traits< C > >(); 

            fstream->open(m_Filename.c_str(), fmode);
            if (!fstream->is_open())
            {
                delete fstream;
                throw Reflect::StreamException( TXT( "Unable to open '%s' for %s" ) , m_Filename.c_str(), m_OpenForWrite ? "write" : "read");
            }

            m_Stream    = fstream; 
            m_OwnStream = true; 
        }

        virtual void Close() NOC_OVERRIDE
        {
            std::basic_fstream< C, std::char_traits< C > >* fstream = static_cast< std::basic_fstream< C, std::char_traits< C > > *>( &GetInternal() );

            fstream->close();
            if (fstream->is_open())
            {
                throw Reflect::StreamException( TXT( "Unable to close '%s' after %s" ), m_Filename.c_str(), m_OpenForWrite ? "write" : "read");
            }
        }

    protected: 
        tstring     m_Filename; 
        bool        m_OpenForWrite; 
    };
}
