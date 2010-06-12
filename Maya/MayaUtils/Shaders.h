#pragma once

#include "API.h"
#include <maya/MGlobal.h>
#include <maya/MPlugArray.h>
#include <maya/MDagPath.h>

namespace Maya
{
  ////////////////////////////////////////////////////////////////////////////////
  //
  // getShadingEngineShaderId
  //
  ///   \param   shadingEngine  a shading engine node with an insomniac styled surface shader
  ///   \return                 "globalShaderId" attribute value from the surface shader
  //
  ////////////////////////////////////////////////////////////////////////////////
  MAYAUTILS_API int getShadingEngineShaderId( const MObject & shadingEngine );

  ////////////////////////////////////////////////////////////////////////////////
  //
  // appendShaderNames
  //
  ///   \param  shader_id     a shader number (insomniac globalShaderId attribute)
  ///   \param  result        append the names of all matching surface shaders
  ////////////////////////////////////////////////////////////////////////////////
  MAYAUTILS_API void appendShaderNames( int shader_id, MStringArray & result );

  ////////////////////////////////////////////////////////////////////////////////
  //
  // getSelectedShaderIds
  //
  ///   \param  selection   a maya selection list to be scanned for globalShaderId on attached surface shaders
  ///   \param  shaderIds   appends all the globalShaderId values found in the selection list
  //
  ////////////////////////////////////////////////////////////////////////////////
  MAYAUTILS_API MStatus getSelectedShaderIds( const MSelectionList &selection, MIntArray & shaderIds );

  ////////////////////////////////////////////////////////////////////////////////
  //
  // getSelectedShaderEngines
  //
  ///   \param  selection		    a maya selection list to be scanned for attached surface shaders
  ///   \param  shaderEngines   appends all the shading engines found (no duplicates)
  //
  ////////////////////////////////////////////////////////////////////////////////
  MAYAUTILS_API MStatus getSelectedShaderEngines( const MSelectionList &selection, MObjectArray & shaderEngines );

  ////////////////////////////////////////////////////////////////////////////////
  //
  // nonReferencedShadingEngine
  //
  ///   \param  shadingEngine  given a shading engine, which optionally has an insomniac surface shader
  ///   \return                the same shading engine, or a copy that is not a reference and has the same globalShaderId
  //
  ////////////////////////////////////////////////////////////////////////////////
  MAYAUTILS_API MObject nonReferencedShadingEngine( MObject shadingEngine );

  ////////////////////////////////////////////////////////////////////////////////
  //
  // uniqueShadingEngine
  //
  ///   \param  source         a shading engine to make unique (deletes if the shader is now no longer connected to anything)
  ///   \param  makeUniqueFor  object to make the shader unique for - must be a dagpath so we can ensure it has a definite path
  ///   \return                a unique copy of the shader
  //
  ////////////////////////////////////////////////////////////////////////////////
  MAYAUTILS_API MObject makeUniqueShadingEngine( MObject &source , const MDagPath &makeUniqueFor );

  ////////////////////////////////////////////////////////////////////////////////
  //
  // nonReferencedShadingEngine
  //
  ///   \param  shadingEngine  given a shading engine, which optionally has an insomniac surface shader
  ///   \return                the same shading engine, or a copy that is not a reference and has the same globalShaderId
  //
  ////////////////////////////////////////////////////////////////////////////////
  MAYAUTILS_API MObject nonReferencedShadingEngine( MObject shadingEngine );

  ////////////////////////////////////////////////////////////////////////////////
  //
  // sortAndRemoveDuplicates
  //
  ///   \param  array   an int array to be storted in ascending order without duplicates
  //
  ////////////////////////////////////////////////////////////////////////////////
  MAYAUTILS_API void sortAndRemoveDuplicates( MIntArray & array );
}
