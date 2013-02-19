
namespace Helium
{
    void Entity::DeployComponents( ComponentDefinitionSet &_components, ParameterSet &_parameters )
    {
        Components::DeployComponents(_components, _parameters, m_Components);
    }

    template <class T>
    void Entity::FindAllComponentsThatImplement( DynamicArray<T *> &_components )
    {
        Components::FindAllComponentsThatImplement<T>(m_Components, _components);
    }

    template <class T>
    void Entity::FindAllComponents( DynamicArray<T *> &_components )
    {
        Components::FindAllComponents<T>(m_Components, _components);
    }

    template <class T>
    T* Entity::FindOneComponentThatImplements()
    {
        return Components::FindOneComponentThatImplements<T>(m_Components);
    }

    template <class T>
    T* Entity::FindOneComponent()
    {
        return Components::FindOneComponent<T>(m_Components);
    }

    template <class T>
    T* Entity::Allocate()
    {
        return Components::Allocate<T>(m_Components);
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