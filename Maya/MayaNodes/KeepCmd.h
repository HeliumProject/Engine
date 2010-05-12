#pragma once

#include <maya/MPxCommand.h>

class KeepCmd : public MPxCommand
{
public:
  static MString CommandName;

  static void * creator()               { return new KeepCmd; }
  static MSyntax newSyntax();

  virtual bool isUndoable()             { return false; }
  virtual bool hasSyntax()              { return true; }

  virtual MStatus doIt( const MArgList & args );
};
