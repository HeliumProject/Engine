//----------------------------------------------------------------------------------------------------------------------
// WindowProc.h
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

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
