#include "Precompile.h"

#include "ExportInfoCmd.h"
#include "ExportNode.h"

#include <maya/MArgDatabase.h>
#include <maya/MSyntax.h>
#include <maya/MFnIkJoint.h>
#include <maya/MFnSet.h>
#include <maya/MFileIO.h>

//-----------------------------------------------------------------------------
// the name of the ExportInfoCmd command
//-----------------------------------------------------------------------------
MString ExportInfoCmd::CommandName( "exportInfo" );

//-----------------------------------------------------------------------------
// arguments to the ExportInfoCmd command
//-----------------------------------------------------------------------------
const char* ExportInfoCmd::s_flag_long_geom       = "-geometry";
const char* ExportInfoCmd::s_flag_short_geom      = "-g";
const char* ExportInfoCmd::s_flag_long_skel       = "-skeleton";
const char* ExportInfoCmd::s_flag_short_skel      = "-s";
const char* ExportInfoCmd::s_flag_long_bang       = "-bangle";
const char* ExportInfoCmd::s_flag_short_bang      = "-b";
const char* ExportInfoCmd::s_flag_long_water      = "-water";
const char* ExportInfoCmd::s_flag_short_water     = "-w";
const char* ExportInfoCmd::s_flag_long_water_sim  = "-waterSimulation";
const char* ExportInfoCmd::s_flag_short_water_sim = "-ws";
const char* ExportInfoCmd::s_flag_long_hrc        = "-highResCollision";
const char* ExportInfoCmd::s_flag_short_hrc       = "-hrc";
const char* ExportInfoCmd::s_flag_long_lrc        = "-lowResCollision";
const char* ExportInfoCmd::s_flag_short_lrc       = "-lrc";
const char* ExportInfoCmd::s_flag_long_path       = "-pathfinding";
const char* ExportInfoCmd::s_flag_short_path      = "-p";
const char* ExportInfoCmd::s_flag_long_index      = "-index";
const char* ExportInfoCmd::s_flag_short_index     = "-i";
const char* ExportInfoCmd::s_flag_long_long       = "-long";
const char* ExportInfoCmd::s_flag_short_long      = "-l";
const char* ExportInfoCmd::s_flag_long_verbose    = "-verbose";
const char* ExportInfoCmd::s_flag_short_verbose   = "-v";
const char* ExportInfoCmd::s_flag_long_aos        = "-allowOldStyle";
const char* ExportInfoCmd::s_flag_short_aos       = "-aos";
const char* ExportInfoCmd::s_flag_long_an         = "-artNumber";
const char* ExportInfoCmd::s_flag_short_an        = "-an";
const char* ExportInfoCmd::s_flag_long_node       = "-node";
const char* ExportInfoCmd::s_flag_short_node      = "-n";
const char* ExportInfoCmd::s_flag_long_groupNode  = "-groupNode";
const char* ExportInfoCmd::s_flag_short_groupNode = "-gn";
const char* ExportInfoCmd::s_flag_long_lightmap  = "-lightmapped";
const char* ExportInfoCmd::s_flag_short_lightmap = "-lm";
const char* ExportInfoCmd::s_flag_long_fragment  = "-fragment";
const char* ExportInfoCmd::s_flag_short_fragment = "-frg";

const MString ExportInfoCmd::s_igNodeNotFound("igNodeNotFound");


//-----------------------------------------------------------------------------
// ExportInfoCmd::newSyntax
// definition of the syntax for command
//-----------------------------------------------------------------------------
MSyntax ExportInfoCmd::newSyntax()
{
  MSyntax syntax;

  // the -q flag is used to read the export type of nodes
  syntax.enableQuery();
  // the type of export (default all)
  syntax.addFlag( s_flag_short_geom, s_flag_long_geom );
  syntax.addFlag( s_flag_short_lightmap, s_flag_long_lightmap );
  syntax.addFlag( s_flag_short_fragment, s_flag_long_fragment );
  syntax.addFlag( s_flag_short_skel, s_flag_long_skel );
  syntax.addFlag( s_flag_short_bang, s_flag_long_bang );
  syntax.addFlag( s_flag_short_water, s_flag_long_water );
  syntax.addFlag( s_flag_short_water_sim, s_flag_long_water_sim );
  syntax.addFlag( s_flag_short_hrc, s_flag_long_hrc );
  syntax.addFlag( s_flag_short_lrc, s_flag_long_lrc );
  syntax.addFlag( s_flag_short_path, s_flag_long_path );
  // the 'index' to be queried (default all)
  syntax.addFlag( s_flag_short_index, s_flag_long_index, MSyntax::kLong );
  // whether to return 'long' pathnames
  syntax.addFlag( s_flag_short_long, s_flag_long_long );
  // whether to return lots of info or only node names
  syntax.addFlag( s_flag_short_verbose, s_flag_long_verbose );
  // whether to return old style export info if no other info avail
  syntax.addFlag( s_flag_short_aos, s_flag_long_aos );
  // the artwork number that is sought (default all)
  syntax.addFlag( s_flag_short_an, s_flag_long_an, MSyntax::kLong );
  // the specific node to be queried
  syntax.addFlag( s_flag_short_node, s_flag_long_node, MSyntax::kString );
  // the group node to look under
  syntax.addFlag( s_flag_short_groupNode, s_flag_long_groupNode, MSyntax::kString );

  return syntax;
}

//-----------------------------------------------------------------------------
// ExportInfoCmd::parseArgs
// Load the arguments and validate.
//-----------------------------------------------------------------------------
bool ExportInfoCmd::parseArgs( const MArgList & args )
{
  static const MString groundPlane_transform("|groundPlane_transform");
  MStatus status;

  // parse the command line arguments using the declared syntax
  MArgDatabase argParser( syntax(), args, &status );

  if (status==MS::kSuccess)
  {
    // see what flags are set
    bool hasFlagGeom      = argParser.isFlagSet(s_flag_short_geom);
    bool hasFlagSkel      = argParser.isFlagSet(s_flag_short_skel);
    bool hasFlagBang      = argParser.isFlagSet(s_flag_short_bang);
    bool hasFlagHrc       = argParser.isFlagSet(s_flag_short_hrc);
    bool hasFlagLrc       = argParser.isFlagSet(s_flag_short_lrc);
    bool hasFlagPath      = argParser.isFlagSet(s_flag_short_path);
    bool hasFlagIndex     = argParser.isFlagSet(s_flag_short_index);
    bool hasFlagLong      = argParser.isFlagSet(s_flag_short_long);
    bool hasFlagVerbose   = argParser.isFlagSet(s_flag_short_verbose);
    bool hasFlagAos       = argParser.isFlagSet(s_flag_short_aos);
    bool hasFlagAn        = argParser.isFlagSet(s_flag_short_an);
    bool hasFlagNode      = argParser.isFlagSet(s_flag_short_node);
    bool hasFlagGroupNode = argParser.isFlagSet(s_flag_short_groupNode);
    bool hasFlagLightmap  = argParser.isFlagSet(s_flag_short_lightmap);
    bool hasFlagFragment  = argParser.isFlagSet(s_flag_short_fragment);

    // inspect any type flags
    int typesCount = 0;
    m_type = Content::ContentTypes::Default;
    if ( hasFlagGeom ) { typesCount++; m_type = Content::ContentTypes::Geometry; }
    if ( hasFlagSkel ) { typesCount++; m_type = Content::ContentTypes::Skeleton; }
    if ( hasFlagBang ) { typesCount++; m_type = Content::ContentTypes::Bangle; }
    if ( hasFlagHrc ) { typesCount++; m_type = Content::ContentTypes::HighResCollision; }
    if ( hasFlagLrc ) { typesCount++; m_type = Content::ContentTypes::LowResCollision; }
    if ( hasFlagPath ) { typesCount++; m_type = Content::ContentTypes::Pathfinding; }
    if ( hasFlagLightmap ) { typesCount++; m_type = Content::ContentTypes::LightMapped; }
    if ( hasFlagFragment ) { typesCount++; m_type = Content::ContentTypes::FragmentGroup; }

    // retrieve the value of the index flag
    m_index = -1;
    if ( hasFlagIndex )
    {
      argParser.getFlagArgument( s_flag_short_index, 0, m_index );
    }

    // retrieve the value of the artwork number flag
    m_artNumber = -1;
    if ( hasFlagAn )
    {
      argParser.getFlagArgument( s_flag_short_an, 0, m_artNumber );
    }

    // retrieve the name of the queried node, if any
    m_selection.clear();
    if ( hasFlagNode )
    {
      MSelectionList selection;
      MString        nodeName;
      argParser.getFlagArgument( s_flag_short_node, 0, nodeName );
      if ( nodeName==groundPlane_transform )
      {
        nodeName = MString("|persp");
      }
      status = MGlobal::getSelectionListByName( nodeName, selection );
      for (unsigned int i=0; i<selection.length(); i++)
      {
        if ( status == MS::kSuccess )
        {
          MDagPath dagPath;
          status = selection.getDagPath( i, dagPath );
          if ( status == MS::kSuccess )
          {
            m_selection.push_back( dagPath.fullPathName().asTChar() );
          }
        }
      }
      if ( status != MS::kSuccess || selection.isEmpty() )
      {
        MGlobal::displayError(CommandName+" : problem finding node with name \""+nodeName+"\"");
      }
    }

    // retrieve the name of the group node, if any
    m_groupNode.clear();
    if ( hasFlagGroupNode )
    {
      MSelectionList selection;
      MString        nodeName;
      argParser.getFlagArgument( s_flag_short_groupNode, 0, nodeName );
      if ( nodeName.length() > 0 )
      {
        status = MGlobal::getSelectionListByName( nodeName, selection );
        for (unsigned int i=0; i<selection.length(); i++)
        {
          if ( status == MS::kSuccess )
          {
            MDagPath dagPath;
            status = selection.getDagPath( i, dagPath );
            if ( status == MS::kSuccess )
            {
              m_groupNode.push_back( dagPath.fullPathName().asTChar() );
            }
          }
        }
        if ( status != MS::kSuccess || selection.isEmpty() )
        {
          MGlobal::displayError(CommandName+" : problem finding node with name \""+nodeName+"\"");
        }
      }
    }

    // see what kind of operation this seems to be
    bool isQuery  = argParser.isQuery();

    // set some properties for return info
    m_longNames = hasFlagLong;
    m_verbose = hasFlagVerbose;
    m_allowOldStyle = hasFlagAos;

    // avoid any of the failure conditions
    if ( typesCount > 1 )            // cannot inspect multiple types at once
    {
      status = MS::kFailure;
    }
  }
  return (status==MS::kSuccess);
}

//-----------------------------------------------------------------------------
// ExportInfoCmd::doIt
// execution of the command
//-----------------------------------------------------------------------------
MStatus ExportInfoCmd::doIt( const MArgList & args )
{
  clearResult();

  if ( parseArgs(args) )
  {
    Maya::V_ExportInfo queryResult;
    Maya::InitExportInfo( m_allowOldStyle, m_selection, m_groupNode );
    Maya::GetExportInfo( queryResult, MObject::kNullObj, m_artNumber, m_type, m_index );
    Maya::V_ExportInfo::iterator itr = queryResult.begin();
    Maya::V_ExportInfo::iterator end = queryResult.end();
    for ( ; itr != end; ++itr )
    {
      if ( m_longNames )
      {
        appendToResult( MString( (*itr).m_pathStrFull.c_str() ) );
      }
      else
      {
        appendToResult( MString( (*itr).m_pathStrPartial.c_str() ) );
      }
      if ( m_verbose )
      {
        appendToResult( MString() + (*itr).m_artNumber );
        appendToResult( MString( Maya::ContentTypesString[ (*itr).m_type ] ) );
        appendToResult( MString() + (*itr).m_index );
      }
    }
  }
  else
  {
    MGlobal::displayWarning(CommandName + TXT(": flags are incompatible"));
  }
  return MS::kSuccess;
}

