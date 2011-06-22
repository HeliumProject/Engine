#include "EditorPch.h"
#include "DropTarget.h"
#include "Editor/Clipboard/ClipboardDataObject.h"
#include "Platform/Exception.h"

using namespace Helium;
using namespace Helium::Editor;

///////////////////////////////////////////////////////////////////////////////
// Constructor
// 
DropTarget::DropTarget() 
{
    SetDataObject( new ClipboardDataObject() );
}

///////////////////////////////////////////////////////////////////////////////
// Destructor
// 
DropTarget::~DropTarget()
{
}

///////////////////////////////////////////////////////////////////////////////
// Allows you to set the callback for when a drag enters the target.
// 
void DropTarget::SetDragEnterCallback( const DragEnterCallback::Delegate& func )
{
    if ( m_DragEnter.Count() == 0 )
    {
        m_DragEnter.Add( func );
    }
    else
    {
        throw Helium::Exception( TXT( "Only one callback for 'drag enter' events is valid in DropTarget." ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Allows you to set the callback for when a drag event over the target occurs.
// 
void DropTarget::SetDragOverCallback( const DragOverCallback::Delegate& func )
{
    if ( m_DragOver.Count() == 0 )
    {
        m_DragOver.Add( func );
    }
    else
    {
        throw Helium::Exception( TXT( "Only one callback for 'drag over' events is valid in DropTarget." ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Allows you to set the callback for when a drag leaves the target.
// 
void DropTarget::SetDragLeaveCallback( const DragLeaveCallback::Delegate& func )
{
    if ( m_DragLeave.Count() == 0 )
    {
        m_DragLeave.Add( func );
    }
    else
    {
        throw Helium::Exception( TXT( "Only one callback for 'drag leave' events is valid in DropTarget." ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Sets the callback to occur during a drop operation (can only be called once).
// 
void DropTarget::SetDropCallback( const DropCallback::Delegate& func )
{
    if ( m_Drop.Count() == 0 )
    {
        m_Drop.Add( func );
    }
    else
    {
        throw Helium::Exception( TXT( "Only one callback for 'drop' events is valid in DropTarget." ) );
    }
}

///////////////////////////////////////////////////////////////////////////////
// Notifies listener that a drag event has entered the target.
// 
wxDragResult DropTarget::OnEnter( wxCoord x, wxCoord y, wxDragResult def )
{
    if ( m_DragEnter.Count() > 0 )
    {
        GetData();
        m_DragEnter.Raise( DragArgs( x, y, static_cast< ClipboardDataObject* >( GetDataObject() ), def ) );
    }

    return wxDropTarget::OnEnter( x, y, def );
}

///////////////////////////////////////////////////////////////////////////////
// Notifies listener that a drag over event has occurred.
// 
wxDragResult DropTarget::OnDragOver( wxCoord x, wxCoord y, wxDragResult def )
{
    wxDragResult result = wxDragError;
    if ( m_DragOver.Count() > 0 )
    {
        GetData();
        DragArgs args ( x, y, static_cast< ClipboardDataObject* >( GetDataObject() ), def );
        m_DragOver.Raise( args );
        result = args.m_Result;
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Notifies listener that a drop event has occurred.
// 
wxDragResult DropTarget::OnData( wxCoord x, wxCoord y, wxDragResult def )
{
    if ( !GetData() )
        return wxDragNone;

    wxDragResult result = def;
    if ( m_Drop.Count() > 0 )
    {
        DragArgs args ( x, y, static_cast< ClipboardDataObject* >( GetDataObject() ), def );
        m_Drop.Raise( args );
        result = args.m_Result;
    }
    return result;
}

///////////////////////////////////////////////////////////////////////////////
// Notifies listener that the drag has left the target area.
// 
void DropTarget::OnLeave()
{
    if ( m_DragLeave.Count() > 0 )
    {
        m_DragLeave.Raise( Helium::Void() );
    }
}
