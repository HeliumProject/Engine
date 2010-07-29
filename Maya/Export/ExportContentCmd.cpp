#include "Precompile.h"
#include "ExportContentCmd.h"

#include <maya/MArgDatabase.h>
#include <maya/MArgList.h>
#include <maya/MSyntax.h>
#include <maya/MFnStringData.h>
#include <maya/MFileIO.h>
#include <maya/MFnDependencyNode.h>
#include <maya/M3dView.h>

#include "Export/MayaContentCmd.h"

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
    tstring currentFile = MFileIO::currentFile().asTChar();
    Helium::Path::Normalize( currentFile );

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
    EXPORT_SCOPE_TIMER((""));

    MStatus stat;

    // parse the command line arguments using the declared syntax
    MArgDatabase argParser( syntax(), args, &stat );

    tstring currentFile = MFileIO::currentFile().asTChar();
    Helium::Path::Normalize( currentFile );

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
                    MessageBox( M3dView::applicationShell(), TXT("Could not save maya file, cancelling export!"), TXT("Save Error"), MB_OK | MB_ICONERROR );
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
            tstring error = currentFile + TXT(" was not checked out or is not writable and has not been saved.  Do you still want to export this file? (If you export the file, its appearance will change in the game, but the source data will not match it and the export data will be different if someone else exports the file!)");
            // return if they don't want to export anymore
            if ( IDNO == MessageBox( M3dView::applicationShell(), error.c_str(), TXT("File Not Saved"), MB_YESNO | MB_ICONEXCLAMATION ) )
            {
                return MS::kFailure;
            }

            MessageBox( M3dView::applicationShell(), TXT("WARNING: Exporting file without saving source data!"), TXT("Export Warning"), MB_OK | MB_ICONHAND );
        }
        else
        {
            std::cerr << "WARNING: Exporting file without saving source data!" << std::endl;
            return MS::kFailure;
        }
    }

    return ExportContent( argParser );
}

MStatus ExportContentCmd::ExportContent( MArgDatabase& argParser )
{
    try
    {
#pragma TODO("Set data type")
        MayaContentCmd::ExportCurrentScene( MayaContentCmd::kScene );
    }
    catch (Helium::Exception& e)
    {
        MGlobal::displayError( MString("Failed to export: ") + e.What() );
        if ( MGlobal::mayaState() == MGlobal::kInteractive )
        {
            MessageBox( M3dView::applicationShell(),  e.What(), TXT("Export Error"), MB_OK | MB_ICONERROR );
        }
        return MS::kFailure;
    }

    return MS::kSuccess;
}