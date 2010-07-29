#pragma once

//
// Win32
//
#define _CRT_RAND_S // required for rand_s, must be declared before including stdlib.h

#include "Platform/Windows/Windows.h"

//
// Std Library
//

#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

//
// wxWidgets
//

#include <wx/wx.h>
#include <wx/msw/private.h>

#include <wx/artprov.h>
#include <wx/aui/aui.h>
#include <wx/button.h>
#include <wx/clipbrd.h>
#include <wx/colordlg.h>
#include <wx/config.h>
#include <wx/grid.h>
#include <wx/image.h>
#include <wx/listctrl.h>
#include <wx/menu.h>
#include <wx/notebook.h>
#include <wx/regex.h>
#include <wx/spinctrl.h>
#include <wx/splitter.h>
#include <wx/statline.h>
#include <wx/timer.h>
#include <wx/tokenzr.h>
#include <wx/treectrl.h>
#include <wx/variant.h>
#include <wx/wxhtml.h>
#include <wx/xml/xml.h>

#include <wx/propgrid/propgrid.h>
#include <wx/propgrid/advprops.h>

//
// D3D
//

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif

#undef DrawText
#include <d3d9.h>
#include <d3dx9.h>
