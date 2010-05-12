#pragma once

#include "LunaGenerated.h"
#include <wx/animate.h>

namespace Luna
{
  namespace RevisionStats
  {
    enum RevsionStat
    {
      None,
      CheckedOut,
      CheckedOutByMe,
      OutOfDate,
    };
  }
  typedef RevisionStats::RevsionStat RevisionStat;

  class GameRowPanel : public GameRowGenerated
  {
  public:
    GameRowPanel( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
    virtual ~GameRowPanel();

    void SetLabel( const wxString& label );

    wxBitmap GetIcon() const;
    void SetIcon( const wxBitmap& icon );
    
    void SetRevisionStatus( RevisionStat status );

    void SetLoading( bool loading = true );

    bool IsSelected() const;
    virtual void Select( bool select = true );
    void Deselect() { Select( false ); }

    void SetContextMenu( wxMenu* menu );
    void DeleteContextMenu();
    wxMenu* DetachContextMenu();

  private:
    void OnMouseLeftUp( wxMouseEvent& args );
    void OnMouseRightDown( wxMouseEvent& args );
    void OnMouseRightUp( wxMouseEvent& args );
    void OnPaint( wxPaintEvent& args );
    void TemporaryCheckBoxCallback( wxCommandEvent& args );

  private:
    bool m_IsSelected;
    RevisionStat m_RevisionStatus;
    bool m_IsLoading;
    wxMenu* m_ContextMenu;
    wxAnimationCtrl* m_LoadingIcon;
    wxSize m_TextRectInflation;
    wxColour m_DefaultTextBackground;
    wxColour m_DefaultTextForeground;
  };
}
