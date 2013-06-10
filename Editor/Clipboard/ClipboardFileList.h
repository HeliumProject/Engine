#pragma once

#include "ReflectClipboardData.h"

#include "Reflect/TranslatorDeduction.h"

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
            REFLECT_DECLARE_OBJECT( ClipboardFileList, ReflectClipboardData );
            static void PopulateStructure( Reflect::Structure& comp );

            ClipboardFileList();
            virtual ~ClipboardFileList();

            bool AddFilePath( const std::string& file );
            const std::set< std::string >& GetFilePaths() const;

            virtual bool Merge( const ReflectClipboardData* source ) HELIUM_OVERRIDE;

        private:
            std::set< std::string > m_Files;
            bool                m_IsDirty;
        };

        typedef Helium::StrongPtr< ClipboardFileList > ClipboardFileListPtr;
    }
}