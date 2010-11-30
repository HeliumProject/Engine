#pragma once

#include <map>

#include "Platform/Types.h"
#include "Platform/Utility.h"
#include "Foundation/Memory/SmartPtr.h"

#include "API.h"
#include "Enumeration.h"
#include "ReflectionInfo.h"

namespace Helium
{
    namespace Reflect
    {
        class Composite;

        namespace FieldFlags
        {
            enum FieldFlags
            {
                Discard     = 1 << 0,        // disposable fields are not serialized
                Force       = 1 << 1,        // forced fields are always serialized
                Share       = 1 << 2,        // shared fields are not cloned or compared deeply
                Hide        = 1 << 3,        // hidden fields are not inherently visible in UI
                ReadOnly    = 1 << 4,        // read-only fields cannot be edited in the UI inherently
            };
        }

        class FOUNDATION_API Field : public ReflectionInfo, NonCopyable
        {
        public:
            REFLECTION_BASE( ReflectionTypes::Field );

        protected:
            Field(const Composite* type);
            virtual ~Field();

        public:
            // protect external allocation to keep inlined code in this dll
            static Field* Create(const Composite* type);

            // creates a suitable serializer for this field in the passed object
            virtual DataPtr CreateData(Element* instance = NULL) const;

            // checks to see if the default value matches the value of this field in the passed object
            bool HasDefaultValue(Element* instance) const;

            // sets the default value of this field in the passed object
            bool SetDefaultValue(Element* instance) const;

            // set the name (and UI name if its not set)
            void SetName(const tstring& name);

            const Composite*    m_Type;         // the type we are a field of
            tstring             m_Name;         // name of this field
            tstring             m_UIName;       // friendly name
            uint32_t            m_Size;         // the size of this field
            uintptr_t           m_Offset;       // the offset to the field
            uint32_t            m_Flags;        // flags for special behavior
            int32_t             m_FieldID;      // the unique id of this field
            int32_t             m_DataID; // type id of the serializer to use
            DataPtr       m_Default;      // the value of the default
            CreateObjectFunc    m_Creator;      // function to create a new instance for this field (optional)
        };

        typedef Helium::SmartPtr< Field > FieldPtr;
        typedef Helium::SmartPtr< const Field > ConstFieldPtr;
        typedef std::map< int32_t, ConstFieldPtr > M_FieldIDToInfo;
        typedef std::map< tstring, ConstFieldPtr > M_FieldNameToInfo;
        typedef std::map< uint32_t, ConstFieldPtr > M_FieldOffsetToInfo;

        //
        // ElementField store additional information the compile-time type of a pointer
        //

        class FOUNDATION_API ElementField : public Field
        {
        public:
            REFLECTION_TYPE( ReflectionTypes::ElementField );

        protected:
            ElementField(const Composite* type);
            virtual ~ElementField();

        public:
            // protect external allocation to keep inlined code in this dll
            static ElementField* Create( const Composite* type );

            // creates a suitable serializer (that has a pointer to the enum info)
            virtual DataPtr CreateData ( Element* instance = NULL ) const HELIUM_OVERRIDE;

            int32_t m_TypeID;
        };

        //
        // EnumerationField store additional information to map enumerations as string values in the file
        //

        class FOUNDATION_API EnumerationField : public Field
        {
        public:
            REFLECTION_TYPE( ReflectionTypes::EnumerationField );

        protected:
            EnumerationField(const Composite* type, const Enumeration* enumeration);
            virtual ~EnumerationField();

        public:
            // protect external allocation to keep inlined code in this dll
            static EnumerationField* Create( const Composite* type, const Enumeration* enumeration );

            // creates a suitable serializer (that has a pointer to the enum info)
            virtual DataPtr CreateData ( Element* instance = NULL ) const HELIUM_OVERRIDE;

            const Enumeration* m_Enumeration;
        };
    }
}