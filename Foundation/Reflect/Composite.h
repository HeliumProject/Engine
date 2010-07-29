#pragma once

#include <typeinfo>

#include "Type.h"
#include "Field.h"
#include "Visitor.h"

//
//  Composite Binary format:
//
//  struct Field
//  {
//      i32 name;               // string pool index of the name of this field
//      i32 serializer_id;      // string pool index of the short name of the serializer type
//  };
//
//  struct Composite
//  {
//      i32 short_name;         // string pool index of the short name for this type
//      i32 count;              // number of field infos to follow
//      Field[] fields;         // field rtti data
//      i32 term;               // -1
//  };
//

namespace Reflect
{
    class Field;
    class Composite;

    class PointerSerializer;
    class ElementArraySerializer;
    class ElementSetSerializer;
    class EnumerationSerializer;
    class BitfieldSerializer;

    template< class KeyT >
    class SimpleElementMapSerializer;

    typedef void (*CompositeEnumerator)(void* type);


    //
    // Composite (struct or class)
    //

    class FOUNDATION_API Composite : public Type
    {
    public:
        REFLECTION_TYPE( ReflectionTypes::Composite );

        tstring               m_Base;               // the base type name
        std::set<tstring>     m_Derived;            // the derived type names

        CompositeEnumerator   m_Enumerator;         // the function to enumerate this type
        bool                  m_Enumerated;         // flag if we are enumerated

        M_FieldNameToInfo     m_FieldNameToInfo;    // maps field name to field info block
        M_FieldIDToInfo       m_FieldIDToInfo;      // maps field id to field info block
        M_FieldOffsetToInfo   m_FieldOffsetToInfo;  // maps offset (through pointer to member reference) to field info block

        i32                   m_FirstFieldID;       // first field id of this class's fields (exclusive of base and derived class's fields)
        i32                   m_LastFieldID;        // last field id of this class's fields (exclusive of base and derived class's fields)
        i32                   m_NextFieldID;        // id used for the next field (as we are enumerating)

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
            std::vector<const Reflect::Composite*> bases;
            if ( !m_Base.empty() )
            {
                tstring baseName = m_Base;
                while ( !baseName.empty() )
                {
                    const Reflect::Composite* base = Reflect::Registry::GetInstance()->GetClass( baseName );
                    if (base)
                    {
                        bases.push_back(base);
                        baseName = base->m_Base;
                    }
                    else
                    {
                        HELIUM_BREAK();
                        baseName.clear();
                    }
                }
            }

            // walk that list from base to derived
            std::vector<const Reflect::Composite*>::const_reverse_iterator itr = bases.rbegin();
            std::vector<const Reflect::Composite*>::const_reverse_iterator end = bases.rend();
            for ( ; itr != end; ++itr )
            {
                const Reflect::Composite* base = *itr;

                // enumerate our base type information, note we use the derived instance since its ctor could modify base members
                if (base->m_Enumerator)
                {
                    base->m_Enumerator(&compositor);
                }

                // handle HELIUM_ABSTRACT base classes by enumerating their type info with our derived instance
                if (!base->m_Enumerated)
                {
                    const_cast<Composite*>(base)->EnumerateInstance<T>(instance);
                }
            }

            // mark the first field of *this* type
            m_FirstFieldID = m_NextFieldID;

            // enumerate our derived type information
            if (m_Enumerator)
            {
                m_Enumerator(&compositor);
            }

            // mark the last field of *this* type
            m_LastFieldID = m_NextFieldID-1;

            // we are now enumerated
            m_Enumerated = true;
        }

        //
        // Add fields to the composite
        //

        Reflect::Field* AddField ( Element& instance, const std::string& name, const u32 offset, u32 size, i32 serializerID, i32 flags = 0 );
        Reflect::ElementField* AddElementField ( Element& instance, const std::string& name, const u32 offset, u32 size, i32 serializerID, i32 typeID, i32 flags = 0 );
        Reflect::EnumerationField* AddEnumerationField ( Element& instance, const std::string& name, const u32 offset, u32 size, i32 serializerID, const std::string& enumName, i32 flags = 0 );

        //
        // Report information to stdout
        //

        void Report() const;

        //
        // Test for type of this or it base classes
        //

        bool HasType(i32 type) const;

        // 
        // Name utilities
        //

        static tstring ShortenName(const tstring& fullName);

        //
        // Find a field by name
        //

        const Field* FindFieldByName(const tstring& name) const;

        // 
        // Finds the field info given a pointer to a member variable on a class.
        // FieldT is the member variable's type and ClassT is the class that the 
        // member variable belongs to.
        // 

        template<typename FieldT, class ClassT>
        const Field* FindField( FieldT ClassT::* pointerToMember ) const
        {
            M_FieldOffsetToInfo::const_iterator found = m_FieldOffsetToInfo.find( Reflect::Compositor<ClassT>::GetOffset<FieldT>( pointerToMember ) );
            if ( found != m_FieldOffsetToInfo.end() )
            {
                return found->second;
            }
            return NULL;
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
        // Visits RTTI data fields recursively, used to interactively traverse structures
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
        static inline u32 GetOffset( FieldT T::* field )
        {
            return (u32) (uintptr) &( ((T*)NULL)->*field); 
        }

        template <class FieldT>
        inline Reflect::Field* AddField( FieldT T::* field, const std::string& name, i32 flags = 0, i32 serializerType = -1 )
        {
            return m_Composite.AddField(
                m_Instance,
                GetName(name),
                GetOffset(field),
                sizeof(FieldT),
                serializerType < 0 ? Reflect::GetType<FieldT>() : serializerType,
                flags );
        }

        template <class ElementT>
        inline Reflect::ElementField* AddField( Helium::SmartPtr<ElementT> T::* field, const std::string& name, i32 flags = 0 )
        {
            return m_Composite.AddElementField(
                m_Instance,
                GetName(name),
                GetOffset(field),
                sizeof(uintptr),
                Reflect::GetType<Reflect::PointerSerializer>(),
                Reflect::GetType<ElementT>(),
                flags );
        }

        template <class ElementT>
        inline Reflect::ElementField* AddField( std::vector< Helium::SmartPtr< ElementT > >T::* field, const std::string& name, i32 flags = 0 )
        {
            return m_Composite.AddElementField(
                m_Instance,
                GetName(name),
                GetOffset(field),
                sizeof(std::vector< Helium::SmartPtr< ElementT > >),
                Reflect::GetType<Reflect::ElementArraySerializer>(),
                Reflect::GetType<ElementT>(),
                flags );
        }

        template <class ElementT>
        inline Reflect::ElementField* AddField( std::set< Helium::SmartPtr< ElementT > >T::* field, const std::string& name, i32 flags = 0 )
        {
            return m_Composite.AddElementField(
                m_Instance,
                GetName(name),
                GetOffset(field),
                sizeof(std::set< Helium::SmartPtr< ElementT > >),
                Reflect::GetType<Reflect::ElementSetSerializer>(),
                Reflect::GetType<ElementT>(),
                flags );
        }

        template <class KeyT, class ElementT>
        inline Reflect::ElementField* AddField( std::map< KeyT, Helium::SmartPtr< ElementT > >T::* field, const std::string& name, i32 flags = 0 )
        {
            return m_Composite.AddElementField( 
                m_Instance, 
                GetName(name), 
                GetOffset(field), 
                sizeof(std::map< KeyT, Helium::SmartPtr< ElementT > >), 
                Reflect::GetType<Reflect::SimpleElementMapSerializer< KeyT > >(), 
                Reflect::GetType<ElementT>(), 
                flags );
        }

        template <class FieldT>
        inline Reflect::EnumerationField* AddEnumerationField( FieldT T::* field, const std::string& name, i32 flags = 0 )
        {
            return m_Composite.AddEnumerationField(
                m_Instance,
                GetName(name),
                GetOffset(field),
                sizeof(FieldT),
                Reflect::GetType<Reflect::EnumerationSerializer>(),
                typeid(FieldT).name(),
                flags );
        }

        template <class EnumT, class FieldT>
        inline Reflect::EnumerationField* AddBitfieldField( FieldT T::* field, const std::string& name, i32 flags = 0 )
        {
            return m_Composite.AddEnumerationField(
                m_Instance,
                GetName(name),
                GetOffset(field),
                sizeof(FieldT),
                Reflect::GetType<Reflect::BitfieldSerializer>(),
                typeid(EnumT).name(),
                flags );
        }
    };
}