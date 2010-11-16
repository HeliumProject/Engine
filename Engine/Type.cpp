//----------------------------------------------------------------------------------------------------------------------
// Type.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "EnginePch.h"
#include "Engine/Type.h"

namespace Lunar
{
    L_IMPLEMENT_OBJECT_NOINITTYPE( Type, Engine );

    PackagePtr Type::sm_spTypePackage;
    Type::LookupMap* Type::sm_pLookupMap = NULL;

    /// Constructor.
    Type::Type()
        : m_typeFlags( 0 )
    {
    }

    /// Destructor.
    Type::~Type()
    {
    }

    /// Initialize this type.
    ///
    /// @param[in] pParent    Parent type.
    /// @param[in] pTemplate  Template object.
    /// @param[in] flags      Type flags.
    ///
    /// @return  True if initialization was successful, false if initialization failed or if this type has already been
    ///          initialized.
    bool Type::Initialize( Type* pParent, Object* pTemplate, uint32_t flags )
    {
        HELIUM_ASSERT( pTemplate );

        // Make sure we haven't already been initialized.
        if( m_spTypeTemplate )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "Type::Initialize(): Type \"%s\" has already been initialized.\n" ),
                GetName().Get() );

            return false;
        }

        // Set up the template object name, and set this object as its parent.
        if( !pTemplate->SetOwner( this ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "Type::Initialize(): Failed to set type \"%s\" template object owner.\n" ),
                GetName().Get() );

            return false;
        }

        static Name nameTemplate( TXT( "Template" ) );
        if( !pTemplate->SetName( nameTemplate ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "Type::Initialize(): Failed to set type \"%s\" template object name.\n" ),
                GetName().Get() );

            return false;
        }

        // Register the template object with the object system.
        if( !Object::RegisterObject( pTemplate ) )
        {
            HELIUM_TRACE(
                TRACE_ERROR,
                TXT( "Type::Initialize(): Failed to register type \"%s\" template object.\n" ),
                GetName().Get() );

            return false;
        }

        // Store the type parameters.
        m_spTypeParent = pParent;
        m_spTypeTemplate = pTemplate;

        m_typeFlags = flags;

        return true;
    }

    /// Get whether this type is a subtype of the given type.
    ///
    /// @param[in] pType  Type against which to check.
    bool Type::IsSubtypeOf( const Type* pType ) const
    {
        HELIUM_ASSERT( pType );

        for( const Type* pThisType = this; pThisType != NULL; pThisType = pThisType->GetTypeParent() )
        {
            if( pThisType == pType )
            {
                return true;
            }
        }

        return false;
    }

    /// @copydoc Object::Serialize()
    void Type::Serialize( Serializer& s )
    {
        L_SERIALIZE_SUPER( s );

        s << Serializer::Tag( TXT( "TypeFlags" ) ) << m_typeFlags;
        s << Serializer::Tag( TXT( "TypeTemplate" ) ) << m_spTypeTemplate;
    }

    /// Get the package in which all Type object packages are stored.
    ///
    /// @param[in] pPackage  Main type package.
    ///
    /// @see GetTypePackage()
    void Type::SetTypePackage( Package* pPackage )
    {
        HELIUM_ASSERT( pPackage );

        // Only allow the type package to be set once.
        HELIUM_ASSERT( !sm_spTypePackage );

        sm_spTypePackage = pPackage;
    }

    /// Register a type.
    ///
    /// @param[in] pType  Type to register.
    ///
    /// @return  Pointer to the registered type.  If the type was previously registered, then this will point to the
    ///          type that was originally registered, otherwise it will point to the type given.
    ///
    /// @see Unregister()
    Type* Type::Register( Type* pType )
    {
        HELIUM_ASSERT( pType );

        // Lazily initialize the lookup map.  Note that this is not inherently thread-safe, but there should always be
        // at least one type registered before any sub-threads are spawned.
        if( !sm_pLookupMap )
        {
            sm_pLookupMap = new LookupMap;
            HELIUM_ASSERT( sm_pLookupMap );
        }

        // Register the type.
        LookupMap::Accessor typeAccessor;
        if( !sm_pLookupMap->Insert( typeAccessor, std::pair< const Name, Type* >( pType->GetName(), pType ) ) )
        {
            // Type already exists in the map, so retrieve it.
            pType = typeAccessor->second;
            HELIUM_ASSERT( pType );
        }

        return pType;
    }

    /// Unregister a type.
    ///
    /// @param[in] pType  Type to unregister.  References to the parent type and the type template will be released as
    ///                   well.
    ///
    /// @see Register()
    void Type::Unregister( Type* pType )
    {
        HELIUM_ASSERT( pType );

        HELIUM_ASSERT( sm_pLookupMap );
        HELIUM_VERIFY( sm_pLookupMap->Remove( pType->GetName() ) );

        pType->m_spTypeParent.Release();
        pType->m_spTypeTemplate.Release();
    }

    /// Look up a type by name.
    ///
    /// @param[in] typeName  Name of the type to look up.
    ///
    /// @return  Pointer to the specified type if found, null pointer if not found.
    Type* Type::Find( Name typeName )
    {
        Type* pType = NULL;
        if( sm_pLookupMap )
        {
            LookupMap::ConstAccessor typeAccessor;
            if( sm_pLookupMap->Find( typeAccessor, typeName ) )
            {
                pType = typeAccessor->second;
                HELIUM_ASSERT( pType );
            }
        }

        return pType;
    }

    /// Look up a type by its full path name.
    ///
    /// @param[in] typePath  Full path of the type to look up.
    ///
    /// @return  Pointer to the specified type if found, null pointer if not found.
    Type* Type::Find( ObjectPath typePath )
    {
        Type* pType = Find( typePath.GetName() );

        return ( pType && pType->GetPath() == typePath ? pType : NULL );
    }

    /// Begin iterating on all the registered types.
    ///
    /// @param[out] rIterator  Accessor set to reference the first registered type.
    ///
    /// @return  True if the iterator was set to the first type, false if no types are registered.
    bool Type::GetFirstType( ConstIterator& rIterator )
    {
        if( !sm_pLookupMap )
        {
            rIterator.m_accessor.Release();

            return false;
        }

        return sm_pLookupMap->First( rIterator.m_accessor );
    }

    /// Perform shutdown of the Type registration system.
    ///
    /// This releases all final references to objects and releases all allocated memory.  This should be called during
    /// the shutdown process prior to calling Object::Shutdown().
    ///
    /// @see Object::Shutdown()
    void Type::Shutdown()
    {
        HELIUM_TRACE( TRACE_INFO, TXT( "Shutting down Type registration.\n" ) );

        // Make sure the Object type is unregistered, as it does not get included in the unregistration of the Engine
        // type package.
        Object::ReleaseStaticType();

        delete sm_pLookupMap;
        sm_pLookupMap = NULL;

        // Release the reference to the main "Types" package.
        sm_spTypePackage.Release();

        HELIUM_TRACE( TRACE_INFO, TXT( "Type registration shutdown complete.\n" ) );
    }

    /// Initialize the static type information for the "Type" class.
    ///
    /// @return  Static "Type" type.
    Type* Type::InitStaticType()
    {
        Type* pType = sm_spStaticType;
        if( !pType )
        {
            // Type type is registered manually during Object type initialization, so retrieve the type info from the
            // existing registered data.
            HELIUM_VERIFY( Object::InitStaticType() );

            pType = Type::Find( Name( TXT( "Type" ) ) );
            HELIUM_ASSERT( pType );

            sm_spStaticType = pType;
            sm_spStaticTypeTemplate = static_cast< Type* >( pType->GetTypeTemplate() );
            HELIUM_ASSERT( sm_spStaticTypeTemplate );
        }

        return pType;
    }
}
