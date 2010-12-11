#pragma once

#include <typeinfo>

#include "Foundation/Reflect/Type.h"
#include "Foundation/Reflect/Field.h"
#include "Foundation/Reflect/Visitor.h"
#include "Foundation/Automation/Attribute.h"

//
//  Composite Binary format:
//
//  struct Field
//  {
//      int32_t name;               // string pool index of the name of this field
//      int32_t serializer_id;      // string pool index of the short name of the serializer type
//  };
//
//  struct Composite
//  {
//      int32_t short_name;         // string pool index of the short name for this type
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

        typedef void (*CompositeEnumerator)(void* type);


        //
        // Composite (struct or class)
        //

        class FOUNDATION_API Composite : public Type
        {
        public:
            REFLECTION_TYPE( ReflectionTypes::Composite );

            const Composite*                        m_Base;                 // the base type name
            mutable std::set< const Composite* >    m_Derived;              // the derived type names, mutable since its populated by other objects
            CompositeEnumerator                     m_Enumerator;           // the function to enumerate this type
            bool                                    m_Enumerated;           // flag if we are enumerated
            std::vector< ConstFieldPtr >            m_Fields;               // fields in this composite

        protected:
            Composite();
            virtual ~Composite();

        public:
            //
            // Call all the enumerator functions related to this type
            //

            template<class T>
            void EnumerateInstance(T& instance)
            {
                Compositor<T> compositor (*this, instance);

                // walk our base classes and build a list
                std::stack< const Reflect::Composite* > bases;
                for ( const Composite* base = m_Base; base; base = base->m_Base )
                {
                    bases.push( base );
                }

                // walk that list from base to derived
                while ( !bases.empty() )
                {
                    const Composite* current = bases.top();
                    bases.pop();

                    // enumerate our base type information, note we use the derived instance since its ctor could modify base members
                    if ( current->m_Enumerator )
                    {
                        current->m_Enumerator( &compositor );
                    }

                    // handle abstract base classes by enumerating their type info with our derived instance
                    if ( !current->m_Enumerated )
                    {
                        const_cast< Composite* >( current )->EnumerateInstance<T>( instance );
                    }
                }

                // enumerate our derived type information
                if (m_Enumerator)
                {
                    m_Enumerator(&compositor);
                }

                // we are now enumerated
                m_Enumerated = true;
            }

            //
            // Add fields to the composite
            //

            Reflect::Field* AddField( Element& instance, const std::string& name, const uint32_t offset, uint32_t size, const Class* dataClass, int32_t flags = 0 );
            Reflect::ElementField* AddElementField( Element& instance, const std::string& name, const uint32_t offset, uint32_t size, const Class* dataClass, const Type* type, int32_t flags = 0 );
            Reflect::EnumerationField* AddEnumerationField( Element& instance, const std::string& name, const uint32_t offset, uint32_t size, const Class* dataClass, const Enumeration* enumeration, int32_t flags = 0 );

            //
            // Report information to stdout
            //

            void Report() const;

            //
            // Test for type of this or it base classes
            //

            bool HasType(const Type* type) const;

            // 
            // Name utilities
            //

            static tstring ShortenName(const tstring& name);

            //
            // Find a field by name
            //

            const Field* FindFieldByName(const tstring& name) const;
            const Field* FindFieldByIndex(uint32_t index) const;
            const Field* FindFieldByOffset(uint32_t offset) const;

            // 
            // Finds the field info given a pointer to a member variable on a class.
            // FieldT is the member variable's type and ClassT is the class that the 
            // member variable belongs to.
            // 

            template<typename FieldT, class ClassT>
            const Field* FindField( FieldT ClassT::* pointerToMember ) const
            {
                return FindFieldByOffset( Reflect::Compositor<ClassT>::GetOffset<FieldT>( pointerToMember ) );
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
        };


        //
        // Compositor is a helper for generating a Composite
        //

        template<class T>
        class Compositor
        {
        private:
            Composite&  m_Composite;
            T&          m_Instance;

        public:
            // construction happens in the type creator template function
            Compositor(Composite& composite, T& instance)
                : m_Composite (composite)
                , m_Instance (instance)
            {

            }

            Composite& GetComposite()
            {
                return m_Composite;
            }

            T& GetInstance()
            {
                return m_Instance;
            }

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

            template <class FieldT>
            static inline uint32_t GetOffset( FieldT T::* field )
            {
                return (uint32_t) (uintptr_t) &( ((T*)NULL)->*field); 
            }

            template <class FieldT>
            inline Reflect::Field* AddField( FieldT T::* field, const std::string& name, int32_t flags = 0, const Class* dataClass = NULL )
            {
                return m_Composite.AddField(
                    m_Instance,
                    GetName(name),
                    GetOffset(field),
                    sizeof(FieldT),
                    dataClass ? dataClass : Reflect::GetDataClass<FieldT>(),
                    flags );
            }

            template <class FieldT>
            inline Reflect::Field* AddField( Attribute<FieldT> T::* field, const std::string& name, int32_t flags = 0, const Class* dataClass = NULL )
            {
                return m_Composite.AddField(
                    m_Instance,
                    GetName(name),
                    GetOffset(field),
                    sizeof(FieldT),
                    dataClass ? dataClass : Reflect::GetDataClass<FieldT>(),
                    flags );
            }

            template <class ElementT>
            inline Reflect::ElementField* AddField( StrongPtr< ElementT > T::* field, const std::string& name, int32_t flags = 0 )
            {
                return m_Composite.AddElementField(
                    m_Instance,
                    GetName(name),
                    GetOffset(field),
                    sizeof(uintptr_t),
                    Reflect::GetClass<Reflect::PointerData>(),
                    Reflect::GetType<ElementT>(),
                    flags );
            }

            template <class ElementT>
            inline Reflect::ElementField* AddField( Attribute< StrongPtr< ElementT > > T::* field, const std::string& name, int32_t flags = 0 )
            {
                return m_Composite.AddElementField(
                    m_Instance,
                    GetName(name),
                    GetOffset(field),
                    sizeof(uintptr_t),
                    Reflect::GetClass<Reflect::PointerData>(),
                    Reflect::GetType<ElementT>(),
                    flags );
            }

            template <class ElementT>
            inline Reflect::ElementField* AddField( std::vector< StrongPtr< ElementT > > T::* field, const std::string& name, int32_t flags = 0 )
            {
                return m_Composite.AddElementField(
                    m_Instance,
                    GetName(name),
                    GetOffset(field),
                    sizeof(std::vector< StrongPtr< ElementT > >),
                    Reflect::GetClass<Reflect::ElementStlVectorData>(),
                    Reflect::GetType<ElementT>(),
                    flags );
            }

            template <class ElementT>
            inline Reflect::ElementField* AddField( Attribute< std::vector< StrongPtr< ElementT > > > T::* field, const std::string& name, int32_t flags = 0 )
            {
                return m_Composite.AddElementField(
                    m_Instance,
                    GetName(name),
                    GetOffset(field),
                    sizeof(std::vector< StrongPtr< ElementT > >),
                    Reflect::GetClass<Reflect::ElementStlVectorData>(),
                    Reflect::GetType<ElementT>(),
                    flags );
            }

            template <class ElementT>
            inline Reflect::ElementField* AddField( std::set< StrongPtr< ElementT > > T::* field, const std::string& name, int32_t flags = 0 )
            {
                return m_Composite.AddElementField(
                    m_Instance,
                    GetName(name),
                    GetOffset(field),
                    sizeof(std::set< StrongPtr< ElementT > >),
                    Reflect::GetClass<Reflect::ElementStlSetData>(),
                    Reflect::GetType<ElementT>(),
                    flags );
            }

            template <class ElementT>
            inline Reflect::ElementField* AddField( Attribute< std::set< StrongPtr< ElementT > > > T::* field, const std::string& name, int32_t flags = 0 )
            {
                return m_Composite.AddElementField(
                    m_Instance,
                    GetName(name),
                    GetOffset(field),
                    sizeof(std::set< StrongPtr< ElementT > >),
                    Reflect::GetClass<Reflect::ElementStlSetData>(),
                    Reflect::GetType<ElementT>(),
                    flags );
            }

            template <class KeyT, class ElementT>
            inline Reflect::ElementField* AddField( std::map< KeyT, StrongPtr< ElementT > > T::* field, const std::string& name, int32_t flags = 0 )
            {
                return m_Composite.AddElementField( 
                    m_Instance, 
                    GetName(name), 
                    GetOffset(field), 
                    sizeof(std::map< KeyT, StrongPtr< ElementT > >), 
                    Reflect::GetClass<Reflect::SimpleElementStlMapData< KeyT > >(), 
                    Reflect::GetType<ElementT>(), 
                    flags );
            }

            template <class KeyT, class ElementT>
            inline Reflect::ElementField* AddField( Attribute< std::map< KeyT, StrongPtr< ElementT > > > T::* field, const std::string& name, int32_t flags = 0 )
            {
                return m_Composite.AddElementField( 
                    m_Instance, 
                    GetName(name), 
                    GetOffset(field), 
                    sizeof(std::map< KeyT, StrongPtr< ElementT > >), 
                    Reflect::GetClass<Reflect::SimpleElementStlMapData< KeyT > >(), 
                    Reflect::GetType<ElementT>(), 
                    flags );
            }

            template <class FieldT>
            inline Reflect::EnumerationField* AddEnumerationField( FieldT T::* field, const std::string& name, int32_t flags = 0 )
            {
                return m_Composite.AddEnumerationField(
                    m_Instance,
                    GetName(name),
                    GetOffset(field),
                    sizeof(FieldT),
                    Reflect::GetClass<Reflect::EnumerationData>(),
                    Reflect::GetEnumeration<FieldT>(),
                    flags );
            }

            template <class FieldT>
            inline Reflect::EnumerationField* AddEnumerationField( Attribute< FieldT > T::* field, const std::string& name, int32_t flags = 0 )
            {
                return m_Composite.AddEnumerationField(
                    m_Instance,
                    GetName(name),
                    GetOffset(field),
                    sizeof(FieldT),
                    Reflect::GetClass<Reflect::EnumerationData>(),
                    Reflect::GetEnumeration<FieldT>(),
                    flags );
            }

            template <class EnumT, class FieldT>
            inline Reflect::EnumerationField* AddBitfieldField( FieldT T::* field, const std::string& name, int32_t flags = 0 )
            {
                return m_Composite.AddEnumerationField(
                    m_Instance,
                    GetName(name),
                    GetOffset(field),
                    sizeof(FieldT),
                    Reflect::GetClass<Reflect::BitfieldData>(),
                    Reflect::GetEnumeration<EnumT>(),
                    flags );
            }

            template <class EnumT, class FieldT>
            inline Reflect::EnumerationField* AddBitfieldField( Attribute< FieldT > T::* field, const std::string& name, int32_t flags = 0 )
            {
                return m_Composite.AddEnumerationField(
                    m_Instance,
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