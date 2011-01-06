#pragma once

#include "ReflectClipboardData.h"

#include "Foundation/Reflect/Data/SimpleData.h"
#include "Foundation/Reflect/Data/ElementStlVectorData.h"

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
            REFLECT_DECLARE_CLASS( ClipboardElementArray, ReflectClipboardData );
            static void AcceptCompositeVisitor( Reflect::Composite& comp );

            ClipboardElementArray();
            virtual ~ClipboardElementArray();

            const Reflect::Class* GetCommonBaseClass() const;
            void SetCommonBaseTypeID( const Reflect::Type* type );
            bool Add( const Reflect::ElementPtr& item );
            virtual bool Merge( const ReflectClipboardData* source ) HELIUM_OVERRIDE;

        protected:
            bool CanAdd( const Reflect::ElementPtr& item ) const;

        public:
            tstring m_CommonBaseClass;
            std::vector< Reflect::ElementPtr > m_Elements;
        };
        typedef Helium::SmartPtr< ClipboardElementArray > ClipboardElementArrayPtr;
    }
}