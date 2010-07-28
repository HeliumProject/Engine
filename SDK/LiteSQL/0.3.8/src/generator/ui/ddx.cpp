#include "ddx.h"


IMPLEMENT_DYNAMIC_CLASS(StdStringValidator,wxTextValidator)

StdStringValidator::StdStringValidator(long style, LiteSQL_String *val)
: pValue(val), 
value(*val),
wxTextValidator(style,&value)
{ 
};

StdStringValidator ::StdStringValidator (const StdStringValidator& val) 
{
  pValue = val.pValue;
  value = val.value;
  Copy(val);
  m_stringValue = &value;
};

wxObject *StdStringValidator ::Clone() const
{ return new StdStringValidator(*this); }

// Called to transfer data to the window
bool StdStringValidator ::TransferToWindow() 
{ 
  value = wxString::FromUTF8(pValue->c_str()); 
  return wxTextValidator::TransferToWindow();
}

// Called to transfer data from the window
bool StdStringValidator ::TransferFromWindow() 
{ 
  bool result = wxTextValidator::TransferFromWindow();
  *pValue = value.ToUTF8(); 
  return result; 
}
