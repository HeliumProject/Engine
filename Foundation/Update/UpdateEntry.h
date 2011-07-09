#pragma once

#include "Foundation/API.h"
#include "Foundation/TUID.h"

#include "Platform/Types.h"
#include "Platform/Timer.h"

#include <set>
#include <map>

namespace Helium
{
    class UpdateEntry;
    typedef void (UpdateCallback)( UpdateEntry* updateEntry, float64_t deltaTime, float64_t time );

    class HELIUM_FOUNDATION_API UpdateEntry
    {
    public:
        typedef std::set< tuid > DependencySet;

        UpdateEntry( UpdateCallback* callback, uint32_t frequency, tuid id = TUID::Null )
            : m_UpdateCallback( callback )
            , m_Frequency( frequency )
            , m_LastUpdateTime( 0.0f )
            , m_Called( false )
            , m_Complete( false )
            , m_Id( id == TUID::Null ? TUID::Generate() : id )
        {
        }

        tuid GetId()
        {
            return m_Id;
        }

        UpdateCallback* GetCallback()
        {
            return m_UpdateCallback;
        }

        uint32_t GetFrequency()
        {
            return m_Frequency;
        }

        float64_t GetLastUpdateTime()
        {
            return m_LastUpdateTime;
        }

        void AddDependency( tuid dependencyId )
        {
            m_Dependencies.insert( dependencyId );
        }

        void RemoveDependency( tuid dependencyId )
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

        void SetComplete( bool complete = true )
        {
            m_Complete = complete;
        }
        bool IsComplete()
        {
            return m_Complete;
        }

        void Call()
        {
            m_Called = true;
            float64_t updateTime = Timer::GetSeconds();
            m_UpdateCallback( this, m_LastUpdateTime > 0.0f ? ( updateTime - m_LastUpdateTime ) : 0.0f, updateTime );
            m_LastUpdateTime = updateTime;
        }

        void Reset()
        {
            m_Called = false;
            m_Complete = false;
        }

        bool operator==( const UpdateEntry& rhs )
        {
            return m_Id == rhs.m_Id;
        }

    private:
        UpdateCallback* m_UpdateCallback;
        DependencySet m_Dependencies;
        tuid m_Id;
        float64_t m_LastUpdateTime;
        int32_t m_Frequency; // hertz
        bool m_Called;
        bool m_Complete;
    };

    typedef std::map< uint64_t, UpdateEntry* > UpdateEntryMap;
}