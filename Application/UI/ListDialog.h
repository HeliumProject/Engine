#pragma once

#include "Application/API.h"
#include "ListView.h"
#include <wx/wx.h>

namespace Nocturnal
{
  //////////////////////////////////////////////////////////////////////////////
  // Dialog that displays a list of messages.
  // 
  class APPLICATION_API ListDialog : public wxDialog 
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
