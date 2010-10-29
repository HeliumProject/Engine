#pragma once

#include "Editor/API.h"
#include "MRU.h"

#include "Foundation/Automation/Event.h"
#include "Platform/Types.h"

#include <wx/window.h>

class MenuMRUEvtHandler;

namespace Helium
{
    namespace Editor
    {
        // Arguements and events for changes to the MRU.
        struct MRUArgs
        {
            tstring m_Item;

            MRUArgs( const tstring& item )
                : m_Item( item )
            {
            }
        };
        typedef Helium::Signature< const MRUArgs& > MRUSignature;

        /////////////////////////////////////////////////////////////////////////////
        // Class for managing a list of "most recently used" items.  The items are
        // strings (paths to files).  The MRU can be saved to and restored from
        // the registry.  In order to be useful, a piece of UI would need to display
        // the MRU and allow choosing items.
        // 
        class MenuMRU : public MRU< tstring >
        {
        public:
            MenuMRU( int32_t maxItems, wxWindow* owner );
            virtual ~MenuMRU();

            void RemoveInvalidItems( bool tuidRequired = false );

            void AddItemSelectedListener( const MRUSignature::Delegate& listener );
            void RemoveItemSelectedListener( const MRUSignature::Delegate& listener );
            void PopulateMenu( wxMenu* menu );

        private:
            wxWindow* m_Owner;
            MenuMRUEvtHandler* m_MenuMRUEvtHandler;
        };

        typedef Helium::SmartPtr< MenuMRU > MenuMRUPtr;
    }
}