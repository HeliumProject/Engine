#pragma once

#include "Platform/Exception.h"
#include "Foundation/SmartPtr.h"

namespace Helium
{
    //
    // Property API types
    //

    namespace GetterTypes
    {
        enum Type
        {
            Parameter,
            Reference,
            Value
        };
    }
	typedef GetterTypes::Type GetterType;

    namespace SetterTypes
    {
        enum Type
        {
            Reference,
            Value
        };
    }
	typedef SetterTypes::Type SetterType;

    class PropertyException : public Helium::Exception
    {
    public:
        inline PropertyException( const tchar_t *msgFormat, ... );

    protected:
        inline PropertyException();
    };

    //
    // Property HELIUM_ABSTRACT base class exposes the ability to read/write data of a type
    //

    template <class V>
    class Property HELIUM_ABSTRACT : public Helium::RefCountBase< Property<V> >
    {
    public:
        virtual V Get() const = 0;
        virtual bool Set(const V& value) = 0;
    };

    //
    // Static Property calls through static or C-style functions
    //

    template <class V>
    class StaticProperty : public Property<V>
    {
    public:
        typedef void (*GetterParam)(V&);
        typedef const V& (*GetterReference)();
        typedef V (*GetterValue)();

        typedef void (*SetterReference)(const V&);
        typedef void (*SetterValue)(V);

        inline StaticProperty(GetterParam g, SetterReference s);
        inline StaticProperty(GetterReference g, SetterReference s);
        inline StaticProperty(GetterValue g, SetterReference s);
        inline StaticProperty(GetterParam g, SetterValue s);
        inline StaticProperty(GetterReference g, SetterValue s);
        inline StaticProperty(GetterValue g, SetterValue s);

        virtual V Get() const;
        virtual bool Set(const V& value);

	private:
        GetterType				m_GetterType;
        union
        {
            GetterParam			m_GetParam;
            GetterReference		m_GetReference;
            GetterValue			m_GetValue;
        };

		SetterType				m_SetterType;
        union
        {
            SetterReference		m_SetReference;
            SetterValue			m_SetValue;
        };
    };

    //
    // Member Property call through instance and member function pointers
    //

    template <class T, class V>
    class MemberProperty : public Property<V>
    {
    public:
        typedef void (T::*GetterParam)(V&) const;
        typedef const V& (T::*GetterReference)() const;
        typedef V (T::*GetterValue)() const;

        typedef void (T::*SetterReference)(const V&);
        typedef void (T::*SetterValue)(V);

		inline MemberProperty(T* o, GetterParam g, SetterReference s);
        inline MemberProperty(T* o, GetterReference g, SetterReference s);
        inline MemberProperty(T* o, GetterValue g, SetterReference s);
        inline MemberProperty(T* o, GetterParam g, SetterValue s);
        inline MemberProperty(T* o, GetterReference g, SetterValue s);
        inline MemberProperty(T* o, GetterValue g, SetterValue s);

        virtual T* Target();
        virtual V Get() const;
        virtual bool Set(const V& value);

	private:
        GetterType			m_GetterType;
        union
        {
            GetterParam		m_GetParam;
            GetterReference m_GetReference;
            GetterValue		m_GetValue;
        };

		SetterType			m_SetterType;
        union
        {
            SetterReference m_SetReference;
            SetterValue		m_SetValue;
        };

		T* m_Target;
    };
}

#include "Foundation/Property.inl"