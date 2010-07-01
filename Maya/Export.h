#pragma once

#include "API.h"
#include "Foundation/TUID.h"

#include <maya/MGlobal.h>
#include <maya/MDagPath.h>
#include <maya/MFnTransform.h>
#include "Utils.h"

#include <string>
#include <vector>
#include <set>
#include <map>

#include "Foundation/TUID.h"
#include "Pipeline/Content/ContentTypes.h"

namespace Maya
{
  struct ExportInfo
  {
    tstring          m_pathStrFull;
    tstring          m_pathStrPartial;
    MDagPath             m_path;
    Content::ContentType m_type;
    int                  m_index;
    int                  m_artNumber;
    Nocturnal::TUID m_ID;
    tuid                 m_entityID;

    ExportInfo() 
      : m_type( Content::ContentTypes::Default )
      , m_index( 0 )
      , m_artNumber( 0 )
      , m_entityID( Nocturnal::TUID::Null )
    {
    }

    ExportInfo( const ExportInfo & other )
    {
      m_pathStrFull    = other.m_pathStrFull;
      m_pathStrPartial = other.m_pathStrPartial;
      m_path           = other.m_path;
      m_type           = other.m_type;
      m_index          = other.m_index;
      m_artNumber      = other.m_artNumber;
      m_ID             = other.m_ID;
      m_entityID       = other.m_entityID;
    }

    bool operator==( const ExportInfo& rhs ) const
    {
      return m_type == rhs.m_type
        && m_ID == rhs.m_ID
        && m_pathStrFull == rhs.m_pathStrFull;
    }
  };

  typedef std::vector<ExportInfo> V_ExportInfo;

  typedef std::map<MObject, int, Maya::MObjectCompare> M_MObject_ExportInfoIndex;

  struct NodeDecode
  {
    Content::ContentType m_type;
    int                m_index;
  };

  typedef std::map<tstring, NodeDecode> M_StringNodeDecode;

  extern MAYA_API const tchar* ContentTypesString[Content::ContentTypes::NumContentTypes];

  extern MAYA_API M_StringNodeDecode g_standardNodes;

  extern MAYA_API V_ExportInfo       g_foundNodes;

  Content::ContentType MAYA_API ContentTypeFromString( const MString & typeName );

  // this call must be made prior to queries
  void MAYA_API InitExportInfo( bool                     allowOldStyle,
                       std::vector<tstring> selection,
                       std::vector<tstring> groupNode );

  // these call makes queries after InitExportInfo has been called
  void MAYA_API GetExportInfo( M_MObject_ExportInfoIndex& result,
                               const MObjectArray&        nodes );

  void MAYA_API GetExportInfo( V_ExportInfo&      result,
                               const MObject&     node      = MObject::kNullObj,
                               int                artNumber = -1,
                               Content::ContentType type      = Content::ContentTypes::Default,
                               int                index     = -1 );

  void MAYA_API GetExportInfo( std::vector< i32 >&             result,
                               const MObject&     node      = MObject::kNullObj,
                               int                artNumber = -1,
                               Content::ContentType type      = Content::ContentTypes::Default,
                               int                index     = -1 );

  bool MAYA_API GetExportInfo( const Nocturnal::TUID& id, ExportInfo*& info );

  bool MAYA_API RemoveMultiMapDupe( const Nocturnal::TUID& id );

  // makes queries via GetExportInfo
  Content::ContentType MAYA_API ExportType( const MObject & dagNodeObject );
  Content::ContentType MAYA_API ExportType( const MString & typeName );

  MStatus MAYA_API CreateExportNodeFromTransform( const MObject &transform, MObject& exportNode, Content::ContentType exportType = Content::ContentTypes::Default, u32 num = 0 );
  MStatus MAYA_API ReplaceWithExportNode( const MObject &transFn, Content::ContentType exportType = Content::ContentTypes::Default, u32 num = 0 );
  Content::ContentType MAYA_API ClassifyOldNode( const tstring &name, const tstring& fullname, u32 &num );
  Content::ContentType MAYA_API ClassifyOldNode( MFnTransform &name, u32 &num );

  // makes queries via GetExportInfo
  int ExportIndex( const MObject & dagNodeObject );

  /// @brief Check if a node should be excluded during the export process
  /// If a node has a content type of Content::ContentTypes::Exclude OR
  /// if it is a child node of another node of that content type, then we
  /// do not want it to be exported.
  /// @param node the node we want to check for exclusion
  /// @return true if this node should NOT be exported. false otherwise
  /// @see Content::ContentTypes
  bool MAYA_API IsExcluded( const MObject& node );

  /// @brief Check if the node is a decendant of a type of content node
  /// Check if a parent of this node contains the following content type. This
  /// function does NOT check the type of the current node, only its parents.
  /// @param node the node whos parents we want to check the content type of
  /// @param type the type of content node that we are looking for
  /// @param recurse if this is set to false we only check the direct parents of this node
  bool MAYA_API DescendantOf( const MObject& node, const Content::ContentType type, bool recurse = true );

  /// @brief Check if the node is a decendant of a type of node
  /// Check if a parent of this node is of the specified type. This
  /// function does NOT check the type of the current node, only its parents.
  /// @param node the node whos parents we want to check the type of
  /// @param nodeID the type of node that we are looking for
  /// @param recurse if this is set to false we only check the direct parents of this node
  bool MAYA_API DescendantOf( const MObject& node, const MTypeId& typeID, bool recurse = true );

} // namespace Maya
