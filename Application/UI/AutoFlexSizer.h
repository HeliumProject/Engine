#pragma once

#include "Application/API.h"

#include "Platform/Types.h"

#include <wx/sizer.h>

namespace Nocturnal
{
  class APPLICATION_API AutoFlexSizer : public wxFlexGridSizer
  {
  public:
    AutoFlexSizer( int vgap = 0, int hgap = 0 );
    virtual ~AutoFlexSizer();

    virtual void RecalcSizes();
    virtual wxSize CalcMin();
  
  protected:
    void UpdateRowsCols();
    int GetMaxColumnWidth( int numItems );
  };
}
