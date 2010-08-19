#pragma once

#include <map>

#include "Platform/Types.h"
#include "Foundation/Automation/Event.h"
#include "Application/API.h"

namespace Helium
{
    namespace Inspect
    {
        class APPLICATION_API Control;


        //
        // Static context menu support
        //

        struct ContextMenuEventArgs
        {
            Control* m_Control;

            const tstring& m_Item;

            ContextMenuEventArgs(Control* control, const tstring& item)
                : m_Control (control)
                , m_Item (item)
            {

            }
        };

        // the delegate for a per-item callback to be called upon activation
        typedef Helium::Signature<void, const ContextMenuEventArgs&> ContextMenuSignature;

        // container for each delegate of each context menu item
        typedef std::map<tstring, ContextMenuSignature::Delegate> M_ContextMenuDelegate;


        //
        // Dynamic context menu support
        //

        class APPLICATION_API ContextMenu;
        typedef Helium::SmartPtr<ContextMenu> ContextMenuPtr;

        // popup-time context menu setup delegate
        typedef Helium::Signature<void, ContextMenuPtr> SetupContextMenuSignature;


        //
        // The menu class
        //

        class APPLICATION_API ContextMenu : public Helium::RefCountBase<ContextMenu>
        {
        public:
            ContextMenu(Control* control);
            ~ContextMenu();

        public:
            void AddItem(const tstring& item, ContextMenuSignature::Delegate delegate);
            void AddSeperator();

            const std::vector< tstring >& GetItems()
            {
                return m_Items;
            }

            void Resize(size_t size)
            {
                m_Items.resize(size);
            }

            void Clear()
            {
                m_Items.clear();
            }

        protected:
            void ControlRealized( Control* control );

#ifdef INSPECT_REFACTOR
            void OnShow( wxContextMenuEvent& event );
            void OnItem( wxCommandEvent& event );
#endif

        protected:
            Control*                m_Control;
            std::vector< tstring >  m_Items;
            M_ContextMenuDelegate   m_Delegates;
        };

        typedef Helium::SmartPtr<ContextMenu> ContextMenuPtr;
    }
}