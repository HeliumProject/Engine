//----------------------------------------------------------------------------------------------------------------------
// Asset.inl
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "AssetLoader.h"

namespace Helium
{
    /// DynamicCast() implementation for up-cast or same conversions.
    ///
    /// @param[in] pObject    Object to cast.
    /// @param[in] rIsUpcast  std::true_type.
    ///
    /// @return  Cast pointer if the conversion is valid, null if the object is not of the target type.
    template< typename TargetType, typename SourceType >
    TargetType* _DynamicCast( SourceType* pObject, const std::true_type& /*rIsUpcast*/ )
    {
        return pObject;
    }

    /// DynamicCast() implementation for down-cast or invalid conversions.
    ///
    /// @param[in] pObject    Object to cast.
    /// @param[in] rIsUpcast  std::false_type.
    ///
    /// @return  Cast pointer if the conversion is valid, null if the object is not of the target type.
    template< typename TargetType, typename SourceType >
    TargetType* _DynamicCast( SourceType* pObject, const std::false_type& /*rIsUpcast*/ )
    {
        return ( pObject && pObject->IsClass( TargetType::GetStaticType() )
                 ? static_cast< TargetType* >( pObject )
                 : NULL );
    }

    /// StaticCast() implementation for up-cast or same conversions.
    ///
    /// @param[in] pObject    Object to cast.
    /// @param[in] rIsUpcast  std::true_type.
    ///
    /// @return  Cast pointer.
    template< typename TargetType, typename SourceType >
    TargetType* _StaticCast( SourceType* pObject, const std::true_type& /*rIsUpcast*/ )
    {
        return pObject;
    }

    /// StaticCast() implementation for down-cast or invalid conversions.
    ///
    /// @param[in] pObject    Object to cast.
    /// @param[in] rIsUpcast  std::false_type.
    ///
    /// @return  Cast pointer.
    template< typename TargetType, typename SourceType >
    TargetType* _StaticCast( SourceType* pObject, const std::false_type& /*rIsUpcast*/ )
    {
        HELIUM_ASSERT( !pObject || pObject->IsClass( TargetType::GetStaticType() ) );

        return static_cast< TargetType* >( pObject );
    }

    /// Get the name of this object.
    ///
    /// @return  Object name.
    ///
    /// @see GetOwner(), GetInstanceIndex(), Rename()
    Name Asset::GetName() const
    {
        return m_name;
    }

    /// Get the owner of this object.
    ///
    /// @return  Object owner.
    ///
    /// @see GetName(), GetInstanceIndex(), Rename(), GetChildCount(), GetChild(), GetChildren()
    Asset* Asset::GetOwner() const
    {
        return m_spOwner;
    }

    /// Get the instance index associated with this object.
    ///
    /// @return  Object instance index.
    ///
    /// @see GetName(), GetOwner(), Rename()
    uint32_t Asset::GetInstanceIndex() const
    {
        return m_instanceIndex;
    }

    /// Get the unique ID for this object.
    ///
    /// @return  Object ID.
    uint32_t Asset::GetId() const
    {
        return m_id;
    }

    /// Get the object flags for this object.
    ///
    /// Note that all object flag functions are thread-safe.
    ///
    /// @return  Object flags.
    ///
    /// @see GetAnyFlagSet(), GetAllFlagsSet(), SetFlags(), ClearFlags(), ToggleFlags()
    uint32_t Asset::GetFlags() const
    {
        return m_flags;
    }

    /// Get whether any of the object flags covered by the given mask are set.
    ///
    /// Note that all object flag functions are thread-safe.
    ///
    /// @param[in] flagMask  Object flag bit mask.
    ///
    /// @return  True if any object flags in the given mask are set, false if not.
    ///
    /// @see GetFlags(), GetAllFlagsSet(), SetFlags(), ClearFlags(), ToggleFlags()
    bool Asset::GetAnyFlagSet( uint32_t flagMask ) const
    {
        HELIUM_ASSERT( flagMask != 0 );

        return ( ( m_flags & flagMask ) != 0 );
    }

    /// Get whether all of the object flags covered by the given mask are set.
    ///
    /// Note that all object flag functions are thread-safe.
    ///
    /// @param[in] flagMask  Object flag bit mask.
    ///
    /// @return  True if all the object flags in the given mask are set, false if not.
    ///
    /// @see GetFlags(), GetAnyFlagSet(), SetFlags(), ClearFlags(), ToggleFlags()
    bool Asset::GetAllFlagsSet( uint32_t flagMask ) const
    {
        HELIUM_ASSERT( flagMask != 0 );

        return ( ( m_flags & flagMask ) == flagMask );
    }

    /// Get the first object in the list of objects of which this object is the immediate parent.
    ///
    /// @return  First object in the child object list.
    ///
    /// @see GetNextSibling()
    const AssetWPtr& Asset::GetFirstChild() const
    {
        return m_wpFirstChild;
    }

    /// Get the next object in the list of objects who share the same parent as this object.
    ///
    /// @return  Next sibling object in list of child objects for this object's parent.
    ///
    /// @see GetFirstChild()
    const AssetWPtr& Asset::GetNextSibling() const
    {
        return m_wpNextSibling;
    }

    /// Get the full path name for this object.
    ///
    /// @return  Object path name.
    AssetPath Asset::GetPath() const
    {
        return m_path;
    }

    /// Get whether this object is fully loaded and ready for use.
    ///
    /// @return  True if this object is fully loaded, false if not.
    bool Asset::IsFullyLoaded() const
    {
        return GetAllFlagsSet( FLAG_PRELOADED | FLAG_LINKED | FLAG_LOADED );
    }

    /// Get whether this object is the default template object for its type.
    ///
    /// This uses the FLAG_DEFAULT_TEMPLATE flag to determine whether this object is the default template for the type.
    ///
    /// @return  True if this object is the default type template, false if not.
    bool Asset::IsDefaultTemplate() const
    {
        return GetAnyFlagSet( FLAG_DEFAULT_TEMPLATE );
    }

    /// Get whether this object is a package based on the object flags.
    ///
    /// Note that this only returns true for Package objects that are *not* the template object for the Package type, as
    /// it is not intended to actually be used or serialized as a package.
    ///
    /// @return  True if this is a package, false if not.
    ///
    /// @see Asset::FLAG_PACKAGE
    bool Asset::IsPackage() const
    {
        return GetAnyFlagSet( FLAG_PACKAGE );
    }

    /// Get whether this object is a specific instance of the specified type (not one of its subtypes).
    ///
    /// @param[in] pType  Type against which to test.
    ///
    /// @return  True if this is an instance of the given type, false if not.
    ///
    /// @see GetAssetType()
    bool Asset::IsInstanceOf( const AssetType* pType ) const
    {
        const AssetType* pThisType = GetAssetType();
        HELIUM_ASSERT( pThisType );

        return ( pThisType == pType );
    }

    /// Call FinalizeLoad() on this object and set the FLAG_LOADED flag if it is not set.
    ///
    /// @see FinalizeLoad()
    void Asset::ConditionalFinalizeLoad()
    {
        if( !GetAnyFlagSet( Asset::FLAG_LOADED ) )
        {
            FinalizeLoad();
            SetFlags( Asset::FLAG_LOADED );
        }
    }

    /// Create a new object.
    ///
    /// @param[out] rspObject             Pointer to the newly created object if object creation was successful.  Note
    ///                                   that any object reference stored in this strong pointer prior to calling this
    ///                                   function will always be cleared by this function, regardless of whether object
    ///                                   creation is successful.
    /// @param[in]  name                  Object name.
    /// @param[in]  pOwner                Object owner.
    /// @param[in]  pTemplate             Optional override template object.  If null, the default template for the
    ///                                   object type will be used.
    /// @param[in]  bAssignInstanceIndex  True to assign an instance index to the object, false to leave the index
    ///                                   invalid.
    ///
    /// @return  True if object creation was successful, false if not.
    ///
    /// @see Create()
    template< typename T >
    bool Asset::Create(
        StrongPtr< T >& rspObject,
        Name name,
        Asset* pOwner,
        T* pTemplate,
        bool bAssignInstanceIndex )
    {
        const AssetType* pType = T::GetStaticType();
        HELIUM_ASSERT( pType );

        AssetPtr spAsset;
        bool bResult = CreateObject( spAsset, pType, name, pOwner, pTemplate, bAssignInstanceIndex );

        rspObject = Reflect::AssertCast< T >( spAsset.Get() );

        return bResult;
    }

    /// Find an object based on its path name, filtering by a specific type.
    ///
    /// @param[in] path  FilePath of the object to locate.
    ///
    /// @return  Pointer to the object if found, null pointer if not found.
    template< typename T >
    T* Asset::Find( AssetPath path )
    {
        Asset* pObject = FindObject( path );
        if( pObject )
        {
            const AssetType* pType = T::GetStaticType();
            HELIUM_ASSERT( pType );
            if( !pObject->IsClass( pType->GetClass() ) )
            {
                pObject = NULL;
            }
        }

        return static_cast< T* >( pObject );
    }

    /// Constructor.
    Asset::RenameParameters::RenameParameters()
        : name( NULL_NAME )
        , instanceIndex( Invalid< uint32_t >() )
    {
    }

    /// Cast an object to a given type if the object is of that type.
    ///
    /// @param[in] pObject  Object to cast.
    ///
    /// @return  Cast pointer if the conversion is valid, null if the object is not of the target type.
    ///
    /// @see StaticCast()
    template< typename TargetType, typename SourceType >
    TargetType* DynamicCast( SourceType* pObject )
    {
        return _DynamicCast< TargetType, SourceType >( pObject, std::is_base_of< TargetType, SourceType >() );
    }

    /// Cast an object to a given type, only checking if the object is of that type and triggering an assert if
    /// assertions are enabled.
    ///
    /// If assertions are enabled, this will perform the check of whether the conversion is valid through an assertion
    /// only.  No checking is performed if assertions are disabled, and a null pointer is never returned if the cast is
    /// invalid.
    ///
    /// @param[in] pObject  Object to cast.
    ///
    /// @return  Cast pointer.
    ///
    /// @see DynamicCast()
    template< typename TargetType, typename SourceType >
    TargetType* StaticCast( SourceType* pObject )
    {
        return _StaticCast< TargetType, SourceType >( pObject, std::is_base_of< TargetType, SourceType >() );
    }
}
