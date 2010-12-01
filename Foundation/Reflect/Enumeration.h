#pragma once

#include <hash_map>

#include "Type.h"

#include "Foundation/Memory/SmartPtr.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API EnumerationElement : public Helium::AtomicRefCountBase< EnumerationElement >
        {
            friend class AtomicRefCountBase< EnumerationElement >;

        public:
            uint32_t    m_Value;    // the value of the element
            tstring     m_Name;     // the name of the element
            tstring     m_Label;    // the label (friendly name) of the element
            tstring     m_HelpText; // the help text for the element

        protected:
            // for where the friendly name (used for UI AND serialization) is not the real name
            EnumerationElement( uint32_t value, const tstring& name, const tstring& label, const tstring& helpText = TXT( "FIXME: SET THE HELP TEXT FOR THIS ENUMERATION ELEMENT" ) );
            virtual ~EnumerationElement();

        public:
            // protect external allocation to keep inlined code in this dll
            static EnumerationElement* Create( uint32_t value, const tstring& name, const tstring& label, const tstring& helpText = TXT( "FIXME: SET THE HELP TEXT FOR THIS ENUMERATION ELEMENT" ) );
        };

        typedef Helium::SmartPtr<EnumerationElement> EnumerationElementPtr;
        typedef std::vector<EnumerationElementPtr>                    V_EnumerationElement; // order of declaration
        typedef stdext::hash_map<tstring, EnumerationElementPtr>      M_StrEnumerationElement; // sorted by name
        typedef stdext::hash_map<int32_t, EnumerationElementPtr>          M_ValueEnumerationElement; // sorted by value

        class FOUNDATION_API Enumeration;
        typedef Helium::SmartPtr<Enumeration> EnumerationPtr;

        class FOUNDATION_API Enumeration : public Type
        {
        public:
            REFLECTION_TYPE( ReflectionTypes::Enumeration );

            V_EnumerationElement         m_Elements;        // the elements of this enum
            M_StrEnumerationElement      m_ElementsByName;  // the elements by name
            M_StrEnumerationElement      m_ElementsByLabel; // the elements by name
            M_ValueEnumerationElement    m_ElementsByValue; // the elements by value

        protected:
            Enumeration();
            virtual ~Enumeration();

        public:
            // protect external allocation to keep inlined code in this dll
            static Enumeration* Create();

            template<class T>
            static Enumeration* Create( const tstring& name )
            {
                Enumeration* info = Enumeration::Create();

                info->m_Size = sizeof(T);
                info->m_Name = name;
                info->m_UIName = info->m_Name;

                T::EnumerateEnum( *info );

                return info;
            }

            //
            // Equality check
            //

            bool Equals(const Enumeration* rhs) const;

            //
            // Element data
            //

            void AddElement(uint32_t value, const tstring& name, const tstring& label = TXT(""), const tstring& helpText = TXT( "FIXME: SET THE HELP TEXT FOR THIS ENUMERATION ELEMENT" ) );

            bool GetElementValue(const tstring& label, uint32_t& value) const;
            bool GetElementLabel(const uint32_t value, tstring& label) const;

            //
            // Support for using 'enum' as a bitfield
            //

            bool GetBitfieldValue(const tstring& str, uint32_t& value) const;
            bool GetBitfieldValue(const std::vector< tstring >& strs, uint32_t& value) const;

            bool GetBitfieldString(const uint32_t value, tstring& str) const;
            bool GetBitfieldStrings(const uint32_t value, std::vector< tstring >& strs) const;

            inline static bool IsFlagSet(uint32_t value, uint32_t flag)
            {
                return ((value & flag) == flag);
            }

            inline static void SetFlags(uint32_t& value, uint32_t flags)
            {
                value |= flags;
            }
        };
    }
}
