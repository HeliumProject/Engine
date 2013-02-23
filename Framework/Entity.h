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
#include "Engine/Asset.h"

#include "Framework/ComponentDefinitionSet.h"
#include "Framework/Slice.h"

namespace Helium
{
    class Slice;
    typedef Helium::WeakPtr< Slice > SliceWPtr;
    typedef Helium::WeakPtr< const Slice > ConstSliceWPtr;
    
    class World;
    typedef Helium::WeakPtr< World > WorldWPtr;
    typedef Helium::WeakPtr< const World > ConstWorldWPtr;

    class HELIUM_FRAMEWORK_API Entity : public Reflect::Object
    {
    public:
        REFLECT_DECLARE_OBJECT(Helium::Entity, Helium::Reflect::Object);
        static void PopulateComposite( Reflect::Composite& comp );
        
        ~Entity();

		/// @name General Info
		//@{
		const AssetPath &GetDefinitionPath() { return m_DefinitionPath; }
		//@}

        /// @name Component Management
        //@{
        template <class T>  inline T*  Allocate();
        template <class T>  inline T*  FindOneComponent();
        template <class T>  inline T*  FindOneComponentThatImplements();
        template <class T>  inline void FindAllComponents(DynamicArray<T *> &_components);
        template <class T>  inline void FindAllComponentsThatImplement(DynamicArray<T *> &_components);
                            
        inline void DeployComponents(const Helium::ComponentDefinitionSet &_components, const ParameterSet &_parameters);
        //@}
        
        /// @name SceneDefinition Registration
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
		/// Path to creating definition. Not storing definition because we don't want to
		/// keep it allocated if we don't need to.
		AssetPath m_DefinitionPath;
        
    };
    typedef Helium::StrongPtr<Entity> EntityPtr;
}

#include "Framework/Entity.inl"

#endif  // HELIUM_FRAMEWORK_ENTITY_H
