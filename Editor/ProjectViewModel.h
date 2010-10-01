#pragma once

#include "Core/Project.h"
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
                //Details,

                COUNT //Do not use: must be last
            };

            static const tchar* s_Labels[COUNT+1] = 
            {
                TXT( "Name" ),
                //TXT( "Details" ),

                TXT( "Unknown" ), //COUNT
            };

            inline const tchar* Label( u32 id )
            {
                HELIUM_ASSERT( id >= 0 );
                HELIUM_ASSERT( id < COUNT );
                return s_Labels[id];
            }
        }
        typedef ProjectModelColumns::ProjectModelColumn ProjectModelColumn;


        ///////////////////////////////////////////////////////////////////////
        class ProjectViewModelNode;
        typedef Helium::SmartPtr< ProjectViewModelNode > ProjectViewModelNodePtr;
        typedef std::set< ProjectViewModelNodePtr > S_ProjectViewModelNodeChildren;

        class ProjectViewModelNode : public Helium::RefCountBase< ProjectViewModelNode >
        {
        public:
            ProjectViewModelNode* m_ParentNode;
            S_ProjectViewModelNodeChildren m_ChildNodes;
            bool m_IsContainer;

            Helium::Path m_Path;

            wxString m_Name;
            //wxString m_Details;

        public:
            ProjectViewModelNode( ProjectViewModelNode* parent, const Helium::Path& path )
                : m_ParentNode( parent )
                , m_Path( path )
                , m_IsContainer( false )
                , m_Name( path.Basename() )
            {
            }

            virtual ~ProjectViewModelNode()
            {
                m_ParentNode = NULL;
                m_ChildNodes.clear();
            }

            bool operator<( const ProjectViewModelNode& rhs ) const
            {
                return ( _tcsicmp( m_Path.c_str(), rhs.m_Path.c_str() ) < 0 );
            }

            bool operator==( const ProjectViewModelNode& rhs ) const
            {
                return ( _tcsicmp( m_Path.c_str(), rhs.m_Path.c_str() ) == 0 );
            }

            ProjectViewModelNode* GetParent()
            {
                return m_ParentNode;
            }

            S_ProjectViewModelNodeChildren& GetChildren()
            {
                return m_ChildNodes;
            }

            bool IsContainer() const
            {
                // TODO: OR the file is a scene file, reflect file with manifest
                return ( m_IsContainer || m_ChildNodes.size() > 0 || m_Path.IsDirectory() || m_ParentNode ) ? true : false;
            }

            void SetPath( const Helium::Path& path )
            {
                if ( _tcsicmp( m_Path.c_str(), path.c_str() ) != 0 )
                {
                    m_Path = path;
                    m_Name = path.Basename();
                }
            }

            void PathChanged( const Attribute< Helium::Path >::ChangeArgs& text )
            {
                SetPath( text.m_NewValue );
            }
        };


        ///////////////////////////////////////////////////////////////////////
        class ProjectViewModel : public wxDataViewModel
        {
        public:
            ProjectViewModel();
            virtual ~ProjectViewModel();

            void SetProject( Project* project );
            
            bool AddChild( const wxDataViewItem& item, const Helium::Path& path );
            bool RemoveChild( const wxDataViewItem& item, const Helium::Path& path );

            void Delete( const wxDataViewItem& item );

            // Project events
            void OnPathAdded( const Helium::Path& path );
            void OnPathRemoved( const Helium::Path& path );

        public:
            // wxDataViewModel pure virtual interface
            virtual unsigned int GetColumnCount() const HELIUM_OVERRIDE;
            virtual wxString GetColumnType( unsigned int type ) const HELIUM_OVERRIDE;

            virtual void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int column ) const HELIUM_OVERRIDE;
            virtual bool SetValue( const wxVariant& variant, const wxDataViewItem& item, unsigned int column ) HELIUM_OVERRIDE;

            virtual wxDataViewItem GetParent( const wxDataViewItem& item ) const HELIUM_OVERRIDE;
            virtual unsigned int GetChildren( const wxDataViewItem& item, wxDataViewItemArray& items ) const HELIUM_OVERRIDE;

            virtual bool IsContainer( const wxDataViewItem& item ) const HELIUM_OVERRIDE;

        protected:
            ProjectPtr m_Project;
            ProjectViewModelNodePtr m_RootNode;
        };
    }
}