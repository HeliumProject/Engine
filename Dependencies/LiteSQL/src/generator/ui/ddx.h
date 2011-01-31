#ifndef DDX_H
#define DDX_H

#include <wx/choice.h>
#include <wx/valtext.h>
#include <string>

class StdStringValidator : public wxTextValidator {

  DECLARE_DYNAMIC_CLASS(StdStringValidator)

public:
  StdStringValidator (long style = wxFILTER_NONE, LITESQL_String *val = 0);
  StdStringValidator (const StdStringValidator& val);

  virtual wxObject *Clone() const;
    
  // Called to transfer data to the window
  virtual bool TransferToWindow();

    // Called to transfer data from the window
  virtual bool TransferFromWindow();

private:
  LITESQL_String* pValue;
  wxString value; 
};



#endif // #ifndef DDX_H