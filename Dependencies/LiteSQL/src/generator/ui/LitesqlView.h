#ifndef LITESQL_VIEW_H
#define LITESQL_VIEW_H

#include <wx/docview.h>
#include <wx/treebook.h>

class LitesqlModelTreePanel;

class LitesqlView: public wxView
{
public:
    wxMDIChildFrame *frame;
	LitesqlModelTreePanel* panel;

    LitesqlView();
    virtual ~LitesqlView();


protected:
    wxPanel* GetSelectedPage();
  wxMenu* GetContextMenu();

    bool OnCreate(wxDocument *doc, long flags);
    void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    
    void OnDraw(wxDC *dc);
    bool OnClose(bool deleteWindow = true);
   
    void OnPageChanged(wxTreebookEvent& event);
    void OnPageChanging(wxTreebookEvent& event);

    void OnContextMenu(wxContextMenuEvent& event);
    
    void OnAddObject(wxCommandEvent& event );
    void OnRemoveObject(wxCommandEvent& event );

    void OnAddField(wxCommandEvent& event);
    void OnRemoveField(wxCommandEvent& event);

    void OnAddMethod(wxCommandEvent& event);
    void OnRemoveMethod(wxCommandEvent& event);

    void OnAddRelated(wxCommandEvent&);
    void OnRemoveRelated(wxCommandEvent&);

    void OnAddRelation(wxCommandEvent&);
    void OnRemoveRelation(wxCommandEvent&);

    void OnGenerate(wxCommandEvent& event );

private:
    DECLARE_DYNAMIC_CLASS(LitesqlView)
    DECLARE_EVENT_TABLE()

    wxMenu* m_ctxMenu;

};

#endif // #ifndef LITESQL_VIEW_H