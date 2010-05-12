#pragma once

#include "API.h"

#include "Common/Types.h"

namespace UIToolKit
{
  class UITOOLKIT_API AutoFlexSizer : public wxFlexGridSizer
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
