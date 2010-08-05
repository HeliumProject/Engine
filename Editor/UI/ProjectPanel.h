#pragma once

#include "Foundation/Container/OrderedSet.h"
#include "Core/Project.h"

#include "EditorGenerated.h"

#include "Application/UI/FileDropTarget.h"

namespace Helium
{
    namespace Editor
    {
        class ProjectViewModel : public wxDataViewModel
        {
        public:
            void SetProject( Helium::Core::Project* project );

            // Document event handlers
            void ChildAdded( const Reflect::DocumentHierarchyChangeArgs& args );
            void ChildRemoved( const Reflect::DocumentHierarchyChangeArgs& args );

            // wxDataModel virtual interface
            unsigned int GetColumnCount() const HELIUM_OVERRIDE;
            wxString GetColumnType(unsigned int) const HELIUM_OVERRIDE;

            void GetValue(wxVariant &,const wxDataViewItem &,unsigned int) const HELIUM_OVERRIDE;
            bool SetValue(const wxVariant &,const wxDataViewItem &,unsigned int) HELIUM_OVERRIDE;

            wxDataViewItem GetParent(const wxDataViewItem &) const HELIUM_OVERRIDE;
            unsigned int GetChildren(const wxDataViewItem &,wxDataViewItemArray &) const HELIUM_OVERRIDE;

            bool IsContainer(const wxDataViewItem &) const HELIUM_OVERRIDE;

        protected:
            Helium::Core::ProjectPtr    m_Project;
        };

        class ProjectPanel : public ProjectPanelGenerated
        {
        public:
            ProjectPanel( wxWindow* parent );
            virtual ~ProjectPanel();

            void SetProject( Helium::Core::Project* project );

            // UI event handlers
			virtual void OnAddFile( wxCommandEvent& event ) HELIUM_OVERRIDE;
			virtual void OnCreateFolder( wxCommandEvent& event ) HELIUM_OVERRIDE;
			virtual void OnDelete( wxCommandEvent& event ) HELIUM_OVERRIDE;

            virtual void OnDroppedFiles( const FileDroppedArgs& args );

        protected:
            Helium::Core::ProjectPtr                            m_Project;
            wxObjectDataPtr<ProjectViewModel>                   m_Model;
            Helium::OrderedSet< Reflect::DocumentElementPtr >   m_Selected;
            Helium::FileDropTarget*                             m_DropTarget;
        };
    }
}