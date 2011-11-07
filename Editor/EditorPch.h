#pragma once

#include "Platform/Platform.h"

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

#ifdef HELIUM_OS_WIN
# include <wx/msw/private.h>
#endif

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

#ifdef HELIUM_OS_WIN
# ifdef _DEBUG
#  define D3D_DEBUG_INFO
# endif
# include <d3d9.h>
# include <d3dx9.h>
#endif

//
// Helium
//

#include "Platform/Assert.h"
#include "Platform/Memory.h"
#include "Platform/Trace.h"

#if 0
use /Zm128+
#include "Foundation/Reflect/Object.h"
#include "Foundation/Reflect/Registry.h"
#include "Foundation/Reflect/Data/DataDeduction.h"
#endif
