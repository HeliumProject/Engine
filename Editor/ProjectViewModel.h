#pragma once

#include "Pipeline/Project.h"
#include "Foundation/Document/Document.h"
#include "Foundation/Document/DocumentManager.h"
#include "Foundation/Memory/SmartPtr.h"
#include "Foundation/File/Path.h"

#include <wx/dataview.h>

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
                Details,
                FileSize,

                COUNT //Do not use: must be last
            };

            static const tchar* s_Labels[COUNT+1] = 
            {
                TXT( "Name" ),
                TXT( "Details" ),
                TXT( "Size" ),

                TXT( "Unknown" ), //COUNT
            };

            inline const tchar* Label( uint32_t id )
            {
                HELIUM_ASSERT( id >= 0 );
                HELIUM_ASSERT( id < COUNT );
                return s_Labels[id];
            }

            static const uint32_t s_Widths[COUNT+1] = 
            {
                200, // Name
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


        ///////////////////////////////////////////////////////////////////////
        namespace ProjectMenuIDs
        {
            enum ProjectMenuID
            {
                Filename = 0,
                FullPath,
                RelativePath,

                COUNT //Do not use: must be last
            };
            static void ProjectMenuIDsEnumerateEnum( Reflect::Enumeration* info )
            {
                info->AddElement( Filename, TXT( "Filename" ) );
                info->AddElement( FullPath, TXT( "FullPath" ) );
                info->AddElement( RelativePath, TXT( "RelativePath" ) );
            }

            static const tchar* s_Labels[COUNT] = 
            {
                TXT( "Filename" ),
                TXT( "Full Path" ),
                TXT( "Relative Path" ),
            };

            inline const tchar* Label( uint32_t id )
            {
                HELIUM_ASSERT( id >= 0 );
                HELIUM_ASSERT( id < COUNT );
                return s_Labels[id];
            }
        }
        typedef ProjectMenuIDs::ProjectMenuID ProjectMenuID;


        ///////////////////////////////////////////////////////////////////////
        class ProjectViewModelNode;
        typedef Helium::SmartPtr< ProjectViewModelNode > ProjectViewModelNodePtr;
        typedef std::set< ProjectViewModelNodePtr > S_ProjectViewModelNodeChildren;

        class ProjectViewModelNode : public Helium::RefCountBase< ProjectViewModelNode >
        {
        public:
            ProjectViewModelNode( ProjectViewModelNode* parent,
                const Helium::Path& path,
                const Document* document = NULL,
                const bool isContainer = false );
            virtual ~ProjectViewModelNode();

            ProjectViewModelNode* GetParent();
            S_ProjectViewModelNodeChildren& GetChildren();

            bool IsContainer() const;

            void SetPath( const Helium::Path& path );
            const Helium::Path& GetPath();
            //void PathChanged( const Attribute< Helium::Path >::ChangeArgs& text );

            tstring GetName() const;
            tstring GetDetails() const;
            tstring GetFileSize() const;

            const Document* GetDocument() const;
            void ConnectDocument( const Document* document);
            void DisconnectDocument();

            void DocumentSaved( const DocumentEventArgs& args );
            void DocumentClosed( const DocumentEventArgs& args );
            void DocumentPathChanged( const DocumentPathChangedArgs& args );

            inline bool operator<( const ProjectViewModelNode& rhs ) const
            {
                return ( _tcsicmp( m_Path.c_str(), rhs.m_Path.c_str() ) < 0 );
            }

            inline bool operator==( const ProjectViewModelNode& rhs ) const
            {
                return ( ( _tcsicmp( m_Path.c_str(), rhs.m_Path.c_str() ) == 0 )
                    && m_ParentNode == rhs.m_ParentNode ) ;
            }

        private:
            ProjectViewModelNode* m_ParentNode;
            S_ProjectViewModelNodeChildren m_ChildNodes;
            bool m_IsContainer;

            Helium::Path m_Path;
            const Document* m_Document;
        };


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

            void SetProject( Project* project, const Document* document );

            bool AddChildItem( const wxDataViewItem& parenItem, const Helium::Path& path );
            bool RemoveChildItem( const wxDataViewItem& parenItem, const Helium::Path& path );

            void RemoveItem( const wxDataViewItem& item );

            bool IsDropPossible( const wxDataViewItem& item );

            // Project Events
            void OnPathAdded( const Helium::Path& path );
            void OnPathRemoved( const Helium::Path& path );

            // Document and DocumentManager Events
            void OnProjectSave( const DocumentEventArgs& args );
            void OnProjectPathChanged( const DocumentPathChangedArgs& args );
            void OnDocumentAdded( const DocumentEventArgs& args );
            void OnDocumentRemoved( const DocumentEventArgs& args );

        public:
            // wxDataViewModel pure virtual interface
            virtual unsigned int GetColumnCount() const HELIUM_OVERRIDE;
            virtual wxString GetColumnType( unsigned int type ) const HELIUM_OVERRIDE;

            virtual void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int column ) const HELIUM_OVERRIDE;
            virtual bool SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int column ) HELIUM_OVERRIDE;

            virtual wxDataViewItem GetParent( const wxDataViewItem& item ) const HELIUM_OVERRIDE;
            virtual unsigned int GetChildren( const wxDataViewItem& item, wxDataViewItemArray& items ) const HELIUM_OVERRIDE;

            virtual bool IsContainer( const wxDataViewItem& item ) const HELIUM_OVERRIDE;

        private:
            Document* ConnectDocument( const Document* document, ProjectViewModelNode* node );
            void DisconnectDocument( ProjectViewModelNode* node );

        private:
            DocumentManager* m_DocumentManager;
            Project* m_Project;
            ProjectViewModelNodePtr m_RootNode;

            typedef std::multimap< const Helium::Path, ProjectViewModelNode* > MM_ProjectViewModelNodesByPath;
            MM_ProjectViewModelNodesByPath m_MM_ProjectViewModelNodesByPath;

            typedef std::vector< uint32_t > M_ColumnLookupTable;
            M_ColumnLookupTable m_ColumnLookupTable;
        };
    }
}