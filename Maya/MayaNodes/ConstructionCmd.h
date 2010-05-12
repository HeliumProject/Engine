#pragma once

#include <maya/MPxCommand.h>

class ConstructionCmd : public MPxCommand
{
public:
  static MString CommandName;

  static MString ConnectFlagShort;
  static MString ConnectFlagLong;

  static MString DisconnectFlagShort;
  static MString DisconnectFlagLong;

  static MString DispatchFlagShort;
  static MString DispatchFlagLong;

  static MString SetEntityClassShort;
  static MString SetEntityClassLong;

  static void * creator()               { return new ConstructionCmd; }
  static MSyntax newSyntax();

  virtual bool isUndoable()             { return false; }
  virtual bool hasSyntax()              { return true; }

  virtual MStatus doIt( const MArgList & args );
};
