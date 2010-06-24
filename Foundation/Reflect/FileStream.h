#pragma once

#include "Stream.h" 

namespace Reflect
{
    class FileStream : public Stream
    {
    public: 
        FileStream(const tstring& filename, bool write); 
        ~FileStream();

        virtual void Open(); 
        virtual void Close(); 

    protected: 
        tstring     m_Filename; 
        bool        m_OpenForWrite; 
    }; 
}
