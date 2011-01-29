#pragma once

#include "Composite.h"

namespace Helium
{
    namespace Reflect
    {
        //
        // Structure (struct or class)
        //

        class FOUNDATION_API Structure : public Composite
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

                return info;
            }
        };
    }
}

// declares type checking functions
#define _REFLECT_DECLARE_BASE_STRUCTURE( STRUCTURE ) \
public: \
typedef STRUCTURE This; \
static Helium::Reflect::Structure* CreateStructure( const tchar_t* name ); \
static const Helium::Reflect::Structure* s_Structure;

#define _REFLECT_DECLARE_DERIVED_STRUCTURE( STRUCTURE, BASE ) \
public: \
typedef BASE Base; \
typedef STRUCTURE This; \
static Helium::Reflect::Structure* CreateStructure( const tchar_t* name ); \
static const Helium::Reflect::Structure* s_Structure;

// defines the static type info vars
#define _REFLECT_DEFINE_BASE_STRUCTURE( STRUCTURE ) \
Helium::Reflect::Structure* STRUCTURE::CreateStructure( const tchar_t* name ) \
{ \
    HELIUM_ASSERT( s_Structure == NULL ); \
    Helium::Reflect::Structure* type = Helium::Reflect::Structure::Create<STRUCTURE>(name, NULL); \
    s_Structure = type; \
    return type; \
} \
const Helium::Reflect::Structure* STRUCTURE::s_Structure = NULL;

#define _REFLECT_DEFINE_DERIVED_STRUCTURE( STRUCTURE ) \
Helium::Reflect::Structure* STRUCTURE::CreateStructure( const tchar_t* name ) \
{ \
    HELIUM_ASSERT( s_Structure == NULL ); \
    HELIUM_ASSERT( STRUCTURE::Base::s_Structure != NULL ); \
    Helium::Reflect::Structure* type = Helium::Reflect::Structure::Create<STRUCTURE>(name, STRUCTURE::Base::s_Structure->m_Name); \
    s_Structure = type; \
    return type; \
} \
const Helium::Reflect::Structure* STRUCTURE::s_Structure = NULL;

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
