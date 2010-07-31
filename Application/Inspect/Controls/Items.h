#pragma once

#include "Application/API.h"
#include "Application/Inspect/Controls/Control.h"
#include "Foundation/Automation/Event.h"

#include "Foundation/Reflect/Class.h"

namespace Helium
{
    namespace Inspect
    {
        //
        // Items control (base class for comboboxes and listboxes)
        //

        const static tchar ITEMS_ATTR_ITEM[]      = TXT( "item" );
        const static tchar ITEMS_ATTR_ITEM_DELIM  = TXT( '|' );
        const static tchar ITEMS_ATTR_REQUIRED[]  = TXT( "required" );
        const static tchar ITEMS_ATTR_PREFIX[]    = TXT( "prefix" );

        class APPLICATION_API Items : public Reflect::AbstractInheritor<Items, Control>
        {
        protected:
            tstring m_Value;
            tstring m_Prefix;
            V_Item m_Items;
            V_Item m_Statics;
            bool m_Required;
            bool m_Highlight;

        public:
            Items();

        protected:
            virtual bool Process(const tstring& key, const tstring& value) HELIUM_OVERRIDE;

            virtual void SetDefaultAppearance(bool def) HELIUM_OVERRIDE;

            void SetToDefault(const ContextMenuEventArgs& event);

        public:
            virtual void Realize(Container* parent) HELIUM_OVERRIDE;

            // clear all the items
            virtual void Clear();

            // add an entry to the control
            virtual const V_Item& GetItems() const;
            virtual void SetItems(const V_Item& items);

            // get the value of an entry data
            virtual tstring GetValue();
            virtual void SetValue(const tstring& data);

            // query item membership
            virtual bool Contains(const tstring& data);

            // set the highlight state
            virtual void SetHighlight(bool highlighted);
        };
    }
}