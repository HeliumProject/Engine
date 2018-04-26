#pragma once

#include "ReflectClipboardData.h"

#include "Reflect/TranslatorDeduction.h"

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
            HELIUM_DECLARE_CLASS( ClipboardElementArray, ReflectClipboardData );
            static void PopulateMetaType( Reflect::MetaStruct& comp );

            ClipboardElementArray();
            virtual ~ClipboardElementArray();

            const Reflect::MetaClass* GetCommonBaseClass() const;
            void SetCommonBaseClass( const Reflect::MetaClass* type );
            bool Add( const Reflect::ObjectPtr& item );
            virtual bool Merge( const ReflectClipboardData* source ) override;

        protected:
            bool CanAdd( const Reflect::ObjectPtr& item ) const;

        public:
            std::string m_CommonBaseClass;
            std::vector< Reflect::ObjectPtr > m_Elements;
        };
        typedef Helium::SmartPtr< ClipboardElementArray > ClipboardElementArrayPtr;
    }
}