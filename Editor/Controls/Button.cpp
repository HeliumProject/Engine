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
: wxButton( parent, id, label, pos, size, style, validator, name )
, m_ButtonOption( ButtonOptions::Default )
{
    for ( int32_t index = 0; index < wxButton::State_Max; index++ )
    {
        m_IsStateBitmapSetByUser[index] = false;
    }
}

Button::~Button()
{
}

ButtonOption Button::GetButtonOptions() const
{
    return m_ButtonOption;
}

void Button::SetButtonOptions( const ButtonOption option )
{
    if ( option != m_ButtonOption )
    {
        m_ButtonOption = option;

        long wxOldStyle = GetWindowStyle();
        long wxNewStyle = wxOldStyle;

        // HideLabel
        if ( HasFlags<ButtonOption>( m_ButtonOption, ButtonOptions::HideLabel )
            && !HasFlags<long>( wxOldStyle, wxBU_NOTEXT ) )
        {
            wxNewStyle |= wxBU_NOTEXT;
        }
        else if ( !HasFlags<ButtonOption>( m_ButtonOption, ButtonOptions::HideLabel )
            && HasFlags<long>( wxOldStyle, wxBU_NOTEXT ) )
        {
            wxNewStyle &= ~wxBU_NOTEXT;
        }

        // Reset the wx window style flags
        if ( wxNewStyle != wxOldStyle )
        {
            wxControl::SetWindowStyle( wxNewStyle );
        }
        
        /////////////////////////

        // update the flags that wont be updated with a call to MSWGetStyle
        long mswOldStyle = ::GetWindowLong( GetHwnd(), GWL_STYLE );
        long mswNewStyle = mswOldStyle;

        // make it a toggle button
        if ( HasFlags<ButtonOption>( m_ButtonOption, ButtonOptions::Toggle ) )
        {
            mswNewStyle |= ( BS_AUTOCHECKBOX | BS_PUSHLIKE | WS_TABSTOP );
        }
        else
        {
            mswNewStyle &= ~( BS_AUTOCHECKBOX | BS_PUSHLIKE | WS_TABSTOP );
        }

        // Reset the window style
        if ( mswNewStyle != mswOldStyle )
        {
            ::SetWindowLong( GetHwnd(), GWL_STYLE, mswNewStyle );
        }

        Refresh( true );
    }
}

bool Button::GetValue() const
{
    return ::SendMessage( GetHwnd(), BM_GETCHECK, 0, 0 ) == BST_CHECKED;
}

void Button::SetValue( bool value )
{
    ::SendMessage( GetHwnd(), BM_SETCHECK, value, 0 );
}

bool Button::SendClickOrToggleEvent()
{
    if( HasFlags<ButtonOption>( m_ButtonOption, ButtonOptions::Toggle ) )
    {
        wxCommandEvent event( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, GetId() );
        event.SetInt( GetValue() ? 1 : 0 );
        event.SetEventObject( this );
        return ProcessCommand( event );
    }
    else
    {
        wxCommandEvent event( wxEVT_COMMAND_BUTTON_CLICKED, GetId() );
        event.SetEventObject( this );
        return ProcessCommand( event );
    }
}

void Button::Command( wxCommandEvent& event )
{
    if( HasFlags<ButtonOption>( m_ButtonOption, ButtonOptions::Toggle ) )
    {
        SetValue( !GetValue() );
    }

    ProcessCommand( event );
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
    if ( HasFlags<ButtonOption>( m_ButtonOption, ButtonOptions::Toggle ) )
    {
        msStyle |= ( BS_AUTOCHECKBOX | BS_PUSHLIKE | WS_TABSTOP );
    }

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

// See also: wxBitmapButton::DoSetBitmap
void Button::DoSetBitmap( const wxBitmap& bitmap, State which )
{
    if ( bitmap.IsOk() )
    {
        __super::DoSetBitmap( bitmap, which );

        switch ( which )
        {
            default:
                // nothing special to do but include the default clause to
                // suppress gcc warnings
                //HELIUM_ASSERT();
                break;

            case State_Normal:
                m_IsStateBitmapSetByUser[State_Normal] = true;
#if wxUSE_IMAGE
                if ( !m_IsStateBitmapSetByUser[State_Disabled] )
                {
                    wxImage disabledImage( bitmap.ConvertToImage().ConvertToGreyscale() );
                    __super::DoSetBitmap( disabledImage, State_Disabled );
                }
#endif // wxUSE_IMAGE
                break;

            case State_Current:
                m_IsStateBitmapSetByUser[State_Current] = true;
                break;

            case State_Pressed:
                m_IsStateBitmapSetByUser[State_Pressed] = true;
                break;

            case State_Disabled:
                m_IsStateBitmapSetByUser[State_Disabled] = true;
                break;

            case State_Focused:
                m_IsStateBitmapSetByUser[State_Focused] = true;

                // if the focus bitmap is specified but current one isn't, use
                // the focus bitmap for hovering as well if this is consistent
                // with the current Windows version look and feel
                //
                // rationale: this is compatible with the old wxGTK behaviour
                // and also makes it much easier to do "the right thing" for
                // all platforms (some of them, such as Windows XP, have "hot"
                // buttons while others don't)
                if ( !m_IsStateBitmapSetByUser[State_Current] )
                {
                    __super::DoSetBitmap( bitmap, State_Current );
                }
                break;
        }
    }
}