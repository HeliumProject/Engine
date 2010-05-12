#pragma once

#include "API.h"

#include <maya/MPxCommand.h>

class MAYANODES_API ZoneCmd : public MPxCommand
{
public:
  static MString CommandName;
  
  static void * creator()
  {
    return new ZoneCmd;
  }

  static MSyntax newSyntax();

  virtual bool isUndoable()
  {
    return false;
  }

  virtual bool hasSyntax()
  {
    return true;
  }

  virtual MStatus doIt( const MArgList & args );

  static MStatus Load( const std::string& filePath, bool importArt = true );
  static MStatus Save( const std::string& filePath );
};
