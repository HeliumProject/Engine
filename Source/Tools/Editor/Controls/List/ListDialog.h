#pragma once

#include <wx/dialog.h>

#include "ListView.h"

namespace Helium
{
    namespace Editor
    {
        //////////////////////////////////////////////////////////////////////////////
        // Dialog that displays a list of messages.
        // 
        class ListDialog : public wxDialog 
        {
        protected:
            wxStaticText* m_StaticText;
            SortableListView* m_MsgList;
            wxButton* m_OK;
            std::string m_Description;

        public:
            ListDialog( wxWindow* parent, const std::string& title, const std::string& desc, const std::vector< std::string >& msgs );
        };
    }
}