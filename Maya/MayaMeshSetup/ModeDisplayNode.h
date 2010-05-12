#pragma once

#include "API.h"
#include "MeshSetupDisplayNode.h"

namespace Maya
{
  ///////////////////////////////////////////////////////////////////////////////
  // ModeDisplayNode - Draws all polys that are members of a connected set.  This is used
  // to visualize all polys that have a specific flag ( all polys for a specific flag
  // are kept in a specific set ).
  //
  // This no longer uses proper maya sets.  The "inputDrawSet" should contain the name
  // of an integer array attribute that you have added to each mesh.  The contents
  // of the integer array attribute should be this:
  //  element 0: the total number of faces in the mesh ( for sanity checking purposes )
  //  elements 1..n: the face indexes on the mesh that should be visualized.
  //
  // Right now, there should only ever be one ModeDisplayNode in the scene.  You can
  // use all of the static methods below to help guarantee that only one ModeDisplayNode
  // is ever created.  However, these are more for legacy.  The interface is
  // completely driven by MEL at this point.
  //
  class MAYAMESHSETUP_API ModeDisplayNode : public MPxLocatorNode
  {
  public:
    ModeDisplayNode();
    virtual ~ModeDisplayNode(); 

    virtual MStatus compute( const MPlug& plug, MDataBlock& data );
    virtual void    draw( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status );

    static  void*   Creator();
    static  MStatus Initialize();	

    static  void    RemoveAllModeNodes();
    static  bool    GetModeNode( std::string& globalModeNodeName );
    static  bool    SetModeNodeColor( double& red, double& green, double& blue );
    static  bool    ConnectModeNodeToSet( const char* setName );
    static  bool    DisconnectModeNodeFromSet();
    static  bool    IsModeNodeConnectedToSet() { return ( s_ConnectedSetStr.length() > 0 ); }

  protected:
    static bool     CreateModeNode();
    static MPointArray m_VertexBuffer;

  public: 
    static MTypeId   m_NodeId;
    static MString   m_NodeName;

    bool             m_Active;

    // Declare attributes
    static MObject   m_Set;    // input set to visualize
    static MObject   m_Color;  // color of visualized set faces

    // Not an attribute - just used by this class
    static std::string s_ConnectedSetStr;  // set the global mode node is currently connected to
  };

} // namespace Maya