#pragma once

#include <string>

#include "Platform/Types.h"

#include "Foundation/API.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Nocturnal
{ 
    class FOUNDATION_API Handle : public Nocturnal::RefCountBase< Handle >
    {
    public:
        Handle( const tstring& path, const tchar* mode = TXT( "r" ) );

        virtual ~Handle()
        {
            // we don't just call Close() because it is linked to Open() calls
            if ( m_FileHandle )
            {
                fclose( m_FileHandle );
            }
        }

        bool IsValid();

        bool Open();
        void Close();

        const tstring& GetPath() const
        {
            return m_Path;
        }

        const tchar* GetMode() const
        {
            return m_Mode;
        }

        FILE* Get() const
        {
            return m_FileHandle;
        }

    private:
        u32  m_OpenCount;
        tstring m_Path;
        const tchar* m_Mode;
        FILE* m_FileHandle;
    };

    typedef Nocturnal::SmartPtr< Handle > HandlePtr;
}