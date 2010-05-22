#pragma once

#include "API.h"

#include "Asset/AssetFile.h"
#include "Common/Memory/SmartPtr.h"
#include "Reflect/Registry.h"
#include "Reflect/Serializers.h"
#include "TUID/TUID.h"

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
            return m_spFileReference->GetHash() == rhs.m_spFileReference->GetHash();
        }

        bool operator!=( const AssetCollection& rhs ) const
        {
            return !( *this == rhs );
        }

        const std::string& GetName() const { return m_Name; }
        void SetName( const std::string& name );

        File::Reference& GetFileReference() const
        {
            return *m_spFileReference;
        }
        void SetFileReference( const File::Reference& fileRef )
        {
            if ( m_spFileReference )
            {
                delete m_spFileReference;
            }
            
            m_spFileReference = new File::Reference( fileRef );
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

        void SetAssetReferences( const File::S_Reference& references );
        const File::S_Reference& GetAssetReferences() const { return m_AssetReferences; };
        bool AddAsset( const File::Reference& fileRef );
        bool AddAssets( const File::S_Reference& assets );
        bool RemoveAsset( const File::Reference& fileRef  );
        bool ContainsAsset( u64 id ) const;  
        void ClearAssets(); 

        static AssetCollectionPtr LoadFrom( File::Reference& fileRef );

        template <class T>
        static Nocturnal::SmartPtr<T> LoadFrom( File::Reference& fileRef )
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
        File::ReferencePtr m_spFileReference;
        u32           m_Flags;
        File::S_Reference m_AssetReferences;
    };

}
