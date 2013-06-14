#pragma once

struct WindowData
{
    HWND hMainWnd;
    HWND hSubWnd;
    bool bProcessMessages;
    bool bShutdownRendering;
    int resultCode;
};

LRESULT CALLBACK WindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
