#pragma once

#include "Luna/API.h"

#include "Pipeline/Asset/AssetFile.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Reflect/Registry.h"
#include "Reflect/Serializers.h"
#include "Foundation/TUID.h"

namespace Luna
{
    ///////////////////////////////////////////////////////////////////////////////
    namespace AssetCollectionFlags
    {
        enum AssetCollectionFlag
        {
            Dynamic              = 1 << 0, // Collection is created dynamically by dependencies or search query
            Temporary            = 1 << 1, // Will not be saved to disc
            CanRename            = 1 << 2, // The user can rename it from the UI
            CanHandleDragAndDrop = 1 << 3, // Collection can handle Inspect::ClipboardFileList
            ReadOnly             = 1 << 4, // File cannot be written to
        };

        const u32 Default = 0;
    }

    ///////////////////////////////////////////////////////////////////////////////
    /// class AssetCollection
    ///////////////////////////////////////////////////////////////////////////////
    class AssetCollection;
    typedef Nocturnal::SmartPtr<AssetCollection> AssetCollectionPtr;
    typedef std::map< u64, AssetCollectionPtr > M_AssetCollections;

    class AssetCollection : public Reflect::Element
    {
    public:
        AssetCollection();
        AssetCollection( const std::string& name, const u32 flags = AssetCollectionFlags::Default );
        virtual ~AssetCollection(); 

        // Called when collections are added to/removed from a collection manager 
        // for one time setup and tear down.  Override as needed.
        virtual void InitializeCollection() { }
        virtual void CleanupCollection() { }

        void Freeze();
        void Thaw();

        bool operator==( const AssetCollection& rhs ) const
        {
            return m_Path.Hash() == rhs.m_Path.Hash();
        }

        bool operator!=( const AssetCollection& rhs ) const
        {
            return !( *this == rhs );
        }

        const std::string& GetName() const { return m_Name; }
        void SetName( const std::string& name );

        const Nocturnal::Path& GetPath() const
        {
            return m_Path;
        }
        void SetPath( const Nocturnal::Path& path )
        {
            m_Path = path;
        }

        virtual std::string GetDisplayName() const;
        virtual std::string GetQueryString() const;

        virtual void SetFlags( const u32 flags );
        virtual u32 GetFlags() const { return m_Flags; }
        bool IsDynamic() const;
        bool IsTemporary() const;    
        bool CanRename() const;
        bool CanHandleDragAndDrop() const;
        bool ReadOnly() const;

        void SetAssetReferences( const Nocturnal::S_Path& references );
        const Nocturnal::S_Path& GetAssetPaths() const { return m_AssetPaths; };
        bool AddAsset( const Nocturnal::Path& path );
        bool AddAssets( const Nocturnal::S_Path& assets );
        bool RemoveAsset( const Nocturnal::Path& path  );
        bool ContainsAsset( u64 id ) const;  
        void ClearAssets(); 

        static AssetCollectionPtr LoadFrom( const Nocturnal::Path& path );

        template <class T>
        static Nocturnal::SmartPtr<T> LoadFrom( Nocturnal::Path& path )
        {
            return Reflect::TryCast<T>( LoadFrom( fileRef ) );
        }

        static bool SaveTo( const AssetCollection* collection, const std::string& path );

        static bool IsValidCollectionName( const std::string& name, std::string& errors );

    public:
        static void CreateSignature( const std::string& str, std::string& signature );
        static void CreateSignature( tuid id, std::string& signature );
        static void CreateFilePath( const std::string name, std::string& filePath, const std::string& folder = std::string("") );

        bool operator<( const AssetCollection& rhs ) const;

    public:
        REFLECT_DECLARE_CLASS( AssetCollection, Reflect::Element );
        static void EnumerateClass( Reflect::Compositor<AssetCollection>& comp );

    protected:
        void DirtyField( const Reflect::Field* field );

    protected:
        i32           m_FreezeCount;
        std::string   m_Name;
        Nocturnal::Path m_Path;
        u32           m_Flags;
        Nocturnal::S_Path m_AssetPaths;
    };

}
