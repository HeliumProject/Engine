#pragma once

#include "API.h"
#include "MeshSetupDisplayNode.h"

namespace Maya
{

  ///////////////////////////////////////////////////////////////////////////////
  // 
  //
  class SegmentDisplayNode : public MPxLocatorNode
  {
  public:
    SegmentDisplayNode();
    virtual ~SegmentDisplayNode(); 

    virtual MStatus compute( const MPlug& plug, MDataBlock& data );
    virtual void    draw( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status );

    static  void*   Creator();
    static MStatus  Initialize();

    void            IterateGroupNode( const MDagPath& path, M3dView& view, int displayMode, int& unsegmentedPolys, int currentSegment );
    void            RenderMesh( MFnMesh& meshFn, M3dView& view, int displayMode, int& unsegmentedPolys, int currentSegment );

  public: 

    static MTypeId  m_NodeId;
    static MString  m_NodeName;

    //
    // Custom Attributes
    //

    static MObject  displayOn;
    static MObject  currentSegment;
  };

} // namespace Maya