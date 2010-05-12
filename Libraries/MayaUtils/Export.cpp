#include "Export.h"
#include "NodeTypes.h"
#include "UniqueID.h"

#include <string.h>
#include <sstream>

#include <maya/MFileIO.h>
#include <maya/MVector.h>
#include <maya/MEulerRotation.h>
#include <maya/MFnDagNode.h>
#include <maya/MItDag.h>
#include <maya/MFnTransform.h>
#include <maya/MPlug.h>
#include <maya/MSelectionList.h>
#include <maya/MObjectArray.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnSet.h>

#include "TUID/TUID.h"
#include "Common/Container/Insert.h" 

using Nocturnal::Insert; 
using namespace Content;

namespace Maya
{
  //******************** !!IMPORTANT!! ***********************
  //
  //        !!ELEMENT ORDER MUST BE PRESERVED!!
  //
  // The ContentTypes values are stored in the maya files,
  // Reordering the elements, or inserting into the middle
  // of the element list will break the data in existing 
  // Maya files.
  //
  // "Last Element" should always be: NumContentTypes
  //
  // The ContentTypes enum in these files must match up:
  //  * shared/tools/libraries/Content/ContentTypes.h
  //  * shared/tools/libraries/MayaUtils/Export.cpp
  //  * shared/tools/distro/maya/scripts/igExportNodes.mel
  //
  //******************** !!IMPORTANT!! ***********************
  const char* ContentTypesString[ContentTypes::NumContentTypes] =
  {
    "Default",
    "Geometry",
    "Skeleton",
    "Bangle",
    "HighResCollision",
    "LowResCollision",
    "Pathfinding",
    "LowResPathfinding",
    "LightMapped",
    "VertexLit",
    "Overlay",
    "PreShell",
    "BloomPreShell",
    "PostShell",
    "BloomPostShell",
    "Foliage",
    "GeomGroup",
    "MonitorCam",
    "Water",
    "RisingWater",
    "WrinkleMap",
    "Exclude",
    "Destruction",
    "Debris",
    "Glue",
    "Pins",
  };

  M_StringNodeDecode  g_standardNodes;

  typedef std::map< UniqueID::TUID, u32 > M_UIDU32;

  typedef std::multimap< UniqueID::TUID, u32 > MM_UIDU32;
  typedef std::map< std::string, u32 > M_STRU32;

  V_ExportInfo        g_foundNodes;
  MM_UIDU32           g_nodeIDIndexMap;

  

  ContentType ExportType( const MObject & dagNodeObject )
  {
    V_ExportInfo exportInfo;
    GetExportInfo( exportInfo, dagNodeObject );
    if ( exportInfo.empty() )
    {
      return ContentTypes::Null;
    }
    else
    {
      return exportInfo[0].m_type;
    }
  }

  int ExportIndex( const MObject & dagNodeObject )
  {
    V_ExportInfo exportInfo;
    GetExportInfo( exportInfo, dagNodeObject );
    if ( exportInfo.empty() )
    {
      return -1;
    }
    else
    {
      return exportInfo[0].m_index;
    }
  }

  ContentType ContentTypeFromString( const MString & typeName )
  {
    for (int i=0; i<ContentTypes::NumContentTypes; i++)
    {
      if (stricmp(typeName.asChar(), ContentTypesString[i])==0)
      {
        return static_cast<ContentType>(i);
      }
    }
    int ival = atoi(typeName.asChar());
    return static_cast<ContentType>(ival);
  }

  //-----------------------------------------------------------------------------
  // getReferencePath
  // For nodes that are reference nodes this will return the pathname of the reference.
  //-----------------------------------------------------------------------------
  inline static MString getReferencePath( MFnDagNode & dagNode )
  {
    MString result;
    MString cmd( MString("reference -q -f ") + dagNode.partialPathName() );
    MStringArray cmdResult;
    MGlobal::executeCommand( cmd, cmdResult );
    if ( cmdResult.length()>0 )
    {
      result = cmdResult[0];
    }
    return result;
  }

  //-----------------------------------------------------------------------------
  // getArtNumber
  // Whether this file has referenced art or not, this will return the art number.
  // For example moby69 would return 69, tie454 would return 454
  //-----------------------------------------------------------------------------
  inline static int getArtNumber( MFnDagNode & dagNode )
  {
    MString referencePath;
    int artNumber = -1;
    if ( dagNode.isFromReferencedFile() )
    {
      referencePath = getReferencePath( dagNode );
    }
    else
    {
      referencePath = MFileIO::currentFile();
    }
    MStringArray splitPath;
    referencePath.split( '/', splitPath );
    if ( splitPath.length() > 4 && splitPath[2] == MString("art") )
    {
      std::string artDir = splitPath[4].asChar();
      unsigned int numStart = 0;
      while ( numStart < artDir.size() && (artDir[numStart] < '0' || artDir[numStart] > '9') )
      {
        numStart++;
      }
      if ( numStart < artDir.size() )
      {
        artNumber = atoi( artDir.substr( numStart, artDir.size() - numStart ).c_str() );
      }
    }
    return artNumber;
  }

  //-----------------------------------------------------------------------------
  // getEntityID
  // Cinematics store the tuid of the entity on the ExportNodeSet
  //-----------------------------------------------------------------------------
  inline static tuid getEntityID( const MDagPath & path )
  {
    MStatus        status;
    MSelectionList selection;
    MObjectArray   sets;
    u32            setCount;
    tuid           entityID = TUID::Null;

    MFnDagNode     dagNode( path, &status );
    if ( status==MS::kSuccess )
    {
      do
      {
        selection.clear();
        sets.clear();
        selection.add( dagNode.dagPath() );
        MGlobal::getAssociatedSets( selection, sets );
        setCount = sets.length();
        for ( u32 i=0; i<setCount
                    && status==MS::kSuccess
                    && entityID==TUID::Null; i++ )
        {
          MFnSet set( sets[i], &status );
          if ( status==MS::kSuccess )
          {
            // long name "entityTuid" used in place of short name "tuid"
            // this is so the animators have an easier time with scenes
            // whose entityTuid attributes were created manually (won't happen in future)
            MPlug tuidPlug = set.findPlug( "entityTuid", true );
            if ( !tuidPlug.isNull() )
            {
              MString tuidMString;
              tuidPlug.getValue( tuidMString );
              if ( tuidMString.length() > 0 )
              {
                TUID::Parse( tuidMString.asChar(), entityID );
              }
            }
          }
        }
        if ( status==MS::kSuccess && entityID==TUID::Null )
        {
          if ( dagNode.parentCount() > 0 )
          {
            // move up to the parent and iterate
            dagNode.setObject( dagNode.parent(0) );
            MDagPath parentPath;
            dagNode.getPath( parentPath );
            status = dagNode.setObject( parentPath );
          }
          else
          {
            // no more parents, we've reached the root
            status = MS::kFailure;
          }
        }
      } while ( status==MS::kSuccess && entityID==TUID::Null );
    }
    return entityID;
  }

  //-----------------------------------------------------------------------------
  // isAPrefix
  // determine if the prefix parameter is a prefix of one of the given strings
  //-----------------------------------------------------------------------------
  inline static bool isAPrefix( const std::string & prefix,
                                const std::vector<std::string> & strings )
  {
    bool found = false;
    std::vector<std::string>::const_iterator iter = strings.begin();
    std::vector<std::string>::const_iterator end  = strings.end();
    for ( ; (!found) && iter!=end; ++iter )
    {
      found = ((*iter).find( prefix ) == 0 );
    }
    return found;
  }

  //-----------------------------------------------------------------------------
  // isAPrefix
  // determine if the prefix parameter is a prefix of one of the given strings
  //-----------------------------------------------------------------------------
  inline static bool isAPrefix( const std::vector<std::string> & prefix,
                                const std::string              & full )
  {
    bool found = false;
    std::vector<std::string>::const_iterator iter = prefix.begin();
    std::vector<std::string>::const_iterator end  = prefix.end();
    for ( ; (!found) && iter!=end; ++iter )
    {
      found = (full.find( *iter ) == 0 );
    }
    return found;
  }

  //-----------------------------------------------------------------------------
  // foundNode
  // Once a node is found that could potentially be the result of an arbitrary
  // query this function is called to store the node information in f_foundNodes.
  // Note that if the m_selection list is unempty, then the node must be an
  // ancestor of some node in selection, or it must be in the selection itself.
  //-----------------------------------------------------------------------------
  void foundNode( const MObject                & node,
                  const ContentType              type,
                  const int                      index,
                  const std::vector<std::string> selection,
                  const std::vector<std::string> groupNode )
  {
    MStatus status;
    MFnDagNode dagNode( node, &status );
    if ( status == MS::kSuccess )
    {
      MDagPath dagPath;
      status = dagNode.getPath( dagPath );
      if ( status == MS::kSuccess )
      {
        ExportInfo rec;
        rec.m_pathStrFull = dagPath.fullPathName().asChar();
        rec.m_pathStrPartial = dagPath.partialPathName().asChar();
        rec.m_path = dagPath;
        rec.m_type = type;
        rec.m_index = index;
        rec.m_artNumber = getArtNumber( dagNode );
        rec.m_ID = Maya::GetNodeID( node );
        rec.m_entityID = getEntityID( dagPath );
        if ( ( selection.empty() || isAPrefix( rec.m_pathStrFull, selection ) ) 
          && ( groupNode.empty() || isAPrefix( groupNode, rec.m_pathStrFull ) ) )
        {
          bool add = true;
          if ( rec.m_ID != UniqueID::TUID::Null )
          {
            //Insert<MM_UIDU32>::Result result = g_nodeIDIndexMap.insert( MM_UIDU32::value_type( rec.m_ID, (u32)g_foundNodes.size() ) );
            g_nodeIDIndexMap.insert( MM_UIDU32::value_type( rec.m_ID, (u32)g_foundNodes.size() ) );
            // make sure if it is already in the map that it is unique
            // FIXME - this breaks horribly in the case of cinematics, two references to the same
            // actor
            //NOC_ASSERT( result.second || rec == g_foundNodes[ result.first->second ] );

            //add = result.second;
          }

          if ( add )
            g_foundNodes.push_back( rec );
        }
      }
    }
  }

  //-----------------------------------------------------------------------------
  // findAllExportNodes
  // Each export node found in the scene is passed to foundNode() in order to
  // save the node in the m_found vector.
  //-----------------------------------------------------------------------------
  void findAllExportNodes( const std::vector<std::string> & selection,
                           const std::vector<std::string> & groupNode )
  {
    MStatus status;
    MItDag dagIter( MItDag::kBreadthFirst, MFn::kPluginTransformNode, &status );
    if ( status == MS::kSuccess )
    {
      for ( ; !dagIter.isDone(); dagIter.next() )
      {
        MFnDagNode dagNode( dagIter.item(), &status );
        if ( status == MS::kSuccess && dagNode.typeId() == MTypeId(IGL_EXPORTNODE_ID) )
        {
          MPlug contentType = dagNode.findPlug( "contentType" );
          MPlug contentIndex = dagNode.findPlug( "contentNumber" );
          int node_itype;
          int node_index;
          contentType.getValue( node_itype );
          contentIndex.getValue( node_index );
          foundNode( dagNode.object(), (ContentType)node_itype, node_index, selection, groupNode );
        }
      }
    }
  }

  //-----------------------------------------------------------------------------
  // rememberDecode
  // The defs of node names and the associated attributes are extended by
  // each time rememberDecode is called.  One element represents an index
  // zero without any number (eg: coll) and also each index value from
  // lowIndex to highIndex is stored as an element (eg: coll3)
  //-----------------------------------------------------------------------------
  inline static void rememberDecode( M_StringNodeDecode    & defs,
                               const std::string             baseName,
                                     int                     lowIndex,
                                     int                     highIndex,
                                     ContentType       type )
  {
    NodeDecode data;
    data.m_index = 0;
    data.m_type = type;
    defs[ baseName ] = data;
    for (int index=lowIndex; index<=highIndex; index++)
    {
      std::stringstream strm;
      data.m_index = index;
      strm << baseName << index;
      defs[ strm.str() ] = data;
    }
  }

  //-----------------------------------------------------------------------------
  // findAllNamedNodes
  // If old style nodes are requested then this method is called.  The method
  // extends the m_found vector by each arbitrary old style named node.
  //-----------------------------------------------------------------------------
  void findAllNamedNodes( const std::vector<std::string> & selection,
                          const std::vector<std::string> & groupNode )
  {
    static std::string REFS       = "REFS";
    static std::string ROOT       = "root";
    if ( g_standardNodes.empty() )
    {
      // initialize standard node names once
      rememberDecode( g_standardNodes, "geom", 0, -1, ContentTypes::Geometry );
      rememberDecode( g_standardNodes, "mn", 0, 4, ContentTypes::Geometry );
      rememberDecode( g_standardNodes, "bn", 0, 99, ContentTypes::Bangle );
      rememberDecode( g_standardNodes, "coll", 0, 4, ContentTypes::HighResCollision );
      rememberDecode( g_standardNodes, "coll_low", 0, 4, ContentTypes::LowResCollision );
      rememberDecode( g_standardNodes, "pathfinding", 0, -1, ContentTypes::Pathfinding );
      rememberDecode( g_standardNodes, "pathfinding_low", 0, -1, ContentTypes::LowResPathfinding );
    }

    MStatus status;
    MItDag dagIter( MItDag::kBreadthFirst, MFn::kTransform, &status );
    if ( status == MS::kSuccess )
    {
      for ( ; !dagIter.isDone(); dagIter.next() )
      {
        MFnDagNode dagNode( dagIter.item(), &status );
        if ( status == MS::kSuccess  )
        {
          std::string name = dagNode.name().asChar();
          const char * nameCStr = name.c_str();
          const char * colon = strchr( nameCStr, ':' );
          if ( colon != NULL )
          {
            // remove any maya namespace prefix before examining the name
            int colonPos = (int)(colon - nameCStr);
            name.erase( 0, colonPos + 1 );
          }
          if ( name.find( REFS ) != std::string::npos )
          {
            dagIter.prune();
          }
          else
          {
            M_StringNodeDecode::iterator found = g_standardNodes.find( name );
            if ( found != g_standardNodes.end() )
            {
              foundNode( dagNode.object(), (*found).second.m_type, (*found).second.m_index, selection, groupNode );
              dagIter.prune();
            }
            else
            {
              MFn::Type apiType = dagNode.object().apiType();
              if ( apiType == MFn::kJoint && name == ROOT )
              {
                foundNode( dagNode.object(), ContentTypes::Skeleton, 0, selection, groupNode );
                dagIter.prune();
              }
              if ( apiType == MFn::kPluginTransformNode && dagNode.typeId() == MTypeId(IGL_EXPORTNODE_ID) )
              {
                dagIter.prune();
              }
            }
          }
        }
      }
    }
  }

  void InitExportInfo( bool                     allowOldStyle,
                       std::vector<std::string> selection,
                       std::vector<std::string> groupNode )
  {
    g_foundNodes.clear();
    g_nodeIDIndexMap.clear();
    findAllExportNodes( selection, groupNode );
    if ( allowOldStyle )
    {
      findAllNamedNodes( selection, groupNode );
    }
  }

  void GetExportInfo( M_MObject_ExportInfoIndex & result,
                const MObjectArray              & nodes )
  {
    const unsigned int nodeCount = nodes.length();
    V_i32 exportInfoIndex;
    result.clear();
    for ( unsigned int i=0; i<nodeCount; i++ )
    {
      GetExportInfo( exportInfoIndex, nodes[i] );
      if ( exportInfoIndex.size() > 0 )
      {
        result[ nodes[i] ] = exportInfoIndex[0];
      }
    }
  }

  void GetExportInfo( V_ExportInfo & result,
                const MObject      & node,
                      int            artNumber,
                      ContentType    type,
                      int            index )
  {
    V_i32 result_index;
    GetExportInfo( result_index, node, artNumber, type, index );
    V_i32::iterator itr = result_index.begin();
    V_i32::iterator end = result_index.end();
    result.clear();
    for ( ; itr != end ; ++itr )
    {
      result.push_back( g_foundNodes[ (*itr) ] );
    }
  }

  void GetExportInfo( V_i32 & result,
                const MObject          & node,
                      int                artNumber,
                      ContentType        type,
                      int                index )
  {
    std::string nodeName;
    if ( node != MObject::kNullObj )
    {
      MStatus status;
      MFnDagNode dagNode( node, &status );
      if ( status == MS::kSuccess )
      {
        nodeName = dagNode.fullPathName().asChar();
      }
    }
    const int exportNodeCount = (int)g_foundNodes.size();
    result.clear();
    for ( int i=0; i<exportNodeCount; i++ )
    {
      ExportInfo * info = &( g_foundNodes[i] );
      if ( ( (type == ContentTypes::Default ) || ((*info).m_type      == type ) )
        && ( (index < 0)                      || ((*info).m_index     == index ) )
        && ( (artNumber < 0)                  || ((*info).m_artNumber == artNumber ) )
        && ( (node == MObject::kNullObj)      || (nodeName.find((*info).m_pathStrFull)!=std::string::npos) ) )
      {
        result.push_back( i ); 
      }
    }
  }

  bool RemoveMultiMapDupe( const UniqueID::TUID& id )
  {
    if ( g_nodeIDIndexMap.count( id ) > 1 )
    {
      g_nodeIDIndexMap.erase( g_nodeIDIndexMap.find( id ) );
    } 
    return true;
  }

  bool GetExportInfo( const UniqueID::TUID& id, ExportInfo*& info )
  {
    MM_UIDU32::const_iterator itr = g_nodeIDIndexMap.find( id );
    if ( itr == g_nodeIDIndexMap.end() )
      return false;

    info = &g_foundNodes[ (*itr).second ]; 

    return true;
  }

  ContentType ClassifyOldNode( MFnTransform &node, u32 &num )
  {
    // special case for pieces of ufrag set up for lighting in the old lighting pipeline
    if( node.hasAttribute( "groupLightMapMsg" ) )
      return ContentTypes::FragmentGroup;

    return ClassifyOldNode( node.name().asChar(), node.fullPathName().asChar(), num );
  }

  ContentType ClassifyOldNode( const std::string &name, const std::string& fullname, u32 &num )
  {
    size_t len = name.length();
    num = 0;

    if( len < 3 )
      return ContentTypes::Default;

    if( name == "coll" )
      return ContentTypes::HighResCollision;
    if( name == "coll_low" )
      return ContentTypes::LowResCollision;

     // THIS IS BULLSHIT
    if ( fullname.find( "|coll|" ) == std::string::npos && fullname.find( "|coll_low|" ) == std::string::npos )
    {
      if( name == "pathfinding" )
        return ContentTypes::Pathfinding;
      if ( name == "pathfinding_low" )
        return ContentTypes::LowResPathfinding;
      if( name == "vert" )
        return ContentTypes::VertexLit;
      if( name == "map" )
        return ContentTypes::LightMapped;
      if( name == "overlay" )
        return ContentTypes::Overlay;
      if( name == "preshells" )
        return ContentTypes::PreShell;
      if( name == "bloompreshells" )
        return ContentTypes::BloomPreShell;
      if( name == "postshells" )
        return ContentTypes::PostShell;
      if( name == "bloompostshells" )
        return ContentTypes::BloomPostShell;
      if( name == "Foliage" )
        return ContentTypes::Foliage;
      if( name == "geom" )
        return ContentTypes::Geometry;

      std::string mainStr = name.substr( 0, 2 );
      std::stringstream numStrm( name.substr( 2, len-1 ) );
      std::string tmpStr = numStrm.str();

      int tmp;
      numStrm >> tmp;
      bool fail = numStrm.fail();
      bool empty = numStrm.rdbuf()->in_avail() == 0;
      if ( !fail && empty  )
      {
        num = tmp;

        if( mainStr == "mn" )
        {
          return ContentTypes::Geometry;
        }
        if( mainStr == "bn" )
        {
          return ContentTypes::Bangle;
        }
      }
    }

    return ContentTypes::Default;

  }

  MStatus CreateExportNodeFromTransform( const MObject &transform, MObject& exportNode, ContentType exportType, u32 num )
  {
    MStatus stat;
    MFnTransform transFn( transform, &stat );
    if( !stat )
      return stat;

    // don't replace an ExportNode with an ExportNode !
    if( transFn.typeId() == MTypeId(IGL_EXPORTNODE_ID) )
      return MStatus::kFailure;

    u32 numParents = transFn.parentCount();

    if( numParents > 1)
      return MStatus::kFailure;

    MObject parent;
    if( numParents != 0 )
    {
      parent = transFn.parent( 0 );
    }

    //create a new ExportNode

    MFnDagNode nodeFn;
    exportNode = nodeFn.create( MTypeId(IGL_EXPORTNODE_ID), parent, &stat );
    if( !stat )
      return stat;
    MFnTransform exportNodeFn( exportNode );

    MVector translation;
    MEulerRotation rotation;
    double scale[3];
    double shear[3];

    // get the data from the old transform node
    translation = transFn.getTranslation( MSpace::kTransform );
    transFn.getRotation( rotation );
    transFn.getScale( scale );
    transFn.getShear( shear );

    // put it onto the new ExportNode
    exportNodeFn.setTranslation( translation, MSpace::kTransform );
    exportNodeFn.setRotation( rotation );
    exportNodeFn.setScale( scale );
    exportNodeFn.setShear( shear );

    // classify if asked to
    if( exportType == ContentTypes::Default )
    {
      exportType = ClassifyOldNode( exportNodeFn, num );
    }

    MPlug exportPlug = exportNodeFn.findPlug( "contentType" );
    exportPlug.setValue( (i32)exportType );

    exportPlug = exportNodeFn.findPlug( "contentNumber" );
    exportPlug.setValue( (i32)num );

    return MS::kSuccess;
  }

  MStatus ReplaceWithExportNode( const MObject &transform, ContentType exportType, u32 num )
  {
    MStatus stat;
    MObject exportNode;
    if( CreateExportNodeFromTransform( transform, exportNode, exportType, num ) != MS::kSuccess )
      return MS::kFailure;

    MFnTransform exportNodeFn( exportNode );
    MFnTransform transFn( transform );
     
    MString name = transFn.name();

    // add the old transforms children to the new ExportNode    
    while( transFn.childCount() )
    {
      MObject child = transFn.child( 0, &stat );
      stat = exportNodeFn.addChild( child );
    }

    // delete the old transform node
    MGlobal::deleteNode( transFn.object() );

    // wait until after we delete the old node to name the ExportNode, so the names don't clash
    exportNodeFn.setName( name );

    
    //return MS::kSuccess;
    return stat; 
  }

  bool MAYAUTILS_API IsExcluded( const MObject& node )
  {
    if( node.isNull() )
    {
      return false;
    }

    // Check if our content type is Content::ContentTypes::Exclude
    MFnDependencyNode   currNodeFn( node );
    
    // are we an export node
    if( currNodeFn.typeId() == MTypeId( IGL_EXPORTNODE_ID ) )
    {
      MPlug               currContentTypePlug = currNodeFn.findPlug("contentType");

      if( !currContentTypePlug.isNull() )
      {
        int currContentTypeData;
        currContentTypePlug.getValue( currContentTypeData );
        Content::ContentType currType = static_cast<Content::ContentType>(currContentTypeData);

        if( currType == Content::ContentTypes::Exclude ) return true;
      }
    }

    // check if we are decendant of an excluded node
    return DescendantOf( node, Content::ContentTypes::Exclude, true );
  }

  bool MAYAUTILS_API DescendantOf( const MObject& node, const Content::ContentType type, bool recurse )
  {
    if( node.isNull() ) 
    {
      return false;
    }

    // check all the parent nodes for the content type
    MStatus stat;
    MFnDagNode        nodeFn( node, &stat );

    for( unsigned int i = 0; i < nodeFn.parentCount(); i++ )
    {
      MObject parent = nodeFn.parent(i, &stat);
      
      if( parent.isNull() )
      {
        continue;
      }
      
      MFnDependencyNode   parentDepNodeFn( parent );

      // are we an export node
      if( parentDepNodeFn.typeId() == MTypeId( IGL_EXPORTNODE_ID ) )
      {
        MPlug contentTypePlug = parentDepNodeFn.findPlug("contentType");

        if( !contentTypePlug.isNull() )
        {
          int contentTypeData;
          contentTypePlug.getValue( contentTypeData );
          Content::ContentType currType = static_cast<Content::ContentType>(contentTypeData);

          if( currType == type ) 
          {
            return true;
          }
        }
      }

      // if we are not a descendant of the node type, we want to continue
      // checking the parent nodes
      if( recurse && DescendantOf( parent, type, recurse ) )
      {
        return true;
      }
    }
    return false;
  }

  bool MAYAUTILS_API DescendantOf( const MObject& node, const MTypeId& typeID, bool recurse )
  {
    if( node.isNull() ) 
    {
      return false;
    }

    // check all the parent nodes for the specified type
    MStatus stat;
    MFnDagNode        nodeFn( node, &stat );

    for( unsigned int i = 0; i < nodeFn.parentCount(); i++ )
    {
      MObject parent = nodeFn.parent(i, &stat);
      
      if( parent.isNull() )
      {
        continue;
      }
      
      MFnDependencyNode   parentDepNodeFn( parent );

      // are we an export node
      if( parentDepNodeFn.typeId() == MTypeId( typeID ) )
      {
        return true;
      }

      // if we are not a descendant of the node type, we want to continue
      // checking the parent nodes
      if( recurse && DescendantOf( parent, typeID, recurse ) )
      {
        return true;
      }
    }
    return false;
  }
} // namespace Maya
