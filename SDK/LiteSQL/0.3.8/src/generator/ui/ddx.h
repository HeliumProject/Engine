#ifndef DDX_H
#define DDX_H

#include <string>
#include <wx/choice.h>
#include <wx/valtext.h>

class StdStringValidator : public wxTextValidator {

  DECLARE_DYNAMIC_CLASS(StdStringValidator)

public:
  StdStringValidator (long style = wxFILTER_NONE, std::string *val = 0);
  StdStringValidator (const StdStringValidator& val);

  virtual wxObject *Clone() const;
    
  // Called to transfer data to the window
  virtual bool TransferToWindow();

    // Called to transfer data from the window
  virtual bool TransferFromWindow();

private:
  std::string* pValue;
  wxString value; 
};



#endif // #ifndef DDX_H
