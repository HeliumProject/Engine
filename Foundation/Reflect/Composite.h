#pragma once

#include <typeinfo>

#include "Foundation/Reflect/Type.h"
#include "Foundation/Reflect/Visitor.h"
#include "Foundation/Automation/Attribute.h"
#include "Foundation/Container/Set.h"
#include "Foundation/Container/DynArray.h"

//
//  Composite Binary format:
//
//  struct Field
//  {
//      int32_t name;               // string pool index of the name of this field
//      int32_t serializer_id;      // string pool index of the name of the data type
//  };
//
//  struct Composite
//  {
//      int32_t short_name;         // string pool index of the name for this type
//      int32_t count;              // number of field infos to follow
//      Field[] fields;             // field rtti data
//      int32_t term;               // -1
//  };
//

namespace Helium
{
    namespace Reflect
    {
        class Composite;
        typedef void (*AcceptVisitor)( Composite& );

        namespace FieldFlags
        {
            enum Enum
            {
                Discard     = 1 << 0,       // disposable fields are not serialized
                Force       = 1 << 1,       // forced fields are always serialized
                Share       = 1 << 2,       // shared fields are not cloned or compared deeply
                Hide        = 1 << 3,       // hidden fields are not inherently visible in UI
                ReadOnly    = 1 << 4,       // read-only fields cannot be edited in the UI inherently
            };
        }

        //
        // Field (an object of a composite)
        //

        class FOUNDATION_API Field : public PropertyCollection
        {
        public:
            Field();

            // creates a suitable data for this field
            DataPtr CreateData() const;

            // allocate and connect to an instance
            DataPtr CreateData(void* instance) const;
            DataPtr CreateData(const void* instance) const;

            // allocate and connect to the default
            DataPtr CreateTemplateData() const;

            // determine if this field should be serialized
            DataPtr ShouldSerialize(const void* instance) const;

            const Composite*        m_Composite;    // the type we are a field of
            const tchar_t*          m_Name;         // name of this field
            uint32_t                m_Size;         // the size of this field
            uintptr_t               m_Offset;       // the offset to the field
            uint32_t                m_Flags;        // flags for special behavior
            uint32_t                m_Index;        // the unique id of this field
            const Type*             m_Type;         // the type of this field (NULL for POD types)
            const Class*            m_DataClass;    // type id of the data to use
        };

        //
        // Composite (struct or class)
        //

        class FOUNDATION_API Composite : public Type
        {
        public:
            REFLECTION_TYPE( ReflectionTypes::Composite );

            const Composite*                        m_Base;                 // the base type name
            mutable const Composite*                m_FirstDerived;         // head of the derived linked list, mutable since its populated by other objects
            mutable const Composite*                m_NextSibling;          // next in the derived linked list, mutable since its populated by other objects
            DynArray< Field >                       m_Fields;               // fields in this composite
            AcceptVisitor                           m_Accept;               // function to populate this structure
            const void*                             m_Template;             // default template instance

        protected:
            Composite();
            virtual ~Composite();

        public:
            template< class CompositeT >
            static void Create( const tchar_t* name, const tchar_t* baseName, AcceptVisitor accept, Composite* info )
            {
                // the size
                info->m_Size = sizeof( CompositeT );

                // the name of this composite
                info->m_Name = name;

                // lookup base class
                info->m_Base = Reflect::Registry::GetInstance()->GetClass( baseName );

                // if you hit this break your base class is not registered yet!
                HELIUM_ASSERT( info->m_Base );

                // populate base classes' derived class list (unregister will remove it)
                info->m_Base->AddDerived( info );

                // c++ can give us the address of base class static functions,
                //  so check each base class to see if this is really a base class enumerate function
                bool baseAccept = false;
                {
                    const Reflect::Composite* base = info->m_Base;
                    while ( !baseAccept && base )
                    {
                        if (base)
                        {
                            baseAccept = base->m_Accept && base->m_Accept == accept;
                            base = base->m_Base;
                        }
                        else
                        {
                            HELIUM_BREAK(); // if you hit this break your base class is not registered yet!
                            baseName = NULL;
                        }
                    }
                }

                // if our enumerate function isn't one from a base class
                if ( !baseAccept )
                {
                    // the accept function will populate our field data
                    info->m_Accept = accept;
                }

                // populate reflection information
                if ( info->m_Accept )
                {
                    info->m_Accept( *info );
                }
            }

            // Overloaded functions from Type
            virtual void Report() const HELIUM_OVERRIDE;
            virtual void Unregister() const HELIUM_OVERRIDE;

            // Inheritance Hierarchy
            bool IsType(const Composite* type) const;
            void AddDerived( const Composite* derived ) const;
            void RemoveDerived( const Composite* derived ) const;

            //
            // Equals compares all reflect-aware data, this is only really safe for data types, since
            //  users could add non-reflect aware fields that would not be used in the comparison.
            //  We could possibly use the default comparison operator (if the compiler generates one)
            //  to affirm that non-reflect aware fields are equals, but this would not work for non-reflect
            //  aware field pointers (since their pointer values would be used by the comparison operator).
            //

            bool Equals( const void* a, const void* b ) const;

            //
            // Visits fields recursively, used to interactively traverse structures
            //

            void Visit( void* instance, Visitor& visitor ) const;

            // 
            // Copies data from one instance to another by finding a common base class and cloning all of the
            //  fields from the source object into the destination object.
            // 

            void Copy( const void* source, void* destination ) const;

            //
            // Find a field in this composite
            //

            const Field* FindFieldByName(uint32_t crc) const;
            const Field* FindFieldByIndex(uint32_t index) const;
            const Field* FindFieldByOffset(uint32_t offset) const;

            // 
            // Finds the field info given a pointer to a member variable on a class.
            // FieldT is the member variable's type and CompositeT is the class that the 
            // member variable belongs to.
            // 

            template< class CompositeT, typename FieldT >
            const Field* FindField( FieldT CompositeT::* pointerToMember ) const
            {
                return FindFieldByOffset( Reflect::Composite::GetOffset<CompositeT, FieldT>( pointerToMember ) );
            }

            //
            // Add fields to the composite
            //

            // computes the number of fields in all our base classes (the base index for our fields)
            uint32_t GetBaseFieldCount() const;

            // concrete field population functions, called from template functions below with deducted data
            Reflect::Field* AddField( const tchar_t* name, const uint32_t offset, uint32_t size, const Class* dataClass, int32_t flags = 0 );
            Reflect::Field* AddObjectField( const tchar_t* name, const uint32_t offset, uint32_t size, const Class* dataClass, const Type* type, int32_t flags = 0 );
            Reflect::Field* AddEnumerationField( const tchar_t* name, const uint32_t offset, uint32_t size, const Class* dataClass, const Enumeration* enumeration, int32_t flags = 0 );

            //
            // Reflection Generation Functions
            //

            template < class CompositeT, class FieldT >
            static inline uint32_t GetOffset( FieldT CompositeT::* field )
            {
                return (uint32_t) (uintptr_t) &( ((CompositeT*)NULL)->*field); 
            }

            template < class CompositeT, class FieldT >
            inline Reflect::Field* AddField( FieldT CompositeT::* field, const tchar_t* name, int32_t flags = 0, const Class* dataClass = NULL )
            {
                return AddField(
                    name,
                    GetOffset(field),
                    sizeof(FieldT),
                    dataClass ? dataClass : Reflect::GetDataClass<FieldT>(),
                    flags );
            }

            template < class CompositeT, class FieldT >
            inline Reflect::Field* AddField( Attribute<FieldT> CompositeT::* field, const tchar_t* name, int32_t flags = 0, const Class* dataClass = NULL )
            {
                return AddField(
                    name,
                    GetOffset(field),
                    sizeof(FieldT),
                    dataClass ? dataClass : Reflect::GetDataClass<FieldT>(),
                    flags );
            }

            template < class CompositeT, class ObjectT >
            inline Reflect::Field* AddField( StrongPtr< ObjectT > CompositeT::* field, const tchar_t* name, int32_t flags = 0 )
            {
                return AddObjectField(
                    name,
                    GetOffset(field),
                    sizeof(uintptr_t),
                    Reflect::GetClass<Reflect::PointerData>(),
                    Reflect::GetClass<ObjectT>(),
                    flags );
            }

            template < class CompositeT, class ObjectT >
            inline Reflect::Field* AddField( Attribute< StrongPtr< ObjectT > > CompositeT::* field, const tchar_t* name, int32_t flags = 0 )
            {
                return AddObjectField(
                    name,
                    GetOffset(field),
                    sizeof(uintptr_t),
                    Reflect::GetClass<Reflect::PointerData>(),
                    Reflect::GetClass<ObjectT>(),
                    flags );
            }

            template < class CompositeT, class ObjectT >
            inline Reflect::Field* AddField( std::vector< StrongPtr< ObjectT > > CompositeT::* field, const tchar_t* name, int32_t flags = 0 )
            {
                return AddObjectField(
                    name,
                    GetOffset(field),
                    sizeof(std::vector< StrongPtr< ObjectT > >),
                    Reflect::GetClass<Reflect::ObjectStlVectorData>(),
                    Reflect::GetClass<ObjectT>(),
                    flags );
            }

            template < class CompositeT, class ObjectT >
            inline Reflect::Field* AddField( Attribute< std::vector< StrongPtr< ObjectT > > > CompositeT::* field, const tchar_t* name, int32_t flags = 0 )
            {
                return AddObjectField(
                    name,
                    GetOffset(field),
                    sizeof(std::vector< StrongPtr< ObjectT > >),
                    Reflect::GetClass<Reflect::ObjectStlVectorData>(),
                    Reflect::GetClass<ObjectT>(),
                    flags );
            }

            template < class CompositeT, class ObjectT >
            inline Reflect::Field* AddField( std::set< StrongPtr< ObjectT > > CompositeT::* field, const tchar_t* name, int32_t flags = 0 )
            {
                return AddObjectField(
                    name,
                    GetOffset(field),
                    sizeof(std::set< StrongPtr< ObjectT > >),
                    Reflect::GetClass<Reflect::ObjectStlSetData>(),
                    Reflect::GetClass<ObjectT>(),
                    flags );
            }

            template < class CompositeT, class ObjectT >
            inline Reflect::Field* AddField( Attribute< std::set< StrongPtr< ObjectT > > > CompositeT::* field, const tchar_t* name, int32_t flags = 0 )
            {
                return AddObjectField(
                    name,
                    GetOffset(field),
                    sizeof(std::set< StrongPtr< ObjectT > >),
                    Reflect::GetClass<Reflect::ObjectStlSetData>(),
                    Reflect::GetClass<ObjectT>(),
                    flags );
            }

            template < class CompositeT, class KeyT, class ObjectT >
            inline Reflect::Field* AddField( std::map< KeyT, StrongPtr< ObjectT > > CompositeT::* field, const tchar_t* name, int32_t flags = 0 )
            {
                return AddObjectField( 
                    name, 
                    GetOffset(field), 
                    sizeof(std::map< KeyT, StrongPtr< ObjectT > >), 
                    Reflect::GetClass<Reflect::SimpleObjectStlMapData< KeyT > >(), 
                    Reflect::GetClass<ObjectT>(), 
                    flags );
            }

            template < class CompositeT, class KeyT, class ObjectT >
            inline Reflect::Field* AddField( Attribute< std::map< KeyT, StrongPtr< ObjectT > > > CompositeT::* field, const tchar_t* name, int32_t flags = 0 )
            {
                return AddObjectField( 
                    name, 
                    GetOffset(field), 
                    sizeof(std::map< KeyT, StrongPtr< ObjectT > >), 
                    Reflect::GetClass<Reflect::SimpleObjectStlMapData< KeyT > >(), 
                    Reflect::GetClass<ObjectT>(), 
                    flags );
            }

            template < class CompositeT, class FieldT >
            inline Reflect::Field* AddEnumerationField( FieldT CompositeT::* field, const tchar_t* name, int32_t flags = 0 )
            {
                return AddEnumerationField(
                    name,
                    GetOffset(field),
                    sizeof(FieldT),
                    Reflect::GetClass<Reflect::EnumerationData>(),
                    Reflect::GetEnumeration<FieldT>(),
                    flags );
            }

            template < class CompositeT, class FieldT >
            inline Reflect::Field* AddEnumerationField( Attribute< FieldT > CompositeT::* field, const tchar_t* name, int32_t flags = 0 )
            {
                return AddEnumerationField(
                    name,
                    GetOffset(field),
                    sizeof(FieldT),
                    Reflect::GetClass<Reflect::EnumerationData>(),
                    Reflect::GetEnumeration<FieldT>(),
                    flags );
            }

            template < class CompositeT, class EnumT, class FieldT >
            inline Reflect::Field* AddBitfieldField( FieldT CompositeT::* field, const tchar_t* name, int32_t flags = 0 )
            {
                return AddEnumerationField(
                    name,
                    GetOffset(field),
                    sizeof(FieldT),
                    Reflect::GetClass<Reflect::BitfieldData>(),
                    Reflect::GetEnumeration<EnumT>(),
                    flags );
            }

            template < class CompositeT, class EnumT, class FieldT >
            inline Reflect::Field* AddBitfieldField( Attribute< FieldT > CompositeT::* field, const tchar_t* name, int32_t flags = 0 )
            {
                return AddEnumerationField(
                    name,
                    GetOffset(field),
                    sizeof(FieldT),
                    Reflect::GetClass<Reflect::BitfieldData>(),
                    Reflect::GetEnumeration<EnumT>(),
                    flags );
            }
        };
    }
}