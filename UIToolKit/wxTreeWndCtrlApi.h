/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wxTreeWndCtrlApi.h
// Purpose:     This header file is used for defining DLL export stuff.
// Author:      Insomniac Games
// Modified by:
// Created:     03/23/09
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TREEWNDCTRLAPI_H_
#define _WX_TREEWNDCTRLAPI_H_

#if defined(WXMAKINGDLL_TWC)
    #define WXDLLIMPEXP_TWC WXEXPORT
#elif defined(WXUSINGDLL)
    #define WXDLLIMPEXP_TWC WXIMPORT
#else
    #define WXDLLIMPEXP_TWC
#endif

#endif // _WX_TREEWNDCTRLAPI_H_
