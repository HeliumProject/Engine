#include "stdafx.h"
#include "ConstructionCmd.h"
#include "ConstructionTool.h"

#include "File/Manager.h"

#include "MayaUtils/ErrorHelpers.h"

#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>

//-----------------------------------------------------------------------------
// the name of the ConstructionCmd command
//-----------------------------------------------------------------------------
MString ConstructionCmd::CommandName( "construction" );

//-----------------------------------------------------------------------------
// arguments to the ConstructionCmd command
//-----------------------------------------------------------------------------
MString ConstructionCmd::ConnectFlagShort = "-c";
MString ConstructionCmd::ConnectFlagLong  = "-connect";

MString ConstructionCmd::DisconnectFlagShort = "-d";
MString ConstructionCmd::DisconnectFlagLong  = "-disconnect";

MString ConstructionCmd::DispatchFlagShort = "-dp";
MString ConstructionCmd::DispatchFlagLong  = "-dispatch";

MString ConstructionCmd::SetEntityClassShort = "-ec";
MString ConstructionCmd::SetEntityClassLong = "-entityclass";

//-----------------------------------------------------------------------------
// ConstructionCmd::newSyntax
// definition of the syntax for command
//-----------------------------------------------------------------------------
MSyntax ConstructionCmd::newSyntax()
{
  MSyntax syntax;

  syntax.enableEdit();
  syntax.setObjectType(MSyntax::kSelectionList, 0); // will function without error, but do nothing with no selection
  syntax.useSelectionAsDefault( true );

  syntax.addFlag( ConnectFlagShort.asChar(),    ConnectFlagLong.asChar() );
  syntax.addFlag( DisconnectFlagShort.asChar(), DisconnectFlagLong.asChar() );
  syntax.addFlag( DispatchFlagShort.asChar(),   DispatchFlagLong.asChar() );
  syntax.addFlag( SetEntityClassShort.asChar(), SetEntityClassLong.asChar(), MSyntax::kString );

  return syntax;
}

//-----------------------------------------------------------------------------
// ConstructionCmd::doIt
// execution of the command
//-----------------------------------------------------------------------------
MStatus ConstructionCmd::doIt( const MArgList & args )
{
  MStatus stat;

  // parse the command line arguments using the declared syntax
  MArgDatabase argParser( syntax(), args, &stat );

  if( argParser.isFlagSet( SetEntityClassLong.asChar() ) )
  {
    MString entityPath;
    argParser.getFlagArgument( SetEntityClassLong.asChar(), 0, entityPath );
    Construction::g_ClassID = File::GlobalManager().GetID( entityPath.asChar() );
  }

  // don't need a selection object for these flags
  if( argParser.isFlagSet( ConnectFlagLong.asChar() ) )
  {
    MAYA_START_EXCEPTION_HANDLING();
    Construction::Initialize();
    MAYA_FINISH_EXCEPTION_HANDLING();
  }
  else if( argParser.isFlagSet( DisconnectFlagLong.asChar() ) )
  {
    MAYA_START_EXCEPTION_HANDLING();
    Construction::Cleanup();
    MAYA_FINISH_EXCEPTION_HANDLING();
  }
  else if( argParser.isFlagSet( DispatchFlagLong.asChar() ) )
  {
    MAYA_START_EXCEPTION_HANDLING();
    Construction::Dispatch();
    MAYA_FINISH_EXCEPTION_HANDLING();
  }

  return MS::kSuccess;
}