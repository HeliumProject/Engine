
namespace Helium
{
	ComponentCollection & Helium::Entity::GetComponents()
	{
		return m_Components;
	}

	void Entity::DeployComponents( const ComponentDefinitionSet &_components, const ParameterSet &_parameters )
	{
		Components::DeployComponents(*this, _components, _parameters);
	}

	template <class T>
	T* Entity::GetFirst()
	{
		m_Components.GetFirst<T>();
	}

	template <class T>
	T* Entity::Allocate()
	{
		return this->VirtualGetComponentManager()->Allocate<T>(this, m_Components);
	}

	/// Get the slice to which this entity is currently bound.
	///
	/// @return  EntityDefinition slice.
	///
	/// @see GetSliceIndex(), SetSliceInfo(), SetSliceIndex(), ClearSliceInfo()
	const SliceWPtr& Entity::GetSlice() const
	{
		return m_spSlice;
	}

	/// Get the index of this entity in the slice to which it is currently bound.
	///
	/// Note that this index may change at runtime, so it should not be used to track an entity.
	///
	/// @return  EntityDefinition index within its slice.
	///
	/// @see GetSlice(), SetSliceInfo(), SetSliceIndex(), ClearSliceInfo()
	size_t Entity::GetSliceIndex() const
	{
		return m_sliceIndex;
	}
}