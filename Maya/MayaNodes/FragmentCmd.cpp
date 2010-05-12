#include "stdafx.h"

#include "FragmentCmd.h"
#include "ExportNode.h"

#include "RCS/rcs.h"
#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ContentSpecs.h"

#include "File/Manager.h"
#include "FileSystem/FileSystem.h"

#include "Asset/Exceptions.h"
#include "Asset/AssetClass.h"
#include "Attribute/AttributeHandle.h"
#include "Asset/ArtFileAttribute.h"

#include "boost/algorithm/string.hpp"
#include "Common/String/Tokenize.h"
#include "Common/Boost/Regex.h" 

#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>

using namespace Asset;
using namespace File;
using namespace Reflect;
using namespace Attribute;

//-----------------------------------------------------------------------------
// the name of the FragmentCmd command
//-----------------------------------------------------------------------------
MString FragmentCmd::CommandName( "fragment" );

//-----------------------------------------------------------------------------
// arguments to the FragmentCmd command
//-----------------------------------------------------------------------------

static const char* CreateFlagShort = "-c";
static const char* CreateFlagLong  = "-create";
static const char* FragmentFlagShort = "-f";
static const char* FragmentFlagLong = "-fragment";

FragmentCmd::FragmentCmd()
{
  m_CreateFragment = false;
  m_SendFragments = false;
}

FragmentCmd::~FragmentCmd()
{

}

//-----------------------------------------------------------------------------
// FragmentCmd::newSyntax
// definition of the syntax for command
//-----------------------------------------------------------------------------
MSyntax FragmentCmd::newSyntax()
{
  MSyntax syntax;

  syntax.enableEdit();
  syntax.setObjectType(MSyntax::kSelectionList, 0); // will function without error, but do nothing with no selection
  syntax.useSelectionAsDefault( true );

  syntax.addFlag( CreateFlagShort, CreateFlagLong );
  syntax.addFlag( FragmentFlagShort, FragmentFlagLong );

  return syntax;
}

//-----------------------------------------------------------------------------
// FragmentCmd::doIt
// execution of the command
//-----------------------------------------------------------------------------
MStatus FragmentCmd::doIt( const MArgList & args )
{
  MStatus stat;

  // parse the command line arguments using the declared syntax
  MArgDatabase argParser( syntax(), args, &stat );

  argParser.getObjects( m_Selection );

  if( argParser.isFlagSet( CreateFlagLong ) )
  {
    m_CreateFragment = true;
  }
  else if( argParser.isFlagSet( FragmentFlagLong ) )
  {
    m_SendFragments = true;
  }

  return redoIt();
}

MStatus FragmentCmd::redoIt()
{
  if( m_CreateFragment )
  {
    return CreateNewFragment();
  }
  else if( m_SendFragments )
  {
    return Fragment();
  }
  return MStatus::kFailure;
}

MStatus FragmentCmd::undoIt()
{
  MStatus status = m_DagMod.undoIt();
  return status;
}

MStatus FragmentCmd::CreateNewFragment()
{
  MObject fragment = ExportNode::CreateExportNode( Content::ContentTypes::FragmentGroup );
  MObject geometry = ExportNode::CreateExportNode( Content::ContentTypes::Geometry);
  MObject map      = ExportNode::CreateExportNode( Content::ContentTypes::LightMapped );
  MObject overlay  = ExportNode::CreateExportNode( Content::ContentTypes::Overlay );

  MFnDagNode nodeFn( fragment );
  nodeFn.setName( "NewFragment" );
  nodeFn.addChild( geometry );

  nodeFn.setObject( geometry );
  nodeFn.setName( "Geometry" );
  nodeFn.addChild( map );

  nodeFn.setObject( overlay );
  nodeFn.setName( "Overlays" );

  nodeFn.setObject( map );
  nodeFn.setName( "LightMappedMeshes" );

  nodeFn.addChild( overlay );

  u32 len = m_Selection.length();
  for( u32 i = 0; i < len; ++i )
  {
    MObject object;
    m_Selection.getDependNode( i, object );

    if( object.hasFn( MFn::kTransform ) || object.hasFn( MFn::kMesh ) )
    {
      m_DagMod.reparentNode( object, nodeFn.object() );
    }
  }
  return m_DagMod.doIt();
}

MStatus FragmentCmd::Fragment()
{
  if( 1 )
  {
    return MS::kFailure;
  }

  MGlobal::displayInfo( "Beginning Fragmentation..." );

  MObjectArray fragmentNodes;
  ExportNode::FindExportNodes( fragmentNodes, Content::ContentTypes::FragmentGroup );

  Asset::AssetClassPtr assetClass = Asset::AssetClass::FindAssetClass( TUID::Null ); //Construction::g_ClassID );  
  std::string assetClassPath = assetClass->GetFilePath();
  FileSystem::StripLeaf( assetClassPath );

  const boost::regex fragmentsPattern( "/fragments/.*" );

  assetClassPath = boost::regex_replace( assetClassPath, fragmentsPattern, "/" );

  MFnDagNode nodeFn;

  std::string newAssetClassPath;
  Asset::AssetClass* newAssetClass = NULL;

  //save the old selection list
  MSelectionList oldSelection;
  MGlobal::getActiveSelectionList( oldSelection );

  MString artFilePath = MFileIO::currentFile();

  u32 numFragments = fragmentNodes.length();

  for( u32 i = 0; i < numFragments; ++i )
  {
    nodeFn.setObject( fragmentNodes[i] );

    newAssetClassPath = assetClassPath + "/fragments/";
    newAssetClassPath += nodeFn.name().asChar();
    newAssetClassPath += "/";
    newAssetClassPath += nodeFn.name().asChar();
    newAssetClassPath += FinderSpecs::Asset::ENTITY_DECORATION.GetDecoration();

    FileSystem::CleanName( newAssetClassPath );

    RCS::File rcsFile( newAssetClassPath );

    tuid fragmentID = GlobalManager().GetID( newAssetClassPath );
    if( fragmentID != TUID::Null )
    {
      rcsFile.GetInfo();
      if( rcsFile.ExistsInDepot() )
      {
        // hax because existsindepot can return true when you have a file open for add
        try
        {
          rcsFile.Sync();
        }
        catch( RCS::Exception& )
        {
        }
      }

      try
      {
        newAssetClass = Asset::AssetClass::FindAssetClass( fragmentID );
      }
      catch( Asset::UnableToLoadAssetClassException&  )
      {
        newAssetClass = NULL;
      }
    }
    else
    {
      fragmentID = GlobalManager().Open( newAssetClassPath );
      newAssetClass = NULL;
    }


    if( !newAssetClass )
    {
      try
      {
        MString message( "\to Creating new fragment " );

        // assetClass sure as hell better be of type AssetClass...so the dangerous_cast is ok
        ElementPtr newElement = assetClass->Clone();
        newAssetClass = DangerousCast< AssetClass >( newElement );
        newAssetClass->m_AssetClassID = fragmentID;

        rcsFile.Open();

        message += newAssetClassPath.c_str();
        MGlobal::displayInfo( message + "..." );

        AttributeEditor< ArtFileAttribute > artFile( newAssetClass );
        artFile->m_FragmentNode = nodeFn.name().asChar();
        artFile.Commit();
        newAssetClass->Serialize();

        MGlobal::displayInfo( "\t\tdone!" );
      }
      catch( Nocturnal::Exception& e )
      {
        MString error( "\t\tUnable to create new fragment: ");
        error += e.what();
        MGlobal::displayError( error );
      }
    }
  }

  return MS::kSuccess;
}
