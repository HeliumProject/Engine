#pragma once

#include "Foundation/API.h"
#include "Foundation/Update/UpdateEntry.h"

namespace Helium
{
    class HELIUM_FOUNDATION_API UpdateManager
    {
    public:

        UpdateManager();
        ~UpdateManager();

        void Initialize();
        void Shutdown();

        static UpdateManager& GetStaticInstance();
        static void DestroyStaticInstance();

        void Start();
        void Stop();

        void AddEntry( UpdateEntry* ue );
        void RemoveEntry( UpdateEntry* ue );
        void RemoveEntry( tuid id );

    private:
        static UpdateManager* sm_pInstance;

        bool m_Run;

        UpdateEntryMap m_UpdateEntries;
    };
}
