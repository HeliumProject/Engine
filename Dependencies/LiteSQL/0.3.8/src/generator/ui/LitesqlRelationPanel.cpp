#include "LitesqlRelationPanel.h"

#include "objectmodel.hpp"
#include "ddx.h"

using namespace xml;

LitesqlRelationPanel::LitesqlRelationPanel( wxWindow* parent, Relation* pRelation )
: ui::RelationPanel( parent ),
  m_pRelation(pRelation)
{
  m_textCtrlName->SetValidator(StdStringValidator(wxFILTER_ALPHANUMERIC,&m_pRelation->name));
}