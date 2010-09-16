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
        class ClipboardDataWrapper : public Reflect::Element
        {
        public:
            ClipboardDataWrapper();
            virtual ~ClipboardDataWrapper();

        public:
            ReflectClipboardDataPtr m_Data;

        public:
            REFLECT_DECLARE_CLASS( ClipboardDataWrapper, Reflect::Element );
            static void EnumerateClass( Reflect::Compositor<ClipboardDataWrapper>& comp );
        };
        typedef Helium::SmartPtr< ClipboardDataWrapper > ClipboardDataWrapperPtr;
    }
}