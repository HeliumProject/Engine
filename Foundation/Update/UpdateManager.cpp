#include "FoundationPch.h"
#include "Foundation/Update/UpdateManager.h"

#include "Platform/PlatformUtility.h"

using namespace Helium;

UpdateManager* UpdateManager::sm_pInstance = NULL;

UpdateManager::UpdateManager()
: m_Run( true )
{
    if ( !Timer::IsInitialized() )
    {
        Timer::StaticInitialize();
    }
}

UpdateManager::~UpdateManager()
{
}

void UpdateManager::Initialize()
{
}

void UpdateManager::Shutdown()
{
    for ( UpdateEntryMap::iterator itr = m_UpdateEntries.begin(), end = m_UpdateEntries.end(); itr != end; ++itr )
    {
        UpdateEntry* ue = (*itr).second;
        delete ue;
    }
}

void UpdateManager::Start()
{
    while( m_Run )
    {
        bool allEntriesComplete = false;

        while( !allEntriesComplete && m_Run )
        {
            allEntriesComplete = true;

            for ( UpdateEntryMap::iterator itr = m_UpdateEntries.begin(), end = m_UpdateEntries.end(); itr != end && m_Run; ++itr )
            {
                UpdateEntry* ue = (*itr).second;

                float64_t currentTime = Timer::GetSeconds();

                if ( ( ( currentTime - ue->GetLastUpdateTime() ) > ( 1.0f / ue->GetFrequency() ) ) && !ue->IsComplete() && !ue->IsCalled() )
                {
                    bool dependenciesComplete = true;
                    UpdateEntry::DependencySet* deps = ue->GetDependencies();
                    for ( UpdateEntry::DependencySet::iterator itr = deps->begin(), end = deps->end(); itr != end && m_Run; ++itr )
                    {
                        if ( !m_UpdateEntries[ *itr ]->IsComplete() )
                        {
                            dependenciesComplete = false;
                            break;
                        }
                    }

                    if ( dependenciesComplete && m_Run )
                    {
                        ue->Call();
                    }                            
                }

                if ( ue->IsCalled() && !ue->IsComplete() )
                {
                    allEntriesComplete = false;
                }

                // yield
                Helium::Sleep( 0 );
            }
        }

        for ( UpdateEntryMap::iterator itr = m_UpdateEntries.begin(), end = m_UpdateEntries.end(); itr != end; ++itr )
        {
            (*itr).second->Reset();
        }
    }
}

void UpdateManager::Stop()
{
    m_Run = false;

    for ( UpdateEntryMap::iterator itr = m_UpdateEntries.begin(), end = m_UpdateEntries.end(); itr != end; ++itr )
    {
        (*itr).second->Reset();
    }
}

UpdateManager& UpdateManager::GetStaticInstance()
{
    if( !sm_pInstance )
    {
        sm_pInstance = new UpdateManager();
        HELIUM_ASSERT( sm_pInstance );
    }

    return *sm_pInstance;
}

void UpdateManager::DestroyStaticInstance()
{
    sm_pInstance->Stop();
    sm_pInstance->Shutdown();
    delete sm_pInstance;
    sm_pInstance = NULL;
}

void UpdateManager::AddEntry( UpdateEntry* ue )
{
    HELIUM_ASSERT( m_UpdateEntries.find( ue->GetId() ) == m_UpdateEntries.end() );

    m_UpdateEntries[ ue->GetId() ] = ue;
}

void UpdateManager::RemoveEntry( UpdateEntry* ue )
{
    for ( UpdateEntryMap::iterator itr = m_UpdateEntries.begin(), end = m_UpdateEntries.end(); itr != end; ++itr )
    {
        if ( *(*itr).second == *ue )
        {
            m_UpdateEntries.erase( itr );
            delete (*itr).second;
            break;
        }
    }
}

void UpdateManager::RemoveEntry( tuid id )
{
    for ( UpdateEntryMap::iterator itr = m_UpdateEntries.begin(), end = m_UpdateEntries.end(); itr != end; ++itr )
    {
        if ( (*itr).second->GetId() == id )
        {
            m_UpdateEntries.erase( itr );
            delete (*itr).second;
            break;
        }
    }
}
