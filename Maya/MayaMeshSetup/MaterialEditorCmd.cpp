#include "stdafx.h"

#include "MaterialEditorCmd.h"
#include "MaterialEditor.h"
#include "MaterialDisplayNode.h"

using namespace Maya;


MString MaterialEditorCmd::m_CommandName( "materialEditor" );

//
// Command flags
//

const char* EditorUIFlag = "-e";
const char* EditorUIFlagLong = "-editor";


//
// Usage Display Text
//

const static char *s_UsageMessage =
"\n \
Usage: materialEditor [-editor] \n \
\n \
Flags: \n \
\t -e, editor       -- Open the MaterialEditor Tool. \n \
";


MaterialEditorPtr g_MaterialEditorDlg = NULL;


MaterialEditorCmd::MaterialEditorCmd()
{
}

MaterialEditorCmd::~MaterialEditorCmd()
{
}


void *MaterialEditorCmd::Creator()
{
  return new MaterialEditorCmd();
}


MSyntax MaterialEditorCmd::CommandSyntax()
{
  MSyntax syntax;

  syntax.addFlag( EditorUIFlag, EditorUIFlagLong );

  syntax.setObjectType(MSyntax::kSelectionList, 0, 1);
  // if no objects are specified, then tell the command to use the selected objects.
  //syntax.useSelectionAsDefault(true);

  return syntax;
}


void MaterialEditorCmd::DisplayUsage(void) const
{
  MPxCommand::displayError( MString( s_UsageMessage ) );
}


MStatus MaterialEditorCmd::doIt( const MArgList &args )
{
  MStatus status( MStatus::kSuccess );

  // Parse the arguments
  MArgDatabase argDatabase( syntax(), args, &status );

  if ( !status )
  {
    DisplayUsage();
    return status;
  }

  if ( argDatabase.isFlagSet( EditorUIFlag ) )
  {
    return ShowMaterialEditor();
  }

  return status;
}

///////////////////////////////////////////////////////////////////////////////
// Open and show the MaterialEditor tool
//
MStatus MaterialEditorCmd::ShowMaterialEditor()
{
  MStatus status( MStatus::kSuccess );

  const MObject& materialDispNode = MaterialDisplayNode::GetMaterialDisplayNode( &status );
  NOC_ASSERT( materialDispNode != MObject::kNullObj );

  if ( !g_MaterialEditorDlg )
  {
    g_MaterialEditorDlg = new MaterialEditor(); 
  }
  g_MaterialEditorDlg->Show();
  g_MaterialEditorDlg->SetFocus();

  return status;
}


///////////////////////////////////////////////////////////////////////////////
// Callback for when maya s existing to make sure that the MaterialEditor
// window is hidden
//
void MaterialEditorCmd::MayaExistingCallBack( void* clientData )
{
  if ( g_MaterialEditorDlg )
  {
    g_MaterialEditorDlg->Hide();
  }
}