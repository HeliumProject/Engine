#pragma once

#include "API.h"  
#include "Exceptions.h"

#include "Platform/Assert.h"

namespace Reflect
{
    class FOUNDATION_API Stream : public Nocturnal::RefCountBase<Stream>
    {
    public: 
        Stream(); 
        Stream(std::iostream* stream, bool ownStream = false); 
        virtual ~Stream(); 

        // new custom interfaces

        // virtual Open call so that derived classes can do special work
        // that may throw an exception (which we don't want to do in a 
        // constructor) 
        // 
        virtual void    Open(); 

        // virtual Close so that derived classes can clean up and be 
        // able to throw without doing all that work in the destructor
        // 
        virtual void    Close(); 

        // skip to the next whitespace character on the input stream. 
        // 
        void            SkipWhitespace(); 

        // implementation of std::stream interfaces 

        Stream& SeekWrite(std::streamoff offset, std::ios_base::seekdir dir); 
        Stream& SeekRead(std::streamoff offset, std::ios_base::seekdir dir); 

        std::streampos TellWrite(); 
        std::streampos TellRead(); 

        std::streamsize BytesRead() const; 

        bool    Fail(void); 
        bool    Done(); 
        void    Clear(); 
        void    SetPrecision(int p);

        Stream&         Flush(); 
        std::streamsize BytesAvailable(); 

        Stream& WriteBuffer(const void* t, std::streamsize size); 

        template <typename T>
        inline Stream& Write(const T* ptr)
        {
            return WriteBuffer( ptr, sizeof(T)); 
        }

        Stream& ReadBuffer(void* t, std::streamsize size); 
    
        template <typename T>
        inline Stream& Read(T* ptr)
        {
            return ReadBuffer( ptr, sizeof(T)); 
        }

        std::iostream* GetInternal()
        {
            return m_Stream;
        } 

    protected: 
        std::iostream*  m_Stream; 
        bool            m_OwnStream; 
    };

    template <class T>
    Stream& operator>>(Stream& stream, T& val)
    {
        *stream.GetInternal() >> val;

        if(stream.Fail() && !stream.Done())
        {
            throw Reflect::StreamException( TXT( "General read failure" ) ); 
        }

        return stream; 
    }

    template <class T>
    Stream& operator<<(Stream& stream, const T& val)
    {
        *stream.GetInternal() << val; 

        if(stream.Fail())
        {
            throw Reflect::StreamException( TXT( "General write failure" ) ); 
        }

        return stream; 
    }

    // pointer declaration. 
    typedef Nocturnal::SmartPtr<Stream> StreamPtr; 
}
