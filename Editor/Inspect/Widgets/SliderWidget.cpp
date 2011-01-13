#include "Precompile.h"
#include "SliderWidget.h"

using namespace Helium;
using namespace Helium::Editor;

BEGIN_EVENT_TABLE( SliderWindow, wxSlider )
EVT_SCROLL( SliderWindow::OnScroll )
EVT_LEFT_DOWN( SliderWindow::OnMouseDown )
EVT_SCROLL_CHANGED( SliderWindow::OnScrollChanged )
END_EVENT_TABLE()

SliderWindow::SliderWindow( wxWindow* parent, SliderWidget* sliderWidget )
: wxSlider( parent, wxID_ANY, 0, 0, 1000 )
, m_SliderWidget( sliderWidget )
, m_Override( false )
{

}

void SliderWindow::OnScroll( wxScrollEvent& e)
{
    // NOTE: This skip is important.  If you remove it,
    // the EVT_SCROLL_CHANGED event will never get fired.
    e.Skip();

    if ( !m_Override )
    {
        m_SliderWidget->GetControl()->Write();
    }
}

void SliderWindow::OnMouseDown( wxMouseEvent& e )
{
    e.Skip();
    m_SliderWidget->Start();
}

void SliderWindow::OnScrollChanged( wxScrollEvent& e )
{
    e.Skip();
    m_SliderWidget->End();
}

REFLECT_DEFINE_OBJECT( SliderWidget );

SliderWidget::SliderWidget( Inspect::Slider* slider )
: m_SliderControl( slider )
, m_SliderWindow( NULL )
, m_StartDragValue( 0.f )
, m_Tracking( false )
{
    SetControl( slider );
}

void SliderWidget::CreateWindow( wxWindow* parent )
{
    HELIUM_ASSERT( !m_SliderWindow );

    // allocate window and connect common listeners
    SetWindow( m_SliderWindow = new SliderWindow( parent, this ) );

    // clear tick marks from the UI
    m_SliderWindow->ClearTicks();

    // init layout metrics
    wxSize size( m_Control->GetCanvas()->GetDefaultSize( SingleAxes::X ), m_Control->GetCanvas()->GetDefaultSize( SingleAxes::Y ) );
    m_Window->SetSize( size );
    m_Window->SetMinSize( size );

    // add listeners
    m_SliderControl->a_Min.Changed().AddMethod( this, &SliderWidget::MinChanged );
    m_SliderControl->a_Max.Changed().AddMethod( this, &SliderWidget::MaxChanged );

    // update state of attributes that are not refreshed during Read()
    m_SliderControl->a_Min.RaiseChanged();
    m_SliderControl->a_Max.RaiseChanged();
}

void SliderWidget::DestroyWindow()
{
    HELIUM_ASSERT( m_SliderWindow );

    SetWindow( NULL );

    // remove listeners
    m_SliderControl->a_Min.Changed().RemoveMethod( this, &SliderWidget::MinChanged );
    m_SliderControl->a_Max.Changed().RemoveMethod( this, &SliderWidget::MaxChanged );

    // destroy window
    m_SliderWindow->Destroy();
    m_SliderWindow = NULL;
}

void SliderWidget::Read()
{
    HELIUM_ASSERT( m_SliderControl->IsBound() );

    SetUIValue( GetValue() );
}

bool SliderWidget::Write()
{
    HELIUM_ASSERT( m_SliderControl->IsBound() );

    // Get the float value from the UI
    float value = GetUIValue();

    // Write the value back into the data
    tostringstream str;
    str << value;
    return m_SliderControl->WriteStringData( str.str(), m_Tracking );
}

///////////////////////////////////////////////////////////////////////////////
// Sets the minimum value allowed by the slider.  If clamp is true and the
// current value is less than the min, the value will be set to the min.
// 
void SliderWidget::MinChanged( const Attribute<float>::ChangeArgs& args )
{
    if ( GetValue() < args.m_NewValue )
    {
        SetValue( args.m_NewValue );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Sets the maximum value allowed by the slider.  If clamp is true and the
// current value is greater than max, the value will be set to the max.
void SliderWidget::MaxChanged( const Attribute<float>::ChangeArgs& args )
{
    if ( GetValue() > args.m_NewValue )
    {
        SetValue( args.m_NewValue );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Indicates that the user is starting a click (possibly drag) operation within
// the UI.
// 
void SliderWidget::Start()
{
    // Store the value when the user starts dragging the thumb
    m_StartDragValue = GetUIValue();
    m_Tracking = true;
}

///////////////////////////////////////////////////////////////////////////////
// Indicates that the user has completed a previously started user interaction.
// 
void SliderWidget::End()
{
    // The user is done dragging around the thumb, so reset the
    // value back to what it was when the drag began, then force
    // an undoable command that jumps to the final value.
    if ( m_Tracking )
    {
        m_SliderWindow->Freeze();
        float temp = GetUIValue();
        SetValue( m_StartDragValue );
        m_Tracking = false;
        SetValue( temp );
        m_SliderWindow->Thaw();
    }
}

///////////////////////////////////////////////////////////////////////////////
// Returns the value of the underlying data.
// 
float SliderWidget::GetValue() const
{
    HELIUM_ASSERT( m_SliderControl->IsBound() );

    float value = 0.f;

    tstring str;
    m_SliderControl->ReadStringData( str );
    if ( str != Inspect::MULTI_VALUE_STRING && str != Inspect::UNDEF_VALUE_STRING )
    {
        value = static_cast< float >( _tstof( str.c_str() ) );
    }

    return value;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the underlying data and the UI to the specified value.
// 
void SliderWidget::SetValue(float value)
{
    // If we have a data pointer, update it
    HELIUM_ASSERT( m_SliderControl->IsBound() );

    tostringstream str;
    str << value;
    m_SliderControl->WriteStringData( str.str() );

    SetUIValue( value );
}

///////////////////////////////////////////////////////////////////////////////
// Returns the value currently displayed in the UI.
// 
float SliderWidget::GetUIValue() const
{
    int val = m_SliderWindow->GetValue();
    const float result = m_SliderControl->a_Min.Get() + ( ( m_SliderControl->a_Max.Get() - m_SliderControl->a_Min.Get() ) * ( static_cast< float >( val ) / 1000.f ) );
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Sets the UI to the specified value.
// 
void SliderWidget::SetUIValue( float value )
{
    // If the control has been realized, update the UI
    if ( m_SliderControl->a_AutoAdjustMinMax.Get() )
    {
        if ( value > m_SliderControl->a_Max.Get() )
        {
            m_SliderControl->a_Max.Set( value );
        }
        else if ( value < m_SliderControl->a_Min.Get() )
        {
            m_SliderControl->a_Min.Set( value );
        }
    }

    int val = static_cast< int >( 0.5f + ( ( (value - m_SliderControl->a_Min.Get() ) / ( m_SliderControl->a_Max.Get() - m_SliderControl->a_Min.Get() ) ) * 1000.f ) );

    if ( val < 0 )
    {
        val = 0;
    }

    if ( val > 1000 )
    {
        val = 1000;
    }

    m_SliderWindow->SetOverride( true );
    m_SliderWindow->SetValue( val );
    m_SliderWindow->SetOverride( false );
}
