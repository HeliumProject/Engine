#include "Foundation/Update/UpdateManager.h"

#include "Platform/PlatformUtility.h"

using namespace Helium;

UpdateManager* UpdateManager::sm_pInstance = NULL;

/// Constructor.
UpdateManager::UpdateManager()
: m_Run( true )
{
}

/// Destructor.
UpdateManager::~UpdateManager()
{
}

/// Initialize
void UpdateManager::Initialize()
{
}

/// Shut down
void UpdateManager::Shutdown()
{
    for ( PhaseMap::iterator itr = m_Phases.begin(), end = m_Phases.end(); itr != end; ++itr )
    {
        UpdateEntryMap& phase = (*itr).second;

        for ( UpdateEntryMap::iterator itr = phase.begin(), end = phase.end(); itr != end; ++itr )
        {
            UpdateEntry* ue = (*itr).second;
            delete ue;
        }
    }
}

void UpdateManager::Start()
{
    while( m_Run )
    {

        for ( uint32_t phaseId = 0; phaseId < PhaseIds::MaxPhaseId; ++phaseId )
        {
            Print( TXT( "UM: Executing phase id: %d\n" ), phaseId );

            UpdateEntryMap& phase = m_Phases[ phaseId ];

            bool phaseComplete = false;

            while ( !phaseComplete )
            {
                phaseComplete = true;

                for ( UpdateEntryMap::iterator itr = phase.begin(), end = phase.end(); itr != end; ++itr )
                {
                    UpdateEntry* ue = (*itr).second;

                    Print( TXT( "UM:  Executing UpdateEntry (id: %d)\n" ), ue->GetId() );

                    if ( !ue->IsComplete() && !ue->IsCalled() )
                    {
                        bool dependenciesComplete = true;
                        UpdateEntry::DependencySet* deps = ue->GetDependencies();
                        for ( UpdateEntry::DependencySet::iterator itr = deps->begin(), end = deps->end(); itr != end; ++itr )
                        {
                            if ( !phase[ *itr ]->IsComplete() )
                            {
                                Print( TXT( "UM:    Waiting on completion of UpdateEntry with id (%d)\n" ), *itr );

                                dependenciesComplete = false;
                                break;
                            }
                        }

                        if ( dependenciesComplete )
                        {
                            ue->Call();
                        }                            
                    }

                    if ( !ue->IsComplete() )
                    {
                        phaseComplete = false;
                    }
                }
            }

            for ( UpdateEntryMap::iterator itr = phase.begin(), end = phase.end(); itr != end; ++itr )
            {
                UpdateEntry* ue = (*itr).second;
                ue->Reset();
            }

        }
    }
}

void UpdateManager::Stop()
{
    m_Run = false;
}

/// Get the singleton instance, creating it if necessary.
///
/// @return  Reference to the instance.
///
/// @see DestroyStaticInstance()
UpdateManager& UpdateManager::GetStaticInstance()
{
    if( !sm_pInstance )
    {
        sm_pInstance = new UpdateManager();
        HELIUM_ASSERT( sm_pInstance );
    }

    return *sm_pInstance;
}

/// Destroy the singleton instance.
///
/// @see GetStaticInstance()
void UpdateManager::DestroyStaticInstance()
{
    sm_pInstance->Stop();
    sm_pInstance->Shutdown();
    delete sm_pInstance;
    sm_pInstance = NULL;
}

void UpdateManager::AddEntry( PhaseId phaseId, UpdateEntry* ue )
{
    HELIUM_ASSERT( phaseId < PhaseIds::MaxPhaseId );
    HELIUM_ASSERT( m_Phases[ phaseId ].find( ue->GetId() ) == m_Phases[ phaseId ].end() );

    (m_Phases[ phaseId ])[ ue->GetId() ] = ue;
}

void UpdateManager::RemoveEntry( PhaseId phaseId, uint64_t id )
{
    HELIUM_ASSERT( phaseId < PhaseIds::MaxPhaseId );

    for ( UpdateEntryMap::iterator itr = m_Phases[ phaseId ].begin(), end = m_Phases[ phaseId ].end(); itr != end; ++itr )
    {
        if ( (*itr).second->GetId() == id )
        {
            m_Phases[ phaseId ].erase( itr );
            delete (*itr).second;
            break;
        }
    }
}

void UpdateManager::RemoveEntry( PhaseId phaseId, UpdateEntry* ue )
{
    HELIUM_ASSERT( (uint32_t)phaseId < m_Phases.size() );

    for ( UpdateEntryMap::iterator itr = m_Phases[ phaseId ].begin(), end = m_Phases[ phaseId ].end(); itr != end; ++itr )
    {
        if ( (*itr).second == ue )
        {
            m_Phases[ phaseId ].erase( itr );
            delete (*itr).second;
            break;
        }
    }
}
