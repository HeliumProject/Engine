#pragma once

#include <hash_map>

#include "Type.h"

#include "Foundation/Atomic.h"

namespace Reflect
{
    class FOUNDATION_API EnumerationElement : public Foundation::AtomicRefCountBase
    {
    public:
        u32         m_Value;  // the value of the element
        tstring     m_Name;   // the name of the element
        tstring     m_Label;  // the label (friendly name) of the element

    protected:
        // for where the friendly name (used for UI AND serialization) is not the real name
        EnumerationElement( u32 value, const tstring& name, const tstring& label );
        virtual ~EnumerationElement();

    public:
        // protect external allocation to keep inlined code in this dll
        static EnumerationElement* Create( u32 value, const tstring& name, const tstring& label );
    };

    typedef Helium::SmartPtr<EnumerationElement> EnumerationElementPtr;
    typedef std::vector<EnumerationElementPtr>                    V_EnumerationElement; // order of declaration
    typedef stdext::hash_map<tstring, EnumerationElementPtr>      M_StrEnumerationElement; // sorted by name
    typedef stdext::hash_map<i32, EnumerationElementPtr>          M_ValueEnumerationElement; // sorted by value

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
        static Enumeration* Create (const tstring& name)
        {
            Enumeration* info = Enumeration::Create();

            info->m_TypeID = AssignTypeID();
            info->m_Size = sizeof(T);
            info->m_ShortName = name;
            bool converted = Platform::ConvertString( typeid(T).name(), info->m_FullName );
            HELIUM_ASSERT( converted );
            info->m_UIName = info->m_ShortName;

            return info;
        }

        //
        // Equality check
        //

        bool Equals(const Enumeration* rhs) const;

        //
        // Element data
        //

        void AddElement(u32 value, const tstring& name, tstring label = TXT("") );

        bool GetElementValue(const tstring& label, u32& value) const;
        bool GetElementLabel(const u32 value, tstring& label) const;

        //
        // Support for using 'enum' as a bitfield
        //

        bool GetBitfieldValue(const tstring& str, u32& value) const;
        bool GetBitfieldValue(const std::vector< tstring >& strs, u32& value) const;

        bool GetBitfieldString(const u32 value, tstring& str) const;
        bool GetBitfieldStrings(const u32 value, std::vector< tstring >& strs) const;

        inline static bool IsFlagSet(u32 value, u32 flag)
        {
            return ((value & flag) == flag);
        }

        inline static void SetFlags(u32& value, u32 flags)
        {
            value |= flags;
        }
    };
}
