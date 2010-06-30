#include "LitesqlMethodPanel.h"
#include "objectmodel.hpp"

using namespace xml;

LitesqlMethodPanel::LitesqlMethodPanel( wxWindow* parent , Method* pMethod)
:
MethodPanel( parent ),
m_pMethod(pMethod)
{
  m_textCtrlName->SetValidator(StdStringValidator(wxFILTER_ALPHANUMERIC,&m_pMethod->name));
}
