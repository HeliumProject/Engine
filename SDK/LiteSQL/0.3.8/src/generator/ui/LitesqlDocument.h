#ifndef LITESQL_DOCUMENT_H
#define LITESQL_DOCUMENT_H

#include <wx/docview.h>
#include "objectmodel.hpp"


class LitesqlDocument: public wxDocument
{
  DECLARE_DYNAMIC_CLASS(LitesqlDocument)

public:

  LitesqlDocument(void);
  virtual ~LitesqlDocument(void);

  litesql::ObjectModel* GetModel();
  void RemoveField(xml::Field* pField);

protected:

#if wxUSE_STD_IOSTREAM
  wxSTD ostream& SaveObject(wxSTD ostream& text_stream);
  wxSTD istream& LoadObject(wxSTD istream& text_stream);
#else
  wxOutputStream& SaveObject(wxOutputStream& stream);
  wxInputStream& LoadObject(wxInputStream& stream);
#endif

private:
  litesql::ObjectModel* m_pModel; 
};

#endif // #ifndef LITESQL_DOCUMENT_H
