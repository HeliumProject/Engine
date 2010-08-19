#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/InspectContainer.h"
#include "wx/treebase.h"

namespace Helium
{
    namespace Inspect
    {
        const static tchar PANEL_ATTR_NAME[] = TXT( "text" );

        //
        // Contains other controls and distributes layout logic
        //

        class APPLICATION_API Panel : public Reflect::ConcreteInheritor<Panel, Container>
        {
        public:
            Panel();
            virtual ~Panel();

            const tstring& GetName() const
            {
                return m_Name;
            }

            void SetName(const tstring& name)
            {
                m_Name = name;
            }

            const tstring& GetPath()
            {
                if ( m_Path.empty() )
                {
                    BuildPath( m_Path );
                }

                return m_Path;
            }

            // build the path of titles to this panel
            void BuildPath(tstring& path);

            virtual bool Process(const tstring& key, const tstring& value) HELIUM_OVERRIDE;
            virtual void Realize(Container* parent) HELIUM_OVERRIDE;
            virtual void Unrealize() HELIUM_OVERRIDE;
            virtual void Read() HELIUM_OVERRIDE;

        protected:
            wxTreeItemId GetParentTreeNode(Container* parent);

        public:
            // set tree node state
            void SetShowTreeNode(bool showTreeNode);

            const tstring& GetText() const;

            friend class APPLICATION_API Canvas;

        protected:
            // the title of the panel
            tstring m_Name;

            // the path of the panel
            tstring m_Path;

            // do we own the window we are creating?
            bool m_OwnWindow;
        };

        typedef Helium::SmartPtr<Panel> PanelPtr;
        typedef std::vector<Panel*> V_Panel;
    }
}