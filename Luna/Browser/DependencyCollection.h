#pragma once

#include "Luna/API.h"

#include "Pipeline/Asset/AssetFile.h"

#include "AssetCollection.h"
#include "DependencyLoader.h"

namespace Luna
{

    ///////////////////////////////////////////////////////////////////////////////
    /// class DependencyCollection
    ///////////////////////////////////////////////////////////////////////////////
    class DependencyCollection : public AssetCollection
    {
    public:
        DependencyCollection();
        DependencyCollection( const tstring& name, const u32 flags = AssetCollectionFlags::Default, const bool reverse = false );
        ~DependencyCollection();

        virtual void InitializeCollection() NOC_OVERRIDE;
        virtual void CleanupCollection() NOC_OVERRIDE;

        virtual tstring GetDisplayName() const;

        void SetRoot( const Nocturnal::Path& path );
        const Nocturnal::Path& GetRoot()
        {
            return m_RootPath;
        }

        tstring GetAssetName() const;

        virtual void SetFlags( const u32 flags );

        bool IsReverse() const { return m_IsReverse; }
        void SetReverse( const bool reverse );

        u32 GetRecursionDepthForLoad() const;

        void LoadDependencies( bool threaded = true );

        bool IsLoading() const { return m_IsLoading; }
        void IsLoading( bool isLoading );

    private:
        void OnPreferencesChanged( const Reflect::ElementChangeArgs& args );

    public:
        REFLECT_DECLARE_CLASS( DependencyCollection, AssetCollection );
        static void EnumerateClass( Reflect::Compositor<DependencyCollection>& comp );
        virtual void PreDeserialize() NOC_OVERRIDE;
        virtual void PostDeserialize() NOC_OVERRIDE;

    private:
        Nocturnal::Path m_RootPath;
        bool m_IsReverse;

        // Cache
        Asset::AssetFilePtr m_AssetFile;

        bool m_IsLoading;
        DependencyLoader* m_DependencyLoader;
    };
    typedef Nocturnal::SmartPtr< DependencyCollection > DependencyCollectionPtr;

}
