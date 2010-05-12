#include "stdafx.h"

#include "ZoneCmd.h"
#include "EntityNode.h"
#include "EntityAssetNode.h"
#include "ConstructionTool.h"

#include "Content/Volume.h"
#include "GameplayCuboid.h"
#include "GameplaySphere.h"
#include "GameplayCylinder.h"
#include "GameplayCapsule.h"

#include "File/Manager.h"
#include "FileSystem/FileSystem.h"
#include "FileBrowser/FileBrowser.h"
#include "Finder/Finder.h"
#include "Finder/AssetSpecs.h"
#include "Finder/ContentSpecs.h"
#include "Finder/ExtensionSpecs.h"

#include "RCS/rcs.h"
#include "UIToolkit/FileDialog.h"
#include "Windows/Clipboard.h"

#include "MayaUtils/UniqueID.h"
#include "MayaUtils/NodeTypes.h"
#include "Math/EulerAngles.h"

#include <maya/MSyntax.h>
#include <maya/MArgDatabase.h>


//-----------------------------------------------------------------------------
// the name of the ZoneCmd command
//-----------------------------------------------------------------------------
MString ZoneCmd::CommandName( "zone" );

//-----------------------------------------------------------------------------
// arguments to the ZoneCmd command
//-----------------------------------------------------------------------------
static const char* ClipboardFlag = "-c";
static const char* ClipboardFlagLong = "-clipboard";

static const char* FileFlag = "-f";
static const char* FileFlagLong = "-file";

static const char* LoadFlag = "-l";
static const char* LoadFlagLong  = "-load";

static const char* SaveFlag = "-s";
static const char* SaveFlagLong  = "-save";

static void ConvertMatrix(const MMatrix& matrix, Math::Matrix4& outMatrix)
{
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
      outMatrix[i][j] = (float)matrix[i][j];

  outMatrix.t.x *= Math::CentimetersToMeters;
  outMatrix.t.y *= Math::CentimetersToMeters;
  outMatrix.t.z *= Math::CentimetersToMeters;
}

static void ConvertMatrix(const Math::Matrix4& matrix, MMatrix& outMatrix)
{
  for (int i=0; i<4; i++)
    for (int j=0; j<4; j++)
      outMatrix[i][j] = matrix[i][j];

  outMatrix[3][0] *= Math::MetersToCentimeters;
  outMatrix[3][1] *= Math::MetersToCentimeters;
  outMatrix[3][2] *= Math::MetersToCentimeters;
}


//-----------------------------------------------------------------------------
// ZoneCmd::newSyntax
// definition of the syntax for command
//-----------------------------------------------------------------------------
MSyntax ZoneCmd::newSyntax()
{
  MSyntax syntax;

  syntax.enableEdit();
  syntax.setObjectType(MSyntax::kSelectionList, 0); // will function without error, but do nothing with no selection
  syntax.useSelectionAsDefault( true );

  syntax.addFlag( ClipboardFlag, ClipboardFlagLong );
  syntax.addFlag( FileFlag, FileFlagLong, MSyntax::kString );
  syntax.addFlag( LoadFlag, LoadFlagLong );
  syntax.addFlag( SaveFlag, SaveFlagLong );

  return syntax;
}

//-----------------------------------------------------------------------------
// ZoneCmd::doIt
// execution of the command
//-----------------------------------------------------------------------------
MStatus ZoneCmd::doIt( const MArgList & args )
{
  MStatus stat;

  // parse the command line arguments using the declared syntax
  MArgDatabase argParser( syntax(), args, &stat );
  
  // get the file path if it was passed via flag
  MString filePath;
  if ( argParser.isFlagSet( FileFlagLong ) )
  {
    if ( argParser.getFlagArgument( FileFlagLong, 0, filePath ) != MS::kSuccess )
    {
      MGlobal::displayError( MString ("Unable to parse argument for flag ") + FileFlagLong );
      return MS::kFailure;
    }
  }

  if( argParser.isFlagSet( LoadFlagLong ) )
  {
    if ( !argParser.isFlagSet( ClipboardFlagLong ) && filePath.length() == 0 )
    {
      UIToolKit::FileDialog dialog ( NULL, wxFileSelectorPromptStr, wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, UIToolKit::FileDialogStyles::DefaultOpen );

      dialog.SetFilter( FinderSpecs::Extension::REFLECT_BINARY.GetFilter() );

      if ( dialog.ShowModal() == wxID_OK )
      {
        filePath = dialog.GetPath();
      }
      else
      {
        return MS::kSuccess;
      }
    }

    return Load( filePath.asChar() );
  }
  else if( argParser.isFlagSet( SaveFlagLong ) )
  {
    if ( !argParser.isFlagSet( ClipboardFlagLong ) && filePath.length() == 0 )
    {
      UIToolKit::FileDialog dialog ( NULL, wxFileSelectorPromptStr, wxEmptyString, wxEmptyString, wxFileSelectorDefaultWildcardStr, UIToolKit::FileDialogStyles::DefaultSave );

      dialog.SetFilter( FinderSpecs::Extension::REFLECT_BINARY.GetFilter() );

      if ( dialog.ShowModal() == wxID_OK )
      {
        filePath = dialog.GetPath();
      }
      else
      {
        return MS::kSuccess;
      }
    }

    return Save( filePath.asChar() );
  }

  return MS::kSuccess;
}

MStatus ZoneCmd::Load( const std::string& filePath, bool importArt )
{
  Reflect::V_Element elements;

  if ( filePath.empty() )
  {
    std::string xml, error;
    if ( !Windows::RetrieveFromClipboard( M3dView::applicationShell(), xml, error ) )
    {
      MGlobal::displayError( MString( "Unable to get data from clipboard: " ) + error.c_str() );
      return MS::kFailure;
    }

    try
    {
      Reflect::Archive::FromXML( xml, elements );
    }
    catch ( Nocturnal::Exception& ex )
    {
      MGlobal::displayError( MString( "Unable to load xml: " ) + ex.what() );
      return MS::kFailure;
    }
  }
  else
  {
    try
    {
      Reflect::Archive::FromFile( filePath, elements );
    }
    catch ( Nocturnal::Exception& ex )
    {
      MGlobal::displayError( MString( "Unable to load file '" ) + filePath.c_str() + "': " + ex.what() );
      return MS::kFailure;
    }
  }

  Reflect::V_Element::iterator itor = elements.begin();
  Reflect::V_Element::iterator end  = elements.end();
  for( ; itor != end; ++itor )
  {
    Asset::EntityPtr entity = Reflect::ObjectCast< Asset::Entity >( *itor );
    if( entity )
    {
      EntityAssetNode::CreateInstance( entity );
      continue;
    }

    Content::VolumePtr volume = Reflect::ObjectCast< Content::Volume >( *itor );
    if ( volume )
    {
      MTypeId type;

      switch ( volume->m_Shape )
      {
      case Content::VolumeShapes::Cube:
        {
          type = GameplayCuboid::s_TypeID;
          break;
        }

      case Content::VolumeShapes::Sphere:
        {
          type = GameplaySphere::s_TypeID;
          break;
        }

      case Content::VolumeShapes::Cylinder:
        {
          type = GameplayCylinder::s_TypeID;
          break;
        }

      case Content::VolumeShapes::Capsule:
        {
          type = GameplayCapsule::s_TypeID;
          break;
        }
      }

      if ( type != MFn::kInvalid )
      {
        MFnDagNode nodeFn;
        MObject obj = nodeFn.create( type, volume->GetName().c_str() );

        MMatrix matrix;
        ConvertMatrix( volume->m_GlobalTransform, matrix );

        MFnTransform transFn ( nodeFn.object() );
        MCheckErr( transFn.set( matrix ), "Unable to do: MFnTransform::set()" );

        Maya::SetNodeID( obj, volume->m_ID );
      }

      continue;
    }
  }

  return MS::kSuccess;
}

MStatus ZoneCmd::Save( const std::string& filePath )
{
  Reflect::V_Element elements;

  M_TuidClassTransform::iterator classItor = EntityAssetNode::s_ClassTransformsMap.begin();
  M_TuidClassTransform::iterator classEnd  = EntityAssetNode::s_ClassTransformsMap.end();
  for( ; classItor != classEnd; ++classItor )
  {
    M_EntityNode::const_iterator itor = classItor->second->GetInstances().begin();
    M_EntityNode::const_iterator end = classItor->second->GetInstances().end();
    for( ; itor != end; ++itor )
    {
      itor->second->UpdateBackingEntity();
      elements.push_back( itor->second->GetBackingEntity() );
    }
  }

  MSelectionList volumes;
  for ( MItDag itr; !itr.isDone(); itr.next() )
  {
    MFn::Type objType = itr.item().apiType();

    Content::VolumePtr volume;

    switch ( objType )
    {
    case MFn::kPluginLocatorNode:
      {
        MFnDagNode nodeFn ( itr.item() );

        switch( nodeFn.typeId().id() )
        {
        case IGL_GP_CUBOID:
        case IGL_GP_SPHERE:
        case IGL_GP_CYLINDER:
        case IGL_GP_CAPSULE:
          volume = new Content::Volume ( Maya::GetNodeID( itr.item(), true ) );
          break;
        }

        if ( volume )
        {
          switch( nodeFn.typeId().id() )
          {
          case IGL_GP_CUBOID:
            volume->m_Shape = Content::VolumeShapes::Cube;
            break;

          case IGL_GP_SPHERE:
            volume->m_Shape = Content::VolumeShapes::Sphere;
            break;

          case IGL_GP_CYLINDER:
            volume->m_Shape = Content::VolumeShapes::Cylinder;
            break;

          case IGL_GP_CAPSULE:
            volume->m_Shape = Content::VolumeShapes::Capsule;
            break;
          }

          MDagPath path = MDagPath::getAPathTo( nodeFn.parent( 0 ) );
          ConvertMatrix( path.inclusiveMatrix(), volume->m_GlobalTransform );
          volume->m_ObjectTransform = volume->m_GlobalTransform;

          Math::Vector3 scale;
          Math::Matrix3 rotate;
          Math::Vector3 translate;
          volume->m_ObjectTransform.Decompose( (Math::Scale&)scale, rotate, translate );
          volume->m_Scale = scale;
          volume->m_Rotate = Math::EulerAngles ( rotate ).angles;
          volume->m_Translate = translate;

          elements.push_back( volume );
        }
      }
    }
  }  

  if ( filePath.empty() )
  {
    std::string xml, error;

    try
    {
      Reflect::Archive::ToXML( elements, xml );
    }
    catch ( Nocturnal::Exception& ex )
    {
      MGlobal::displayError( MString( "Unable to load xml: " ) + ex.what() );
      return MS::kFailure;
    }

    if ( !Windows::CopyToClipboard( M3dView::applicationShell(), xml, error ) )
    {
      MGlobal::displayError( MString( "Unable to put data into clipboard: " ) + error.c_str() );
      return MS::kFailure;
    }
  }
  else
  {
    try
    {
      Reflect::Archive::ToFile( elements, filePath );
    }
    catch ( Nocturnal::Exception& ex )
    {
      MGlobal::displayError( MString( "Unable to save file '" ) + filePath.c_str() + ": " + ex.what() );
      return MS::kFailure;
    }
  }

  return MS::kSuccess;
}

