#include "Precompile.h"
#include "BrowserStatusBar.h"

#include "Application.h"

#include "Application/UI/ImageManager.h"

using namespace Luna;

BEGIN_EVENT_TABLE( BrowserStatusBar, wxStatusBar )
EVT_SIZE(BrowserStatusBar::OnSize)
END_EVENT_TABLE()

static const std::string s_TrackingInProgress = "Indexing assets...";
static const std::string s_TrackingUpdating = "Updating index...";
static const std::string s_TrackingComplete = "Indexing complete";
static const std::string s_TrackingFailed = "ERROR: Indexing failed!";

static const std::string s_TooltipInProgress = "The files on your computer are being scanned. Until this operation completes, searches may return partial results.";
static const std::string s_TooltipComplete = "";
static const std::string s_TooltipFailed = "The Asset Tracker failed to index your files. Check the console for additional info. You can try restarting Luna, report this if the problem persists.";

static const i32 s_ProgressMiliseconds = 2000; 

BrowserStatusBar::BrowserStatusBar( wxWindow *parent )
: wxStatusBar( parent, wxID_ANY )
, m_InitialIndexingCompleted( false )
, m_IndexingFailed( false )
, m_CurrentStatus( "Done" )
, m_StatusText( new wxStaticText( this, wxID_ANY, "Done" ) )
, m_Throbber( new wxAnimationCtrl( this, wxID_ANY, Nocturnal::GlobalImageManager().GetAnimation( "animation/process-working.png" ) ) )
, m_Message( new wxStaticText( this, wxID_ANY, s_TrackingInProgress ) )
{
    static const int widths[ FieldCount ] = { -1, 150 };

    SetFieldsCount( FieldCount );
    SetStatusWidths( FieldCount, widths );

    m_StatusText->Show();
    m_Throbber->Hide();
    m_Message->Hide();

    // Artificial size event to layout all child controls
    wxPostEvent( this, wxSizeEvent( wxDefaultSize, GetId() ) );

    m_ProgressTimer.SetOwner( this );
    Connect( m_ProgressTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler( BrowserStatusBar::OnTimer ), NULL, this );
}

BrowserStatusBar::~BrowserStatusBar()
{
    Disconnect( m_ProgressTimer.GetId(), wxEVT_TIMER, wxTimerEventHandler( BrowserStatusBar::OnTimer ), NULL, this );
}

///////////////////////////////////////////////////////////////////////////////
// Trims the specified string to the width (in pixels) requested.  The string
// will be prefixed with "...".
// 
bool BrowserStatusBar::TrimString( wxString& strText, wxStaticText* textField, int maxWidth, bool prefix )
{
    int curWidth;
    textField->GetTextExtent( strText, &curWidth, NULL );

    if ( strText.size() < 1 || curWidth <= maxWidth )
    {
        // nothing to trim
        return false;
    }

    // try to find a good starting point
    int avgCharWidth = curWidth / ( int ) strText.size();
    int testLen = ( maxWidth / avgCharWidth );

    wxString testStr = strText.Mid( 0, testLen ) + "...";
    textField->GetTextExtent( testStr, &curWidth, NULL );

    int low = 0;
    int high = (int) strText.size();
    while( curWidth != maxWidth && low < high )
    {
        if ( curWidth < maxWidth )
        {
            low = testLen;
            testLen = ( high + low ) / 2;
        }
        else if ( curWidth > maxWidth )
        {
            high = testLen;
            testLen = ( high + low ) / 2;
        }
        else
        {
            // we found an exact match... weird
            break;
        }

        if ( testLen + 1 == high )
        {
            break;
        }

        testStr = strText.Mid( 0, testLen ) + "...";
        textField->GetTextExtent( testStr, &curWidth, NULL );
    }

    if ( testLen > 3 )
    {
        if ( prefix )
        {
            strText = "..." + strText.Mid( strText.length() - testLen );
        }
        else
        {
            strText = strText.Mid( 0, testLen ) + "...";
        }
    }
    else
    {
        strText = "...";
    }
    return true;
}

// From Help on SB_SETTEXT
// The text for each part is limited to 127 characters. 
void BrowserStatusBar::SetStatusText( const wxString& strText, int nField )
{
    if ( nField == FieldMain )
    {
        wxRect rect;
        GetFieldRect( FieldMain, rect );

        m_CurrentStatus = strText;
        wxString newText = m_CurrentStatus;
        TrimString( newText, m_StatusText, rect.GetWidth() - 10 );
        m_StatusText->SetLabel( newText );
        m_StatusText->SetToolTip( m_CurrentStatus );
    }
}

void BrowserStatusBar::OnSize( wxSizeEvent& args )
{
    const u32 border = 2;
    const u32 spaceBetweenItems = 4;

    // FieldMain:
    {
        wxRect rect;
        GetFieldRect( FieldMain, rect );

        // Layout the controls from left to right, track the left edge as we go
        u32 x = rect.x + border;

        wxSize statusSize = m_StatusText->GetSize();
        m_StatusText->Move( x, rect.y + ( rect.height - statusSize.y ) / 2 );

        wxString newText = m_CurrentStatus;
        TrimString( newText, m_StatusText, rect.GetWidth() - 10 );
        m_StatusText->SetLabel( newText );
        m_StatusText->SetToolTip( m_CurrentStatus );
    }

    // FieldAssetTracker:
    // Throbber and indexing status message
    {
        wxRect rect;
        GetFieldRect( FieldAssetTracker, rect );

        // Layout the controls from left to right, track the left edge as we go
        u32 x = rect.x + border;

        // Throbber
        wxSize throbberSize = m_Throbber->GetSize();
        m_Throbber->Move( x, rect.y + ( rect.height - throbberSize.y ) / 2 );
        x+= throbberSize.x + spaceBetweenItems;

        // Message text
        wxSize msgSize = m_Message->GetSize();
        m_Message->Move( x, rect.y + ( rect.height - msgSize.y ) / 2 );
        x+= msgSize.x + spaceBetweenItems;
    }

    args.Skip();
}

void BrowserStatusBar::OnTimer( wxTimerEvent& args )
{
}

