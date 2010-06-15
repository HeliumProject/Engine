#pragma once

#include "Luna/API.h"

#include "Platform/Types.h"

namespace Luna
{
  class LUNA_CORE_API AutoFlexSizer : public wxFlexGridSizer
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
