#pragma once

/// @file Button.h
/// @brief A GUI button that can be used as a regular button or a toggle button
/// This button is derived from bmpButton. Different images can be used to reflect different
///    states of the button including hover, selected, toggled, normal, and disabled. This button
///    also has the ability to be set to a toggle mode, which sends a wxEVT_COMMAND_TOGGLEBUTTON_CLICKED
///    event when a click event occurs.
#include "API.h"
#include <wx/bmpbuttn.h>
#include <wx/dcbuffer.h>

namespace UIToolKit
{
  namespace BitmapAlignments
  {
    /// @enum BitmapAlignment
    /// @brief alignment of where the bitmap is displayed on the button
    enum BitmapAlignment
    {
      BU_BMP_LEFT,          //!< display bitmap to the left of a label if it exists
      BU_BMP_TOP,           //!< display bitmap on the top of a label if it exists
      BU_BMP_RIGHT,         //!< display bitmap on the right of a label if it exists
      BU_BMP_BOTTOM,        //!< display bitmap on the bottom of a label if it exists
    };
  }

  namespace ButtonStyles
  {
    /// @enum ButtonStyle
    /// @brief style flags used to set up the look of a button
    /// These flags can be OR'ed together to create different styles of buttons. The default style is
    /// a button that contains both a centered label and bitmap. ie: BU_CENTER | BU_LABEL | BU_BITMAP 
    enum ButtonStyle
    {
      BU_LABEL                 =   0x1<<0,              //!< This button contains a label
      BU_BITMAP                =   0x1<<1,              //!< This button contains a bitmap
      BU_TOGGLE                =   0x1<<2,              //!< This is a toggle button

      // stretch the image to fit the background
      BU_BMP_BACKGROUND = 0x1<<9,                       //!< Use the current bitmap for this button as the background
      BU_BMP_BACKGROUND_STRETCH    = 0x1<<10,           //!< Stretch the background image to ensure it is the same size as the button

      // alignment of bmp and text
      BU_CENTER_HORIZONTAL     =   0x1<<13,             //!< Center the element of this button horizontally
      BU_CENTER_VERTICAL       =   0x1<<14,             //!< Center the elements of this button vertically
      BU_CENTER                =   BU_CENTER_HORIZONTAL | BU_CENTER_VERTICAL,   //!< Convenience method( BU_CENTER_HORIZONTAL | BU_CENTER_VERTICAL )
      BU_DEFAULT               =   BU_CENTER | BU_LABEL | BU_BITMAP,            //!< Default style settings
    };
  }

  /// @class Button
  /// @brief implement a button class which will allow us to display a bitmap and labe at the same time
  class UITOOLKIT_API Button : public wxBitmapButton
  {
  protected:
    ButtonStyles::ButtonStyle     m_ButtonStyle;              //<! The current style settings of this button
    bool                          m_ToggleState;              //<! when in toggle mode, this stores info about if we are clicked or not
    BitmapAlignments::BitmapAlignment    m_BitmapAlignment;   //<! the alignment of the bitmap to the label if necessary
  protected:
    RECT          m_Margins;            //<! Margins for each side of the button so we can have some space
    unsigned int  m_Padding;            //<! Amount of padding between a bmp and label if they both exist
    wxBitmap      m_BmpToggled;         //<! bitmap to display when we are toggled
    wxColor       m_GradientColor1;     //<! Use this as the start color when using a gradient fill
    wxColor       m_GradientColor2;     //<! Use this as the end color when using a gradient fill
    bool          m_GradientFill;       //<! Have we enabled gradient fill
    wxDirection   m_GradientDirection;  //<! What direction are we going to be doing the gradient

  protected:
    /// @function
    /// @brief convenience function for checking if multiple flags are set in a field
    /// @param flag member variable to check
    /// @param flags that we want to check for OR'ed together. returns true if ALL passed in flags are set
    bool CheckFlagValues( UINT src, UINT flags ) const;

    void DrawBitmapBackground( wxBufferedDC& bufferDC, wxBitmap* bitmap, wxRect& rect );

  public:
    // wxWidgets setup
    DECLARE_DYNAMIC_CLASS( Button )
    DECLARE_EVENT_TABLE( )

  public:
    /// @function Button
    /// @brief Constructor
    Button() { }

    /// @function Button
    /// @brief Constructor
    Button(wxWindow *parent,
      wxWindowID id,
      const wxBitmap& bitmap,
      const wxPoint& pos = wxDefaultPosition,
      const wxSize& size = wxDefaultSize,
      long style = wxBU_EXACTFIT | wxNO_FULL_REPAINT_ON_RESIZE ,
      const wxValidator& validator = wxDefaultValidator,
      const wxString& name = wxButtonNameStr) ;

    /// @function Destructor
    virtual ~Button(void);

    /// @function MSWOnDraw
    /// @brief Override the draw function so that we can handle drawing a label and a bitmap
    virtual bool MSWOnDraw(WXDRAWITEMSTRUCT *item) override;

  public:

    /// @function SetMargins
    /// @param sets all the margins to the same value
    void SetMargins( int allSides );

    /// @function SetMargins
    /// @param sets the left margin
    /// @param sets the top margin
    /// @param sets the right margin
    /// @param sets the bottom margin
    void SetMargins( int left, int top, int right, int bottom );

    /// @function SetPadding
    /// @brief sets the padding between a bmp and label if they both are enabled
    /// @param padding size
    void SetPadding( unsigned int pad );

  public:
    /// @function GetToggleState
    /// @brief returns the current toggle state
    /// @return true if button is pressed down (toggled), false otherwise
    bool GetToggleState( ) const;

    /// @function SetToggleState
    /// @brief Sets the current toggle state
    /// @param value to set the current toggle state to
    void SetToggleState( bool val );

    /// @function SetBitmapAlignment
    /// @brief Sets the current alignment of the bitmap to the top,left,right,bottom of a label on the button
    /// @param alignment flag which sets the position of the bitmap
    void SetBitmapAlignment( BitmapAlignments::BitmapAlignment align = BitmapAlignments::BU_BMP_LEFT );

    /// @function GetbitmapAlignment
    /// @brief returns the current alignment of the bitmap
    /// @return the current alignment of the bitmap
    BitmapAlignments::BitmapAlignment GetBitmapAlignment( ) const;

  public:
    /// @function GetBitmapToggled
    /// @return the current bitmap used when we are in the toggled state
    const wxBitmap&     GetBitmapToggled( ) const;

    /// @function SetBitmapToggled
    /// @brief set the bitmap to use when the button is toggled
    /// @param the bitmap to use when we are in the toggled state
    void                SetBitmapToggled( const wxBitmap& bitmap );

    /// @function SetButtonStyle
    /// @brief use the various ButtonStyles flags to set the look of the button
    /// @param Style flags that are OR'ed together to create a look for this button
    /// @see ButtonStyles
    virtual void        SetButtonStyle( UINT style = ButtonStyles::BU_DEFAULT );

    /// @function GetbuttonStyle
    /// @brief return the current flags that are set on t
    virtual UINT GetButtonStyle( ) const;

    /// @function OnLeftClick
    /// @brief implimentation of left click event handler. Used to send a wxEVT_COMMAND_TOGGLEBUTTON_CLICKED event when a toggle button is clicked
    /// @param event parameters that are sent to the handler
    virtual void OnLeftClick( wxCommandEvent& event );

  public:
    /// @function OnToggle
    /// @brief implimentation of a toggle event handler for a toggle button. used to update the state of the button
    /// @param event parameters that are sent to the handler
    virtual void OnToggle( wxCommandEvent& event );

  protected:

    /// @function ResizeImage
    /// @brief resize the bitmap that is passed in to the specified width and height
    /// @param bitmap to resize
    /// @param new width
    /// @param new height
    void ResizeImage( wxBitmap& bitmap, int width, int height );

    /// @function DoGetBestSize
    /// @brief override used to get the best size for the control from its elements
    /// @return wxSize with best width/height for its elements
    virtual wxSize DoGetBestSize( ) const NOC_OVERRIDE;

    /// @function BitmapForButtonState
    /// @brief convenience method for selecting the best bitmap out of the available ones based on the specified state
    /// @return pointer to bitmap to use for this state ( normal, hover, disabled... etc )
    virtual wxBitmap* BitmapForButtonState(  ) const;

  protected:
    /// @function Clamp
    /// @brief clamps a number to a min/max value. Could not find this anywhere else
    /// @todo this is proably implemented somewhere more sane
    /// @param the variable we want to clamp
    /// @param the min value
    /// @param the max value
    void Clamp( int& var, int min, int max );

  public:
    /// @function EnableGradientFill
    /// @brief turn on a background gradient fill
    /// @param enable or disable the gradient fill
    void EnableGradientFill( bool fill = false );

    /// @function SetGradientFill
    /// @brief sets the colors to be used during a gradient fill
    /// @param start color
    /// @param end color
    /// @param direction we want the gradient fill to travel
    void SetGradientFill( const wxColor& c1, const wxColor& c2, wxDirection dir = wxSOUTH );

    /// @function OnEraseBackground
    /// @brief handle the event wxEVT_ERASE. Ignore this so we can do all drawing.
    /// We effectively catch, and disable this since we do all the drawing
    /// @param event paramenters passed to Erase Event
    void OnEraseBackground( wxEraseEvent& event );

    /// @function GetBestSize
    /// @brief returns a size the ecompases all the elements of this button
    /// The size returned by this call should encompass all the elements that are enabled on the button.
    /// It should also take into account things like image/text sizes and margins.
    /// @return a good size
    wxSize GetBestSize( ) const;

  };

} // namespace UIToolKit