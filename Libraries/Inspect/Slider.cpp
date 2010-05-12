#include "stdafx.h"
#include "Slider.h"
#include "Canvas.h"

using namespace Inspect;

///////////////////////////////////////////////////////////////////////////////
// Custom wrapper around wxSlider to interact with Slider.
// 
class StdSlider : public wxSlider
{
public:
  Slider* m_Slider;

  bool m_Override;

  // Constructor
  StdSlider( wxWindow* parent, Slider* slider )
  : wxSlider( parent, wxID_ANY, 0, 0, 1000 )
  , m_Slider( slider )
  {

  }

  // Callback every time a value changes on the slider.
  void OnScroll( wxScrollEvent& e)
  {
    // NOTE: This skip is important.  If you remove it,
    // the EVT_SCROLL_CHANGED event will never get fired.
    e.Skip();

    if ( !m_Override )
    {
      m_Slider->Write();
    }
  }

  // Callback when a mouse click occurs on the slider.
  void OnMouseDown( wxMouseEvent& e )
  {
    e.Skip();
    m_Slider->Start();
  }

  // Callback when a scroll operation has completed (such as
  // when the user stops dragging the thumb).
  void OnScrollChanged( wxScrollEvent& e )
  {
    e.Skip();
    m_Slider->End();
  }

  DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE( StdSlider, wxSlider )
EVT_SCROLL( StdSlider::OnScroll )
EVT_LEFT_DOWN( StdSlider::OnMouseDown )
EVT_SCROLL_CHANGED( StdSlider::OnScrollChanged )
END_EVENT_TABLE()


///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
Slider::Slider()
: m_Min( 0.0f )
, m_Max( 100.0f )
, m_CurrentValue( 0.0f )
, m_AutoAdjustMinMax( true )
{
}

///////////////////////////////////////////////////////////////////////////////
// Creates the control.
// 
void Slider::Realize(Container* parent)
{
  PROFILE_SCOPE_ACCUM( g_RealizeAccumulator );

  if ( m_Window != NULL )
    return;

  StdSlider* slider = new StdSlider( parent->GetWindow(), this );

  slider->ClearTicks();

  m_Window = slider;

  wxSize size( -1, m_Canvas->GetStdSize( Math::SingleAxes::Y ) );
  m_Window->SetSize( size );
  m_Window->SetMinSize( size );
  m_Window->SetMaxSize( size );

  __super::Realize( parent );

  // Push the min and max values down to the slider.
  SetRangeMin( m_Min, false );
  SetRangeMax( m_Max, false );
}

///////////////////////////////////////////////////////////////////////////////
// Sets the UI to match the underlying data value.
// 
void Slider::Read()
{
  if ( IsBound() )
  {
    SetUIValue( GetValue() );

    __super::Read();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Takes the value from the UI and pushes it down to the underlying data.
// 
bool Slider::Write()
{
  bool result = false;

  if ( IsBound() )
  {
    // Get the float value from the UI
    m_CurrentValue = GetUIValue();

    // Write the value back into the data
    std::ostringstream str;
    str << m_CurrentValue;
    result = WriteData( str.str(), m_Tracking );
  }

  return result;
}

///////////////////////////////////////////////////////////////////////////////
// Indicates that the user is starting a click (possibly drag) operation within
// the UI.
// 
void Slider::Start()
{
  // Store the value when the user starts dragging the thumb
  m_StartDragValue = GetUIValue();
  m_Tracking = true;
}

///////////////////////////////////////////////////////////////////////////////
// Indicates that the user has completed a previously started user interaction.
// 
void Slider::End()
{
  // The user is done dragging around the thumb, so reset the
  // value back to what it was when the drag began, then force
  // an undoable command that jumps to the final value.
  if ( m_Tracking )
  {
    Freeze();
    float temp = GetUIValue();
    SetValue( m_StartDragValue );
    m_Tracking = false;
    SetValue( temp );
    Thaw();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the value of the underlying data.
// 
float Slider::GetValue()
{
  if ( IsBound() )
  {
    std::string str;
    ReadData( str );
    if ( str != MULTI_VALUE_STRING && str != UNDEF_VALUE_STRING )
    {
      m_CurrentValue = static_cast< float >( atof( str.c_str() ) );
    }
  }

  return m_CurrentValue;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the underlying data and the UI to the specified value.
// 
void Slider::SetValue(float value)
{
  // Update our cached value
  m_CurrentValue = value;

  // If we have a data pointer, update it
  if ( IsBound() )
  {
    std::ostringstream str;
    str << m_CurrentValue;
    WriteData( str.str() );
  }

  SetUIValue( m_CurrentValue );
}

///////////////////////////////////////////////////////////////////////////////
// Sets the minimum value allowed by the slider.  If clamp is true and the
// current value is less than the min, the value will be set to the min.
// 
void Slider::SetRangeMin(float min, bool clamp)
{
  m_Min = min;

  if ( clamp && GetValue() < m_Min )
  {
    SetValue( m_Min );
  }
}

///////////////////////////////////////////////////////////////////////////////
// Sets the maximum value allowed by the slider.  If clamp is true and the
// current value is greater than max, the value will be set to the max.
void Slider::SetRangeMax(float max, bool clamp)
{
  m_Max = max;

  if ( clamp && GetValue() > m_Max )
  {
    SetValue( m_Max );
  }
}

///////////////////////////////////////////////////////////////////////////////
// If true and the value is set below the min or above the max, the slider
// will automatically adjust its min or max to include the new value.
// 
void Slider::SetAutoAdjustMinMax( bool autoAdjust )
{
  m_AutoAdjustMinMax = autoAdjust;
}

///////////////////////////////////////////////////////////////////////////////
// Process script commands.
// 
bool Slider::Process( const std::string& key, const std::string& value )
{
  if (__super::Process(key, value))
    return true;

  if (key == SLIDER_ATTR_MIN)
  {
    SetRangeMin( static_cast< float >( atof( value.c_str() ) ) );
    return true;
  }
  else if (key == SLIDER_ATTR_MAX)
  {
    SetRangeMax( static_cast< float >( atof( value.c_str() ) ) );
    return true;
  }

  return false;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the UI to the specified value.
// 
void Slider::SetUIValue( float value )
{
  // If the control has been realized, update the UI
  if ( IsRealized() )
  {
		if ( m_AutoAdjustMinMax )
    {
      if ( value > m_Max )
      {
        m_Max = value;
      }
      else if ( value < m_Min )
      {
        m_Min = value;
      }
    }

    int val = static_cast< int >( 0.5f + ( ( (m_CurrentValue - m_Min ) / ( m_Max - m_Min ) ) * 1000.f ) );

    if ( val < 0 )
    {
      val = 0;
    }

    if ( val > 1000 )
    {
      val = 1000;
    }

    StdSlider* slider = Control::Cast<StdSlider>( this );
    slider->m_Override = true;
    slider->SetValue( val );
    slider->m_Override = false;
  }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the value currently displayed in the UI.
// 
float Slider::GetUIValue() const
{
  int val = Control::Cast< wxSlider >( this )->GetValue();
  const float result = m_Min + ( ( m_Max - m_Min ) * ( static_cast< float >( val ) / 1000.f ) );
  return result;
}
