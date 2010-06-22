#pragma once

#include "Application/API.h"
#include "KeyArray.h"

#include <wx/menu.h>
#include <wx/panel.h>

namespace Inspect
{
  /////////////////////////////////////////////////////////////////////////////
  // A color gradient control that allows the user to place and move keys along
  // its length.  Each key specifies a color value that the gradient should 
  // show.  To maniuplate this control, call GetKeyArray and use the functions
  // it provides.
  // 
  class APPLICATION_API KeyControl : public wxPanel
  {
  private:
    KeyArrayPtr m_KeyArray;
    u32 m_DraggingKey;
    i32 m_DragOffset;
    wxMenu m_PopupMenu;

  public:
    KeyControl( wxWindow* parent );
    virtual ~KeyControl();

    KeyArray* GetKeyArray();

  private:
    i32 GetPos( float location ) const;
    float GetLocation( i32 pos ) const;
    Math::Color3 GetColor( i32 pos ) const;
    bool EditKey( u32 index );
    u32 PickKey( i32 pos ) const;
    void DrawGradient( wxDC& dc ) const;
    void DrawKey( wxDC& dc, float location, bool selected ) const;
    bool ToClipboard( const V_KeyPtr& keys );
    bool FromClipboard( V_KeyPtr& keys );
    void EnsureEndCaps();

    void OnPaint( wxPaintEvent& args );
    void OnMouseLeftDown( wxMouseEvent& args );
    void OnMouseLeftUp( wxMouseEvent& args );
    void OnMouseLeftDoubleClick( wxMouseEvent& args );
    void OnMouseRightUp( wxMouseEvent& args );
    void OnMouseMove( wxMouseEvent& args );
    void OnMouseCaptureLost( wxMouseCaptureLostEvent& args );
    void OnCutKey( wxCommandEvent& args );
    void OnCopyKey( wxCommandEvent& args );
    void OnCopyAllKeys( wxCommandEvent& args );
    void OnPasteKeys( wxCommandEvent& args );
    void OnClobberKeys( wxCommandEvent& args );
    void OnDeleteKey( wxCommandEvent& args );
    void OnDeleteAllKeys( wxCommandEvent& args );

    void Changed( const KeyArgs& args );

    DECLARE_EVENT_TABLE();
  };
}

