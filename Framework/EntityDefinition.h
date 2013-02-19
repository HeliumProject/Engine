//----------------------------------------------------------------------------------------------------------------------
// EntityDefinition.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_ENTITY_H
#define HELIUM_FRAMEWORK_ENTITY_H

#include "Framework/Framework.h"
#include "Engine/GameObject.h"

#include "MathSimd/Vector3.h"
#include "MathSimd/Quat.h"

#include "Framework/ComponentDefinitionSet.h"

namespace Helium
{
    class World;

    class Slice;
    typedef Helium::WeakPtr< Slice > SliceWPtr;
    typedef Helium::WeakPtr< const Slice > ConstSliceWPtr;
    
    class SliceDefinition;
    typedef Helium::WeakPtr< SliceDefinition > SliceDefinitionWPtr;
    typedef Helium::WeakPtr< const SliceDefinition > ConstSliceDefinitionWPtr;

    class World;
    typedef Helium::WeakPtr< World > WorldWPtr;
    typedef Helium::WeakPtr< const World > ConstWorldWPtr;

    class Entity;
    typedef Helium::StrongPtr< Entity > EntityPtr;

    /// Base type for in-world entities.
    class HELIUM_FRAMEWORK_API EntityDefinition : public GameObject
    {
        HELIUM_DECLARE_OBJECT( EntityDefinition, GameObject );

    public:

        /// @name Construction/Destruction
        //@{
        EntityDefinition();
        virtual ~EntityDefinition();
        //@}

        //@{
        inline const Simd::Vector3& GetPosition() const;
        virtual void SetPosition( const Simd::Vector3& rPosition );

        inline const Simd::Quat& GetRotation() const;
        virtual void SetRotation( const Simd::Quat& rRotation );

        inline const Simd::Vector3& GetScale() const;
        virtual void SetScale( const Simd::Vector3& rScale );
        //@}

        /// @name SliceDefinition Registration
        //@{
        inline const SliceDefinitionWPtr& GetSlice() const;
        inline size_t GetSliceIndex() const;
        void SetSliceInfo( SliceDefinition* pSlice, size_t sliceIndex );
        void SetSliceIndex( size_t sliceIndex );
        void ClearSliceInfo();
        //@}

        EntityPtr CreateEntity();
    protected:

    private:
        /// EntityDefinition position.
        Simd::Vector3 m_position;
        /// EntityDefinition rotation.
        Simd::Quat m_rotation;
        /// EntityDefinition scale.
        Simd::Vector3 m_scale;

        /// EntityDefinition slice.
        SliceDefinitionWPtr m_spSlice;
        /// Runtime index for the entity within its slice.
        size_t m_sliceIndex;

        ComponentDefinitionSetPtr m_ComponentDefinitions;
    };
    typedef Helium::StrongPtr<EntityDefinition> EntityDefinitionPtr;

    class HELIUM_FRAMEWORK_API Entity : public Reflect::Object
    {
    public:
        REFLECT_DECLARE_OBJECT(Helium::Entity, Helium::Reflect::Object);
        static void PopulateComposite( Reflect::Composite& comp );

        
        ~Entity()
        {
            Helium::Components::RemoveAllComponents(m_Components);
        }

        template <class T>
        T*  Allocate()
        {
            return Helium::Components::Allocate<T>(m_Components);
        }

        template <class T>
        T*  FindOneComponent()
        {
            return Helium::Components::FindOneComponent<T>(m_Components);
        }

        template <class T>
        T*  FindOneComponentThatImplements()
        {
            return Helium::Components::FindOneComponentThatImplements<T>(m_Components);
        }

        template <class T>
        void FindAllComponents(DynamicArray<T *> &_components)
        {
            Helium::Components::FindAllComponents<T>(m_Components, _components);
        }

        template <class T>
        void FindAllComponentsThatImplement(DynamicArray<T *> &_components)
        {
            Helium::Components::FindAllComponentsThatImplement<T>(m_Components, _components);
        }

        void DeployComponents(Helium::ComponentDefinitionSet &_components, ParameterSet &_parameters)
        {
            Helium::Components::DeployComponents(_components, _parameters, m_Components);
        }
        
        /// @name SliceDefinition Registration
        //@{
        inline const SliceWPtr& GetSlice() const;
        inline size_t GetSliceIndex() const;
        void SetSliceInfo( Slice* pSlice, size_t sliceIndex );
        void SetSliceIndex( size_t sliceIndex );
        void ClearSliceInfo();

        WorldWPtr GetWorld() const;
        //@}

        virtual void PreUpdate(float dt);

    private:
        Helium::Components::ComponentSet m_Components;
        
        /// EntityDefinition slice.
        SliceWPtr m_spSlice;
        /// Runtime index for the entity within its slice.
        size_t m_sliceIndex;
        
    };
    typedef Helium::StrongPtr<Entity> EntityPtr;
}

#include "Framework/EntityDefinition.inl"

#endif  // HELIUM_FRAMEWORK_ENTITY_H
