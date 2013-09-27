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
			void OnAssetChanged( const AssetEventArgs& args );

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