#pragma once

#include "Framework/Framework.h"

#include "Framework/World.h"

namespace Helium
{
    class EntityDefinition;

    class World;
    typedef Helium::WeakPtr< World > WorldWPtr;
    typedef Helium::WeakPtr< const World > ConstWorldWPtr;
    
    class Slice;
    typedef Helium::StrongPtr< Slice > SlicePtr;
    typedef Helium::StrongPtr< const Slice > ConstSlicePtr;

    class Entity;
    typedef Helium::StrongPtr< Entity > EntityPtr;
    typedef Helium::StrongPtr< const Entity > ConstEntityPtr;

    class HELIUM_FRAMEWORK_API Slice : public Reflect::Object
    {
    public:
        HELIUM_DECLARE_CLASS(Helium::Slice, Reflect::Object);
        Slice();
        ~Slice();

        void Initialize(Helium::SceneDefinition *pSceneDefinition);

        /// @name EntityDefinition Creation
        //@{
		virtual Helium::Entity* CreateEntity(EntityDefinition *pEntityDefinition, ParameterSet *pParameterSet = 0);
        virtual bool DestroyEntity( Entity* pEntity );
        //@}

        /// @name EntityDefinition Access
        //@{
        inline size_t GetEntityCount() const;
        inline Entity* GetEntity( size_t index ) const;
        //@}

        /// @name World Registration
        //@{
        inline World *GetWorld();
        inline const World *GetWorld() const;
        inline size_t GetWorldIndex() const;
        void SetWorldInfo( World* pWorld, size_t worldIndex );
        void SetWorldIndex( size_t worldIndex );
        void ClearWorldInfo();
        //@}

        Helium::SceneDefinition *GetSceneDefinition() const { return m_spSceneDefinition; }

    private:
        Helium::SceneDefinitionPtr m_spSceneDefinition;

        /// Entities.
        DynamicArray< EntityPtr > m_entities;

        /// Slice world.
        WorldWPtr m_spWorld;
        /// Runtime index for the slice within its world.
        size_t m_worldIndex;
    };
}

#include "Framework/World.h"
#include "Framework/Entity.h"
#include "Framework/Slice.inl"
