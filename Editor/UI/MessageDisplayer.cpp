#include "Precompile.h"
#include "UI/MessageDisplayer.h"
#include "UI/YesNoAllDlg.h"

using namespace Helium;
using namespace Helium::Editor;

MessageResult MessageDisplayer::DisplayMessage( const Helium::MessageArgs& args )
{
    unsigned icon = 0;
    switch ( args.m_Priority )
    {
    case MessagePriorities::Question:
        icon = wxICON_QUESTION;
        break;

    case MessagePriorities::Warning:
        icon = wxICON_WARNING;
        break;

    case MessagePriorities::Error:
        icon = wxICON_ERROR;
        break;
    }

    switch ( args.m_Appearance )
    {
    case MessageAppearances::Ok:
        {
            wxMessageBox( args.m_Message, args.m_Caption, wxOK | wxCENTER | icon, m_Parent );
            break;
        }

    case MessageAppearances::YesNo:
        {
            switch ( wxMessageBox( args.m_Message, args.m_Caption, wxYES_NO | wxCENTER | wxICON_WARNING, m_Parent ) )
            {
            case wxYES:
                return MessageResults::Yes;
            case wxNO:
                return MessageResults::No;
            }
            break;
        }

    case MessageAppearances::YesNoCancel:
        {
            switch ( wxMessageBox( args.m_Message, args.m_Caption, wxYES_NO | wxCANCEL | wxCENTER | wxICON_QUESTION, m_Parent ) )
            {
            case wxYES:
                return MessageResults::Yes;
            case wxNO:
                return MessageResults::No;
            case wxCANCEL:
                return MessageResults::Cancel;
            }
            break;
        }

    case MessageAppearances::YesNoCancelToAll:
        {
            YesNoAllDlg dlg( m_Parent, args.m_Caption, args.m_Message );

            switch ( dlg.ShowModal() )
            {
            case wxID_YES:
                return MessageResults::Yes;
            case wxID_YESTOALL:
                return MessageResults::YesToAll;
            case wxID_NO:
                return MessageResults::No;
            case wxID_NOTOALL:
                return MessageResults::NoToAll;
            case wxID_CANCEL:
                return MessageResults::Cancel;
            }

            break;
        }
    }

    return MessageResults::Cancel;
}