#pragma once

#include "Common/Types.h"
#include "Tuid/TUID.h"
#include "Common/Automation/Event.h"
#include "Math/Vector2.h"

namespace Luna
{
  class LightmapUVViewer;

  namespace LightmapUVViewerIDs
  {
    enum LightmapUVViewerID
    {
      SearchForFile = wxID_HIGHEST + 1,
    };

  };
  typedef LightmapUVViewerIDs::LightmapUVViewerID LightmpaUVViewerIDs;

  struct ViewerClosedArgs
  {
    LightmapUVViewer* m_ClosedViewer;
    ViewerClosedArgs( LightmapUVViewer& closedViewer)
      : m_ClosedViewer ( &closedViewer )
    {

    }
  };
  typedef Nocturnal::Signature<void, ViewerClosedArgs&> ViewerClosedSignature;

  class LightmapUVViewer : public wxFrame
  {
  protected:

    wxMenuBar* m_MenuBar;
    wxMenu* m_MenuFile;
    wxStatusBar* m_StatusBar;
    wxCheckBox* m_CheckOverlapsBox;

    wxBrush* m_GreenBrush;

  private:

    f32 m_Scale;
    wxPoint m_OriginOffset;
    wxPoint m_LastMousePos;
    int m_WidthAndHeight;

    tuid m_AssetID;

    std::map< u32, std::set<u32> > m_OverlapingTris;
    std::vector<V_u32> m_TriangleUVIndices;
    std::vector< Math::V_Vector2 > m_UVShells;


  public:

    ViewerClosedSignature::Event m_ViewerClosedSignature;

  public:
    LightmapUVViewer( wxWindow* parent, const std::string& title, int xpos, int ypos, int width, int height );
    LightmapUVViewer( wxWindow* parent, tuid assetID, const std::string& title, int xpos, int ypos, int width, int height );
    ~LightmapUVViewer();


  private:

    void InitializeUI();

    void Draw();

    bool LoadUVShells();

    // wx event handlers
    void OnClose( wxCommandEvent& event );

    void OnPaint( wxPaintEvent& event );

    void OnScale( wxMouseEvent& event );

    void OnMiddleDrag( wxMouseEvent& event );

    void StartMiddleDrag( wxMouseEvent& event );

    void EndMiddleDrag( wxMouseEvent& event );

    void OnFind( wxCommandEvent& event );

    void OnOpen( wxCommandEvent& event );

    void OnOverlapCheckBox( wxCommandEvent& event );

    

  public:

    bool DoOpen( tuid id );    

    DECLARE_EVENT_TABLE();


  };
}
