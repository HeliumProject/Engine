#include "Precompile.h"
#include "Editor/MessageDisplayer.h"
#include "Editor/Dialogs/YesNoAllDialog.h"

using namespace Helium;
using namespace Helium::Editor;

void MessageDisplayer::DisplayMessage( const Helium::MessageArgs& args )
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
                args.m_Result = MessageResults::Yes;
                break;
            case wxNO:
                args.m_Result = MessageResults::No;
                break;
            }
            break;
        }

    case MessageAppearances::YesNoCancel:
        {
            switch ( wxMessageBox( args.m_Message, args.m_Caption, wxYES_NO | wxCANCEL | wxCENTER | wxICON_QUESTION, m_Parent ) )
            {
            case wxYES:
                args.m_Result = MessageResults::Yes;
                break;
            case wxNO:
                args.m_Result = MessageResults::No;
                break;
            case wxCANCEL:
                args.m_Result = MessageResults::Cancel;
                break;
            }
            break;
        }

    case MessageAppearances::YesNoCancelToAll:
        {
            YesNoAllDialog dlg( m_Parent, args.m_Caption, args.m_Message );

            switch ( dlg.ShowModal() )
            {
            case wxID_YES:
                args.m_Result = MessageResults::Yes;
                break;
            case wxID_YESTOALL:
                args.m_Result = MessageResults::YesToAll;
                break;
            case wxID_NO:
                args.m_Result = MessageResults::No;
                break;
            case wxID_NOTOALL:
                args.m_Result = MessageResults::NoToAll;
                break;
            case wxID_CANCEL:
                args.m_Result = MessageResults::Cancel;
                break;
            }

            break;
        }
    }
}