#pragma once

#include "Grid.h"

#include "Common/Compiler.h"

namespace Luna
{
  /////////////////////////////////////////////////////////////////////////////
  // Overrides Grid and adds a column for rendering icons in each row.
  // 
  class GridWithIcons : public Grid
  {
  private:
    // New list of the column numbers
    enum Columns
    {
      Visibility = 0,
      Selectability,
      Icon,
      Name,

      COLUMN_TOTAL
    };

  protected:
    u32 m_ColumnIcon;

  public:
    GridWithIcons( wxWindow* parent, wxWindowID gridID, bool allowRename = false, bool showColLabels = true );
    virtual ~GridWithIcons();
    virtual bool AddRow( const std::string& name, bool visible, bool selectable, i32 imageIndex );

  protected:
    virtual bool AddRow( const std::string& name, bool visible, bool selectable );
    virtual void ResizeColumns() NOC_OVERRIDE;
  };
}
