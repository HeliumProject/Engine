#pragma once

#include "Application/API.h"
#include "Application/Inspect/Widgets/Control.h"

#include "Foundation/Reflect/Class.h"

class CheckListBox;

namespace Inspect
{
  //
  // Listbox control (list with check boxes next to each item).
  // Underlying data is a map of string to bool (item label to check box state).
  //

  // Not supported for now (data is an std::map< string, bool > so it is sorted already)
  //const static char CHECK_LIST_ATTR_SORTED[]    = "sorted";

  class APPLICATION_API CheckList : public Reflect::ConcreteInheritor<CheckList, Control>
  {
  public:
    CheckList();

    virtual void Realize( Container* parent ) NOC_OVERRIDE;
    virtual void Read() NOC_OVERRIDE;
    virtual bool Write() NOC_OVERRIDE;
    virtual bool IsDefault() const;

  protected:
  };

  typedef Nocturnal::SmartPtr<CheckList> CheckListPtr;
}
