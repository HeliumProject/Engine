#pragma once

#include "Application/API.h"
#include "Platform/Types.h"

#include <wx/wx.h>

namespace Nocturnal
{
  //
  // Status messages - wxMessageDialog helper functions
  //
  namespace MessageDialogIcons
  {
    enum MessageDialogIcon
    {
      Info,
      Error,
      Warning
    };
  }
  typedef MessageDialogIcons::MessageDialogIcon MessageDialogIcon;


  APPLICATION_API void MessageDialog( wxWindow* parent, const tchar* msgFormat, ... );
  APPLICATION_API void MessageDialog( wxWindow* parent, const MessageDialogIcon status, const tchar* msgFormat, ... );
  APPLICATION_API void MessageDialog( wxWindow* parent, const MessageDialogIcon status, long style, const tchar* msgFormat, ... );
  APPLICATION_API void MessageDialog( wxWindow* parent, const tchar* msgFormat, va_list statusArgs, const MessageDialogIcon status = MessageDialogIcons::Info, long style = wxOK | wxSTAY_ON_TOP );

} // namespace Nocturnal