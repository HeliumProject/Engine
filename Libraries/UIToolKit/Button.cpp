#include "stdafx.h"

#include "Button.h"
#include "wx/bmpbuttn.h"
#include "wx/msw/private.h"
#include "tmschema.h"
#include "wx/msw/uxtheme.h"
#include "wx/tglbtn.h"

namespace UIToolKit
{
  //-----------------------------------------------------------
  // macros
  //-----------------------------------------------------------
  IMPLEMENT_DYNAMIC_CLASS(Button, wxBitmapButton);

  // ============================================================================
  // implementation
  // ============================================================================
  BEGIN_EVENT_TABLE(Button, wxBitmapButton)
    END_EVENT_TABLE();


#if wxUSE_UXTHEME
  static
    void MSWDrawXPBackground(wxButton *button, WXDRAWITEMSTRUCT *wxdis, wxBufferedDC& bufferDC )
  {
    LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT)wxdis;
    HDC hdc = (HDC)bufferDC.GetHDC();
    UINT state = lpDIS->itemState;
    RECT rectBtn;
    CopyRect(&rectBtn, &lpDIS->rcItem);

    wxUxThemeHandle theme(button, L"BUTTON");
    int iState;

    if ( state & ODS_SELECTED || state & ODS_CHECKED )
    {
      iState = PBS_PRESSED;
    }
    else if ( button->HasCapture() || button->IsMouseInWindow() )
    {
      iState = PBS_HOT;
    }
    else if ( state & ODS_FOCUS )
    {
      iState = PBS_DEFAULTED;
    }
    else if ( state & ODS_DISABLED )
    {
      iState = PBS_DISABLED;
    }
    else
    {
      iState = PBS_NORMAL;
    }

    // draw parent background if needed
    if ( wxUxThemeEngine::Get()->IsThemeBackgroundPartiallyTransparent(theme,
      BP_PUSHBUTTON,
      iState) )
    {
      wxUxThemeEngine::Get()->DrawThemeParentBackground(GetHwndOf(button), hdc, &rectBtn);
    }

    // draw background
    wxUxThemeEngine::Get()->DrawThemeBackground(theme, hdc, BP_PUSHBUTTON, iState,
      &rectBtn, NULL);

    // calculate content area margins
    MARGINS margins;
    wxUxThemeEngine::Get()->GetThemeMargins(theme, hdc, BP_PUSHBUTTON, iState,
      TMT_CONTENTMARGINS, &rectBtn, &margins);
    RECT rectClient;
    ::CopyRect(&rectClient, &rectBtn);
    ::InflateRect(&rectClient, -margins.cxLeftWidth, -margins.cyTopHeight);

    // if focused and !nofocus rect
    if ( (state & ODS_FOCUS) && !(state & ODS_NOFOCUSRECT) )
    {
      DrawFocusRect(hdc, &rectClient);
    }

    if ( button->UseBgCol() )
    {
      COLORREF colBg = wxColourToRGB(button->GetBackgroundColour());
      HBRUSH hbrushBackground = ::CreateSolidBrush(colBg);

      // don't overwrite the focus rect
      ::InflateRect(&rectClient, -1, -1);
      FillRect(hdc, &rectClient, hbrushBackground);
      ::DeleteObject(hbrushBackground);
    }
  }
#endif // wxUSE_UXTHEME



  static void DrawButtonText(HDC hdc,
    RECT *pRect,
    const wxString& text,
    COLORREF col)
  {
    COLORREF colOld = SetTextColor(hdc, col);
    int modeOld = SetBkMode(hdc, TRANSPARENT);

    if ( text.find(_T('\n')) != wxString::npos )
    {
      // draw multiline label

      // first we need to compute its bounding rect
      RECT rc;
      ::CopyRect(&rc, pRect);
      ::DrawText(hdc, text, (int)text.length(), &rc, DT_CENTER | DT_CALCRECT);

      // now center this rect inside the entire button area
      const LONG w = rc.right - rc.left;
      const LONG h = rc.bottom - rc.top;
      rc.left = (pRect->right - pRect->left)/2 - w/2;
      rc.right = rc.left+w;
      rc.top = (pRect->bottom - pRect->top)/2 - h/2;
      rc.bottom = rc.top+h;

      ::DrawText(hdc, text, (int)text.length(), &rc, DT_CENTER);
    }
    else // single line label
    {
      // Note: we must have DT_SINGLELINE for DT_VCENTER to work.
      ::DrawText(hdc, text, (int)text.length(), pRect,
        DT_SINGLELINE | DT_CENTER | DT_VCENTER);
    }

    SetBkMode(hdc, modeOld);
    SetTextColor(hdc, colOld);
  }

  Button::Button(wxWindow *parent,
    wxWindowID id,
    const wxBitmap& bitmap,
    const wxPoint& pos,
    const wxSize& size,
    long style,
    const wxValidator& validator,
    const wxString& name) 
    : wxBitmapButton( parent, id, bitmap, pos, size, style, validator, name )
    , m_ButtonStyle         ( ButtonStyles::BU_DEFAULT )
    , m_ToggleState     ( false )
    , m_BitmapAlignment ( BitmapAlignments::BU_BMP_LEFT )
    , m_GradientFill ( false )
    , m_Padding( 0 )
  {
    SetMargins(0);

    this->Connect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Button::OnLeftClick ), NULL, this );
    this->Connect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( Button::OnToggle ), NULL, this );
    this->Connect( wxEVT_ERASE_BACKGROUND, wxEraseEventHandler ( Button::OnEraseBackground ), NULL, this );
  }

  bool Button::CheckFlagValues( UINT src, UINT flags ) const
  {
    return ((src & flags) == flags);
  }

  bool Button::MSWOnDraw(WXDRAWITEMSTRUCT *item)
  {
    // if we dont have either style set, no idea what your doing
    if( !CheckFlagValues(m_ButtonStyle, ButtonStyles::BU_LABEL) 
      && !CheckFlagValues(m_ButtonStyle, ButtonStyles::BU_BITMAP)
      && !CheckFlagValues(m_ButtonStyle, ButtonStyles::BU_TOGGLE) )
    {
      wxASSERT(!"Menu Button Style has not been set for this button");
      return false;
    }

    //===================== Set up some variables =============
    LPDRAWITEMSTRUCT lpDIS = (LPDRAWITEMSTRUCT) item;
    HDC hDC                = lpDIS->hDC;
    UINT state             = lpDIS->itemState;
    bool isSelected        = (state & ODS_SELECTED) != 0;
    bool autoDraw          = (GetWindowStyleFlag() & wxBU_AUTODRAW) != 0;
    wxBitmap *bitmap       = BitmapForButtonState( );
    bool noMargins         = !( m_Margins.left || m_Margins.right || m_Margins.top || m_Margins.bottom );

    //================== Variables we use ==============

    // Dimensions for the control area
    int x      = lpDIS->rcItem.left;
    int y      = lpDIS->rcItem.top;
    int width  = lpDIS->rcItem.right - x;
    int height = lpDIS->rcItem.bottom - y;

    int xText = 0,  yText = 0;              // position to draw the text at
    int xBmp = x,   yBmp = y;               // position to draw bitmap if it is not the background

    wxDCTemp backDC( hDC );
    wxBitmap tmpBmp( width, height );;
    wxBufferedDC dst( &backDC, tmpBmp );    // back surface we draw to for fake double buffering

    // dimensions for bitmap and text
    int wBmp   = bitmap->GetWidth();
    int hBmp   = bitmap->GetHeight();
    int wText   = 0;
    int hText   = 0;

    if( m_ButtonStyle & ButtonStyles::BU_LABEL )
    {
      GetTextExtent(GetLabelText(GetLabel()), &wText, &hText);
    }

    //================== BACKGROUND: PASS 1/2 ====================
    if( m_GradientFill && noMargins )
    {
      wxRect fullRect(x,y,width,height);

      dst.GradientFillLinear( fullRect, m_GradientColor1, m_GradientColor2, m_GradientDirection );   
    }
    // we are drawing a bitmap which takes up the entire background
    else if(CheckFlagValues(m_ButtonStyle, ButtonStyles::BU_BMP_BACKGROUND | ButtonStyles::BU_BMP_BACKGROUND_STRETCH) 
      && noMargins
      && bitmap->Ok() )
    {
      DrawBitmapBackground( dst, bitmap, wxRect(x,y,width,height) );
    }

#if wxUSE_UXTHEME
    else if( autoDraw )
    {
      if ( CheckFlagValues( m_ButtonStyle, ButtonStyles::BU_TOGGLE ) && m_ToggleState )
      {
        lpDIS->itemState |= ODS_CHECKED;
      }
      MSWDrawXPBackground(this, item, dst); 
    }
#endif //wxUSE_UXTHEME  

    // we may not be  using all the background. we need to draw filler            
    else
    {
      wxPen origPen = dst.GetPen( );
      wxBrush origBrush = dst.GetBrush();

      wxColour bgColor = GetBackgroundColour();
      if ( CheckFlagValues( m_ButtonStyle, ButtonStyles::BU_TOGGLE ) && m_ToggleState )
      {
        bgColor = wxSystemSettings::GetColour( wxSYS_COLOUR_BTNSHADOW );
      }

      wxPen outline( GetBackgroundColour() );
      wxBrush fill( bgColor );

      dst.SetPen( outline );                  // current pen is used for DrawRectangle outline
      dst.SetBrush( fill );                   // current brush is used for DrawRectangle fill color

      dst.DrawRectangle( x, y, width, height );

      dst.SetPen( origPen );
      dst.SetBrush( origBrush );
    }

    //================== BACKGROUND: PASS 2/2 ====================
    if( !noMargins && m_GradientFill )
    {
      wxRect rect( x + m_Margins.left ,
        y + m_Margins.top ,
        width - m_Margins.right ,
        height - m_Margins.left );

      dst.GradientFillLinear( rect, m_GradientColor1, m_GradientColor2, m_GradientDirection );   
    }

    if( !noMargins && CheckFlagValues( m_ButtonStyle, ButtonStyles::BU_BMP_BACKGROUND ) )
    {
      wxRect rect( x + m_Margins.left ,
        y + m_Margins.top ,
        width - (m_Margins.right + m_Margins.left),
        height - (m_Margins.top + m_Margins.bottom) );

      DrawBitmapBackground( dst, bitmap, rect );
    }

    //================== ALIGN BMP && TEXT ====================
    if( CheckFlagValues(m_ButtonStyle, ButtonStyles::BU_BITMAP|ButtonStyles::BU_LABEL) && !(m_ButtonStyle & ButtonStyles::BU_BMP_BACKGROUND) )    // has both
    {
      // Left / Right alignment
      if( ( m_BitmapAlignment == BitmapAlignments::BU_BMP_LEFT ) 
        || ( m_BitmapAlignment == BitmapAlignments::BU_BMP_RIGHT ) )
      {
        if( m_BitmapAlignment == BitmapAlignments::BU_BMP_LEFT )
        {
          xBmp   = x + m_Margins.left;
          xText  = xBmp + wBmp + m_Padding;
        }
        else if( m_BitmapAlignment == BitmapAlignments::BU_BMP_RIGHT )
        {
          xText = x + m_Margins.left;
          xBmp = xText + wText + m_Padding;
        }

        if( CheckFlagValues(m_ButtonStyle, ButtonStyles::BU_CENTER_VERTICAL) )
        {
          yBmp  = y + ( height - hBmp ) / 2; 
          yText = y + ( height - hText ) / 2;
        }
        else
        {
          yBmp = y + m_Margins.top; 
          yText = y + m_Margins.top;
        }

        if( CheckFlagValues(m_ButtonStyle, ButtonStyles::BU_CENTER_HORIZONTAL) )
        {
          int widthBoth = wBmp + wText + m_Padding;
          int xBegin = x + (width - widthBoth ) / 2;

          if( m_BitmapAlignment == BitmapAlignments::BU_BMP_LEFT )
          {
            xBmp = xBegin;
            xText = xBmp + wBmp + m_Padding;
          }
          else
          {
            xText = xBegin;
            xBmp = xText + wText + m_Padding;
          }
        }
      }
      else if ( (m_BitmapAlignment == BitmapAlignments::BU_BMP_TOP ) 
        || ( m_BitmapAlignment == BitmapAlignments::BU_BMP_BOTTOM ) )
      {
        if( m_BitmapAlignment == BitmapAlignments::BU_BMP_TOP )
        {
          yBmp  = y + m_Margins.top;
          yText = yBmp + hBmp;
        }
        else if( m_BitmapAlignment == BitmapAlignments::BU_BMP_BOTTOM )
        {
          yText = y + m_Margins.top;
          yBmp  = yText + hText;
        }

        if( CheckFlagValues(m_ButtonStyle, ButtonStyles::BU_CENTER_HORIZONTAL) )
        {
          xBmp  = x + ( width - wBmp ) / 2;
          xText = x + ( width - wText ) / 2;
        }
        else
        {
          xBmp  = x + m_Margins.left;
          xText = x + m_Margins.left;
        }

        if( CheckFlagValues(m_ButtonStyle, ButtonStyles::BU_CENTER_VERTICAL) )
        {
          int heightBoth = hBmp + hText + m_Padding;
          int yBegin = y + (height - heightBoth ) / 2;

          if( m_BitmapAlignment == BitmapAlignments::BU_BMP_TOP )
          {
            yBmp = yBegin;
            yText = yBmp + hBmp + m_Padding;
          }
          else
          {
            yText = yBegin;
            yBmp = yText + hText + m_Padding;
          }
        }
      }
    }
    else if( CheckFlagValues(m_ButtonStyle, ButtonStyles::BU_LABEL) )
    {
      xText = CheckFlagValues(m_ButtonStyle, ButtonStyles::BU_CENTER_HORIZONTAL) ? (x + ( width - wText ) * 0.5f ) : (x + m_Margins.left);
      yText = CheckFlagValues(m_ButtonStyle, ButtonStyles::BU_CENTER_VERTICAL) ?  (y + (height - hText) * 0.5f ) : (y + m_Margins.top);
    }
    else if ( CheckFlagValues(m_ButtonStyle, ButtonStyles::BU_BITMAP) && !(m_ButtonStyle & ButtonStyles::BU_BMP_BACKGROUND) )
    {
      xBmp = CheckFlagValues(m_ButtonStyle, ButtonStyles::BU_CENTER_HORIZONTAL) ? (x + ( width - wBmp ) * 0.5f ) : (x + m_Margins.left);
      yBmp = CheckFlagValues( m_ButtonStyle, ButtonStyles::BU_CENTER_VERTICAL ) ? (y + (height - hBmp) * 0.5f ) : (y + m_Margins.top );
    }
    else    // Default case
    {
      xBmp = x + m_Margins.left;
      yBmp = y + m_Margins.top; 
      xText = ( m_ButtonStyle & ButtonStyles::BU_BITMAP ) ?  xBmp + wBmp : x + m_Margins.left;
      yText = y + m_Margins.top;
    }

    //================== DRAW BMP AND TEXT ====================
    if( CheckFlagValues( m_ButtonStyle, ButtonStyles::BU_BITMAP)
      && !(CheckFlagValues(m_ButtonStyle, ButtonStyles::BU_BMP_BACKGROUND ) ) )
    {
      dst.DrawBitmap(*bitmap, xBmp, yBmp, true);    //draw a bitmap on the button
    }

    if( m_ButtonStyle & ButtonStyles::BU_LABEL )
    {
      wxFont oldFont = GetFont();
      dst.SetFont( GetFont() );

      COLORREF colFg = wxColourToRGB(GetForegroundColour());
      wxString label = GetLabel();

      dst.DrawText( GetLabel(), xText, yText );     // draw a label on the button

      dst.SetFont( oldFont );
    }

    // If we are disabled, draw an overlay
    if( autoDraw && !IsEnabled())
    {
      DrawButtonDisable((WXHDC) hDC,
        lpDIS->rcItem.left ,
        lpDIS->rcItem.top ,
        lpDIS->rcItem.right , 
        lpDIS->rcItem.bottom ,
        true);
    }

    if ( autoDraw && (state & ODS_FOCUS) && !(state & ODS_NOFOCUSRECT) )
    {
      RECT rectClient = { lpDIS->rcItem.left ,  lpDIS->rcItem.top ,
        lpDIS->rcItem.right , lpDIS->rcItem.bottom };
      DrawFocusRect(hDC, &rectClient);
    }

    return true;
  }

  void Button::ResizeImage( wxBitmap& bitmap, int width, int height )
  {
    if( !bitmap.Ok() )
      return;

    if( width <= 0 )
    {
      width = 1;
    }

    if( height <= 0 )
    {
      height = 1;
    }

    wxImage img = bitmap.ConvertToImage( );
    img.Rescale( width, height );

    bitmap = wxBitmap( img );
  }

  void Button::SetMargins( int allSides )
  {
    SetMargins( allSides, allSides, allSides, allSides );
  }


  void Button::SetMargins( int left, int top, int right, int bottom )
  {
    m_Margins.top = top;
    m_Margins.left = left;
    m_Margins.right = right;
    m_Margins.bottom = bottom;
  }

  wxSize Button::DoGetBestSize( ) const
  {
    int sumWidth  = 0,    sumHeight = 0;
    int wBmp      = 0,    hBmp = 0;
    int wText     = 0,    hText = 0;

    // we are using the label
    if( m_ButtonStyle & ButtonStyles::BU_LABEL )
    {
      GetTextExtent(GetLabelText(GetLabel()), &wText, &hText);
    }

    // we are using the bitmap
    if( m_ButtonStyle & ButtonStyles::BU_BITMAP )
    {
      wxBitmap *bitmap = BitmapForButtonState();

      if( bitmap->Ok() )
      {  
        wBmp = bitmap->GetWidth();
        hBmp = bitmap->GetHeight();
      }
    }

    // check the alignment of the bitmap
    if( m_BitmapAlignment == BitmapAlignments::BU_BMP_TOP
      || m_BitmapAlignment == BitmapAlignments::BU_BMP_BOTTOM )
    {
      sumHeight = hText + hBmp;
      sumWidth = (wText > wBmp) ? wText : wBmp;
    }
    else if( m_BitmapAlignment == BitmapAlignments::BU_BMP_LEFT
      || m_BitmapAlignment == BitmapAlignments::BU_BMP_RIGHT )
    {
      sumWidth = wText + wBmp;
      sumHeight = (hText > hBmp) ? hText : hBmp;
    }
    else if( m_ButtonStyle & ButtonStyles::BU_BMP_BACKGROUND )
    {
      sumWidth = wText;
      sumHeight = hText;
    }
    else
    {      
      wxASSERT(!"THIS STYLE IS NOT CURRENTLY SUPPORTED");
      return wxSize(-1,-1);   
    }

    // add the size of the margins to the button
    sumWidth += m_Margins.left + m_Margins.right;
    sumHeight += m_Margins.top + m_Margins.bottom;

    if( m_Padding && CheckFlagValues( m_ButtonStyle, ButtonStyles::BU_BITMAP | ButtonStyles::BU_LABEL ) )
    {
      if( !CheckFlagValues( m_ButtonStyle, ButtonStyles::BU_BMP_BACKGROUND ) )
      {
        if( m_BitmapAlignment == BitmapAlignments::BU_BMP_LEFT
          || m_BitmapAlignment == BitmapAlignments::BU_BMP_RIGHT )
        {
          sumWidth += m_Padding;
        }
        else
        {
          sumHeight += m_Padding;
        }
      }
    }

    return wxSize( sumWidth, sumHeight );
  }

  wxBitmap* Button::BitmapForButtonState( ) const
  {
    wxBitmap *bitmap;
    wxMouseState mouse = wxGetMouseState();

    if ( (mouse.LeftDown() && ( HasCapture() || IsMouseInWindow() ) ) && m_bmpSelected.Ok() )
    {
      bitmap = (wxBitmap*) &m_bmpSelected;
    }
    else if (  IsMouseInWindow() && m_bmpHover.Ok() ) 
    {
      bitmap = (wxBitmap*) &m_bmpHover;    
    }
    else if ( HasCapture() && m_bmpFocus.Ok() )   //TODO determine if we have the focus
    {
      bitmap = (wxBitmap*) &m_bmpFocus;
    }
    else if ( !IsEnabled() && m_bmpDisabled.Ok())
    {
      bitmap = (wxBitmap*) &m_bmpDisabled;
    }
    else if ( CheckFlagValues(m_ButtonStyle, ButtonStyles::BU_TOGGLE ) && m_ToggleState && m_BmpToggled.Ok() )
    {
      bitmap = (wxBitmap*) &m_BmpToggled;
    }
    else
    {
      bitmap = (wxBitmap*) &m_bmpNormal;
    }

    return bitmap;
  }

  Button::~Button(void)
  {
    this->Disconnect( wxEVT_ERASE_BACKGROUND, wxEraseEventHandler( Button::OnEraseBackground ), NULL, this );
    this->Disconnect( wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, wxCommandEventHandler( Button::OnToggle ), NULL, this );
    this->Disconnect( wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler( Button::OnLeftClick ), NULL, this );
  }

  const wxBitmap& Button::GetBitmapToggled( ) const 
  { 
    return m_BmpToggled; 
  }

  void Button::SetBitmapToggled( const wxBitmap& bitmap ) 
  { 
    m_BmpToggled = bitmap; 
  }

  bool Button::GetToggleState( ) const 
  { 
    return m_ToggleState; 
  }

  void Button::SetToggleState( bool val ) 
  { 
    if ( val != m_ToggleState )
    {
      m_ToggleState = val; 
      Refresh();
    }
  }

  void Button::OnToggle( wxCommandEvent& event )
  {
    SetToggleState( !GetToggleState() );
    event.Skip();
  }

  void Button::OnLeftClick( wxCommandEvent& event )
  {
    // we only send this event if we are a toggle button
    if( CheckFlagValues( m_ButtonStyle, ButtonStyles::BU_TOGGLE ) )
    {
      wxCommandEvent event(wxEVT_COMMAND_TOGGLEBUTTON_CLICKED, m_windowId);
      event.SetInt(GetToggleState() ? 1 : 0);
      event.SetEventObject(this);
      ProcessCommand(event);
    }

    event.Skip();
  }

  void Button::SetBitmapAlignment( BitmapAlignments::BitmapAlignment align )
  {
    if( align != BitmapAlignments::BU_BMP_LEFT 
      && align != BitmapAlignments::BU_BMP_TOP
      && align != BitmapAlignments::BU_BMP_RIGHT
      && align != BitmapAlignments::BU_BMP_BOTTOM )
    {
      align = BitmapAlignments::BU_BMP_LEFT;
    }

    m_BitmapAlignment = align;
  }

  BitmapAlignments::BitmapAlignment Button::GetBitmapAlignment( ) const
  {
    return m_BitmapAlignment;
  }

  void Button::SetButtonStyle( UINT style ) 
  { 
    m_ButtonStyle = (ButtonStyles::ButtonStyle)style; 
  }

  UINT Button::GetButtonStyle( ) const 
  { 
    return (UINT)m_ButtonStyle; 
  }

  void Button::Clamp( int& var, int min, int max )
  {
    if( var < min ) var = min;
    if( var > max ) var = max;
  }

  void Button::EnableGradientFill( bool fill ) 
  { 
    m_GradientFill = fill; 
  }

  void Button::SetGradientFill( const wxColor& c1, const wxColor& c2, wxDirection dir )
  {
    m_GradientColor1 = c1;
    m_GradientColor2 = c2;
    m_GradientDirection = dir;
  }

  // To do flicker free drawing, we are capturing this event so that
  // we can ignore it. = )
  void Button::OnEraseBackground( wxEraseEvent& event )
  {
    return;
  }

  void Button::DrawBitmapBackground( wxBufferedDC& bufferDC, wxBitmap* bitmap, wxRect& rect )
  {
    wxBitmap tmpDisplayBmp( *bitmap );
    int wBmp = bitmap->GetWidth();
    int hBmp = bitmap->GetHeight();
    int width = rect.GetWidth();
    int height = rect.GetHeight();

    int xBmp = 0;
    int yBmp = 0;
    int x = rect.GetLeft();
    int y = rect.GetTop();

    if( CheckFlagValues(m_ButtonStyle, ButtonStyles::BU_BMP_BACKGROUND_STRETCH) && bitmap->Ok() )
    {
      int marginsVertical = m_Margins.top + m_Margins.bottom;
      int marginsHorizontal = m_Margins.left + m_Margins.right;

      if( (wBmp != (width - marginsHorizontal ) ) || (hBmp != (height - marginsVertical) ) )
      {
        ResizeImage( tmpDisplayBmp, width - marginsHorizontal , (height - marginsVertical) );
        wBmp = tmpDisplayBmp.GetWidth();
        hBmp = tmpDisplayBmp.GetHeight();
      }

      xBmp = x + m_Margins.left;
      yBmp = y + m_Margins.top;
    }

    if( ButtonStyles::BU_CENTER_HORIZONTAL )
    {
      xBmp = x + (width - wBmp) / 2;
      yBmp = y + (height - hBmp ) / 2;
    }

    // dont draw over our border which has the button state info
    if( tmpDisplayBmp.Ok() )
    {
      bufferDC.DrawBitmap(tmpDisplayBmp, xBmp , yBmp, true); // draw the image as a background
    }
  }

  wxSize Button::GetBestSize( ) const
  {
    return DoGetBestSize();
  }

  void Button::SetPadding( unsigned int pad )
  {
    m_Padding = pad;
  }

} // namespace UIToolKit