#pragma once

#include <hash_map>

#include "Foundation/Container/DynArray.h"
#include "Foundation/Reflect/Type.h"

namespace Helium
{
    namespace Reflect
    {
        class FOUNDATION_API EnumerationElement
        {
        public:
            EnumerationElement();
            EnumerationElement( uint32_t value, const tstring& name, const tstring& helpText = TXT( "FIXME: SET THE HELP TEXT FOR THIS ENUMERATION ELEMENT" ) );

            uint32_t    m_Value;    // the value of the object
            tstring     m_Name;     // the name of the object
            tstring     m_HelpText; // the help text for the object
        };

        class FOUNDATION_API Enumeration : public Type
        {
        public:
            REFLECTION_TYPE( ReflectionTypes::Enumeration, Enumeration, Type );

            Enumeration();
            ~Enumeration();

            template<class T>
            static Enumeration* Create( const tchar_t* name )
            {
                Enumeration* info = new Enumeration();

                info->m_Size = sizeof(T);
                info->m_Name = name;

                T::EnumerateEnum( *info );

                return info;
            }

            void AddElement(uint32_t value, const tstring& name, const tstring& helpText = TXT( "FIXME: SET THE HELP TEXT FOR THIS ENUMERATION ELEMENT" ) );

            bool GetElementValue(const tstring& name, uint32_t& value) const;
            bool GetElementLabel(const uint32_t value, tstring& name) const;

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

        public:
            DynArray< EnumerationElement >  m_Elements;
        };
    }
}
