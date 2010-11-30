#pragma once

#include "ReflectClipboardData.h"

#include "Foundation/Reflect/Data/DataDeduction.h"

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // Custom clipboard format to manage a list of files.
        // 
        class ClipboardFileList : public ReflectClipboardData
        {
        public:
            REFLECT_DECLARE_CLASS( ClipboardFileList, ReflectClipboardData );
            static void EnumerateClass( Reflect::Compositor<ClipboardFileList>& comp );

            ClipboardFileList();
            virtual ~ClipboardFileList();

            bool AddFilePath( const tstring& file );
            const std::set< tstring >& GetFilePaths() const;

            virtual bool Merge( const ReflectClipboardData* source ) HELIUM_OVERRIDE;

        private:
            std::set< tstring > m_Files;
            bool                m_IsDirty;
        };

        typedef Helium::StrongPtr< ClipboardFileList > ClipboardFileListPtr;
    }
}