#pragma once

#include "CollectionManager.h"
#include "VaultPreferences.h"
#include "VaultSearch.h"
#include "VaultEvents.h"
#include "SearchQuery.h"
#include "SearchResults.h"
#include "SearchHistory.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Platform/Types.h"
#include "Foundation/InitializerStack.h"
#include "DocumentManager.h"

//
// Forwards
//
namespace Asset
{
    class SearchResults;
    typedef Nocturnal::SmartPtr< SearchResults > SearchResultsPtr;
}

namespace Luna
{
    //
    // Forwards
    //
    class VaultFrame;

    class Vault
    {
    public:
        Vault();
        Vault( const Vault& rhs ) {}
        Vault& operator=( const Vault& rhs ) {}

    protected:
        static Nocturnal::InitializerStack s_InitializerStack;
        static int s_InitCount;

    public:
        ~Vault();

        void SetRootDirectory( const Nocturnal::Path& path )
        {
            m_RootDirectory = path;
            if ( m_VaultSearch )
            {
                m_VaultSearch->SetRootDirectory( path );
            }
        }

        void ShowVault( const tstring& queryString = TXT("") );
        bool HasFrame();

        void InitializePreferences();
        VaultPreferences* GetVaultPreferences();

        friend class VaultFrame;

    protected:
        void OnCloseVault();

    protected:

        //
        // Members
        //
    private:
        Nocturnal::Path       m_RootDirectory;
        VaultSearchPtr      m_VaultSearch;
        VaultFrame*         m_VaultFrame;
        bool                  m_HasFrame;
        SearchHistory*        m_SearchHistory;
        VaultPreferencesPtr m_VaultPreferences;
        CollectionManager*    m_CollectionManager;

    private:

    };


} // namespace Luna
