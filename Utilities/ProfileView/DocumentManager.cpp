#include "Precompiled.h"
#include "DocumentManager.h"

#include <algorithm>

DocumentManager::DocumentManager()
: m_CurrentDocument (NULL)
{

}

void DocumentManager::Open()
{
  DocumentPtr document = new Document ();

  if (document->Open())
  {
    m_Documents.push_back( document );

    m_DocumentAdded.Raise( DocumentArgs (document) );
  }
}

bool DocumentManager::Close()
{
  if ( m_CurrentDocument )
  {
    DocumentPtr document = m_CurrentDocument; // will be deleted when scope closes

    m_DocumentRemoved.Raise( DocumentArgs (document) );

    V_Document::iterator itr = m_Documents.begin();
    V_Document::iterator end = m_Documents.end();
    for ( ; itr != end; ++itr )
    {
      if ( *itr == document )
      {
        m_Documents.erase( itr );
        break;
      }
    }
  }

  return true;
}

bool DocumentManager::CloseAll()
{
  bool closed = true;

  while (!m_Documents.empty() && closed)
  {
    m_CurrentDocument = m_Documents.front();

    closed = Close();
  }

  if (closed)
  {
    m_CurrentDocument = NULL;
  }

  return closed;
}
