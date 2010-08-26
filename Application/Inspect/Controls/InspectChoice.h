#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/InspectControl.h"

namespace Helium
{
    namespace Inspect
    {
        const static tchar CHOICE_ATTR_ENUM[]      = TXT( "enum" );
        const static tchar CHOICE_ATTR_SORTED[]    = TXT( "sorted" );
        const static tchar CHOICE_ATTR_DROPDOWN[]  = TXT( "dropdown" );
        const static tchar CHOICE_ATTR_ITEM[]      = TXT( "item" );
        const static tchar CHOICE_ATTR_ITEM_DELIM  = TXT( '|' );
        const static tchar CHOICE_ATTR_PREFIX[]    = TXT( "prefix" );

        class Choice;

        struct ChoiceArgs
        {
            ChoiceArgs(Choice* choice)
                : m_Choice (choice)
            {

            }

            Choice* m_Choice;
        };
        typedef Helium::Signature<void, const ChoiceArgs&> ChoiceSignature;

        struct ChoiceEnumerateArgs : public ChoiceArgs
        {
            ChoiceEnumerateArgs(Choice* choice, const tstring& enumeration)
                : ChoiceArgs (choice)
                , m_Enumeration (enumeration)
            {

            }

            tstring m_Enumeration;
        };
        typedef Helium::Signature<void, const ChoiceEnumerateArgs&> ChoiceEnumerateSignature;

        struct ChoiceItem
        {
            ChoiceItem(const tstring& key = TXT(""), const tstring& data = TXT(""))
                : m_Key( key )
                , m_Data( data )
            {

            }

            bool operator==( const ChoiceItem& rhs ) const
            {
                return rhs.m_Key == m_Key && rhs.m_Data == m_Data;
            }

            bool operator!=( const ChoiceItem& rhs ) const
            {
                return !operator==( rhs );
            }

            tstring m_Key;
            tstring m_Data;
        };

        class APPLICATION_API Choice : public Reflect::ConcreteInheritor<Choice, Control>
        {
        public:
            Choice();

            virtual bool Process(const tstring& key, const tstring& value) HELIUM_OVERRIDE;
            virtual void SetDefaultAppearance(bool def) HELIUM_OVERRIDE;
            void SetToDefault(const ContextMenuEventArgs& event);

            const tstring& GetPrefix()
            {
                return m_Prefix;
            }

            bool Contains(const tstring& data);
            void Clear();

            virtual void Populate() HELIUM_OVERRIDE;

            Attribute< bool >                       a_Highlight;
            Attribute< bool >                       a_Sorted;
            Attribute< bool >                       a_DropDown;
            Attribute< bool >                       a_EnableAdds;
            Attribute< std::vector< ChoiceItem > >  a_Items;

            ChoiceSignature::Event                  e_Populate;
            ChoiceEnumerateSignature::Event         e_Enumerate;

        private:
            tstring             m_Enum;
            tstring             m_Prefix;
            std::vector< ChoiceItem >        m_Statics;
        };

        typedef Helium::SmartPtr<Choice> ChoicePtr;
    }
}
