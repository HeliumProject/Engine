#pragma once

#include "ReflectClipboardData.h"

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // Helper class to package the ReflectClipboardData format into a single
        // item that can be serialized and stored on the clipboard.
        // 
        class ClipboardDataWrapper : public Reflect::Object
        {
        public:
            HELIUM_DECLARE_CLASS( ClipboardDataWrapper, Reflect::Object );
            static void PopulateMetaType( Reflect::MetaStruct& comp );

            ClipboardDataWrapper();
            virtual ~ClipboardDataWrapper();

        public:
            ReflectClipboardDataPtr m_Data;
        };
        typedef Helium::StrongPtr< ClipboardDataWrapper > ClipboardDataWrapperPtr;
    }
}