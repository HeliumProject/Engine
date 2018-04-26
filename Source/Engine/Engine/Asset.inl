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
		return ( pObject && pObject->IsA( TargetType::GetStaticType() )
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
		HELIUM_ASSERT( !pObject || pObject->IsA( TargetType::GetStaticType() ) );

		return static_cast< TargetType* >( pObject );
	}
}

/// Get the name of this object.
///
/// @return  Object name.
///
/// @see GetOwner(), GetInstanceIndex(), Rename()
Helium::Name Helium::Asset::GetName() const
{
	return m_name;
}

/// Get the owner of this object.
///
/// @return  Object owner.
///
/// @see GetName(), GetInstanceIndex(), Rename(), GetChildCount(), GetChild(), GetChildren()
Helium::Asset* Helium::Asset::GetOwner() const
{
	return m_spOwner;
}

/// Get the package that owns this object, or null if it is "loose"
///
/// @return  Owning package.
///
/// @see GetOwner()
Helium::Package* Helium::Asset::GetOwningPackage()
{
	Asset *pAsset = this;
	while (pAsset && !pAsset->IsPackage())
	{
		pAsset = pAsset->GetOwner();
	}

	return Reflect::AssertCast< Package >(pAsset);
}

/// Get the instance index associated with this object.
///
/// @return  Object instance index.
///
/// @see GetName(), GetOwner(), Rename()
uint32_t Helium::Asset::GetInstanceIndex() const
{
	return m_instanceIndex;
}

/// Get the unique ID for this object.
///
/// @return  Object ID.
uint32_t Helium::Asset::GetId() const
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
uint32_t Helium::Asset::GetFlags() const
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
bool Helium::Asset::GetAnyFlagSet( uint32_t flagMask ) const
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
bool Helium::Asset::GetAllFlagsSet( uint32_t flagMask ) const
{
	HELIUM_ASSERT( flagMask != 0 );

	return ( ( m_flags & flagMask ) == flagMask );
}

/// Get the first object in the list of objects of which this object is the immediate parent.
///
/// @return  First object in the child object list.
///
/// @see GetNextSibling()
const Helium::AssetWPtr& Helium::Asset::GetFirstChild() const
{
	return m_wpFirstChild;
}

/// Get the next object in the list of objects who share the same parent as this object.
///
/// @return  Next sibling object in list of child objects for this object's parent.
///
/// @see GetFirstChild()
const Helium::AssetWPtr& Helium::Asset::GetNextSibling() const
{
	return m_wpNextSibling;
}

/// Get the full path name for this object.
///
/// @return  Object path name.
Helium::AssetPath Helium::Asset::GetPath() const
{
	return m_path;
}

/// Get whether this object is fully loaded and ready for use.
///
/// @return  True if this object is fully loaded, false if not.
bool Helium::Asset::IsFullyLoaded() const
{
	return GetAllFlagsSet( FLAG_PRELOADED | FLAG_LINKED | FLAG_LOADED );
}

/// Get whether this object is the default template object for its type.
///
/// This uses the FLAG_DEFAULT_TEMPLATE flag to determine whether this object is the default template for the type.
///
/// @return  True if this object is the default type template, false if not.
bool Helium::Asset::IsDefaultTemplate() const
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
bool Helium::Asset::IsPackage() const
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
bool Helium::Asset::IsInstanceOf( const AssetType* pType ) const
{
	const AssetType* pThisType = GetAssetType();
	HELIUM_ASSERT( pThisType );

	return ( pThisType == pType );
}

/// Call FinalizeLoad() on this object and set the FLAG_LOADED flag if it is not set.
///
/// @see FinalizeLoad()
void Helium::Asset::ConditionalFinalizeLoad()
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
bool Helium::Asset::Create(
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
T* Helium::Asset::Find( AssetPath path )
{
	Asset* pObject = FindObject( path );
	if( pObject )
	{
		const AssetType* pType = T::GetStaticType();
		HELIUM_ASSERT( pType );
		if( !pObject->IsA( pType->GetMetaClass() ) )
		{
			pObject = NULL;
		}
	}

	return static_cast< T* >( pObject );
}

/// Constructor.
Helium::Asset::RenameParameters::RenameParameters()
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
TargetType* Helium::DynamicCast( SourceType* pObject )
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
TargetType* Helium::StaticCast( SourceType* pObject )
{
	return _StaticCast< TargetType, SourceType >( pObject, std::is_base_of< TargetType, SourceType >() );
}

template< class ClassT, class BaseT >
Helium::AssetRegistrar< ClassT, BaseT >::AssetRegistrar( const char* name )
	: Reflect::MetaClassRegistrar< ClassT, BaseT >(name)
{

}

template< class ClassT, class BaseT >
void Helium::AssetRegistrar< ClassT, BaseT >::Register()
{
	if ( ClassT::s_MetaClass == NULL )
	{
		Reflect::MetaClassRegistrar< ClassT, BaseT >::Register();
		ClassT::InitStaticType();
	}
}

template< class ClassT, class BaseT >
void Helium::AssetRegistrar< ClassT, BaseT >::Unregister()
{
	if ( ClassT::s_MetaClass != NULL )
	{
		ClassT::ReleaseStaticType();
		Reflect::MetaClassRegistrar< ClassT, BaseT >::Unregister();
	}
}

template< class ClassT >
Helium::AssetRegistrar< ClassT, void >::AssetRegistrar( const char* name )
	: Reflect::MetaClassRegistrar< ClassT, Reflect::Object >(name)
{

}

template< class ClassT >
void Helium::AssetRegistrar< ClassT, void >::Register()
{
	if ( ClassT::s_MetaClass == NULL )
	{
		Reflect::MetaClassRegistrar< ClassT, Reflect::Object >::Register();
		ClassT::InitStaticType();
	}
}

template< class ClassT >
void Helium::AssetRegistrar< ClassT, void >::Unregister()
{
	if ( ClassT::s_MetaClass != NULL )
	{
		ClassT::ReleaseStaticType();
		Reflect::MetaClassRegistrar< ClassT, Reflect::Object >::Unregister();
	}
}

/// Get the name of this type.
///
/// @return  Type name.
Helium::Name Helium::AssetType::GetName() const
{
	return m_name;
}

/// Get the name of this type.
///
/// @return  Type name.
const Helium::Reflect::MetaClass* Helium::AssetType::GetMetaClass() const
{
	return m_class;
}

/// Get the parent of this type, cast to a AssetType.
///
/// @return  Base type, or null if the parent is not a AssetType type (should only be the case with the
///          "Asset" type itself).
const Helium::AssetType* Helium::AssetType::GetBaseType() const
{
	const Reflect::MetaStruct* pBase = m_class->m_Base;
	HELIUM_ASSERT( pBase );
	return static_cast< const AssetType* >( pBase->m_Tag );
}

/// Get the flags associated with this type.
///
/// @return  Type flags.
uint32_t Helium::AssetType::GetFlags() const
{
	return m_flags;
}

/// Get the package in which all template object packages are stored.
///
/// @return  Main type package.
///
/// @see SetTypePackage()
Helium::Package* Helium::AssetType::GetTypePackage()
{
	return sm_spTypePackage;
}

/// Constructor.
///
/// Creates an uninitialized iterator.  Using this is not safe until it is initialized.
Helium::AssetType::ConstIterator::ConstIterator()
{
}

/// Constructor.
///
/// @param[in] iterator  Type map iterator from which to initialize this iterator.
Helium::AssetType::ConstIterator::ConstIterator( LookupMap::ConstIterator iterator )
	: m_iterator( iterator )
{
}

/// Get the type referenced by this iterator.
///
/// @return  Reference to the referenced type.
const Helium::AssetType& Helium::AssetType::ConstIterator::operator*() const
{
	AssetType* pType = m_iterator->Second();
	HELIUM_ASSERT( pType );

	return *pType;
}

/// Get the type referenced by this iterator.
///
/// @return  Pointer to the referenced type.
const Helium::AssetType* Helium::AssetType::ConstIterator::operator->() const
{
	AssetType* pType = m_iterator->Second();
	HELIUM_ASSERT( pType );

	return pType;
}

/// Advance this iterator to the next type.
///
/// @return  Reference to this iterator.
Helium::AssetType::ConstIterator& Helium::AssetType::ConstIterator::operator++()
{
	++m_iterator;

	return *this;
}

/// Advance this iterator to the next type.
///
/// @return  Copy of this iterator prior to advancing.
Helium::AssetType::ConstIterator Helium::AssetType::ConstIterator::operator++( int )
{
	ConstIterator result = *this;
	++m_iterator;

	return result;
}

/// Move this iterator back to the previous type.
///
/// @return  Reference to this iterator.
Helium::AssetType::ConstIterator& Helium::AssetType::ConstIterator::operator--()
{
	--m_iterator;

	return *this;
}

/// Move this iterator back to the previous type.
///
/// @return  Copy of this iterator prior to decrementing.
Helium::AssetType::ConstIterator Helium::AssetType::ConstIterator::operator--( int )
{
	ConstIterator result = *this;
	--m_iterator;

	return result;
}

/// Get whether this iterator is referencing the same type entry as the given iterator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator and the given iterator match, false if not.
bool Helium::AssetType::ConstIterator::operator==( const ConstIterator& rOther ) const
{
	return ( m_iterator == rOther.m_iterator );
}

/// Get whether this iterator is not referencing the same type entry as the given iterator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator and the given iterator do not match, false if they do match.
bool Helium::AssetType::ConstIterator::operator!=( const ConstIterator& rOther ) const
{
	return ( m_iterator != rOther.m_iterator );
}

/// Get whether this iterator is referencing a type entry prior to the given iterator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator is referencing a type entry prior to the given iterator, false if not.
bool Helium::AssetType::ConstIterator::operator<( const ConstIterator& rOther ) const
{
	return ( m_iterator < rOther.m_iterator );
}

/// Get whether this iterator is referencing a type entry after the given iterator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator is referencing a type entry after the given iterator, false if not.
bool Helium::AssetType::ConstIterator::operator>( const ConstIterator& rOther ) const
{
	return ( m_iterator > rOther.m_iterator );
}

/// Get whether this iterator is referencing a type entry prior to or the same as the given iterator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator is referencing the same or a prior type entry, false if not.
bool Helium::AssetType::ConstIterator::operator<=( const ConstIterator& rOther ) const
{
	return ( m_iterator <= rOther.m_iterator );
}

/// Get whether this iterator is referencing a type entry after or the same as the given iterator.
///
/// @param[in] rOther  Iterator with which to compare.
///
/// @return  True if this iterator is referencing the same or a later type entry, false if not.
bool Helium::AssetType::ConstIterator::operator>=( const ConstIterator& rOther ) const
{
	return ( m_iterator >= rOther.m_iterator );
}

/// Get the loader associated with this package.
///
/// @return  Package loader.
///
/// @see SetLoader()
Helium::PackageLoader* Helium::Package::GetLoader() const
{
	return m_pLoader;
}