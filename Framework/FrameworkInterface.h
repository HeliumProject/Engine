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
#include "Platform/Assert.h"

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

/// WorldManager update job launcher.
template< typename EntityUpdateJobType >
class WorldManagerUpdate : Helium::NonCopyable
{
public:
    class Parameters
    {
    public:
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
        inline Parameters();
        //@}
    };

    /// @name Construction/Destruction
    //@{
    inline WorldManagerUpdate();
    inline ~WorldManagerUpdate();
    //@}

    /// @name Parameters
    //@{
    inline Parameters& GetParameters();
    inline const Parameters& GetParameters() const;
    inline void SetParameters( const Parameters& rParameters );
    //@}

    /// @name Job Execution
    //@{
    void Run( JobContext* pContext );
    inline static void RunCallback( void* pJob, JobContext* pContext );
    //@}

private:
    Parameters m_parameters;
};

/// Read-only entity update (entity can only read data, can access other entities).
class HELIUM_FRAMEWORK_API EntityPreUpdate : Helium::NonCopyable
{
public:
    class Parameters
    {
    public:
        /// [in] Entity to update.
        Entity* pEntity;

        /// @name Construction/Destruction
        //@{
        inline Parameters();
        //@}
    };

    /// @name Construction/Destruction
    //@{
    inline EntityPreUpdate();
    inline ~EntityPreUpdate();
    //@}

    /// @name Parameters
    //@{
    inline Parameters& GetParameters();
    inline const Parameters& GetParameters() const;
    inline void SetParameters( const Parameters& rParameters );
    //@}

    /// @name Job Execution
    //@{
    void Run( JobContext* pContext );
    inline static void RunCallback( void* pJob, JobContext* pContext );
    //@}

private:
    Parameters m_parameters;
};

/// Entity resolve update (entity can only read and write its own data, cannot access other entities).
class HELIUM_FRAMEWORK_API EntityPostUpdate : Helium::NonCopyable
{
public:
    class Parameters
    {
    public:
        /// [in] Entity to update.
        Entity* pEntity;

        /// @name Construction/Destruction
        //@{
        inline Parameters();
        //@}
    };

    /// @name Construction/Destruction
    //@{
    inline EntityPostUpdate();
    inline ~EntityPostUpdate();
    //@}

    /// @name Parameters
    //@{
    inline Parameters& GetParameters();
    inline const Parameters& GetParameters() const;
    inline void SetParameters( const Parameters& rParameters );
    //@}

    /// @name Job Execution
    //@{
    void Run( JobContext* pContext );
    inline static void RunCallback( void* pJob, JobContext* pContext );
    //@}

private:
    Parameters m_parameters;
};

}  // namespace Helium

#include "Framework/FrameworkInterface.inl"
#include "Framework/WorldManagerUpdate.inl"

#endif  // HELIUM_FRAMEWORK_FRAMEWORK_INTERFACE_H
