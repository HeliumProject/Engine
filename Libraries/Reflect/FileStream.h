#pragma once

#include "Stream.h" 

namespace Reflect
{
  class FileStream : public Stream
  {
  public: 
    FileStream(const std::string& filename, bool write); 
    ~FileStream();

    virtual void Open(); 
    virtual void Close(); 

  protected: 
    std::string m_Filename; 
    bool        m_OpenForWrite; 
  }; 
}
