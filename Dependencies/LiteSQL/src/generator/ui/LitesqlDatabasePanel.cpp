#include "LitesqlDatabasePanel.h"
#include "xmlObjects.hpp"

#include "ddx.h"

using namespace ui;
using namespace xml;

LitesqlDatabasePanel::LitesqlDatabasePanel( wxWindow* parent,Database* db )
:
DatabasePanel( parent ),
m_database(db)
{
  m_textName->SetValidator(StdStringValidator(wxFILTER_ALPHANUMERIC,&m_database->name));
  m_textInclude->SetValidator(StdStringValidator(wxFILTER_ALPHANUMERIC,&m_database->include));
  m_textNamespace->SetValidator(StdStringValidator(wxFILTER_ALPHANUMERIC,&m_database->nspace));
}