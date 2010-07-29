#ifndef VisualLitesqlApp_H
#define VisualLitesqlApp_H

#include <wx/app.h>
#include <wx/docmdi.h>

class MainFrame;

class VisualLitesqlApp : public wxApp
{
public:
  enum {
        ID_GENERATE = wxID_HIGHEST+1,
        ID_ADD_OBJECT,
        ID_REMOVE_OBJECT,
        ID_ADD_FIELD,
        ID_REMOVE_FIELD,
        ID_ADD_METHOD,
        ID_REMOVE_METHOD,
        ID_ADD_RELATED,
        ID_REMOVE_RELATED,
        ID_ADD_RELATION,
        ID_REMOVE_RELATION,

        ID_ABOUT 
        };
    VisualLitesqlApp(void); 
    virtual ~VisualLitesqlApp(void);

    void info(const wxString& message);
    void warn(const wxString& message);
    void error(const wxString& message);

    wxMDIChildFrame *CreateChildFrame(wxDocument *doc, wxView *view);
    
    wxDocTemplate *m_pGenerateViewTemplate;

protected:
	bool OnInit(void);
   int OnExit(void);
  
private:

  protected:
    wxDocManager* m_docManager;
  
    MainFrame* pMainframe;
};

DECLARE_APP(VisualLitesqlApp)

#endif // #ifndef VisualLitesqlApp_H