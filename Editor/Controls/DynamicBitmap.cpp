#include "Precompile.h"
#include "DynamicBitmap.h"

#include "Editor/ArtProvider.h"
#include "Foundation/Flags.h"

using namespace Helium;
using namespace Helium::Editor;


///////////////////////////////////////////////////////////////////////////////
IMPLEMENT_DYNAMIC_CLASS( DynamicBitmap, wxStaticBitmap );

DynamicBitmap::DynamicBitmap()
: wxStaticBitmap()
, m_CurrentState( wxButtonBase::State_Normal )
{
    Initialize();
}

DynamicBitmap::DynamicBitmap( wxWindow *parent, const wxBitmap& label, const wxPoint& pos, const wxSize& size, long style )
: wxStaticBitmap( parent, wxID_ANY, label, pos, size, style )
, m_CurrentState( wxButtonBase::State_Normal )
{
    Initialize();
}

DynamicBitmap::DynamicBitmap( wxWindow *parent, wxWindowID id, const wxBitmap& label, const wxPoint& pos, const wxSize& size, long style, const wxString& name )
: wxStaticBitmap( parent, id, label, pos, size, style, name )
, m_CurrentState( wxButtonBase::State_Normal )
{
    Initialize();
}

DynamicBitmap::~DynamicBitmap()
{
    Cleanup();
}

void DynamicBitmap::Initialize()
{
    for ( int32_t state = 0; state < wxButton::State_Max; state++ )
    {
        m_Bitmaps[state] = NULL;
        m_WasStateSetByUser[state] = false;
    }

    m_Bitmaps[m_CurrentState] = new wxBitmap( GetBitmap() );

    SetExtraStyle( GetExtraStyle() | wxWS_EX_PROCESS_UI_UPDATES );
    Connect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DynamicBitmap::OnUpdateUI ), NULL, this );
}

void DynamicBitmap::Cleanup()
{
    wxStaticBitmap::Free();

    for ( int32_t state = 0; state < wxButton::State_Max; state++ )
    {
        if ( m_Bitmaps[state] )
        {
            delete m_Bitmaps[state];
            m_Bitmaps[state] = NULL;
        }
    }

    Disconnect( wxEVT_UPDATE_UI, wxUpdateUIEventHandler( DynamicBitmap::OnUpdateUI ), NULL, this );
}

void DynamicBitmap::OnUpdateUI( wxUpdateUIEvent& event )
{
    bool needsUpdate = false;

    if ( !IsEnabled() )
    {
        if ( m_CurrentState != wxButtonBase::State_Disabled )
        {
            m_CurrentState = wxButtonBase::State_Disabled;
            needsUpdate = true;
        }
    }
    else
    {
        if ( IsMouseInWindow() )
        {
            if ( m_CurrentState != wxButtonBase::State_Current )
            {
                m_CurrentState = wxButtonBase::State_Current;
                needsUpdate = true;
            }
        }
        else
        {
            if ( m_CurrentState != wxButtonBase::State_Normal )
            {
                m_CurrentState = wxButtonBase::State_Normal;
                needsUpdate = true;
            }
        }
    }

    if ( needsUpdate )
    {
        RefreshBitmapFromState();
    }
}

void DynamicBitmap::RefreshBitmapFromState()
{
    wxButtonBase::State state = IsEnabled() ? m_CurrentState : wxButtonBase::State_Disabled;
    wxBitmap* currentBitmap = m_Bitmaps[ state ] ? m_Bitmaps[ state ] : m_Bitmaps[ wxButtonBase::State_Normal ];
    HELIUM_ASSERT( currentBitmap );
    SetImage( currentBitmap );
    Refresh();
}

bool DynamicBitmap::Enable( bool enable )
{
    bool result = __super::Enable( enable );
    if ( result )
    {
        RefreshBitmapFromState();
    }
    return result;
}

void DynamicBitmap::SetState( wxButtonBase::State state )
{
    if ( state != m_CurrentState )
    {
        m_CurrentState = state;
        RefreshBitmapFromState();
    }
}

wxButtonBase::State DynamicBitmap::GetState() const
{
    return m_CurrentState;
}

void DynamicBitmap::SetBitmap( const wxBitmap& bitmap )
{
    SetBitmap( bitmap, wxButtonBase::State_Normal );
}

//void DynamicBitmap::SetIcon( const wxIcon& icon )
//{
//    SetBitmap( icon, wxButtonBase::State_Normal );
//}

// See also: wxButton::DoSetBitmap and wxBitmapButton::DoSetBitmap
void DynamicBitmap::SetBitmap( const wxBitmap& bitmap, wxButtonBase::State state )
{
    if ( bitmap.IsOk() )
    {
        DoSetBitmap( bitmap, state );

        switch ( state )
        {
            default:
                // nothing special to do but include the default clause to
                // suppress gcc warnings
                //HELIUM_ASSERT();
                break;

            case wxButtonBase::State_Normal:
                m_WasStateSetByUser[wxButtonBase::State_Normal] = true;
#if wxUSE_IMAGE
                if ( !m_WasStateSetByUser[wxButtonBase::State_Disabled] )
                {
                    wxImage disabledImage( bitmap.ConvertToImage().ConvertToGreyscale() );
                    DoSetBitmap( disabledImage, wxButtonBase::State_Disabled );
                }
#endif // wxUSE_IMAGE
                break;

            case wxButtonBase::State_Current:
                m_WasStateSetByUser[wxButtonBase::State_Current] = true;
                break;

            case wxButtonBase::State_Pressed:
                m_WasStateSetByUser[wxButtonBase::State_Pressed] = true;
                break;

            case wxButtonBase::State_Disabled:
                m_WasStateSetByUser[wxButtonBase::State_Disabled] = true;
                break;

            case wxButtonBase::State_Focused:
                m_WasStateSetByUser[wxButtonBase::State_Focused] = true;

                // if the focus bitmap is specified but current one isn't, use
                // the focus bitmap for hovering as well if this is consistent
                // with the current Windows version look and feel
                //
                // rationale: this is compatible with the old wxGTK behaviour
                // and also makes it much easier to do "the right thing" for
                // all platforms (some of them, such as Windows XP, have "hot"
                // buttons while others don't)
                if ( !m_WasStateSetByUser[wxButtonBase::State_Current] )
                {
                    DoSetBitmap( bitmap, wxButtonBase::State_Current );
                }
                break;
        }
    }
}

// See also: wxButton::DoSetBitmap and wxBitmapButton::DoSetBitmap
void DynamicBitmap::DoSetBitmap( const wxBitmap& bitmap, wxButtonBase::State state )
{
    if ( m_Bitmaps[state] )
    {
        *m_Bitmaps[state] = bitmap;
    }
    else
    {
        m_Bitmaps[state] = new wxBitmap( bitmap );
    }
    //MakeOwnerDrawn();

    // it should be enough to only invalidate the best size when the normal
    // bitmap changes as all bitmaps assigned to the button should be of the
    // same size anyhow
    if ( state == wxButtonBase::State_Normal )
    {
        InvalidateBestSize();
    }

    RefreshBitmapFromState();
}

wxBitmap DynamicBitmap::GetBitmap( wxButtonBase::State state ) const
{
    return m_Bitmaps[state] ? wxBitmap( *m_Bitmaps[state] ) : wxBitmap();
}
