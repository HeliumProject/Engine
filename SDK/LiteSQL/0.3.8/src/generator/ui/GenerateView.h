#ifndef GENERATE_VIEW_H
#define GENERATE_VIEW_H

#include <wx/docview.h>
#include "LitesqlGeneratePanel.h"


class GenerateView: public wxView
{
public:
    wxMDIChildFrame *frame;
  
    GenerateView();
    virtual ~GenerateView();


    bool OnCreate(wxDocument *doc, long flags);
    void OnUpdate(wxView *sender, wxObject *hint = (wxObject *) NULL);
    
    void OnDraw(wxDC *dc);
    bool OnClose(bool deleteWindow = true);

private:
  LitesqlGeneratePanel* m_pGeneratePanel;


    DECLARE_DYNAMIC_CLASS(GenerateView)
    DECLARE_EVENT_TABLE()
};

#endif // #ifndef GENERATE_VIEW_H