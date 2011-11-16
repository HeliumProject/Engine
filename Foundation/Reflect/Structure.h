#pragma once

#include "Foundation/Memory/AutoPtr.h"

#include "Composite.h"

namespace Helium
{
    namespace Reflect
    {
        struct IStructureDynArrayAdapter
        {
            virtual uint32_t GetDynArrayByteSize() const = 0;
            virtual size_t GetSize(const void* dyn_array) const = 0;
            virtual void Clear(void* dyn_array) const = 0;
            virtual DataPtr GetItem(void* dyn_array, size_t at, void* _instance, const Field* _field) const = 0;
            virtual void SetItem(void* dyn_array, size_t at, Data* value, void* instance, const Field* field) const = 0;
            virtual void Insert(void* dyn_array, size_t at, Data* value) const = 0;
            virtual void Remove(void* dyn_array, size_t at) const = 0;
            virtual void Swap(void* dyn_array, size_t at_rhs, size_t at_lhs) const = 0;
            virtual void Resize(void* dyn_array, size_t size) const = 0;
            virtual bool Set( void* dyn_array, void* src, uint32_t flags ) const = 0;
            virtual bool Equals( void* dyn_array, void* src ) const = 0;
            virtual bool Accept( const Composite *composite, void* dyn_array, Visitor& visitor ) const = 0;
        };

        template <class T>
        class StructureDynArrayAdapter : public IStructureDynArrayAdapter
        {
            virtual uint32_t GetDynArrayByteSize() const
            {
                return sizeof(DynArray<T>);
            }

            virtual size_t GetSize(const void* dyn_array) const
            {
                const DynArray<T> *dyn_array_t = static_cast< const DynArray<T> *>(dyn_array);
                return dyn_array_t->GetSize();
            }
            
            virtual void Clear(void* dyn_array) const
            {
                DynArray<T> *dyn_array_t = static_cast< DynArray<T> * >(dyn_array);
                dyn_array_t->Clear();
            }
            
            virtual DataPtr GetItem(void* dyn_array, size_t at, void* instance, const Field* field) const
            {
                DynArray<T> *dyn_array_t = static_cast<DynArray<T> *>(dyn_array);
                return Data::BindStructure(&dyn_array_t->GetElement(at), instance, field);
            }
            
            virtual void SetItem(void* dyn_array, size_t at, Data* value, void* instance, const Field* field) const
            {
                DynArray<T> *dyn_array_t = static_cast<DynArray<T> *>(dyn_array);
                DataPtr data = Data::BindStructure(&dyn_array_t->GetElement(at), instance, field);
                data->Set(value);
            }
            
            virtual void Insert(void* dyn_array, size_t at, Data* value) const
            {
                DynArray<T> *dyn_array_t = static_cast<DynArray<T> *>(dyn_array);
                Data::GetValue( value, dyn_array_t->GetElement(at) );
                
                T temp;
                Data::GetValue( value, temp );
                dyn_array_t->Insert( at, temp );
            }
            
            virtual void Remove(void* dyn_array, size_t at) const
            {
                DynArray<T> *dyn_array_t = static_cast<DynArray<T> *>(dyn_array);
                dyn_array_t->Remove(at);
            }
            
            virtual void Swap(void* dyn_array, size_t at_rhs, size_t at_lhs) const
            {
                DynArray<T> *dyn_array_t = static_cast<DynArray<T> *>(dyn_array);
                Helium::Swap(dyn_array_t->GetElement(at_lhs), dyn_array_t->GetElement(at_rhs));
            }
            
            virtual void Resize(void* dyn_array, size_t size) const
            {
                DynArray<T> *dyn_array_t = static_cast<DynArray<T> *>(dyn_array);
                dyn_array_t->Resize(size);
            }
                        
            virtual bool Set( void* dyn_array, void* src, uint32_t flags ) const
            {
                DynArray<T> *dyn_array_t = static_cast<DynArray<T> *>(dyn_array);
                DynArray<T> *src_t = static_cast<DynArray<T> *>(src);

                *dyn_array_t = *src_t;
                return true;
            }
            
            virtual bool Equals( void* dyn_array, void* src ) const
            {
                DynArray<T> *data = static_cast<DynArray<T> *>(dyn_array);
                DynArray<T> *rhs_data = static_cast<DynArray<T> *>(src);

                return *data == *rhs_data;
            }
            
            virtual bool Accept( const Composite *composite, void* dyn_array, Visitor& visitor ) const
            {
                DynArray<T> *data = static_cast<DynArray<T> *>(dyn_array);

                DynArray< T >::Iterator itr = data->Begin();
                DynArray< T >::Iterator end = data->End();
                for ( ; itr != end; ++itr )
                {
                    composite->Visit(&*itr, visitor);
                }

                return true;
            }
        };

        //
        // Structure (struct or class)
        //

        class HELIUM_FOUNDATION_API Structure : public Composite
        {
        public:
            REFLECTION_TYPE( ReflectionTypes::Structure, Structure, Composite );

        protected:
            Structure();
            ~Structure();

        public:
            // protect external allocation to keep inlined code in this dll
            static Structure* Create();

            template< class StructureT >
            static Structure* Create( const tchar_t* name, const tchar_t* baseName )
            {
                Structure* info = Structure::Create();

                // populate reflection information
                Composite::Create< StructureT >( name, baseName, &StructureT::PopulateComposite, info );

                info->m_Default = new StructureT;
                info->m_DynArrayAdapter.Reset(new StructureDynArrayAdapter<StructureT>());

                return info;
            }

            Helium::AutoPtr<IStructureDynArrayAdapter> m_DynArrayAdapter;
        };

        template< class ClassT, class BaseT >
        class StructureRegistrar : public TypeRegistrar
        {
        public:
            StructureRegistrar(const tchar_t* name);
            ~StructureRegistrar();

            virtual void Register();
            virtual void Unregister();
        };

        template< class ClassT >
        class StructureRegistrar< ClassT, void > : public TypeRegistrar
        {
        public:
            StructureRegistrar(const tchar_t* name);
            ~StructureRegistrar();

            virtual void Register();
            virtual void Unregister();
        };
    }
}

// declares type checking functions
#define _REFLECT_DECLARE_BASE_STRUCTURE( STRUCTURE ) \
public: \
typedef STRUCTURE This; \
static Helium::Reflect::Structure* CreateStructure(); \
static const Helium::Reflect::Structure* s_Structure; \
static Helium::Reflect::StructureRegistrar< STRUCTURE, void > s_Registrar;

#define _REFLECT_DECLARE_DERIVED_STRUCTURE( STRUCTURE, BASE ) \
public: \
typedef BASE Base; \
typedef STRUCTURE This; \
static Helium::Reflect::Structure* CreateStructure(); \
static const Helium::Reflect::Structure* s_Structure; \
static Helium::Reflect::StructureRegistrar< STRUCTURE, BASE > s_Registrar;

// defines the static type info vars
#define _REFLECT_DEFINE_BASE_STRUCTURE( STRUCTURE ) \
Helium::Reflect::Structure* STRUCTURE::CreateStructure() \
{ \
    HELIUM_ASSERT( s_Structure == NULL ); \
    Helium::Reflect::Structure* type = Helium::Reflect::Structure::Create<STRUCTURE>( TXT( #STRUCTURE ), NULL ); \
    s_Structure = type; \
    return type; \
} \
const Helium::Reflect::Structure* STRUCTURE::s_Structure = NULL; \
Helium::Reflect::StructureRegistrar< STRUCTURE, void > STRUCTURE::s_Registrar( TXT( #STRUCTURE ) );

#define _REFLECT_DEFINE_DERIVED_STRUCTURE( STRUCTURE ) \
Helium::Reflect::Structure* STRUCTURE::CreateStructure() \
{ \
    HELIUM_ASSERT( s_Structure == NULL ); \
    HELIUM_ASSERT( STRUCTURE::Base::s_Structure != NULL ); \
    Helium::Reflect::Structure* type = Helium::Reflect::Structure::Create<STRUCTURE>( TXT( #STRUCTURE ), STRUCTURE::Base::s_Structure->m_Name ); \
    s_Structure = type; \
    return type; \
} \
const Helium::Reflect::Structure* STRUCTURE::s_Structure = NULL; \
Helium::Reflect::StructureRegistrar< STRUCTURE, BASE > STRUCTURE::s_Registrar( TXT( #STRUCTURE ) );

// declares a concrete object with creator
#define REFLECT_DECLARE_BASE_STRUCTURE( STRUCTURE ) \
    _REFLECT_DECLARE_BASE_STRUCTURE( STRUCTURE )

#define REFLECT_DECLARE_DERIVED_STRUCTURE( STRUCTURE, BASE ) \
    _REFLECT_DECLARE_DERIVED_STRUCTURE( STRUCTURE, BASE )

// defines a concrete object
#define REFLECT_DEFINE_BASE_STRUCTURE( STRUCTURE ) \
    _REFLECT_DEFINE_BASE_STRUCTURE( STRUCTURE )

#define REFLECT_DEFINE_DERIVED_STRUCTURE( STRUCTURE ) \
    _REFLECT_DEFINE_DERIVED_STRUCTURE( STRUCTURE  )

#include "Foundation/Reflect/Structure.inl"
