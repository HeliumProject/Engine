#pragma once

#include "SceneGraph/Project.h"
#include "Application/DocumentManager.h"
#include "Foundation/SmartPtr.h"
#include "Foundation/FilePath.h"
#include "Engine/AssetPath.h"
#include "Engine/Asset.h"

#if HELIUM_CC_CLANG
#pragma clang push
#pragma clang diagnostic ignored "-Wnull-conversion"
#endif

#include <wx/dataview.h>

#if HELIUM_CC_CLANG
#pragma clang pop
#endif

namespace Helium
{
    namespace Editor
    {
        ///////////////////////////////////////////////////////////////////////
        namespace ProjectModelColumns
        {
            enum ProjectModelColumn
            {
                Name = 0,
				Type,
                Icon,
                Details,
                FileSize,

                COUNT //Do not use: must be last
            };

            static const char* s_Labels[COUNT+1] = 
            {
                TXT( "Name" ),
				TXT( "Type" ),
                TXT( "" ), //Icon
                TXT( "Details" ),
                TXT( "Size" ),

                TXT( "Unknown" ), //COUNT
            };

            inline const char* Label( uint32_t id )
            {
                HELIUM_ASSERT( id >= 0 );
                HELIUM_ASSERT( id < COUNT );
                return s_Labels[id];
            }

            static const uint32_t s_Widths[COUNT+1] = 
            {
                200, // Name
				100, // Type
                18, // Icon
                300, // Details
                50,  // FileSize

                0, //COUNT
            };

            inline uint32_t Width( uint32_t id )
            {
                HELIUM_ASSERT( id >= 0 );
                HELIUM_ASSERT( id < COUNT );
                return s_Widths[id];
            }
        }
        typedef ProjectModelColumns::ProjectModelColumn ProjectModelColumn;


   //     ///////////////////////////////////////////////////////////////////////
   //     class ProjectMenuID
   //     {
   //     public:
   //         enum Enum
   //         {
   //             Filename = 0,
   //             FullPath,
   //             RelativePath,

   //             COUNT //Do not use: must be last
   //         };

   //         HELIUM_DECLARE_ENUM( ProjectMenuID );

   //         static void PopulateMetaType( Reflect::MetaEnum& info )
   //         {
   //             info.AddElement( Filename,      TXT( "Filename" ) );
   //             info.AddElement( FullPath,      TXT( "FullPath" ) );
   //             info.AddElement( RelativePath,  TXT( "RelativePath" ) );
   //         }

   //         static const char* s_Labels[COUNT];

   //         inline const char* Label( uint32_t id )
   //         {
   //             HELIUM_ASSERT( id >= 0 );
   //             HELIUM_ASSERT( id < COUNT );
   //             return s_Labels[id];
   //         }
   //     };

   //     ///////////////////////////////////////////////////////////////////////
   //     class ProjectViewModel;
   //     class ProjectViewModelNode;
   //     typedef Helium::SmartPtr< ProjectViewModelNode > ProjectViewModelNodePtr;
   //     typedef std::set< ProjectViewModelNodePtr > S_ProjectViewModelNodeChildren;

   //     class ProjectViewModelNode : public Helium::RefCountBase< ProjectViewModelNode >
   //     {
   //     public:
   //         ProjectViewModelNode( ProjectViewModel* model,
   //             ProjectViewModelNode* parent,
   //             const Helium::AssetPath& path,
   //             const Document* document = NULL,
   //             const bool isContainer = false,
   //             const bool isActive = false );
   //         virtual ~ProjectViewModelNode();

   //         ProjectViewModelNode* GetParent();
   //         S_ProjectViewModelNodeChildren& GetChildren();

   //         bool IsContainer() const;

   //         void SetPath( const Helium::AssetPath& path );
   //         const Helium::AssetPath& GetPath();

   //         std::string GetName() const;
   //         std::string GetDetails() const;
   //         std::string GetFileSize() const;

   //         const Document* GetDocument() const;
   //         uint32_t GetDocumentStatus() const;
   //         void ConnectDocument( const Document* document);
   //         void DisconnectDocument();

   //         void DocumentSaved( const DocumentEventArgs& args );
   //         void DocumentClosed( const DocumentEventArgs& args );
   //         void DocumentChanging( const DocumentEventArgs& args );
   //         void DocumentChanged( const DocumentEventArgs& args );
   //         void DocumentModifiedOnDiskStateChanged( const DocumentEventArgs& args );
   //         void DocumentPathChanged( const DocumentPathChangedArgs& args );

   //         inline bool operator<( const ProjectViewModelNode& rhs ) const
   //         {
			//	String lhsString, rhsString;
			//	m_Path.ToString( lhsString );
			//	rhs.m_Path.ToString( rhsString );

   //             return ( CaseInsensitiveCompareString( *lhsString, *rhsString ) < 0 );
   //         }

   //         inline bool operator==( const ProjectViewModelNode& rhs ) const
			//{
			//	String lhsString, rhsString;
			//	m_Path.ToString( lhsString );
			//	rhs.m_Path.ToString( rhsString );

   //             return ( ( CaseInsensitiveCompareString( *lhsString, *rhsString ) == 0 )
   //                 && m_ParentNode == rhs.m_ParentNode ) ;
   //         }

   //         friend class ProjectViewModel;

   //     private:
   //         ProjectViewModel* m_Model;
   //         ProjectViewModelNode* m_ParentNode;
   //         S_ProjectViewModelNodeChildren m_ChildNodes;
   //         bool m_IsContainer;
   //         bool m_IsActive;

   //         Helium::AssetPath m_Path;
   //         const Document* m_Document;
   //     };


        ///////////////////////////////////////////////////////////////////////
        class ProjectViewModel : public wxDataViewModel
        {
        public:
            ProjectViewModel( DocumentManager* documentManager );
            virtual ~ProjectViewModel();

            // This returns a column that can be added to wxDataViewCtrl
            // it also updates the m_ColumnLookupTable to associate the column
            // to the ProjectModelColumns
            wxDataViewColumn* CreateColumn( uint32_t id );
            void ResetColumns();

            void OpenProject( Project* project, const Document* document = NULL );
            void CloseProject();

            //bool AddChildItem( const wxDataViewItem& parenItem, const Helium::AssetPath& path ) const;
            //bool RemoveChildItem( const wxDataViewItem& parenItem, const Helium::AssetPath& path );

            //void RemoveItem( const wxDataViewItem& item );

            bool IsDropPossible( const wxDataViewItem& item );

            void SetActive( const AssetPath& path, bool active );

            // Project Events
            void OnPathAdded( const Helium::AssetPath& path );
            void OnPathRemoved( const Helium::AssetPath& path );

            // Document and DocumentManager Events
            void OnDocumentOpened( const DocumentEventArgs& args );
            void OnDocumenClosed( const DocumentEventArgs& args );

			void OnAssetLoaded( const AssetEventArgs& args );
			void OnAssetEditable( const AssetEventArgs& args );

        public:
            // wxDataViewModel pure virtual interface
            virtual unsigned int GetColumnCount() const HELIUM_OVERRIDE;
            virtual wxString GetColumnType( unsigned int type ) const HELIUM_OVERRIDE;

			virtual bool HasContainerColumns(const wxDataViewItem& WXUNUSED(item)) const
			{ return true; }

            virtual void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int column ) const HELIUM_OVERRIDE;
            virtual bool SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int column ) HELIUM_OVERRIDE;
            virtual bool GetAttr( const wxDataViewItem& item, unsigned int column, wxDataViewItemAttr& attr ) const HELIUM_OVERRIDE;

            virtual wxDataViewItem GetParent( const wxDataViewItem& item ) const HELIUM_OVERRIDE;
            virtual unsigned int GetChildren( const wxDataViewItem& item, wxDataViewItemArray& items ) const HELIUM_OVERRIDE;

            virtual bool IsContainer( const wxDataViewItem& item ) const HELIUM_OVERRIDE;

        private:
            //Document* ConnectDocument( const Document* document, ProjectViewModelNode* node );
            //void DisconnectDocument( ProjectViewModelNode* node );

        private:
            //DocumentManager* m_DocumentManager;
            //Project* m_Project;
			//ProjectViewModelNodePtr m_RootNode;
			//ProjectViewModelNodePtr m_Node2;

            //typedef std::multimap< const Helium::AssetPath, ProjectViewModelNode* > MM_ProjectViewModelNodesByPath;
            //mutable MM_ProjectViewModelNodesByPath m_MM_ProjectViewModelNodesByPath;

			mutable Helium::Set<Asset *> m_AssetsInTree;

            typedef std::vector< uint32_t > M_ColumnLookupTable;
            M_ColumnLookupTable m_ColumnLookupTable;

            typedef std::map<std::string, wxArtID> M_FileIconExtensionLookup;
            M_FileIconExtensionLookup m_FileIconExtensionLookup;

            static const wxArtID DefaultFileIcon;
            const wxArtID& GetArtIDFromPath( const AssetPath& path ) const;
        };
    }
}