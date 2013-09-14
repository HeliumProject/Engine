#pragma once

struct WindowData
{
#if HELIUM_OS_WIN
    HWND hMainWnd;
    HWND hSubWnd;
#endif
    bool bProcessMessages;
    bool bShutdownRendering;
    int resultCode;
};

#if HELIUM_OS_WIN
LRESULT CALLBACK WindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
#endif