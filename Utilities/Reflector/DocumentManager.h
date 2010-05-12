#pragma once

#include "Document.h"
#include "Common/Automation/Event.h"

#include <map>

struct DocumentArgs
{
  Document* m_Document;

  DocumentArgs( Document* document )
    : m_Document( document )
  {

  }
};
typedef Nocturnal::Signature<void, const DocumentArgs&> DocumentSignature;

typedef std::vector<DocumentPtr> V_Document;

class DocumentManager
{
public:
  DocumentManager();

  void Open();
  void New();
  void Save();
  void SaveAs();
  void SaveAll();
  bool Close();
  bool CloseAll();

  DocumentSignature::Event& GetDocumentAddedEvent()
  {
    return m_DocumentAdded;
  }
  
  DocumentSignature::Event& GetDocumentRemovedEvent()
  {
    return m_DocumentRemoved;
  }

private:
  V_Document                  m_Documents;
  Document*                   m_CurrentDocument;
  DocumentSignature::Event    m_DocumentAdded;
  DocumentSignature::Event    m_DocumentRemoved;
};