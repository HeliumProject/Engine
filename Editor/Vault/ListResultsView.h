#pragma once

#include <sstream>

#include <wx/panel.h>

#include "VaultMenuIDs.h"
#include "VaultSearchResults.h"
#include "Editor\Controls\List\ListView.h"

namespace Helium
{
    namespace Editor
    {
        ///////////////////////////////////////////////////////////////////////
        namespace DetailsColumnTypes
        {
            enum DetailsColumnType
            {
                NONE        = 0,
                Icon,
                FilePath,
                Filename,
                Directory,
                Size,
            };
        }
        typedef DetailsColumnTypes::DetailsColumnType DetailsColumnType;
        typedef std::vector< DetailsColumnType > V_DetailsColumnType;

        /////////////////////////////////////////////////////////////////////////////
        typedef std::string (*GetFileInfoDataFunc)( const Helium::FilePath& path );

        class DetailsColumn
        {
        public: 
            DetailsColumn()
                : m_DetailsColumnType( DetailsColumnTypes::NONE )
                , m_ColumnName( TXT( "" ) )
                , m_ColumnWidth( 200 )
                , m_GetFileInfoDataFunc( NULL )
            {
            }

            DetailsColumn( DetailsColumnType displayColType, const char* colName, int colWidth, GetFileInfoDataFunc getDataFunc )
                : m_DetailsColumnType( displayColType )
                , m_ColumnName( colName )
                , m_ColumnWidth( colWidth )
                , m_GetFileInfoDataFunc( getDataFunc )
            {
            }

            DetailsColumnType Type()
            {
                return m_DetailsColumnType;
            }

            const std::string& Name()
            {
                return m_ColumnName;
            }

            int Width()
            {
                return m_ColumnWidth;
            }

            std::string Data( const Helium::FilePath& path )
            {
                if ( m_GetFileInfoDataFunc != NULL )
                {
                    return m_GetFileInfoDataFunc( path ); 
                }

                return std::string( TXT( "" ) );
            }

        public:
            static std::string FilePath( const Helium::FilePath& path );
            static std::string Filename( const Helium::FilePath& path );
            static std::string Directory( const Helium::FilePath& path );
            static std::string Size( const Helium::FilePath& path );

        private:
            DetailsColumnType m_DetailsColumnType;
            std::string m_ColumnName;
            int m_ColumnWidth;
            GetFileInfoDataFunc m_GetFileInfoDataFunc;
        };
        typedef std::map<DetailsColumnType, DetailsColumn> M_DetailsColumn;

        ///////////////////////////////////////////////////////////////////////
        class ListResultsView : public wxPanel
        {
        public:
            ListResultsView( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
            ~ListResultsView();

            void InitResults();
            void SetResults( VaultSearchResults* results );
            void ClearResults();
            
            const VaultSearchResults* GetResults() const;

            void SelectPath( const std::string& path );
            void GetSelectedPaths( std::set< Helium::FilePath >& paths );

            std::string GetHighlightedPath() const;

            void ShowDetails( bool showDetails = true );

            VaultSortMethod GetSortMethod() const;
            void Sort( VaultSortMethod method = VaultSortMethods::AlphabeticalByName, uint32_t sortOptions = VaultSortOptions::Refresh | VaultSortOptions::Force );

        private:
            bool m_ShowDetails;
            SortableListView* m_ListCtrl;

            VaultSearchResultsPtr m_Results;
            VaultSortMethod m_SortMethod;

            uint32_t m_CurrentFileIndex;
            M_DetailsColumn m_DetailsColumns;
            V_DetailsColumnType m_DetailsColumnList;
        };
    }
}