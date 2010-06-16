#pragma once

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/props.h>

namespace Nocturnal
{
  /////////////////////////////////////////////////////////////////////////////
  // Enumeration of IDs that can be passed to SetAttribute on a property grid
  // element.
  // 
  namespace PropertyGridAttribs
  {
    // 
    // Duplicate definitions (overlap with predefines from wxPropGrid)
    // 

    // Title on a dialog
    static const char* DialogTitle = wxPG_DIR_DIALOG_MESSAGE;

    // 
    // New definitions
    // 

    // Sets the type of a FileDialog (see FileDialogStyle for valid values)
    static const char* DialogFlags = "Luna::PropertyGridAttribs::DialogFlags";

    // Sets the file filter for a dialog (string form, see FileDialog::SetFilter)
    static const char* DialogFilter = "Luna::PropertyGridAttribs::DialogFilter";
  }
}