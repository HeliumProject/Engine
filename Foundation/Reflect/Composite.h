#pragma once

#include <typeinfo>

#include "Foundation/Reflect/Type.h"
#include "Foundation/Reflect/Field.h"
#include "Foundation/Reflect/Visitor.h"
#include "Foundation/Automation/Attribute.h"
#include "Foundation/Container/Set.h"

//
//  Composite Binary format:
//
//  struct Field
//  {
//      int32_t name;               // string pool index of the name of this field
//      int32_t serializer_id;      // string pool index of the name of the serializer type
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
        class Field;
        class Composite;

        typedef void (*AcceptVisitor)( Composite& );


        //
        // Composite (struct or class)
        //

        class FOUNDATION_API Composite : public Type
        {
        public:
            REFLECTION_TYPE( ReflectionTypes::Composite );

            const Composite*                        m_Base;                 // the base type name
            mutable Set< const Composite* >         m_Derived;              // the derived type names, mutable since its populated by other objects
            std::vector< ConstFieldPtr >            m_Fields;               // fields in this composite
            AcceptVisitor                           m_Accept;

        protected:
            Composite();
            virtual ~Composite();

        public:
            // overload from Type
            virtual void Report() const HELIUM_OVERRIDE;
            virtual void Unregister() const HELIUM_OVERRIDE;

            //
            // Populate composite information
            //

            template< class CompositeT >
            static void CreateComposite( Name name, Name baseName, AcceptVisitor accept, Composite* info )
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
                info->m_Base->m_Derived.Insert( info );

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
                            baseName.Clear();
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

            //
            // Add fields to the composite
            //

            // computes the number of fields in all our base classes (the base index for our fields)
            uint32_t GetBaseFieldCount() const;

            // concrete field population functions, called from template functions below with deducted data
            Reflect::Field* AddField( const std::string& name, const uint32_t offset, uint32_t size, const Class* dataClass, int32_t flags = 0 );
            Reflect::ElementField* AddElementField( const std::string& name, const uint32_t offset, uint32_t size, const Class* dataClass, const Type* type, int32_t flags = 0 );
            Reflect::EnumerationField* AddEnumerationField( const std::string& name, const uint32_t offset, uint32_t size, const Class* dataClass, const Enumeration* enumeration, int32_t flags = 0 );

            //
            // Test for type of this or it base classes
            //

            bool HasType(const Type* type) const;

            //
            // Find a field by name
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
            // Equals compares all reflect-aware data, this is only really safe for serializer types, since
            //  users could add non-reflect aware fields that would not be used in the comparison.
            //  We could possibly use the default comparison operator (if the compiler generates one)
            //  to affirm that non-reflect aware fields are equals, but this would not work for non-reflect
            //  aware field pointers (since their pointer values would be used by the comparison operator).
            //

            static bool Equals(const Element* a, const Element* b);

            //
            // Visits fields recursively, used to interactively traverse structures
            //

            static void Visit (Element* element, Visitor& visitor);

            // 
            // Copies data from one element to another by finding a common base class and cloning all of the
            //  fields from the source element into the destination element.
            // 

            static void Copy( const Element* src, Element* dest );


            //
            // Reflection Generation Functions
            //

            static inline std::string GetName(const std::string& name)
            {
                size_t pos = name.find_last_of( ':' )+1;
                if (pos != std::string::npos)
                {
                    return name.substr( pos );
                }
                else
                {
                    return name;
                }
            }

            template < class CompositeT, class FieldT >
            static inline uint32_t GetOffset( FieldT CompositeT::* field )
            {
                return (uint32_t) (uintptr_t) &( ((CompositeT*)NULL)->*field); 
            }

            template < class CompositeT, class FieldT >
            inline Reflect::Field* AddField( FieldT CompositeT::* field, const std::string& name, int32_t flags = 0, const Class* dataClass = NULL )
            {
                return AddField(
                    GetName(name),
                    GetOffset(field),
                    sizeof(FieldT),
                    dataClass ? dataClass : Reflect::GetDataClass<FieldT>(),
                    flags );
            }

            template < class CompositeT, class FieldT >
            inline Reflect::Field* AddField( Attribute<FieldT> CompositeT::* field, const std::string& name, int32_t flags = 0, const Class* dataClass = NULL )
            {
                return AddField(
                    GetName(name),
                    GetOffset(field),
                    sizeof(FieldT),
                    dataClass ? dataClass : Reflect::GetDataClass<FieldT>(),
                    flags );
            }

            template < class CompositeT, class ElementT >
            inline Reflect::ElementField* AddField( StrongPtr< ElementT > CompositeT::* field, const std::string& name, int32_t flags = 0 )
            {
                return AddElementField(
                    GetName(name),
                    GetOffset(field),
                    sizeof(uintptr_t),
                    Reflect::GetClass<Reflect::PointerData>(),
                    Reflect::GetType<ElementT>(),
                    flags );
            }

            template < class CompositeT, class ElementT >
            inline Reflect::ElementField* AddField( Attribute< StrongPtr< ElementT > > CompositeT::* field, const std::string& name, int32_t flags = 0 )
            {
                return AddElementField(
                    GetName(name),
                    GetOffset(field),
                    sizeof(uintptr_t),
                    Reflect::GetClass<Reflect::PointerData>(),
                    Reflect::GetType<ElementT>(),
                    flags );
            }

            template < class CompositeT, class ElementT >
            inline Reflect::ElementField* AddField( std::vector< StrongPtr< ElementT > > CompositeT::* field, const std::string& name, int32_t flags = 0 )
            {
                return AddElementField(
                    GetName(name),
                    GetOffset(field),
                    sizeof(std::vector< StrongPtr< ElementT > >),
                    Reflect::GetClass<Reflect::ElementStlVectorData>(),
                    Reflect::GetType<ElementT>(),
                    flags );
            }

            template < class CompositeT, class ElementT >
            inline Reflect::ElementField* AddField( Attribute< std::vector< StrongPtr< ElementT > > > CompositeT::* field, const std::string& name, int32_t flags = 0 )
            {
                return AddElementField(
                    GetName(name),
                    GetOffset(field),
                    sizeof(std::vector< StrongPtr< ElementT > >),
                    Reflect::GetClass<Reflect::ElementStlVectorData>(),
                    Reflect::GetType<ElementT>(),
                    flags );
            }

            template < class CompositeT, class ElementT >
            inline Reflect::ElementField* AddField( std::set< StrongPtr< ElementT > > CompositeT::* field, const std::string& name, int32_t flags = 0 )
            {
                return AddElementField(
                    GetName(name),
                    GetOffset(field),
                    sizeof(std::set< StrongPtr< ElementT > >),
                    Reflect::GetClass<Reflect::ElementStlSetData>(),
                    Reflect::GetType<ElementT>(),
                    flags );
            }

            template < class CompositeT, class ElementT >
            inline Reflect::ElementField* AddField( Attribute< std::set< StrongPtr< ElementT > > > CompositeT::* field, const std::string& name, int32_t flags = 0 )
            {
                return AddElementField(
                    GetName(name),
                    GetOffset(field),
                    sizeof(std::set< StrongPtr< ElementT > >),
                    Reflect::GetClass<Reflect::ElementStlSetData>(),
                    Reflect::GetType<ElementT>(),
                    flags );
            }

            template < class CompositeT, class KeyT, class ElementT >
            inline Reflect::ElementField* AddField( std::map< KeyT, StrongPtr< ElementT > > CompositeT::* field, const std::string& name, int32_t flags = 0 )
            {
                return AddElementField( 
                    GetName(name), 
                    GetOffset(field), 
                    sizeof(std::map< KeyT, StrongPtr< ElementT > >), 
                    Reflect::GetClass<Reflect::SimpleElementStlMapData< KeyT > >(), 
                    Reflect::GetType<ElementT>(), 
                    flags );
            }

            template < class CompositeT, class KeyT, class ElementT >
            inline Reflect::ElementField* AddField( Attribute< std::map< KeyT, StrongPtr< ElementT > > > CompositeT::* field, const std::string& name, int32_t flags = 0 )
            {
                return AddElementField( 
                    GetName(name), 
                    GetOffset(field), 
                    sizeof(std::map< KeyT, StrongPtr< ElementT > >), 
                    Reflect::GetClass<Reflect::SimpleElementStlMapData< KeyT > >(), 
                    Reflect::GetType<ElementT>(), 
                    flags );
            }

            template < class CompositeT, class FieldT >
            inline Reflect::EnumerationField* AddEnumerationField( FieldT CompositeT::* field, const std::string& name, int32_t flags = 0 )
            {
                return AddEnumerationField(
                    GetName(name),
                    GetOffset(field),
                    sizeof(FieldT),
                    Reflect::GetClass<Reflect::EnumerationData>(),
                    Reflect::GetEnumeration<FieldT>(),
                    flags );
            }

            template < class CompositeT, class FieldT >
            inline Reflect::EnumerationField* AddEnumerationField( Attribute< FieldT > CompositeT::* field, const std::string& name, int32_t flags = 0 )
            {
                return AddEnumerationField(
                    GetName(name),
                    GetOffset(field),
                    sizeof(FieldT),
                    Reflect::GetClass<Reflect::EnumerationData>(),
                    Reflect::GetEnumeration<FieldT>(),
                    flags );
            }

            template < class CompositeT, class EnumT, class FieldT >
            inline Reflect::EnumerationField* AddBitfieldField( FieldT CompositeT::* field, const std::string& name, int32_t flags = 0 )
            {
                return AddEnumerationField(
                    GetName(name),
                    GetOffset(field),
                    sizeof(FieldT),
                    Reflect::GetClass<Reflect::BitfieldData>(),
                    Reflect::GetEnumeration<EnumT>(),
                    flags );
            }

            template < class CompositeT, class EnumT, class FieldT >
            inline Reflect::EnumerationField* AddBitfieldField( Attribute< FieldT > CompositeT::* field, const std::string& name, int32_t flags = 0 )
            {
                return AddEnumerationField(
                    GetName(name),
                    GetOffset(field),
                    sizeof(FieldT),
                    Reflect::GetClass<Reflect::BitfieldData>(),
                    Reflect::GetEnumeration<EnumT>(),
                    flags );
            }
        };
    }
}