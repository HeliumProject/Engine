#include "Precompile.h"
#include "Widget.h"

using namespace Helium;
using namespace Helium::Editor;

Widget::Widget( Inspect::Control* control )
: m_Window( NULL )
{

}

Widget::~Widget()
{

}

int Widget::GetStringWidth(const tstring& str)
{
    wxClientDC dc (m_Window);

    int x, y;
    wxString wxStr (str.c_str());
    dc.GetTextExtent(wxStr, &x, &y, NULL, NULL, &m_Window->GetFont());

    return x;
}

bool Widget::EllipsizeString(tstring& str, int width)
{
    wxClientDC dc (m_Window);

    int x, y;
    wxString wxStr (str.c_str());
    dc.GetTextExtent(wxStr, &x, &y, NULL, NULL, &m_Window->GetFont());

    if (x <= width)
    {
        return false;
    }

    size_t count = str.size();
    for ( size_t i = count; i>0; i-- )
    {
        wxStr = (str.substr(0, i-1) + TXT( "..." ) ).c_str();

        dc.GetTextExtent(wxStr, &x, &y, NULL, NULL, &m_Window->GetFont());

        if (x < width)
        {
            str = wxStr.c_str();
            return true;
        }
    }

    str = TXT( "..." );
    return true;
}

void Widget::SetWindow( wxWindow* window )
{
    if ( m_Window )
    {
        // release listeners
        m_Control->a_IsEnabled.Changed().RemoveMethod( this, &Widget::IsEnabledChanged );
        m_Control->a_IsReadOnly.Changed().RemoveMethod( this, &Widget::IsReadOnlyChanged );
        m_Control->a_IsFrozen.Changed().RemoveMethod( this, &Widget::IsFrozenChanged );
        m_Control->a_IsHidden.Changed().RemoveMethod( this, &Widget::IsHiddenChanged );
        m_Control->a_ForegroundColor.Changed().RemoveMethod( this, &Widget::ForegroundColorChanged );
        m_Control->a_BackgroundColor.Changed().RemoveMethod( this, &Widget::BackgroundColorChanged );
        m_Control->a_ToolTip.Changed().RemoveMethod( this, &Widget::ToolTipChanged );

        m_Window->Destroy();
    }

    // save the window pointer
    m_Window = window;

    if ( m_Window )
    {
        // when the control goes into the unrealized state, destroy the control (which will free this object)
        m_Control->e_Unrealized.AddMethod( this, &Widget::Unrealized );

        // configure initial state
        m_Window->Enable( m_Control->a_IsEnabled.Get() && !m_Control->a_IsReadOnly.Get() );
        if ( m_Control->a_IsFrozen.Get() )
        {
            m_Window->Freeze();
        }
        if ( m_Control->a_IsHidden.Get() )
        {
            m_Window->Hide();
        }   
        m_Window->SetForegroundColour( m_Control->a_ForegroundColor.Get() );
        m_Window->SetBackgroundColour( m_Control->a_BackgroundColor.Get() );
        m_Window->SetHelpText( m_Control->a_ToolTip.Get() );

        // add listeners
        m_Control->a_IsEnabled.Changed().AddMethod( this, &Widget::IsEnabledChanged );
        m_Control->a_IsReadOnly.Changed().AddMethod( this, &Widget::IsReadOnlyChanged );
        m_Control->a_IsFrozen.Changed().AddMethod( this, &Widget::IsFrozenChanged );
        m_Control->a_IsHidden.Changed().AddMethod( this, &Widget::IsHiddenChanged );
        m_Control->a_ForegroundColor.Changed().AddMethod( this, &Widget::ForegroundColorChanged );
        m_Control->a_BackgroundColor.Changed().AddMethod( this, &Widget::BackgroundColorChanged );
        m_Control->a_ToolTip.Changed().AddMethod( this, &Widget::ToolTipChanged );
    }
}

void Widget::Unrealized( Inspect::Control* control )
{
    m_Window->Destroy(); // will in turn destroy this object
}

void Widget::IsEnabledChanged( const Attribute<bool>::ChangeArgs& args )
{
    m_Window->Enable(args.m_NewValue && !m_Control->a_IsReadOnly.Get());
}

void Widget::IsReadOnlyChanged( const Attribute<bool>::ChangeArgs& args )
{
    m_Window->Enable(!args.m_NewValue && m_Control->a_IsEnabled.Get());
}

void Widget::IsFrozenChanged( const Attribute<bool>::ChangeArgs& args )
{
    if ( args.m_NewValue )
    {
        m_Window->Freeze();
    }
    else
    {
        m_Window->Thaw();
    }
}

void Widget::IsHiddenChanged( const Attribute<bool>::ChangeArgs& args )
{
    if ( args.m_NewValue )
    {
        m_Window->Hide();
    }
    else
    {
        m_Window->Show();
    }
}

void Widget::ForegroundColorChanged( const Attribute<u32>::ChangeArgs& args )
{
    m_Window->SetForegroundColour( args.m_NewValue );
}

void Widget::BackgroundColorChanged( const Attribute<u32>::ChangeArgs& args )
{
    m_Window->SetBackgroundColour( args.m_NewValue );
}

void Widget::ToolTipChanged( const Attribute<tstring>::ChangeArgs& args )
{
    m_Window->SetToolTip( args.m_NewValue );
}