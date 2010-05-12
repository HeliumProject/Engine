#pragma once

#include <maya/MPxCommand.h>
#include <maya/MDagModifier.h>

class FragmentCmd : public MPxCommand
{
public:
  static MString CommandName;

protected:
  MDagModifier m_DagMod;
  MSelectionList m_Selection;
  
  // flags -- these must stick around as object data to support undo/redo
  bool m_CreateFragment;
  bool m_SendFragments;

public:
  FragmentCmd();
  virtual ~FragmentCmd();

  static void * creator()
  {
    return new FragmentCmd;
  }

  static MSyntax newSyntax();

  virtual bool isUndoable() const
  {
    return true;
  }

  virtual bool hasSyntax() const
  {
    return true;
  }

  virtual MStatus doIt( const MArgList & args );
  virtual MStatus redoIt();
  virtual MStatus undoIt();

  MStatus CreateNewFragment();
  MStatus Fragment();
};
