#pragma once

#include "API.h"

#include <wx/wx.h>

namespace UIToolKit
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


  UITOOLKIT_API void MessageDialog( wxWindow* parent, const char* msgFormat, ... );
  UITOOLKIT_API void MessageDialog( wxWindow* parent, const MessageDialogIcon status, const char* msgFormat, ... );
  UITOOLKIT_API void MessageDialog( wxWindow* parent, const MessageDialogIcon status, long style, const char* msgFormat, ... );
  UITOOLKIT_API void MessageDialog( wxWindow* parent, const char* msgFormat, va_list statusArgs, const MessageDialogIcon status = MessageDialogIcons::Info, long style = wxOK | wxSTAY_ON_TOP );

} // namespace UIToolKit