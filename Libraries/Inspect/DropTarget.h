#pragma once

#include "API.h"
#include "Platform/Compiler.h"
#include "Foundation/Automation/Event.h"
#include <wx/dnd.h>


namespace Inspect
{
  // Forwards
  class ClipboardDataObject;

  /////////////////////////////////////////////////////////////////////////////
  // Arguments for drag operations.
  // 
  struct DragArgs
  {
    wxCoord m_X;
    wxCoord m_Y;
    wxDragResult m_Default;
    ClipboardDataObject* m_ClipboardData;

    DragArgs( wxCoord x, wxCoord y, wxDragResult def, ClipboardDataObject* clipboardData )
      : m_X( x )
      , m_Y( y )
      , m_Default( def )
      , m_ClipboardData( clipboardData )
    {
    }
  };

  typedef Nocturnal::Signature< void, const DragArgs& > DragEnterCallback;
  typedef Nocturnal::Signature< wxDragResult, const DragArgs& > DragOverCallback;
  typedef Nocturnal::Signature< void, Nocturnal::Void > DragLeaveCallback;
  typedef Nocturnal::Signature< wxDragResult, const DragArgs& > DropCallback;

  /////////////////////////////////////////////////////////////////////////////
  // Class managing callbacks for when drag/drop operations occur.
  // 
  class INSPECT_API DropTarget : public wxDropTarget
  {
  private:
    DragEnterCallback::Event m_DragEnter;
    DragOverCallback::Event m_DragOver;
    DragLeaveCallback::Event m_DragLeave;
    DropCallback::Event m_Drop;

  public:
    DropTarget();
    virtual ~DropTarget();

    void SetDragEnterCallback( const DragEnterCallback::Delegate& func );
    void SetDragOverCallback( const DragOverCallback::Delegate& func );
    void SetDragLeaveCallback( const DragLeaveCallback::Delegate& func );
    void SetDropCallback( const DropCallback::Delegate& func );

    virtual wxDragResult OnEnter( wxCoord x, wxCoord y, wxDragResult def ) NOC_OVERRIDE;
    virtual wxDragResult OnDragOver( wxCoord x, wxCoord y, wxDragResult def ) NOC_OVERRIDE;
    virtual void OnLeave() NOC_OVERRIDE;
    virtual wxDragResult OnData( wxCoord x, wxCoord y, wxDragResult def ) NOC_OVERRIDE;
  };
}
