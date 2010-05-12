#pragma once

#include "API.h"
#include <maya/MGlobal.h>
#include <maya/MPxCommand.h>
#include <maya/MMessage.h>
#include <maya/MSceneMessage.h>
#include <maya/MArgDatabase.h>

class ExportContentCmd : public MPxCommand
{
public:
  static MString CommandName;

  static void * creator()               { return new ExportContentCmd; }
  static MSyntax newSyntax();

  virtual bool isUndoable()             { return false; }
  virtual bool hasSyntax()              { return true; }

  virtual MStatus doIt( const MArgList & args );
  static MStatus ExportContent( MArgDatabase& argParser );

protected:

  bool parseArgs( const MArgList & args );
};
