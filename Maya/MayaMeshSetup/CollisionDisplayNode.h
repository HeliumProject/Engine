#pragma once


namespace Maya
{

  ///////////////////////////////////////////////////////////////////////////////
  // ModeDisplayNode like segment flagging display node
  //
  class CollisionDisplayNode : public MPxLocatorNode
  {
  public:

    CollisionDisplayNode();
    virtual ~CollisionDisplayNode(); 
    
    virtual MStatus compute( const MPlug& plug, MDataBlock& data );
    virtual void    draw( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status );

    static  void*   Creator();
    static MStatus  initialize() { return MS::kSuccess; }

    void            IterateGroupNode( const MDagPath& path, M3dView& view );
    void            RenderMesh( MFnMesh& meshFn, M3dView& view );

  public: 

    static MTypeId  m_NodeId;
    static MString  m_NodeName;

  };

} // namespace Maya