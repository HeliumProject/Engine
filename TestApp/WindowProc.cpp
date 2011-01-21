//----------------------------------------------------------------------------------------------------------------------
// WindowProc.cpp
//
// Copyright (C) 2010 WhiteMoon Dreams, Inc.
// All Rights Reserved
//----------------------------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "WindowProc.h"

LRESULT CALLBACK WindowProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam )
{
    HELIUM_ASSERT( hWnd );

    switch( msg )
    {
        case WM_DESTROY:
        {
            WindowData* pData = reinterpret_cast< WindowData* >( GetWindowLongPtr( hWnd, GWLP_USERDATA ) );
            HELIUM_ASSERT( pData );
            pData->bShutdownRendering = true;

            PostQuitMessage( 0 );

            if( pData->hMainWnd )
            {
                if( pData->hMainWnd == hWnd )
                {
                    pData->hMainWnd = NULL;
                }
                else
                {
                    DestroyWindow( pData->hMainWnd );
                }
            }

            if( pData->hSubWnd )
            {
                if( pData->hSubWnd == hWnd )
                {
                    pData->hSubWnd = NULL;
                }
                else
                {
                    DestroyWindow( pData->hSubWnd );
                }
            }

            return 0;
        }
    }

    return DefWindowProc( hWnd, msg, wParam, lParam );
}
