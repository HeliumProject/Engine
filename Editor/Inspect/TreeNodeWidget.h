#pragma once

#include "Foundation/Inspect/Container.h"

#include "Editor/Controls/Tree/TreeWndCtrl.h"
#include "Editor/Inspect/Widget.h"

namespace Helium
{
    namespace Editor
    {
        class TreeNodeWidget;

        class TreeItemData : public wxTreeItemData
        {
        public:
            TreeItemData( TreeNodeWidget* widget )
                : m_Widget( widget )
            {

            }
            
            TreeNodeWidget* GetWidget()
            {
                return m_Widget;
            }

        private:
            TreeNodeWidget* m_Widget;
        };

        class TreeNodeWidget : public Reflect::ConcreteInheritor<TreeNodeWidget, Widget>
        {
        public:
            TreeNodeWidget()
                : m_ContainerControl( NULL )
                , m_TreeWndCtrl( NULL )
                , m_ItemData( NULL )
            {

            }

            TreeNodeWidget( Inspect::Container* container );

            wxTreeItemId GetId() const
            {
                return m_ItemData.GetId();
            }

            void SetId( wxTreeItemId id )
            {
                m_ItemData.SetId( id );
            }

            TreeWndCtrl* GetTreeWndCtrl()
            {
                return m_TreeWndCtrl;
            }

            void SetTreeWndCtrl(TreeWndCtrl* ctrl)
            {
                m_Window = m_TreeWndCtrl = ctrl;
            }

            virtual void Create( wxWindow* parent ) HELIUM_OVERRIDE;
            virtual void Destroy() HELIUM_OVERRIDE;

            virtual void Read() HELIUM_OVERRIDE {}
            virtual bool Write() HELIUM_OVERRIDE { return true; }

        private:
            Inspect::Container*     m_ContainerControl;
            TreeWndCtrl*            m_TreeWndCtrl;
            TreeItemData            m_ItemData;
        };
    }
}