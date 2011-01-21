#include "Precompile.h"
#include "Button.h"

#include "Editor/ArtProvider.h"
#include "Foundation/Flags.h"


using namespace Helium;
using namespace Helium::Editor;

IMPLEMENT_DYNAMIC_CLASS( Button, wxButton );

Button::Button()
: wxButton()
{
}

Button::Button( wxWindow *parent, wxWindowID id, const wxString& label, const wxPoint& pos, const wxSize& size, long style, const wxValidator& validator, const wxString& name )
: wxButton( parent, id, label, pos, size, style | wxBU_NOTEXT, validator, name )
, m_ButtonOption( ButtonOptions::Default )
{
    //Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Button::OnLeftClick ), NULL, this );
    //Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( Button::OnToggle ), NULL, this );
}

Button::~Button()
{
    //Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Button::OnLeftClick ), NULL, this );
    //Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( Button::OnToggle ), NULL, this );
}

ButtonOption Button::GetButtonOptions() const
{
    return m_ButtonOption;
}

void Button::SetButtonOptions( const ButtonOption option )
{
    m_ButtonOption = option;
}

bool Button::GetValue() const
{
    return ::SendMessage( GetHwnd(), BM_GETCHECK, 0, 0 ) == BST_CHECKED;
}

void Button::SetValue( bool value )
{
    ::SendMessage( GetHwnd(), BM_SETCHECK, value, 0 );
    
    //if ( value != GetValue() )
    //{
    //    // set the button state
    //    // See also: http://msdn.microsoft.com/en-us/library/bb775988(VS.85).aspx
    //    // SendMessage BM_GETSTATE, BM_SETSTATE
    //    // Results:
    //    //   BST_UNCHECKED      0x0000
    //    //   BST_CHECKED
    //    //   BST_INDETERMINATE
    //    //   BST_PUSHED
    //    //   BST_FOCUS
    //    //   BST_HOT

    //    //LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT)wxdis;
    //    //HDC hdc = lpDIS->hDC;
    //    //UINT state = lpDIS->itemState;
    //    //bool selected = (state & ODS_SELECTED) != 0;
    //    //bool pushed = (::SendMessage(GetHwnd(), BM_GETSTATE, 0, 0) & BST_PUSHED) != 0;
    //    //::SendMessage( GetHwnd(), BM_SETSTATE, state, 0 );

    //    // set the button toggle/checked state:
    //    // SendMessage BM_GETCHECK, BM_SETCHECK
    //    //   BST_UNCHECKED      0x0000
    //    //   BST_CHECKED
    //    //   BST_INDETERMINATE
    //    ::SendMessage( GetHwnd(), BM_SETCHECK, ( value ? BST_CHECKED : BST_UNCHECKED ), 0 );    

    //    Refresh();
    //}
}

//const wxBitmap& Button::GetBitmapToggled() const 
//{ 
//    return m_BmpToggled; // virtual wxBitmap DoGetBitmap(State which) const;
//}

//void Button::SetBitmapToggled( const wxBitmap& bitmap ) 
//{ 
//    m_BmpToggled = bitmap; // virtual void DoSetBitmap(const wxBitmap& bitmap, State which);
//}

bool Button::SendClickOrToggleEvent()
{
    //SetValue( !GetValue() );

    if( HasFlags<ButtonOption>( m_ButtonOption, ButtonOptions::Toggle ) )
    {
        wxCommandEvent event( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, m_windowId );
        event.SetInt( GetValue() ? 1 : 0 );
        event.SetEventObject( this );
        ProcessCommand( event );
        return true;
    }
    else
    {
        wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
        event.SetEventObject(this);
        return ProcessCommand(event);
    }
}

void Button::Command( wxCommandEvent& event )
{
    if( HasFlags<ButtonOption>( m_ButtonOption, ButtonOptions::Toggle ) )
    {
        SetValue( !GetValue() );
        ProcessCommand( event );
    }
    else
    {
        ProcessCommand( event );
    }
}

bool Button::MSWCommand( WXUINT param, WXWORD id )
{
    bool processed = false;
    switch ( param )
    {
        // NOTE: Apparently older versions (NT 4?) of the common controls send
        //       BN_DOUBLECLICKED but not a second BN_CLICKED for owner-drawn
        //       buttons, so in order to send two EVT_BUTTON events we should
        //       catch both types.  Currently (Feb 2003) up-to-date versions of
        //       win98, win2k and winXP all send two BN_CLICKED messages for
        //       all button types, so we don't catch BN_DOUBLECLICKED anymore
        //       in order to not get 3 EVT_BUTTON events.  If this is a problem
        //       then we need to figure out which version of the comctl32 changed
        //       this behaviour and test for it.

        case 1:                     // message came from an accelerator
        case BN_CLICKED:            // normal buttons send this
            processed = SendClickOrToggleEvent();
            break;
    }

    return processed;
}

WXDWORD Button::MSWGetStyle( long style, WXDWORD *exstyle ) const
{
    // buttons never have an external border, they draw their own one
    WXDWORD msStyle = wxControl::MSWGetStyle( ( ( style & ~wxBORDER_MASK) | wxBORDER_NONE ), exstyle );

    // we must use WS_CLIPSIBLINGS with the buttons or they would draw over
    // each other in any resizeable dialog which has more than one button in
    // the bottom
    msStyle |= WS_CLIPSIBLINGS;

    // make it a toggle button
    msStyle |= BS_AUTOCHECKBOX | BS_PUSHLIKE | WS_TABSTOP;

    // don't use "else if" here: weird as it is, but you may combine wxBU_LEFT
    // and wxBU_RIGHT to get BS_CENTER!
    if ( style & wxBU_LEFT )
        msStyle |= BS_LEFT;
    if ( style & wxBU_RIGHT )
        msStyle |= BS_RIGHT;
    if ( style & wxBU_TOP )
        msStyle |= BS_TOP;
    if ( style & wxBU_BOTTOM )
        msStyle |= BS_BOTTOM;
#ifndef __WXWINCE__
    // flat 2d buttons
    if ( style & wxNO_BORDER )
        msStyle |= BS_FLAT;
#endif // __WXWINCE__

    return msStyle;
}