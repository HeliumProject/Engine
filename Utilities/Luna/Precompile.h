#pragma once

//
// Win32
//
#define _CRT_RAND_S // required for rand_s, must be declared before including stdlib.h

#include "Platform/Windows/Windows.h"

//
// D3D
//

#ifdef _DEBUG
#define D3D_DEBUG_INFO
#endif

#include <d3d9.h>
#include <d3dx9.h>

//
// wxWidgets
//


#include <wx/wx.h>
#include <wx/msw/private.h>
#include <wx/aui/aui.h>
#include <wx/notebook.h>
#include <wx/treectrl.h>
#include <wx/listctrl.h>
#include <wx/spinctrl.h>
#include <wx/grid.h>
#include <wx/artprov.h>
#include <wx/clipbrd.h>
#include <wx/colordlg.h>
#include <wx/image.h>
#include <wx/wxhtml.h>
#include <wx/statline.h>

//
// Std Library
//

#include <string>
#include <map>
