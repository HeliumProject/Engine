#pragma once

#include "API.h"

namespace Maya
{
  ///////////////////////////////////////////////////////////////////////////////
  // MaterialEditorCmd creates the maya command: materialEditor
  //

  class MAYAMESHSETUP_API MaterialEditorCmd : public MPxCommand
  {
  public:
    MaterialEditorCmd();
    virtual ~MaterialEditorCmd();

    static void*    Creator();
    static MSyntax  CommandSyntax();
    void            DisplayUsage() const;

    MStatus         doIt( const MArgList &args );

    static void     MayaExistingCallBack( void* clientData );

  public:
    static MString    m_CommandName;

  private:
    static MStatus ShowMaterialEditor();
  };

} // namespace Maya
