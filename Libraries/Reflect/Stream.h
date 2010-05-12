#pragma once

#include "API.h"  
#include "Exceptions.h"

#include "Common/Assert.h"

namespace Reflect
{
  class REFLECT_API Stream : public Nocturnal::RefCountBase<Stream>
  {
  public: 
    Stream(); 
    Stream(std::iostream* p_Stream, bool ownStream = false); 
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

    // block reading and writing functions
    // 
    Stream& ReadBuffer(void* ptr, std::streamsize length)
    {
      return this->read( (char*) ptr, length); 
    }

    Stream& WriteBuffer(const void* ptr, std::streamsize length)
    {
      return this->write( (const char*)ptr, length); 
    }


    // templatized Read and Write, which pay attention to the 
    // type of the pointer you're passing in and automatically choose the size
    //
    // u32 myInt; 
    // m_Stream->Write(&myInt);        // good
    // m_Stream->Write(myInt);         // bad, compile error
    // m_Stream->Write((void*)&myInt); // bad, compile error
    // 
    template <typename T>
    Stream& Read(T* ptr)
    {
      return this->read( (char*)ptr, sizeof(T)); 
    }

    template <typename T>
    Stream& Write(T* ptr)
    {
      return this->write( (const char*)ptr, sizeof(T)); 
    }

    // returns our internal stream, only for specially problematic cases. 
    std::iostream* GetInternal() { return m_Stream; }; 
    
  protected: 
    Stream& write(const char* t, std::streamsize size); 
    Stream& read(char* t, std::streamsize size); 

    std::iostream* m_Stream; 
    bool           m_OwnStream; 


  };

  template <class T> Stream& operator>>(Stream& stream, T& val)
  {
    *stream.GetInternal() >> val; 

    if(stream.Fail() && !stream.Done())
    {
      throw Reflect::StreamException("General read failure"); 
    }

    return stream; 
  }

  template <class T> Stream& operator<<(Stream& stream, const T& val)
  {
    *stream.GetInternal() << val; 

    if(stream.Fail())
    {
      throw Reflect::StreamException("General write failure"); 
    }

    return stream; 
  }

  // pointer declaration. 
  typedef Nocturnal::SmartPtr<Stream> StreamPtr; 
}

namespace std
{
  inline void getline(Reflect::Stream& stream, std::string& output)
  {
    std::getline(*stream.GetInternal(), output); 
  }
}
