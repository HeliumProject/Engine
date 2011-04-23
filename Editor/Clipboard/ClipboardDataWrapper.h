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
            REFLECT_DECLARE_OBJECT( ClipboardDataWrapper, Reflect::Object );
            static void PopulateComposite( Reflect::Composite& comp );

            ClipboardDataWrapper();
            virtual ~ClipboardDataWrapper();

        public:
            ReflectClipboardDataPtr m_Data;
        };
        typedef Helium::StrongPtr< ClipboardDataWrapper > ClipboardDataWrapperPtr;
    }
}