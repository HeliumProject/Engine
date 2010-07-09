#pragma once

#include <wx/dialog.h>

#include "ListView.h"

namespace Luna
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
    tstring m_Description;

  public:
    ListDialog( wxWindow* parent, const tstring& title, const tstring& desc, const std::vector< tstring >& msgs );
  };
}
