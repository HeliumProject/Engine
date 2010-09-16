#pragma once

#include "ReflectClipboardData.h"

#include "Foundation/Reflect/SimpleSerializer.h"
#include "Foundation/Reflect/ElementArraySerializer.h"

namespace Helium
{
    namespace Editor
    {
        /////////////////////////////////////////////////////////////////////////////
        // Stores an array of elements.  You can specify that all elements derive
        // from a certain base class.
        // 
        class ClipboardElementArray : public ReflectClipboardData
        {
        public:
            ClipboardElementArray();
            virtual ~ClipboardElementArray();

            i32 GetCommonBaseTypeID() const;
            void SetCommonBaseTypeID( i32 typeID );
            bool Add( const Reflect::ElementPtr& item );
            virtual bool Merge( const ReflectClipboardData* source ) HELIUM_OVERRIDE;

        protected:
            bool CanAdd( const Reflect::ElementPtr& item ) const;

        public:
            tstring m_CommonBaseClass;
            Reflect::V_Element m_Elements;

        public:
            REFLECT_DECLARE_CLASS( ClipboardElementArray, ReflectClipboardData );
            static void EnumerateClass( Reflect::Compositor<ClipboardElementArray>& comp );
        };
        typedef Helium::SmartPtr< ClipboardElementArray > ClipboardElementArrayPtr;
    }
}