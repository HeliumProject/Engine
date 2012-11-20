#pragma once

#include <map>

#include "Platform/Types.h"

#include "Foundation/SmartPtr.h"

#include "Reflect/API.h"

//
// Obviously the reflection system itself can't use the same type checking as the code
//  its reflecting upon, so define a simple type checking system just for the reflection classes
//

#define REFLECTION_BASE(__ID, __Type) \
    typedef __Type This; \
    const static int s_ReflectionTypeID = __ID; \
    virtual int GetReflectionType() const { return __ID; } \
    virtual bool HasReflectionType(int id) const { return __ID == id; }

#define REFLECTION_TYPE(__ID, __Type, __Base) \
    typedef __Type This; \
    typedef __Base Base; \
    const static int s_ReflectionTypeID = __ID; \
    virtual int GetReflectionType() const HELIUM_OVERRIDE { return __ID; } \
    virtual bool HasReflectionType(int id) const HELIUM_OVERRIDE { return __ID == id || Base::HasReflectionType(id); }

namespace Helium
{
    namespace Reflect
    {
        //
        // All types have to belong to this enum
        //

        namespace ReflectionTypes
        {
            enum ReflectionType
            {
                Invalid = -1,
                Type,
                Enumeration,
                Composite,
                Structure,
                Class,
                Count,
            };

            extern const tchar_t* Strings[ ReflectionTypes::Count ];
        }
        typedef ReflectionTypes::ReflectionType ReflectionType;

        //
        // A block of string-based properties
        //

        class HELIUM_REFLECT_API PropertyCollection
        {
        protected:
            mutable std::map< tstring, tstring > m_Properties;

        public:
            template<class T>
            inline void SetProperty( const tstring& key, const T& value ) const
            {
                tostringstream str;
                str << value;

                if ( !str.fail() )
                {
                    SetProperty<tstring>( key, str.str() );
                }
            }

            template<>
            inline void SetProperty( const tstring& key, const tstring& value ) const
            {
                m_Properties[key] = value;
            }

            template<class T>
            inline bool GetProperty( const tstring& key, T& value ) const
            {
                tstring strValue;
                bool result = GetProperty<tstring>( key, strValue );

                if ( result )
                {
                    tistringstream str( strValue );
                    str >> value;
                    return !str.fail();
                }

                return false;
            }

            template<>
            inline bool GetProperty( const tstring& key, tstring& value ) const
            {
                std::map< tstring, tstring >::const_iterator found = m_Properties.find( key ); 
                if ( found != m_Properties.end() )
                {
                    value = found->second;
                    return true;
                }

                return false;
            }

            inline const tstring& GetProperty( const tstring& key ) const
            {
                std::map< tstring, tstring >::const_iterator found = m_Properties.find( key );
                if ( found != m_Properties.end() )
                {
                    return found->second;
                }

                static tstring empty;
                return empty;
            }
        };

        //
        // This lets us safely cast between reflection class pointers
        //

        class HELIUM_REFLECT_API ReflectionInfo : public Helium::AtomicRefCountBase< ReflectionInfo >, public PropertyCollection
        {
        public:
            REFLECTION_BASE(ReflectionTypes::Invalid, ReflectionInfo);

            ReflectionInfo();
            virtual ~ReflectionInfo();
        };

        template<typename T>
        T* ReflectionCast(ReflectionInfo* info)
        {
            return (info && info->HasReflectionType( T::s_ReflectionTypeID )) ? static_cast<T*>(info) : NULL;
        }

        template<typename T>
        const T* ReflectionCast(const ReflectionInfo* info)
        {
            return (info && info->HasReflectionType( T::s_ReflectionTypeID )) ? static_cast<const T*>(info) : NULL;
        }
    }
}