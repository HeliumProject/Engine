#pragma once

#include "API.h"

#include "Foundation/Memory/SmartPtr.h"

#include "Math/Color3.h"


namespace Maya
{
  ///////////////////////////////////////////////////////////////////////////////
  // ProcessObjectFunctor class used by ProcessSelectedObjects
  //
  class ProcessObjectFunctor
  {
  public:
    ProcessObjectFunctor() {}
    virtual ~ProcessObjectFunctor() {}

    virtual MStatus ProcessLocatorNode( const MDagPath& dagPath )
    {
      return MStatus::kSuccess;
    }

    virtual MStatus ProcessMesh( const MDagPath& dagPath, MObject& components )
    {
      return MStatus::kSuccess;
    }
  };


  ///////////////////////////////////////////////////////////////////////////////
  // Static API
  //

  // Processes all of the selected objects in the scene
  MStatus MAYAMESHSETUP_API ProcessSelectedObjects( ProcessObjectFunctor& processObjectFunctor );

  // Processes all of the objects in the scene
  MStatus MAYAMESHSETUP_API ProcessAllSceneObjects( ProcessObjectFunctor& processObjectFunctor );

  bool MAYAMESHSETUP_API IsNodeVisible( MFnDagNode currentNode );

  // Generates a Math::Color3 based on the hash of the given string
  Math::Color3 MAYAMESHSETUP_API GetColorFromStringHash( const std::string& str );


  //static const unsigned char s_StippleMask[16][128];

} // namespace Maya