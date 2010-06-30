#ifndef _DestructionCmd
#define _DestructionCmd

//
// Copyright (C) Insomniac Games
// 
// File: DestructionCmd.h
//
// Author: Maya Plug-in Wizard 2.0
//

#include <maya/MPxCommand.h>
#include <maya/MSyntax.h>

class MArgList;
struct UVALocalData;
struct ID3DXBuffer;
struct ID3DXMesh;
struct IDirect3DDevice9;

class DestructionCmd : public MPxCommand
{
public:
  DestructionCmd();
  virtual		  ~DestructionCmd();

  MStatus		  doIt(const MArgList &args);
  bool		    isUndoable() const	{ return false; }

  static void * 	creator();
  static MSyntax  newSyntax();


  static MString CommandName;

private:
  MStatus			ParseArgs(const MArgList &args);
};

#endif