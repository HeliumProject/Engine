#pragma once

#include "Platform/System.h"

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

// this brings in the top level wx/setup.h
#include <wx/platform.h>

// this overrides the default wx/setup.h for custom build parameters that we use when building wxWidgets
#if HELIUM_DEBUG
# undef wxUSE_EXCEPTIONS
# define wxUSE_EXCEPTIONS 0
#endif

// bring in the rest of wx now we have set our custom build macros
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
#include <wx/defs.h>
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
#endif

//
// Helium
//

#include "Platform/Assert.h"
#include "Platform/MemoryHeap.h"
#include "Platform/Trace.h"

#if 0
use /Zm128+
#include "Reflect/Object.h"
#include "Reflect/Registry.h"
#include "Reflect/TranslatorDeduction.h"
#endif
