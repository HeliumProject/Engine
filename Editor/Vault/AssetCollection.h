#pragma once

#include "Editor/API.h"

#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Serializers.h"
#include "Foundation/TUID.h"

namespace Editor
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
    typedef Helium::SmartPtr<AssetCollection> AssetCollectionPtr;
    typedef std::map< u64, AssetCollectionPtr > M_AssetCollections;

    class AssetCollection : public Reflect::Element
    {
    public:
        AssetCollection();
        AssetCollection( const tstring& name, const u32 flags = AssetCollectionFlags::Default );
        virtual ~AssetCollection(); 

        static void GetFileFilters( std::vector< tstring >& filters );

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

        const tstring& GetName() const { return m_Name; }
        void SetName( const tstring& name );

        const Helium::Path& GetPath() const
        {
            return m_Path;
        }
        void SetPath( const Helium::Path& path )
        {
            m_Path = path;
        }

        virtual tstring GetDisplayName() const;
        virtual tstring GetQueryString() const;

        virtual void SetFlags( const u32 flags );
        virtual u32 GetFlags() const { return m_Flags; }
        bool IsDynamic() const;
        bool IsTemporary() const;    
        bool CanRename() const;
        bool CanHandleDragAndDrop() const;
        bool ReadOnly() const;

        void SetAssetReferences( const std::set< Helium::Path >& references );
        const std::set< Helium::Path >& GetAssetPaths() const { return m_AssetPaths; };
        bool AddAsset( const Helium::Path& path );
        bool AddAssets( const std::set< Helium::Path >& assets );
        bool RemoveAsset( const Helium::Path& path  );
        bool ContainsAsset( u64 id ) const;  
        void ClearAssets(); 

        static AssetCollectionPtr LoadFrom( const Helium::Path& path );

        template <class T>
        static Helium::SmartPtr<T> LoadFrom( Helium::Path& path )
        {
            return Reflect::TryCast<T>( LoadFrom( fileRef ) );
        }

        static bool SaveTo( const AssetCollection* collection, const tstring& path );

        static bool IsValidCollectionName( const tstring& name, tstring& errors );

    public:
        static void CreateSignature( const tstring& str, tstring& signature );
        static void CreateSignature( tuid id, tstring& signature );
        static void CreateFilePath( const tstring name, tstring& filePath, const tstring& folder = TXT("") );

        bool operator<( const AssetCollection& rhs ) const;

    public:
        REFLECT_DECLARE_CLASS( AssetCollection, Reflect::Element );
        static void EnumerateClass( Reflect::Compositor<AssetCollection>& comp );

    protected:
        void DirtyField( const Reflect::Field* field );

    protected:
        i32           m_FreezeCount;
        tstring   m_Name;
        Helium::Path m_Path;
        u32           m_Flags;
        std::set< Helium::Path > m_AssetPaths;
    };

}
