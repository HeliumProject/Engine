#include "LitesqlFieldPanel.h"
#include "objectmodel.hpp"

//#include "litesql/field.hpp"

#include "ddx.h"

using namespace xml;

IMPLEMENT_DYNAMIC_CLASS(LitesqlFieldPanel,ui::FieldPanel)

/////////////////////////////////////////////////////////////////////////////

LitesqlFieldPanel::LitesqlFieldPanel( wxWindow* parent , Field* pField)
:
FieldPanel( parent ),
m_pField(pField)
{
  m_textCtrlName->Enable(m_pField->isEditable());
  m_choiceFieldtype->Enable(m_pField->isEditable());
  m_textCtrlDefaultValue->Enable(m_pField->isEditable()); 
  m_checkBoxIndexed->Enable(m_pField->isEditable());
  m_checkBoxUnique->Enable(m_pField->isEditable());

  
  m_textCtrlName->SetValidator(StdStringValidator(wxFILTER_ALPHANUMERIC,&m_pField->name));
  m_textCtrlDefaultValue->SetValidator(StdStringValidator(wxFILTER_ALPHANUMERIC,&m_pField->default_));
  // \TODO get Fieldtypes from litesql::model
  m_choiceFieldtype->Append(FieldTypeValidator::FIELDTYPES);
  m_choiceFieldtype->SetValidator(FieldTypeValidator(m_pField));

  m_checkBoxIndexed->SetValidator(FieldIndexedValidator(m_pField));
  m_checkBoxUnique->SetValidator(FieldUniqueValidator(m_pField));
  m_listValues->SetValidator(FieldValuesValidator(m_pField));
}

void LitesqlFieldPanel::OnAddValue( wxCommandEvent& event )
{
  m_listValues->AppendString(_( LiteSQL_L( "newValue" )));
}

void LitesqlFieldPanel::OnRemoveValue( wxCommandEvent& event )
{}

// FieldTypeValidator implementation
/****************************************************************************/
const wxString FTSTRING[] = { 
               _( LiteSQL_L( "" )),
               _( LiteSQL_L( "boolean" )),
               _( LiteSQL_L( "integer" )),
               _( LiteSQL_L( "LiteSQL_String" )),
               _( LiteSQL_L( "float" )),
               _( LiteSQL_L( "double" )),
               _( LiteSQL_L( "time" )),
               _( LiteSQL_L( "date" )),
               _( LiteSQL_L( "datetime" )),
               _( LiteSQL_L( "blob" )) 
};  

const wxArrayString FieldTypeValidator::FIELDTYPES(sizeof(FTSTRING)/sizeof(FTSTRING[0]),FTSTRING);

IMPLEMENT_DYNAMIC_CLASS(FieldTypeValidator,wxGenericValidator)

FieldTypeValidator::FieldTypeValidator (xml::Field *val)
: m_pField(val),
  wxGenericValidator(&value)
{
}

FieldTypeValidator::FieldTypeValidator (const FieldTypeValidator& val)
: wxGenericValidator(val)
{
  m_pField= val.m_pField;
  value = val.value;
  Copy(val);
  m_pString = &value;
}

wxObject *FieldTypeValidator::Clone() const
{ return new FieldTypeValidator(*this); }
    
  // Called to transfer data to the window
bool FieldTypeValidator::TransferToWindow() 
{
  value = wxString::FromUTF8(litesql::toString(m_pField->type));
  return wxGenericValidator::TransferToWindow();
}

    // Called to transfer data from the window
bool FieldTypeValidator::TransferFromWindow()
{
  bool rval = wxGenericValidator::TransferFromWindow();
  m_pField->type = litesql::field_type(value.ToUTF8());
  return rval;
}

// FieldIndexedValidator implementation
/****************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(FieldIndexedValidator,wxGenericValidator)

FieldIndexedValidator::FieldIndexedValidator (xml::Field* pField)
: m_pField(pField),
  wxGenericValidator(&isIndexed)
{
}

FieldIndexedValidator::FieldIndexedValidator (const FieldIndexedValidator& val)
: wxGenericValidator(val)
{
  m_pField= val.m_pField;
  Copy(val);
  m_pBool = &isIndexed;
}

wxObject *FieldIndexedValidator::Clone() const
{ return new FieldIndexedValidator(*this); }
    
  // Called to transfer data to the window
bool FieldIndexedValidator::TransferToWindow() 
{
  isIndexed = m_pField->isIndexed();
  return wxGenericValidator::TransferToWindow();
}

    // Called to transfer data from the window
bool FieldIndexedValidator::TransferFromWindow()
{
  bool rval = wxGenericValidator::TransferFromWindow();
  if (rval) 
  {
    m_pField->indexed = isIndexed ? A_field_indexed_true : A_field_indexed_false; 
  }
  return rval;
}

// FieldUniqueValidator implementation
/****************************************************************************/

IMPLEMENT_DYNAMIC_CLASS(FieldUniqueValidator,wxGenericValidator)

FieldUniqueValidator::FieldUniqueValidator (xml::Field* pField)
: m_pField(pField),
  wxGenericValidator(&isUnique)
{
}

FieldUniqueValidator::FieldUniqueValidator (const FieldUniqueValidator& val)
: wxGenericValidator(val)
{
  m_pField= val.m_pField;
  Copy(val);
  m_pBool = &isUnique;
}

wxObject *FieldUniqueValidator::Clone() const
{ return new FieldUniqueValidator(*this); }
    
  // Called to transfer data to the window
bool FieldUniqueValidator::TransferToWindow() 
{
  isUnique = m_pField->isUnique();
  return wxGenericValidator::TransferToWindow();
}

    // Called to transfer data from the window
bool FieldUniqueValidator::TransferFromWindow()
{
  bool rval = wxGenericValidator::TransferFromWindow();
  if (rval) 
  {
    m_pField->unique = isUnique ? A_field_unique_true : A_field_unique_false; 
  }
  return rval;
}


// FieldValuesValidator implementation
/****************************************************************************/
IMPLEMENT_DYNAMIC_CLASS(FieldValuesValidator,wxValidator)

FieldValuesValidator::FieldValuesValidator (xml::Field *val)
: m_pField(val),
  wxValidator()
{
  // \TODO add values init
}

FieldValuesValidator::FieldValuesValidator (const FieldValuesValidator& val)
{
  Copy(val);
  m_pField= val.m_pField;
}

wxObject *FieldValuesValidator::Clone() const
{ return new FieldValuesValidator(*this); }
    
  // Called to transfer data to the window
bool FieldValuesValidator::TransferToWindow() 
{
  wxWindow * pWin = GetWindow();
  if ( !pWin || !pWin->IsKindOf(CLASSINFO(wxListBox)) )
  {
    return false;    
  }
  else {
    wxListBox* pList = (wxListBox*)pWin; 
    pList->Clear();
    size_t pos = 0;
    for (std::vector<Value>::const_iterator it = m_pField->values.begin(); it != m_pField->values.end();it++)
    {
      pList->Insert(wxString::FromUTF8(it->name.c_str()),pos++);
    }
  }
  return true;
}

    // Called to transfer data from the window
bool FieldValuesValidator::TransferFromWindow()
{
  wxWindow * pWin = GetWindow();
  if ( !pWin || !pWin->IsKindOf(CLASSINFO(wxListBox)) )
  {
    return false;    
  }
  else
  {
    wxArrayString values = ((wxListBox*)pWin)->GetStrings();
    
    m_pField->values.clear();
    size_t i = 0;
    for (wxArrayString::const_iterator it = values.begin(); it != values.end();it++)
    {
      LiteSQL_String name = (*it).ToUTF8();
      xml::Value v(name,toString<int>(i++));
      m_pField->values.push_back( v );
    }
    return true;
  }
}