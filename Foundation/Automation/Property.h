#pragma once

#include "Foundation/Exception.h"
#include "Foundation/Memory/SmartPtr.h"

namespace Nocturnal
{
    //
    // Property API types
    //

    // getter prototype enumeration
    namespace GetterTypes
    {
        enum GetterType
        {
            Parameter,
            Reference,
            Value
        };
    }

    // setter prototype enumeration
    namespace SetterTypes
    {
        enum SetterType
        {
            Reference,
            Value
        };
    }

    // exception class used for bad situations
    class PropertyException : public Nocturnal::Exception
    {
    public:
        PropertyException( const tchar *msgFormat, ... )
        {
            va_list msgArgs;
            va_start( msgArgs, msgFormat );
            SetMessage( msgFormat, msgArgs );
            va_end( msgArgs );
        }

    protected:
        PropertyException() throw() {} // hide default c_tor
    };


    //
    // Property NOC_ABSTRACT base class exposes the ability to read/write data of a type
    //

    template <class V>
    class Property NOC_ABSTRACT : public Nocturnal::RefCountBase< Property<V> >
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
    private:
        // getter
        typedef void (*GetterParam)(V&);
        typedef const V& (*GetterReference)();
        typedef V (*GetterValue)();
        union
        {
            GetterParam m_GetParam;
            GetterReference m_GetReference;
            GetterValue m_GetValue;
        };
        GetterTypes::GetterType m_GetterType;

        // setter
        typedef void (*SetterReference)(const V&);
        typedef void (*SetterValue)(V);
        union
        {
            SetterReference m_SetReference;
            SetterValue m_SetValue;
        };
        SetterTypes::SetterType m_SetterType;


        //
        // Permute each constructor combination
        //

    public:

        //
        // Get by param, set by reference
        //

        StaticProperty(GetterParam g, SetterReference s)
        {
            m_GetParam = g;
            m_GetterType = GetterTypes::Parameter;
            m_SetReference = s;
            m_SetterType = SetterTypes::Reference;
        }

        //
        // Get by return reference, set by reference
        //

        StaticProperty(GetterReference g, SetterReference s)
        {
            m_GetReference = g;
            m_GetterType = GetterTypes::Reference;
            m_SetReference = s;
            m_SetterType = SetterTypes::Reference;
        }

        //
        // Get by return, set by reference
        //

        StaticProperty(GetterValue g, SetterReference s)
        {
            m_GetValue = g;
            m_GetterType = GetterTypes::Value;
            m_SetReference = s;
            m_SetterType = SetterTypes::Reference;
        }

        //
        // Get by param, set by
        //

        StaticProperty(GetterParam g, SetterValue s)
        {
            m_GetParam = g;
            m_GetterType = GetterTypes::Parameter;
            m_SetValue = s;
            m_SetterType = SetterTypes::Value;
        }

        //
        // Get by return reference, set by
        //

        StaticProperty(GetterReference g, SetterValue s)
        {
            m_GetReference = g;
            m_GetterType = GetterTypes::Reference;
            m_SetValue = s;
            m_SetterType = SetterTypes::Value;
        }

        //
        // Get by return, set by
        //

        StaticProperty(GetterValue g, SetterValue s)
        {
            m_GetValue = g;
            m_GetterType = GetterTypes::Value;
            m_SetValue = s;
            m_SetterType = SetterTypes::Value;
        }

        virtual V Get() const
        {
            switch(m_GetterType)
            {
            case GetterTypes::Parameter:
                {
                    V value;
                    (*m_GetParam)(value);
                    return value;
                }

            case GetterTypes::Reference:
                {
                    return (*m_GetReference)();
                }

            case GetterTypes::Value:
                {
                    return (*m_GetValue)();
                }

            default:
                {
                    throw PropertyException ("StaticProperty has no compatible get function");
                }
            }
        }

        virtual bool Set(const V& value)
        {
            switch (m_SetterType)
            {
            case SetterTypes::Reference:
                {
                    (*m_SetReference)(value);
                    return true;
                }

            case SetterTypes::Value:
                {
                    (*m_SetValue)(value);
                    return true;
                }

            default:
                {
                    throw PropertyException ("StaticProperty has no compatible get function");
                }
            }
        }
    };


    //
    // Member Property call through instance and member function pointers
    //

    template <class T, class V>
    class MemberProperty : public Property<V>
    {
    private:
        // getter
        typedef void (T::*GetterParam)(V&) const;
        typedef const V& (T::*GetterReference)() const;
        typedef V (T::*GetterValue)() const;
        union
        {
            GetterParam m_GetParam;
            GetterReference m_GetReference;
            GetterValue m_GetValue;
        };
        GetterTypes::GetterType m_GetterType;

        // setter
        typedef void (T::*SetterReference)(const V&);
        typedef void (T::*SetterValue)(V);
        union
        {
            SetterReference m_SetReference;
            SetterValue m_SetValue;
        };
        SetterTypes::SetterType m_SetterType;

        // instance to get/set
        T* m_Target;


        //
        // Permute each constructor combination
        //

    public:

        //
        // Get by param, set by reference
        //

        MemberProperty(T* o, GetterParam g, SetterReference s)
        {
            m_Target = o;
            m_GetParam = g;
            m_GetterType = GetterTypes::Parameter;
            m_SetReference = s;
            m_SetterType = SetterTypes::Reference;
        }

        //
        // Get by return reference, set by reference
        //

        MemberProperty(T* o, GetterReference g, SetterReference s)
        {
            m_Target = o;
            m_GetReference = g;
            m_GetterType = GetterTypes::Reference;
            m_SetReference = s;
            m_SetterType = SetterTypes::Reference;
        }

        //
        // Get by return, set by reference
        //

        MemberProperty(T* o, GetterValue g, SetterReference s)
        {
            m_Target = o;
            m_GetValue = g;
            m_GetterType = GetterTypes::Value;
            m_SetReference = s;
            m_SetterType = SetterTypes::Reference;
        }

        //
        // Get by param, set by
        //

        MemberProperty(T* o, GetterParam g, SetterValue s)
        {
            m_Target = o;
            m_GetParam = g;
            m_GetterType = GetterTypes::Parameter;
            m_SetValue = s;
            m_SetterType = SetterTypes::Value;
        }

        //
        // Get by return reference, set by
        //

        MemberProperty(T* o, GetterReference g, SetterValue s)
        {
            m_Target = o;
            m_GetReference = g;
            m_GetterType = GetterTypes::Reference;
            m_SetValue = s;
            m_SetterType = SetterTypes::Value;
        }

        //
        // Get by return, set by
        //

        MemberProperty(T* o, GetterValue g, SetterValue s)
        {
            m_Target = o;
            m_GetValue = g;
            m_GetterType = GetterTypes::Value;
            m_SetValue = s;
            m_SetterType = SetterTypes::Value;
        }

        virtual T* Target()
        {
            return m_Target;
        }

        virtual V Get() const
        {
            if (m_Target == NULL)
            {
                throw PropertyException ("MemberProperty has no target");
            }

            switch(m_GetterType)
            {
            case GetterTypes::Parameter:
                {
                    V value;
                    ((*m_Target).*m_GetParam)(value);
                    return value;
                }

            case GetterTypes::Reference:
                {
                    return ((*m_Target).*m_GetReference)();
                }

            case GetterTypes::Value:
                {
                    return ((*m_Target).*m_GetValue)();
                }

            default:
                {
                    throw PropertyException ("MemberProperty has no compatible get function");
                }
            }
        }

        virtual bool Set(const V& value)
        {
            if (m_Target == NULL)
            {
                throw PropertyException ("MemberProperty has no target");
            }

            switch (m_SetterType)
            {
            case SetterTypes::Reference:
                {
                    ((*m_Target).*m_SetReference)(value);
                    return true;
                }

            case SetterTypes::Value:
                {
                    ((*m_Target).*m_SetValue)(value);
                    return true;
                }

            default:
                {
                    throw PropertyException ("MemberProperty has no compatible set function");
                }
            }
        }
    };
}