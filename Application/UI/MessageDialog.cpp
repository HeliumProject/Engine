#include "MessageDialog.h"

namespace Nocturnal
{

#define STATUSSTRING_SIZE 2048

    /////////////////////////////////////////////////////////////////////////////
    void MessageDialog( wxWindow* parent, const tchar* msgFormat, ... )
    {
        va_list statusArgs;
        va_start( statusArgs, msgFormat ); 
        MessageDialog( parent, msgFormat, statusArgs );
        va_end( statusArgs ); 
    }


    /////////////////////////////////////////////////////////////////////////////
    void MessageDialog( wxWindow* parent, const MessageDialogIcon status, const tchar* msgFormat, ... )
    {
        va_list statusArgs;
        va_start( statusArgs, msgFormat ); 
        MessageDialog( parent, msgFormat, statusArgs, status );
        va_end( statusArgs ); 
    }


    /////////////////////////////////////////////////////////////////////////////
    void MessageDialog( wxWindow* parent, long style, const MessageDialogIcon status, const tchar* msgFormat, ... )
    {
        va_list statusArgs;
        va_start( statusArgs, msgFormat ); 
        MessageDialog( parent, msgFormat, statusArgs, status, style );
        va_end( statusArgs ); 
    }


    /////////////////////////////////////////////////////////////////////////////
    void MessageDialog( wxWindow* parent, const tchar* msgFormat, va_list statusArgs, const MessageDialogIcon status, long style )
    {
        if ( _tcscmp( msgFormat, TXT( "" ) ) == 0 )
            return;

        long displayStyle = style;

        i32 icon;
        wxString caption = wxT( "Notice" );

        switch( status )
        {
        default:
        case MessageDialogIcons::Info:
            icon = wxICON_INFORMATION;
            caption = wxT( "Notice" );
            break;

        case MessageDialogIcons::Error:
            icon = wxICON_ERROR;
            caption = wxT( "Error" );
            break;

        case MessageDialogIcons::Warning:
            icon = wxICON_EXCLAMATION;
            caption = wxT( "Warning" );
            break;
        }
        displayStyle |= icon;

        tchar msgBuffer[STATUSSTRING_SIZE];

        _vsntprintf_s( msgBuffer, sizeof(msgBuffer), _TRUNCATE, msgFormat, statusArgs );
        msgBuffer[ sizeof(msgBuffer) - 1] = 0; 

        wxMessageDialog msgDlg( parent, msgBuffer, caption, style );
        msgDlg.ShowModal();
    }

} // namespace Nocturnal