#pragma once

#include <vector>
#include <algorithm>

#include "Platform/Assert.h"
#include "Platform/Exception.h"
#include "Foundation/Memory/SmartPtr.h"

#include "API.h"
#include "TypeID.h"
#include "ReflectionInfo.h"

namespace Helium
{
    namespace Reflect
    {
        //
        // Type, fully qualified type information
        //

        class HELIUM_REFLECT_API Type HELIUM_ABSTRACT : public ReflectionInfo
        {
        public:
            REFLECTION_BASE( ReflectionTypes::Type, Type );

        protected:
            Type();
            ~Type();

        public:
            mutable const void*             m_Tag;          // tag (client) data
            const tchar_t*                  m_Name;         // the name of this type in the codebase
            uint32_t                        m_Size;         // the size of the object in bytes

            virtual void Register() const;
            virtual void Unregister() const;
        };

        namespace RegistrarTypes
        {
            enum RegistrarType
            {
                Enumeration,
                Structure,
                Class,
                Count,
            };
        }
        typedef RegistrarTypes::RegistrarType RegistrarType;

        class HELIUM_REFLECT_API TypeRegistrar
        {
        public:
            TypeRegistrar(const tchar_t* name);

            virtual void Register() = 0;
            virtual void Unregister() = 0;

            static void AddToList( RegistrarType type, TypeRegistrar* registrar );
            static void RemoveFromList( RegistrarType type, TypeRegistrar* registrar );

            static void RegisterTypes( RegistrarType type );
            static void UnregisterTypes( RegistrarType type );

            static void AddTypeToRegistry( const Type* type );
            static void RemoveTypeFromRegistry( const Type* type );

        private:
            const tchar_t*          m_Name;
            TypeRegistrar*          m_Next;
            static TypeRegistrar*   s_Head[ RegistrarTypes::Count ];
            static TypeRegistrar*   s_Tail[ RegistrarTypes::Count ];
        };
    }
}