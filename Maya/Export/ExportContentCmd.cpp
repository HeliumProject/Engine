
#include "ExportContentCmd.h"

#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MSyntax.h>
#include <maya/MFnStringData.h>
#include <maya/MFileIO.h>
#include <maya/MFnDependencyNode.h>
#include <maya/M3dView.h>

#include "MayaContent/MayaContentCmd.h"
#include "MayaUtils/Export.h"

#include "Finder/Finder.h"
#include "Finder/ContentSpecs.h"
#include "Application/RCS/RCS.h"

using namespace MayaContent;

extern void AfterSaveCallback( void *clientData );
extern MCallbackId g_AfterSaveCallbackID;


static const char* ExportAnimShort = "-a";
static const char* ExportAnimLong  = "-animation";

//-----------------------------------------------------------------------------
// the name of the ExportContentCmd command
//-----------------------------------------------------------------------------
MString ExportContentCmd::CommandName( "exportContent" );

void AfterSaveCallback( void *clientData )
{
  std::string currentFile = MFileIO::currentFile().asChar();
  Nocturnal::Path::Normalize( currentFile );

  // i hate mel
  int exportOnSave = 0;
  if ( !MGlobal::executeCommand( "optionVar -q \"exportOnSave\"", exportOnSave ) )
  {
    MGlobal::displayError( "Could not read 'exportOnSave' option value!" );
    return;
  }

  if ( exportOnSave )
  {
    MArgDatabase argParser;
    ExportContentCmd::ExportContent( argParser );
  }
}


//-----------------------------------------------------------------------------
// ExportContentCmd::newSyntax
// definition of the syntax for command
//-----------------------------------------------------------------------------
MSyntax ExportContentCmd::newSyntax()
{
  MSyntax syntax;

  syntax.setObjectType(MSyntax::kSelectionList, 0); // will function without error, but do nothing with no selection
  syntax.useSelectionAsDefault( true );

  syntax.addFlag( ExportAnimShort, ExportAnimLong );

  return syntax;
}

//-----------------------------------------------------------------------------
// ExportContentCmd::doIt
// execution of the command
//-----------------------------------------------------------------------------
MStatus ExportContentCmd::doIt( const MArgList & args )
{
  MAYAEXPORTER_SCOPE_TIMER((""));

  MStatus stat;

  // parse the command line arguments using the declared syntax
  MArgDatabase argParser( syntax(), args, &stat );

  std::string currentFile = MFileIO::currentFile().asChar();
  Nocturnal::Path::Normalize( currentFile );

  // make sure we save before we export, the artists asked us to do this for a variety of reasons
  // including fear that maya would crash during export and lose work
  if ( MGlobal::mayaState() == MGlobal::kInteractive )
  {
    if ( true ) //PromptCheckoutMayaFile( currentFile ) )
    {
      // don't want this callback hit from in here
      MSceneMessage::removeCallback( g_AfterSaveCallbackID );

      MStatus saveStatus = MFileIO::save();

      // re-register our callback
      g_AfterSaveCallbackID = MSceneMessage::addCallback( MSceneMessage::kAfterSave, AfterSaveCallback );

      if ( !saveStatus )
      {
        if ( MGlobal::mayaState() == MGlobal::kInteractive )
        {
          MessageBoxA( M3dView::applicationShell(), "Could not save maya file, cancelling export!", "Save Error", MB_OK | MB_ICONERROR );
        }
        else
        {
          std::cerr << "Could not save maya file, cancelling export!" << std::endl;
        }
        return MS::kFailure;
      }
    }
    else if ( MGlobal::mayaState() == MGlobal::kInteractive )
    {
      std::string error = currentFile + " was not checked out or is not writable and has not been saved.  Do you still want to export this file? (If you export the file, its appearance will change in the game, but the source data will not match it and the export data will be different if someone else exports the file!)";
      // return if they don't want to export anymore
      if ( IDNO == MessageBoxA( M3dView::applicationShell(), error.c_str(), "File Not Saved", MB_YESNO | MB_ICONEXCLAMATION ) )
      {
        return MS::kFailure;
      }

      MessageBoxA( M3dView::applicationShell(), "WARNING: Exporting file without saving source data!", "Export Warning", MB_OK | MB_ICONHAND );
    }
    else
    {
      std::cerr << "WARNING: Exporting file without saving source data!" << std::endl;
      return MS::kFailure;
    }
  }

  //without this initexportinfo we don't find the nodes it needs to export animation data
  std::vector<std::string> selection;
  std::vector<std::string> groupNode;
  Maya::InitExportInfo( true, selection, groupNode );

  MGlobal::executeCommand( "finalizeDestruction" );

  return ExportContent( argParser );
}

MStatus ExportContentCmd::ExportContent( MArgDatabase& argParser )
{
  try
  {
    MayaContentCmd::ExportCurrentScene();
  }
  catch( RCS::FileInUseException& e )
  {
    MGlobal::displayError( MString("Failed to export: ") + e.what() );
    if ( MGlobal::mayaState() == MGlobal::kInteractive )
    {
      MessageBoxA( M3dView::applicationShell(), e.what() , "Export Error", MB_OK | MB_ICONERROR );
    }
    return MS::kFailure;
  }
  catch (Nocturnal::Exception& e)
  {
    MGlobal::displayError( MString("Failed to export: ") + e.what() );
    if ( MGlobal::mayaState() == MGlobal::kInteractive )
    {
      MessageBoxA( M3dView::applicationShell(),  e.what(), "Export Error", MB_OK | MB_ICONERROR );
    }
    return MS::kFailure;
  }
  
  return MS::kSuccess;
}