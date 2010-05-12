#include "stdafx.h"
#include "KeepCmd.h"

#include "MayaUtils/ErrorHelpers.h"

#include <maya/MSyntax.h>

//-----------------------------------------------------------------------------
// the name of the KeepCmd command
//-----------------------------------------------------------------------------
MString KeepCmd::CommandName( "keep" );

//-----------------------------------------------------------------------------
// KeepCmd::newSyntax
// definition of the syntax for command
//-----------------------------------------------------------------------------
MSyntax KeepCmd::newSyntax()
{
  return MSyntax ();
}

//-----------------------------------------------------------------------------
// KeepCmd::doIt
// execution of the command
//-----------------------------------------------------------------------------
MStatus KeepCmd::doIt( const MArgList & args )
{
  MStatus stat;

  MAYA_START_EXCEPTION_HANDLING();

  for ( MItDependencyNodes it; !it.isDone(); it.next() )
  {
    MFnDependencyNode( it.item() ).setDoNotWrite( false );
  }

  MAYA_FINISH_EXCEPTION_HANDLING();

  return stat;
}