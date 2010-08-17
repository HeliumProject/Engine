#pragma once

#include "ReflectClipboardData.h"

#include "Foundation/Reflect/Serializers.h"

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // Custom clipboard format to manage a list of files.
        // 
        class ClipboardFileList : public ReflectClipboardData
        {
        private:
            std::set< tstring > m_Files;

            // These members are not serialized
            bool m_IsDirty;

        public:
            // Runtime Type Info
            REFLECT_DECLARE_CLASS( ClipboardFileList, ReflectClipboardData );
            static void InitializeType();
            static void CleanupType();

        public:
            ClipboardFileList();
            virtual ~ClipboardFileList();

            bool AddFilePath( const tstring& file );
            const std::set< tstring >& GetFilePaths() const;

            virtual bool Merge( const ReflectClipboardData* source ) HELIUM_OVERRIDE;

        public:
            static void EnumerateClass( Reflect::Compositor<ClipboardFileList>& comp );
        };

        typedef Helium::SmartPtr< ClipboardFileList > ClipboardFileListPtr;
    }
}