/////////////////////////////////////////////////////////////////////////////
// Name:        wx/TreeWndCtrlApi.h
// Purpose:     This header file is used for defining DLL export stuff.
// Author:      Insomniac Games
// Modified by:
// Created:     03/23/09
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#pragma once

#if defined(WXMAKINGDLL_TWC)
    #define WXDLLIMPEXP_TWC WXEXPORT
#elif defined(WXUSINGDLL_TWC)
    #define WXDLLIMPEXP_TWC WXIMPORT
#else
    #define WXDLLIMPEXP_TWC
#endif
