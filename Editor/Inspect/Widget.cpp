#include "EditorPch.h"
#include "Widget.h"

#include "Editor/DragDrop/FileDropTarget.h"

REFLECT_DEFINE_ABSTRACT( Helium::Editor::Widget );

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
        m_Control->a_HelpText.Changed().RemoveMethod( this, &Widget::HelpTextChanged );

        m_Window->Disconnect( m_Window->GetId(), wxEVT_CONTEXT_MENU, wxContextMenuEventHandler( Widget::OnContextMenu ), NULL, this );
    }

    // save the window pointer
    m_Window = window;

    if ( m_Window )
    {
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
        m_Window->SetHelpText( m_Control->a_HelpText.Get() );

        // add listeners
        m_Control->a_IsEnabled.Changed().AddMethod( this, &Widget::IsEnabledChanged );
        m_Control->a_IsReadOnly.Changed().AddMethod( this, &Widget::IsReadOnlyChanged );
        m_Control->a_IsFrozen.Changed().AddMethod( this, &Widget::IsFrozenChanged );
        m_Control->a_IsHidden.Changed().AddMethod( this, &Widget::IsHiddenChanged );
        m_Control->a_ForegroundColor.Changed().AddMethod( this, &Widget::ForegroundColorChanged );
        m_Control->a_BackgroundColor.Changed().AddMethod( this, &Widget::BackgroundColorChanged );
        m_Control->a_HelpText.Changed().AddMethod( this, &Widget::HelpTextChanged );

        if ( !m_Control->GetProperty( TXT( "FileFilter" ) ).empty() )
        {
            const tstring& filter = m_Control->GetProperty( TXT( "FileFilter" ) );

            FileDropTarget* dropTarget = new FileDropTarget( filter );
            m_Window->SetDropTarget( dropTarget );
        }

        m_Window->Connect( m_Window->GetId(), wxEVT_CONTEXT_MENU, wxContextMenuEventHandler( Widget::OnContextMenu ), NULL, this );
    }
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

void Widget::ForegroundColorChanged( const Attribute<uint32_t>::ChangeArgs& args )
{
    m_Window->SetForegroundColour( args.m_NewValue );
}

void Widget::BackgroundColorChanged( const Attribute<uint32_t>::ChangeArgs& args )
{
    m_Window->SetBackgroundColour( args.m_NewValue );
}

void Widget::HelpTextChanged( const Attribute<tstring>::ChangeArgs& args )
{
    m_Window->SetHelpText( args.m_NewValue );
}

void Widget::OnContextMenu( wxContextMenuEvent& event )
{
    wxMenu menu;

    Inspect::ContextMenuPtr context_menu = m_Control->GetContextMenu();
    if (!context_menu.ReferencesObject())
    {
      return;
    }

    std::vector< tstring >::const_iterator itr = context_menu->GetItems().begin();
    std::vector< tstring >::const_iterator end = context_menu->GetItems().end();
    for ( int32_t count = 0; itr != end; ++itr, ++count )
    {
        if ( *itr == TXT( "-" ) )
        {
            menu.AppendSeparator();
        }
        else
        {
            menu.Append( wxID_HIGHEST + count, itr->c_str() );
        }
    }

    menu.Connect( wxID_HIGHEST, wxID_HIGHEST + (int)m_Control->GetContextMenu()->GetItems().size() - 1, wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler( Widget::OnContextMenuItem ), NULL, this );

    m_Window->PopupMenu( &menu );
}

void Widget::OnContextMenuItem( wxCommandEvent& event )
{
    const tstring& item ( m_Control->GetContextMenu()->GetItems()[ event.GetId() - wxID_HIGHEST ] );

    Inspect::M_ContextMenuDelegate::const_iterator found = m_Control->GetContextMenu()->GetDelegates().find(item);

    if (found != m_Control->GetContextMenu()->GetDelegates().end())
    {
        found->second.Invoke( Inspect::ContextMenuEventArgs (m_Control, item) );
    }
}