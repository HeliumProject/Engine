#pragma once

#include "Foundation/API.h"

#include "Platform/Types.h"

#include <set>
#include <map>
#include <vector>

namespace Helium
{
    namespace PhaseIds
    {
        enum PhaseId
        {
            PreUpdate = 0,
            Update = 1,
            PostUpdate = 2,
            MaxPhaseId = 3,
        };
    }
    typedef PhaseIds::PhaseId PhaseId;

    typedef void (UpdateCallback)(double time);

    // Update Manager
    class FOUNDATION_API UpdateManager
    {
    public:
        class UpdateEntry
        {
        public:
            typedef std::set< uint64_t > DependencySet;

        private:
            UpdateCallback* m_UpdateCallback;
            DependencySet m_Dependencies;
            uint64_t m_Id;
            bool m_Called;
            bool m_Complete;

        public:            
            UpdateEntry()
                : m_Called( false )
                , m_Complete( false )
                , m_Id( 0 )
            {
            }

            void SetId( uint64_t id )
            {
                m_Id = id;
            }
            uint64_t GetId()
            {
                return m_Id;
            }

            void SetCallback( UpdateCallback* callback )
            {
                m_UpdateCallback = callback;
            }
            UpdateCallback* GetCallback()
            {
                return m_UpdateCallback;
            }

            void AddDependency( uint64_t dependencyId )
            {
                m_Dependencies.insert( dependencyId );
            }

            void RemoveDependency( uint64_t dependencyId )
            {
                m_Dependencies.erase( dependencyId );
            }

            DependencySet* GetDependencies()
            {
                return &m_Dependencies;
            }

            bool IsCalled()
            {
                return m_Called;
            }

            bool IsComplete()
            {
                return m_Complete;
            }

            void Call()
            {
                m_Called = true;
                m_UpdateCallback( 0.0f );
                m_Complete = true;
            }

            void Reset()
            {
                m_Called = false;
                m_Complete = false;
            }

        };

        typedef std::map< uint64_t, UpdateEntry* > UpdateEntryMap;
        typedef std::map< uint32_t, UpdateEntryMap > PhaseMap;

    public:

        UpdateManager();
        ~UpdateManager();

        /// @name Initialization
        //@{
        void Initialize();
        void Shutdown();
        //@}

        /// @name Static Access
        //@{
        static UpdateManager& GetStaticInstance();
        static void DestroyStaticInstance();
        //@}

        void Start();
        void Stop();

        void AddEntry( PhaseId phaseId, UpdateEntry* ue );
        void RemoveEntry( PhaseId phaseId, UpdateEntry* ue );
        void RemoveEntry( PhaseId phaseId, uint64_t systemId );

    private:
        /// Singleton instance.
        static UpdateManager* sm_pInstance;

        bool m_Run;
        
        PhaseMap m_Phases;
    };
}
