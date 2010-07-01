#include "Precompile.h"
#include "Shaders.h"
#include "Duplicate.h"

#include <maya/MDagPath.h>
#include <maya/MFnDagNode.h>
#include <maya/MItDag.h>
#include <maya/MItDependencyNodes.h>
#include <maya/MItSelectionList.h>
#include <maya/MObjectArray.h>
#include <maya/MPlug.h>
#include <maya/MSelectionList.h>
#include <maya/MFnSet.h>
#include <map>
#include <string>

#include "ErrorHelpers.h"

namespace Maya
{
  typedef std::map<tstring, MObject> MObjectMap;

  ////////////////////////////////////////////////////////////////////////////////
  //
  // getShadingEngineShaderId
  //
  ////////////////////////////////////////////////////////////////////////////////
  int getShadingEngineShaderId( const MObject & shadingEngine )
  {
    MPlugArray surfaceShaderArray;
    MStatus    stat      = MS::kSuccess;
    int        shader_id = -1;
    if (shadingEngine!=MObject::kNullObj && shadingEngine.apiType()==MFn::kShadingEngine)
    {
      // setup a dependency node Fn to look at the shading engine node
      MFnDependencyNode seNodeFn( shadingEngine, &stat );
      if (stat==MS::kSuccess)
      {
        // get the plug for the surface shader
        MPlug surfaceShaderPlug = seNodeFn.findPlug( "ss", true, &stat );
        if (stat==MS::kSuccess)
        {
          // follow the surface shader plug to its source
          if ( surfaceShaderPlug.connectedTo( surfaceShaderArray, true, false, &stat ) )
          {
            // there should only be one source
            if (stat==MS::kSuccess && surfaceShaderArray.length()==1)
            {
              // the surface shader is a dependency node
              MObject surfaceShader = surfaceShaderArray[0].node();
              if (surfaceShader!=MObject::kNullObj)
              {
                MFnDependencyNode ssNodeFn( surfaceShader, &stat );
                if (stat==MS::kSuccess)
                {
                  // look for the insomniac globalShaderId attribute
                  MPlug shaderIdPlug = ssNodeFn.findPlug( "globalShaderId", false, &stat );
                  if (stat==MS::kSuccess)
                  {
                    // load the shader id
                    stat = shaderIdPlug.getValue( shader_id );
                  }
                }
              }
            }
          }
        }
      }
    }
    // just to be sure that errors don't return shader ids
    if (stat!=MS::kSuccess)
    {
      shader_id = -1;
    }
    return shader_id;
  }


  ////////////////////////////////////////////////////////////////////////////////
  //
  // appendShaderPlugs
  //
  ////////////////////////////////////////////////////////////////////////////////
  static void appendShaderPlugs( const MDagPath & dagPath, MPlugArray & shadingGroupSourcePlugs )
  {
    MStatus    stat;
    MFnDagNode dagNodeFn( dagPath );
    MPlug      instObjGroupsArrayPlug;
    MPlug      instObjGroupsPlug;
    MPlug      objectGroupArrayPlug;
    MPlug      objectGroupPlug;
    MIntArray  instObjGroupsIndices;
    MIntArray  objectGroupIndices;
    instObjGroupsArrayPlug = dagNodeFn.findPlug(MString("instObjGroups"), true, &stat);
    if ( stat==MS::kSuccess )
    {
      // examine each element of instObjGroups
      int iCount = instObjGroupsArrayPlug.getExistingArrayAttributeIndices( instObjGroupsIndices );
      for (int i=0; i<iCount; i++)
      {
        instObjGroupsPlug = instObjGroupsArrayPlug[ instObjGroupsIndices[i] ];
        // examine each child (should only be one, "object group")
        int children = instObjGroupsPlug.numChildren();
        for (int j=0; j<children; j++)
        {
          objectGroupArrayPlug = instObjGroupsPlug.child( j );
          // examine each connection (to a shader group)
          int kCount = objectGroupArrayPlug.getExistingArrayAttributeIndices( objectGroupIndices );
          if (kCount==0)
          {
            // this mesh has only one shader, so note its source plug
            shadingGroupSourcePlugs.append( instObjGroupsPlug );
          }
          else
          {
            // examine each connection (to a shader group) attached to this mesh
            for (int k=0; k<kCount; k++)
            {
              objectGroupPlug = objectGroupArrayPlug.elementByLogicalIndex( objectGroupIndices[k], &stat );
              if ( stat==MS::kSuccess )
              {
                // we found shading group source plug, add it to the result
                shadingGroupSourcePlugs.append( objectGroupPlug );
              }
            }
          }
        }
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////////////
  //
  // getShaderPlugShaderId
  //
  ////////////////////////////////////////////////////////////////////////////////
  static int getShaderPlugShaderId( const MPlug & objectGroupPlug )
  {
    MStatus    stat;
    int        shader_id = -1;
    MPlugArray shadingGroupPlugArray;
    MObject    shadingGroup;
    if (objectGroupPlug.connectedTo(shadingGroupPlugArray, false, true, &stat) && stat==MS::kSuccess)
    {
      if (shadingGroupPlugArray.length()==1)  // can only be one destination
      {
        shadingGroup = shadingGroupPlugArray[0].node();
        shader_id = getShadingEngineShaderId( shadingGroup );
      }
    }
    return shader_id;
  }

  ////////////////////////////////////////////////////////////////////////////////
  //
  // getShaderPlugShaderEngine
  //
  ////////////////////////////////////////////////////////////////////////////////
  static MObject getShaderPlugShaderEngine( const MPlug & objectGroupPlug )
  {
    MStatus    stat;
    int        shader_id = -1;
    MPlugArray shadingGroupPlugArray;
    MObject    shadingGroup(MObject::kNullObj);
    if (objectGroupPlug.connectedTo(shadingGroupPlugArray, false, true, &stat) && stat==MS::kSuccess)
    {
      if (shadingGroupPlugArray.length()==1)  // can only be one destination
      {
        shadingGroup = shadingGroupPlugArray[0].node();
      }
    }
    return shadingGroup;
  }
  ////////////////////////////////////////////////////////////////////////////////
  //
  // getDagPathShaderIds
  //
  ////////////////////////////////////////////////////////////////////////////////
  static void getDagPathShaderIds( const MDagPath & dagPathRoot, MIntArray & shaderIds )
  {
    MPlugArray shadingGroupSourcePlugs;
    MStatus    stat;
    // iterate through the dag path and append shader plugs for the meshes found
    MItDag dagIterator( MItDag::kDepthFirst, MFn::kMesh, &stat );
    if (stat==MS::kSuccess) 
    {
      stat = dagIterator.reset( dagPathRoot, MItDag::kDepthFirst, MFn::kMesh );
      if (stat==MS::kSuccess)
      {
        for ( ; !dagIterator.isDone(); dagIterator.next() ) {
          MDagPath dagPath;
          stat = dagIterator.getPath( dagPath );
          if ( stat==MS::kSuccess ) {
            appendShaderPlugs( dagPath, shadingGroupSourcePlugs );
          }
        }
        // take a look at each plug that (potentially) leads to a shading group
        int shadingGroupCount = shadingGroupSourcePlugs.length();
        for ( int i=0; i<shadingGroupCount; i++ )
        {
          // see if this plug leads to an insomniac identified shader
          int shader_id = getShaderPlugShaderId( shadingGroupSourcePlugs[i] );
          if ( shader_id != -1 )
          {
            // an insomniac shader_id was found, so remember it
            shaderIds.append( shader_id );
          }
        }
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////////////
  //
  // getDagPathShaderEngines
  //
  ////////////////////////////////////////////////////////////////////////////////
  static void getDagPathShaderEngines( const MDagPath & dagPathRoot, MObjectMap & shaderEngines )
  {
    MPlugArray shadingGroupSourcePlugs;
    MStatus    stat;
    // iterate through the dag path and append shader plugs for the meshes found
    MItDag dagIterator( MItDag::kDepthFirst, MFn::kMesh, &stat );
    if (stat==MS::kSuccess) 
    {
      stat = dagIterator.reset( dagPathRoot, MItDag::kDepthFirst, MFn::kMesh );
      if (stat==MS::kSuccess)
      {
        for ( ; !dagIterator.isDone(); dagIterator.next() ) {
          MDagPath dagPath;
          stat = dagIterator.getPath( dagPath );
          if ( stat==MS::kSuccess ) {
            appendShaderPlugs( dagPath, shadingGroupSourcePlugs );
          }
        }
        // take a look at each plug that (potentially) leads to a shading group
        int shadingGroupCount = shadingGroupSourcePlugs.length();
        MFnDependencyNode nodeFn;
        for ( int i = 0; i < shadingGroupCount; ++i )
        {
          // see if this plug leads to an insomniac identified shader
          MObject shader = getShaderPlugShaderEngine( shadingGroupSourcePlugs[i] );
          if ( shader != MObject::kNullObj )
          {
            nodeFn.setObject(shader);
            shaderEngines[nodeFn.name().asTChar()] = shader;
          }
        }
      }
    }
  }

  static void getDagPathShaderEngines( const MDagPath & dagPathRoot, MObjectArray & shaderEngines )
  {
    MObjectMap shaderEngineMap;
    getDagPathShaderEngines( dagPathRoot, shaderEngineMap );

    MObjectMap::iterator itor;
    for( itor = shaderEngineMap.begin(); itor != shaderEngineMap.end(); ++itor )
    {
      shaderEngines.append(itor->second);
    }
  }

  ////////////////////////////////////////////////////////////////////////////////
  //
  // getDagPathComponentShaderIds
  //
  ////////////////////////////////////////////////////////////////////////////////
  static void getDagPathComponentShaderIds( const MDagPath & dagPath, const MObject & component, MIntArray & shaderIds )
  {
    MSelectionList componentList;
    MObjectArray   setArray;
    componentList.add( dagPath, component );
    MGlobal::getAssociatedSets( componentList, setArray );
    if (setArray.length()==0) {
      // no face assignments are present on this mesh, so use alternative method
      getDagPathShaderIds( dagPath, shaderIds );
    } else {
      // shading groups (sets) were found that contain the specified faces
      int setCount = setArray.length();
      for (int i=0; i<setCount; i++) 
      {
        MObject shadingGroup = setArray[i];
        int shader_id = getShadingEngineShaderId( shadingGroup );
        if ( shader_id != -1 )
        {
          // an insomniac shader_id was found, so remember it
          shaderIds.append( shader_id );
        }
      }
    }
  }

  ////////////////////////////////////////////////////////////////////////////////
  //
  // getDagPathComponentShaderEngines
  //
  ////////////////////////////////////////////////////////////////////////////////
  static void getDagPathComponentShaderEngines( const MDagPath & dagPath, const MObject & component, MObjectMap & shaderEngines )
  {
    MSelectionList componentList;
    MObjectArray   setArray;
    componentList.add( dagPath, component );
    MGlobal::getAssociatedSets( componentList, setArray );
    if (setArray.length()==0) 
    {
      // no face assignments are present on this mesh, so use alternative method
      getDagPathShaderEngines( dagPath, shaderEngines );
    } 
    else 
    {
      // shading groups (sets) were found that contain the specified faces
      int setCount = setArray.length();
      MFnDependencyNode nodeFn;
      for( int i = 0; i < setCount; ++i ) 
      {
        nodeFn.setObject( setArray[i] );
        shaderEngines[nodeFn.name().asTChar()] = nodeFn.object();
      }
    }
  }

  static void getDagPathComponentShaderEngines( const MDagPath & dagPath, const MObject & component, MObjectArray & shaderEngines )
  {
    MObjectMap shaderEngineMap;
    getDagPathComponentShaderEngines( dagPath, component, shaderEngineMap );

    MObjectMap::iterator itor;
    for( itor = shaderEngineMap.begin(); itor != shaderEngineMap.end(); ++itor )
    {
      shaderEngines.append(itor->second);
    }
  }

  ////////////////////////////////////////////////////////////////////////////////
  //
  // getShaderObjects
  //
  ////////////////////////////////////////////////////////////////////////////////
  static void getShaderObjects( int shader_id, MObjectArray & result )
  {
    MStatus             stat;
    MFnDependencyNode   surfaceFn;
    int                 surface_shader_id;
    result.clear();
    MItDependencyNodes  iter( MFn::kDependencyNode, &stat ); // shaders derrive from kDependencyNode
    if (stat==MS::kSuccess)
    {
      for ( ; !iter.isDone(); iter.next() )
      {
        MObject surfaceObject = iter.item(&stat);
        if (stat==MS::kSuccess)
        {
          // see if this surfaceObject is indeed a maya shader
          MFn::Type surfaceType = surfaceObject.apiType();
          if (surfaceType==MFn::kLambert       ||
            surfaceType==MFn::kBlinn         ||
            surfaceType==MFn::kPhong         ||
            surfaceType==MFn::kPhongExplorer ||
            surfaceType==MFn::kLayeredShader ||
            surfaceType==MFn::kReflect       )
          {
            stat = surfaceFn.setObject( surfaceObject );
            if (stat==MS::kSuccess)
            {
              // look for the insomniac globalShaderId attribute
              MPlug shaderIdPlug = surfaceFn.findPlug( "globalShaderId", false, &stat );
              if (stat==MS::kSuccess)
              {
                // load the shader id
                stat = shaderIdPlug.getValue( surface_shader_id );
                if (stat==MS::kSuccess && surface_shader_id==shader_id) {
                  // found one
                  result.append( surfaceFn.object() );
                }
              }
            }
          }
        }
      }
    }
  }


  ////////////////////////////////////////////////////////////////////////////////
  //
  // appendShaderNames
  //
  ////////////////////////////////////////////////////////////////////////////////
  void appendShaderNames( int shader_id, MStringArray & result )
  {
    MStatus           stat;
    int               shaderCount;
    MObjectArray      objs;
    MFnDependencyNode surfaceFn;

    getShaderObjects(shader_id, objs);
    shaderCount = objs.length();
    for (int i=0; i<shaderCount; i++)
    {
      stat = surfaceFn.setObject( objs[i] );
      if (stat==MS::kSuccess)
      {
        result.append( surfaceFn.name() );
      }
    }
  }


  ////////////////////////////////////////////////////////////////////////////////
  //
  // getSelectedShaderIds
  //
  ////////////////////////////////////////////////////////////////////////////////
  MStatus getSelectedShaderIds( const MSelectionList &selection,
    MIntArray    & shaderIds )
  {
    MObjectArray   setArray;
    MDagPath       dagPath;
    MObject        component;
    MStatus        stat;

    // process each selected item
    MItSelectionList iter( selection );
    for ( ; !iter.isDone(); iter.next() )
    {
      MItSelectionList::selItemType itemType = iter.itemType();
      if (itemType == MItSelectionList::kDagSelectionItem )
      {
        // processing for a dag item
        stat = iter.getDagPath( dagPath, component );
        if (stat==MS::kSuccess)
        {
          // add shader id, if any, to the list
          if ( component == MObject::kNullObj )
          {
            // shader information comes from looking at plugs on this node
            getDagPathShaderIds( dagPath, shaderIds );
          }
          else 
          {
            // shader information might come from reviewing set information (for faces)
            getDagPathComponentShaderIds( dagPath, component, shaderIds );
          }
        }
      }
    }

    return stat;
  }

  ////////////////////////////////////////////////////////////////////////////////
  //
  // getSelectedShaderEngines
  //
  ////////////////////////////////////////////////////////////////////////////////
  MStatus getSelectedShaderEngines( const MSelectionList &selection,
    MObjectArray &shaderEngines )
  {
    MObjectArray   setArray;
    MDagPath       dagPath;
    MObject        component;
    MStatus        stat;

    //set to ensure that we have a unique list of shaders
    MObjectMap shaderEngineSet;

    // process each selected item
    MItSelectionList iter( selection );
    for ( ; !iter.isDone(); iter.next() )
    {
      MItSelectionList::selItemType itemType = iter.itemType();
      if (itemType == MItSelectionList::kDagSelectionItem )
      {
        // processing for a dag item
        stat = iter.getDagPath( dagPath, component );
        if (stat==MS::kSuccess)
        {
          // add shader, if any, to the list
          if ( component == MObject::kNullObj )
          {
            // shader information comes from looking at plugs on this node
            getDagPathShaderEngines( dagPath, shaderEngineSet );
          }
          else 
          {
            // shader information might come from reviewing set information (for faces)
            getDagPathComponentShaderEngines( dagPath, component, shaderEngineSet );
          }
        }
      }
    }  

    // push the set items onto the object array
    MObjectMap::iterator itor;
    for( itor = shaderEngineSet.begin(); itor != shaderEngineSet.end(); ++itor )
    {
      shaderEngines.append(itor->second);
    }

    return stat;
  }

  ////////////////////////////////////////////////////////////////////////////////
  //
  // shadingEngineFromShader
  //
  ////////////////////////////////////////////////////////////////////////////////
  inline static MObject shadingEngineFromShader( const MObject &shader )
  {
    MStatus           stat;
    MObject           shadingEngineObj( MObject::kNullObj );
    MFnDependencyNode shaderFn( shader, &stat );
    if (stat==MS::kSuccess)
    {
      // shaders connect their outColor attribute to the shading engine
      MPlug outputColor = shaderFn.findPlug( MString("outColor"), true, &stat );
      if (stat==MS::kSuccess)
      {
        // look for where the outColor goes
        MPlugArray surfaceShaderPlugArray;
        if (outputColor.connectedTo(surfaceShaderPlugArray, false, true, &stat) && stat==MS::kSuccess)
        {
          int engineCount = surfaceShaderPlugArray.length();
          for (int i=0; shadingEngineObj==MObject::kNullObj && i<engineCount; i++)
          {
            MObject node = surfaceShaderPlugArray[i].node(&stat);
            if (stat==MS::kSuccess && node.apiType()==MFn::kShadingEngine)
            {
              // this is a shading engine, lets look for one that isn't a reference
              MFnDependencyNode shadingEngineFn( node, &stat );
              if (stat==MS::kSuccess && !shadingEngineFn.isFromReferencedFile())
              {
                // we found a shading engine 
                shadingEngineObj = node;
              }
            }
          }
          if (shadingEngineObj==MObject::kNullObj && engineCount>0)
          {
            // all shading engines were references, so just take the first one
            shadingEngineObj = surfaceShaderPlugArray[0].node();
          }
        }
      }
    }
    return shadingEngineObj;
  }


  ////////////////////////////////////////////////////////////////////////////////
  //
  // materialInfoFromShadingEngine
  //
  ////////////////////////////////////////////////////////////////////////////////
  inline static MObject materialInfoFromShadingEngine( MObject shadingEngineObj )
  {
    MStatus           stat;
    MObject           materialInfoObj( MObject::kNullObj );
    MFnDependencyNode shadingEngineFn( shadingEngineObj, &stat );
    if (stat==MS::kSuccess)
    {
      MPlug messagePlug = shadingEngineFn.findPlug( MString("message"), true, &stat );
      if (stat==MS::kSuccess)
      {
        MPlugArray messages;
        if (messagePlug.connectedTo(messages, false, true, &stat) && stat==MS::kSuccess)
        {
          unsigned int messageCount = messages.length();
          for (unsigned int i=0; materialInfoObj==MObject::kNullObj && i<messageCount; i++)
          {
            MObject receiverObj = messages[i].node( &stat );
            if (stat==MS::kSuccess && receiverObj.apiType()==MFn::kMaterialInfo)
            {
              // found the material info object
              materialInfoObj = receiverObj;
            }
          }
        }
      }
    }
    return materialInfoObj;
  }


  ////////////////////////////////////////////////////////////////////////////////
  //
  // shadingEngineFromMaterialInfo
  //
  ////////////////////////////////////////////////////////////////////////////////
  inline static MObject shadingEngineFromMaterialInfo( MObject materialInfoObj )
  {
    MStatus           stat;
    MObject           shadingEngineObj( MObject::kNullObj );
    MFnDependencyNode materialInfoFn( materialInfoObj, &stat );
    if (stat==MS::kSuccess)
    {
      MPlug messagePlug = materialInfoFn.findPlug( MString("shadingGroup"), true, &stat );
      if (stat==MS::kSuccess)
      {
        MPlugArray messages;
        if (messagePlug.connectedTo(messages, true, false, &stat) && stat==MS::kSuccess)
        {
          unsigned int messageCount = messages.length();
          for (unsigned int i=0; shadingEngineObj==MObject::kNullObj && i<messageCount; i++)
          {
            MObject senderObj = messages[i].node( &stat );
            if (stat==MS::kSuccess && senderObj.apiType()==MFn::kShadingEngine)
            {
              // found the shading engine object
              shadingEngineObj = senderObj;
            }
          }
        }
      }
    }
    return shadingEngineObj;
  }


  ////////////////////////////////////////////////////////////////////////////////
  //
  // addShadingEngineToRenderPartition
  //
  ////////////////////////////////////////////////////////////////////////////////
  inline static bool addShadingEngineToRenderPartition( MObject shadingEngineObj )
  {
    MStatus stat;
    bool    success = false;
    MFnDependencyNode shadingEngineFn( shadingEngineObj, &stat );
    if (stat==MS::kSuccess)
    {
      MString command( MString("partition -add renderPartition ")+shadingEngineFn.name() );
      stat = MGlobal::executeCommand( command );
      success = (stat==MS::kSuccess);
    }
    return success;
  }
  ////////////////////////////////////////////////////////////////////////////////
  //
  // duplicateShadingEngine
  //
  ////////////////////////////////////////////////////////////////////////////////
  MStatus duplicateShadingEngine( const MObject &source, MObject &dest)
  {

    // duplicate the original shading engine and return it
    MObject materialInfo = materialInfoFromShadingEngine( source );
    if ( materialInfo!=MObject::kNullObj )
    {
      // although duplicating the shading engine is the goal,
      // the material info must be duplicated for interactive maya display
      MObject dupMaterialInfo;
      if ( duplicate( materialInfo, dupMaterialInfo, true, true) )
      {
        // get the shading engine for the duplicated material info
        dest = shadingEngineFromMaterialInfo( dupMaterialInfo );
        if ( dest != MObject::kNullObj )
        {
          if ( !addShadingEngineToRenderPartition( dest ) )
          {
            return MS::kFailure;
          }
        }
      }
      else
      {
        return MS::kFailure;
      }
    }
    return MS::kSuccess;
  }
  ////////////////////////////////////////////////////////////////////////////////
  //
  // shaderIsEmpty
  //
  ////////////////////////////////////////////////////////////////////////////////
  inline bool shaderIsEmpty( const MFnSet &shaderSetFn )
  {
    MSelectionList members;
    shaderSetFn.getMembers( members, false );

    return  members.isEmpty();  
  }

  inline bool shaderIsEmpty( const MObject &shader )
  {
    MFnSet setFn(shader);
    return shaderIsEmpty( setFn );
  }
  ////////////////////////////////////////////////////////////////////////////////
  //
  // nonReferencedShadingEngine
  //
  ////////////////////////////////////////////////////////////////////////////////
  MObject nonReferencedShadingEngine( MObject shadingEngine )
  {
    MStatus stat;
    MObject result = shadingEngine; // default is to return input
    // be certain a shadingEngine was passed in
    if (shadingEngine!=MObject::kNullObj && shadingEngine.apiType()==MFn::kShadingEngine)
    {
      // see if shadingEngine is a reference
      MFnDependencyNode shadingEngineFn( shadingEngine, &stat );
      if (stat==MS::kSuccess && shadingEngineFn.isFromReferencedFile())
      {
        bool needCopy = true;
        int shader_id = getShadingEngineShaderId(shadingEngine);
        if (shader_id>=0)
        {
          // this reference shading engine is for an insomniac shader
          // so, see if there already is a non-referenced version available
          MObjectArray availableShaders;
          getShaderObjects( shader_id, availableShaders );
          int availableCount = availableShaders.length();
          for (int i=0; needCopy && i<availableCount; i++)
          {
            MFnDependencyNode availableFn( availableShaders[i], &stat );
            if (stat==MS::kSuccess && !availableFn.isFromReferencedFile())
            {
              // found an existing shader that isn't a reference, we need no copy now
              result = shadingEngineFromShader( availableShaders[i] );
              needCopy = false;
            }
          }
        }
        if (needCopy)
        {
          MObject dupShadingEngine;
          stat = duplicateShadingEngine( shadingEngine, dupShadingEngine );
          if( stat == MS::kSuccess)
          {
            result = dupShadingEngine;
          }
          if (result==shadingEngine)
          {
            // return the original shading engine, but issue a warning
            MGlobal::displayWarning(MString("unable to copy ")+shadingEngineFn.name());
          }
        }
      }
    }
    return result;
  }

  ////////////////////////////////////////////////////////////////////////////////
  //
  // makeUniqueShadingEngine
  //
  ////////////////////////////////////////////////////////////////////////////////
  MObject makeUniqueShadingEngine( MObject &source , const MDagPath &makeUniqueFor )
  {
    MStatus stat;
    MObject dupShadingEngine;
    // make a duplicate of the shading engine
    stat = Maya::duplicateShadingEngine(source, dupShadingEngine);
    MErr(stat, "Unable to duplicate shading engine");

    MSelectionList shaderMembers;

    MFnSet fnSource(source);
    MFnSet fnDest(dupShadingEngine);

    MDagPath memberDagPath;
    MObject memberComponent;
    MFnDagNode dagNodeFn;

    // get all the original shader's members
    stat = fnSource.getMembers( shaderMembers, false );
    MErr(stat, "Could not get the shader's members");

    // for each member
    unsigned numMembers = shaderMembers.length();
    for (unsigned j = 0; j < numMembers; ++j)
    {
      // get the dag path for each member
      stat = shaderMembers.getDagPath( j, memberDagPath, memberComponent );
      MContinueErr(stat, "Could not get the shader member's dag path");

      dagNodeFn.setObject(memberDagPath);
      // if the member of the shader is a descendant of the specified dagpath
      if( dagNodeFn.isChildOf( makeUniqueFor.node(), &stat ) )
      {
        // remove the member from the original shader
        stat = fnSource.removeMember( memberDagPath, memberComponent );
        // add the member to the newly created shader
        stat = fnDest.addMember( memberDagPath, memberComponent );
        if(stat == MS::kSuccess)
        {
          // if the source shader is now empty, delete it
          if( shaderIsEmpty(fnSource) )
          {
            stat = MGlobal::deleteNode( source );
          }
        }
      }           
    }
    return dupShadingEngine;
  }
  ////////////////////////////////////////////////////////////////////////////////
  //
  // sortAndRemoveDuplicates
  //
  ////////////////////////////////////////////////////////////////////////////////
  void sortAndRemoveDuplicates( MIntArray & array ) 
  {
    bool keepGoing = true;
    int  i, j;
    int  a0, a1;
    for (i=((int)array.length())-1; keepGoing && i>0; i--)
    {
      keepGoing = false;
      for (j=0; j<i; j++)
      {
        a0 = array[j];
        a1 = array[j+1];
        if (a0==a1)
        {
          // remove a duplicate
          array.remove(j);
          i--;
          if (j>=i) break;  // take care when removing at end of array
          j--;
          keepGoing = true;
        }
        else
        {
          if (a0>a1)
          {
            // swap out of order elements
            array[j] = a1;
            array[j+1] = a0;
            keepGoing = true;
          }
        }
      }
    }
  }
}
