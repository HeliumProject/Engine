#pragma once

#include <string>
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

        namespace ReservedTypes
        {
            enum ReservedType
            {
                Invalid  = -1,
                First,
                Any,
            };
        }
        typedef ReservedTypes::ReservedType ReservedType;

        class FOUNDATION_API Type HELIUM_ABSTRACT : public ReflectionInfo
        {
        public:
            REFLECTION_BASE( ReflectionTypes::Type );

            //
            // Fields
            //

            tstring                       m_ShortName;          // the name of this type in the codebase, this is not necessarily unique (templates share the same short name)
            tstring                       m_FullName;           // the fully qualified name of this type in the codebase (this is super-long if its a template type)
            tstring                       m_UIName;             // the friendly name for the field, its optional (will use the short name if not specified)

            int32_t                           m_TypeID;             // the unique id of this type
            uint32_t                           m_Size;               // the size of the object in bytes

            mutable std::vector<void**>   m_Pointers;           // cached pointers to this type
            mutable std::vector<int32_t*>     m_IDs;                // cached ids to this type

        protected:
            Type();
            virtual ~Type();

            // assign unique type id
            static int32_t AssignTypeID();

        public:
            void TrackPointer(void** pointer) const;
            void RemovePointer(void** pointer) const;

            void TrackID(int32_t* id) const;
            void RemoveID(int32_t* id) const;
        };

        template <class T>
        class TypeTracker
        {
        public:
            TypeTracker()
                : m_Type ( NULL )
                , m_Data ( NULL )
            {

            }

            ~TypeTracker()
            {
                if(m_Type)
                {
                    m_Type->RemovePointer( (void**)&m_Type ); 
                    m_Type->RemovePointer( (void**)m_Data );
                }
            }

            void Set( const Type* type, T* data )
            {
                m_Type = type; 
                m_Data = data; 

                // we are a reference to the type. if the type is deleted, we need to know
                // or else we will crash below when trying to access the type
                m_Type->TrackPointer( (void**)&m_Type ); 
                m_Type->TrackPointer( (void**)m_Data );
            }

        private:
            const Type* m_Type;
            T* m_Data;
        };

        class IDTracker
        {
        public:
            IDTracker()
                : m_Type( NULL )
                , m_Data( NULL )
            {

            }

            ~IDTracker()
            {
                if(m_Type)
                {
                    m_Type->RemovePointer( (void**) &m_Type ); 
                    m_Type->RemoveID( m_Data );
                }

            }

            void Set( const Type* type, int32_t* data )
            {
                m_Type = type; 
                m_Data = data; 

                // we are a reference to the type. if the type is deleted, we need to know
                // or else we will crash below when trying to access the type
                m_Type->TrackPointer( (void**) &m_Type ); 
                m_Type->TrackID( m_Data );
            }


        private:
            const Type* m_Type;
            int32_t* m_Data;
        };
    }
}