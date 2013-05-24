//----------------------------------------------------------------------------------------------------------------------
// FrameworkInterface.h
//
// Copyright (C) 2012 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#pragma once
#ifndef HELIUM_FRAMEWORK_FRAMEWORK_INTERFACE_H
#define HELIUM_FRAMEWORK_FRAMEWORK_INTERFACE_H

#include "Framework/Framework.h"

#include "Engine/JobBase.h"

namespace Helium
{
    class JobContext;
}

namespace Helium
{
    class Entity;
    class World;
    typedef Helium::StrongPtr< World > WorldPtr;
    typedef Helium::StrongPtr< const World > ConstWorldPtr;
}

namespace Helium
{
    struct WorldManagerUpdateParameters
    {
        /// [in] Array of worlds to update.
        const WorldPtr* pspWorlds;
        /// [in] Number of worlds in the given array.
        size_t worldCount;
        /// [in] Index of the slice from which to start spawning update jobs.
        size_t startSliceIndex;
        /// [in] Index of the entity from which to start spawning update jobs.
        size_t startEntityIndex;

        /// @name Construction/Destruction
        //@{
        inline WorldManagerUpdateParameters()
            : startSliceIndex(0),
              startEntityIndex(0)
        {
        }
        //@}
    };

    struct WorldManagerUpdateParameters_PreUpdate : public WorldManagerUpdateParameters
    {

    };

    struct WorldManagerUpdateParameters_PostUpdate : public WorldManagerUpdateParameters
    {

    };

    /// WorldManager update job launcher.
    //template< typename EntityUpdateJobType >
    typedef Helium::JobBase< WorldManagerUpdateParameters_PreUpdate > WorldManagerPreUpdate;
    typedef Helium::JobBase< WorldManagerUpdateParameters_PostUpdate > WorldManagerPostUpdate;

    struct EntityPreUpdateParameters
    {
    public:
        /// [in] EntityDefinition to update.
        Entity* pEntity;

        /// @name Construction/Destruction
        //@{
        inline EntityPreUpdateParameters() { }
        //@}
    };

    struct EntityPreUpdate : public Helium::JobBase< EntityPreUpdateParameters >
    {
        void Run( JobContext* pContext );
        inline static void RunCallback( void* pJob, JobContext* pContext )
        {
            HELIUM_ASSERT( pJob );
            HELIUM_ASSERT( pContext );
            static_cast< EntityPreUpdate* >( pJob )->Run( pContext );
        }
    };
    
    struct EntityPostUpdateParameters
    {
    public:
        /// [in] EntityDefinition to update.
        Entity* pEntity;

        /// @name Construction/Destruction
        //@{
        inline EntityPostUpdateParameters();
        //@}
    };

    struct EntityPostUpdate : public Helium::JobBase< EntityPostUpdateParameters >
    {
        inline void Run( JobContext* pContext );
        inline static void RunCallback( void* pJob, JobContext* pContext )
        {
            HELIUM_ASSERT( pJob );
            HELIUM_ASSERT( pContext );
            static_cast< EntityPostUpdate* >( pJob )->Run( pContext );
        }
    };

}  // namespace Helium

#include "Framework/FrameworkInterface.inl"
#include "Framework/WorldManagerUpdate.inl"

#endif  // HELIUM_FRAMEWORK_FRAMEWORK_INTERFACE_H
