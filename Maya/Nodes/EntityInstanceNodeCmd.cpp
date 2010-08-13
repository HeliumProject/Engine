#include "Precompile.h"

#include "EntityInstanceNodeCmd.h"
#include "EntityInstanceNode.h"
#include "EntityNode.h"

#include "Maya/NodeTypes.h"

#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>

using namespace Helium;

//-----------------------------------------------------------------------------
// the name of the EntityNodeCmd command
//-----------------------------------------------------------------------------
MString EntityInstanceNodeCmd::CommandName( "entityNode" );

//-----------------------------------------------------------------------------
// arguments to the EntityNodeCmd command
//-----------------------------------------------------------------------------
static const char* ReloadArt = "-r";
static const char* ReloadArtLong = "-reloadArt";

static const char* ReloadAllArtFlag = "-ra";
static const char* ReloadAllArtFlagLong  = "-reloadAllArt";

static const char* UnloadArt = "-u";
static const char* UnloadArtLong = "-unloadArt";

static const char* UnloadAllArtFlag = "-ua";
static const char* UnloadAllArtFlagLong  = "-unloadAllArt";

static const char* SelectEntityAssetFlag = "-se";
static const char* SelectEntityAssetFlagLong = "-selectEntityAsset";

static const char* UnselectFlag = "-us";
static const char* UnselectFlagLong = "-unselect";

static const char* CreateInstanceFlag = "-cr";
static const char* CreateInstanceFlagLong = "-createInstance";

static const char* Flatten = "-fl";
static const char* FlattenLong = "-flatten";


//-----------------------------------------------------------------------------
// EntityInstanceNodeCmd::newSyntax
// definition of the syntax for command
//-----------------------------------------------------------------------------
MSyntax EntityInstanceNodeCmd::newSyntax()
{
  MSyntax syntax;

  syntax.enableEdit();
  syntax.setObjectType(MSyntax::kSelectionList, 0); // will function without error, but do nothing with no selection
  syntax.useSelectionAsDefault( true );

  syntax.addFlag( ReloadArt, ReloadArtLong );
  syntax.addFlag( ReloadAllArtFlag, ReloadAllArtFlagLong );

  syntax.addFlag( UnloadArt, UnloadArtLong );
  syntax.addFlag( UnloadAllArtFlag, UnloadAllArtFlagLong );

  syntax.addFlag( UnselectFlag, UnselectFlagLong );
  syntax.addFlag( SelectEntityAssetFlag, SelectEntityAssetFlagLong );
  syntax.addFlag( Flatten, FlattenLong );

  return syntax;
}

//-----------------------------------------------------------------------------
// EntityInstanceNodeCmd::doIt
// execution of the command
//-----------------------------------------------------------------------------
MStatus EntityInstanceNodeCmd::doIt( const MArgList & args )
{
  MStatus stat;

  // parse the command line arguments using the declared syntax
  MArgDatabase argParser( syntax(), args, &stat );

  if( argParser.isFlagSet( ReloadAllArtFlagLong ) )
  {
    EntityNode::UnloadAllArt();
    EntityNode::LoadAllArt();
    return MS::kSuccess;
  }
  else if( argParser.isFlagSet( UnloadAllArtFlagLong ) )
  {
    EntityNode::UnloadAllArt();
    return MS::kSuccess;
  }
  else if( argParser.isFlagSet( UnselectFlag ) )
  {
    MGlobal::executeCommand( "select -all" );

    MSelectionList list;
    MGlobal::getActiveSelectionList( list );
    EntityNode::UnselectAll( list );
    MGlobal::setActiveSelectionList( list );

    return MS::kSuccess;
  }
  else if( argParser.isFlagSet( CreateInstanceFlag ) )
  {
      HELIUM_BREAK();
#pragma TODO( "Reimplement to use the Vault" )
    //File::FileBrowser browserDlg( NULL, -1, "Create Instance" );
    //browserDlg.AddFilter( FinderSpecs::Asset::ENTITY_DECORATION );
    //browserDlg.SetFilterIndex( FinderSpecs::Asset::ENTITY_DECORATION );

    //if ( browserDlg.ShowModal() == wxID_OK )
    //{
    //  tstring fullPath = browserDlg.GetPath();
    //  if ( FileSystem::Exists( fullPath ) )
    //  {
    //    if ( FileSystem::HasExtension( fullPath, FinderSpecs::Asset::ENTITY_DECORATION.GetDecoration() ) )
    //    {
    //      Asset::EntityPtr instance = new Asset::Entity( fullPath );
    //      std::pair< EntityNode*, EntityInstanceNode* >result = EntityNode::CreateInstance( instance );
    //      MFnDependencyNode nodeFn( result.second->thisMObject() );
    //    }
    //  }
    //}

    return MS::kSuccess;
  }
  else if( argParser.isFlagSet( FlattenLong ) )
  {
    EntityNode::FlattenInstances();    
    return MS::kSuccess;
  }

  //
  // the following flags need an EntityNode object to proceed
  //

  MSelectionList selection;
  argParser.getObjects( selection );

  MObject selectedNode;
  selection.getDependNode( 0, selectedNode );

  MFnDependencyNode nodeFn;
  nodeFn.setObject( selectedNode );

  EntityNode* classTransform = NULL;
  if( nodeFn.typeId() == EntityInstanceNode::s_TypeID )
  {
    EntityInstanceNode* node = static_cast< EntityInstanceNode* >( nodeFn.userNode( &stat ) );
    if( !node )
    {
      return MS::kFailure;
    }

    classTransform = &EntityNode::Get( node->GetBackingEntity()->GetEntity()->GetPath() );
    if( *classTransform == EntityNode::Null )
    {
      return MS::kFailure;
    }
  }
  else if ( nodeFn.typeId() == EntityNode::s_TypeID )
  {
    classTransform = static_cast< EntityNode* >( nodeFn.userNode( &stat ) );
    if( !classTransform )
    {
      return MS::kFailure;
    }
  }

  if (argParser.isFlagSet( ReloadArtLong ) )
  {
    classTransform->LoadArt();
  }
  else if( argParser.isFlagSet( SelectEntityAssetFlagLong ) )
  {
    MFnDagNode nodeFn( classTransform->thisMObject() );
    MGlobal::executeCommand( "select -r \"" + nodeFn.fullPathName() + "\"" );
  }

  return MS::kSuccess;
}

