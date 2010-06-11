#pragma once

#include "API.h"
#include "Platform/Types.h"

namespace Maya
{
  namespace CollisionModes
  {
    enum CollisionMode
    {
      CameraIgnore      =   1 << 0,
      Impassable        =   1 << 1,
      NonTraversable    =   1 << 2,
      Transparent       =   1 << 3,
    };
    static const u32 Default = 0;
  }
  typedef MAYAMESHSETUP_API u32 CollisionMode;

  namespace CollisionModeDisplayFlags
  {
    enum CollisionModeDisplayFlag
    {
      Off = 0,

      CameraIgnore,
      Impassable,
      NonTraversable,
      Transparent,

      Count,
    };
  }
  typedef MAYAMESHSETUP_API CollisionModeDisplayFlags::CollisionModeDisplayFlag CollisionModeDisplayFlag;

  extern const char* CollisionModeDisplayFlagsStrings[CollisionModeDisplayFlags::Count];


  ///////////////////////////////////////////////////////////////////////////////
  // CollisionModeDisplayNode - Draws all polys that are members of a connected set.  This is used
  // to visualize all polys that have a specific flag ( all polys for a specific flag
  // are kept in a specific set ).
  //
  // This no longer uses proper maya sets.  The "inputDrawSet" should contain the name
  // of an integer array attribute that you have added to each mesh.  The contents
  // of the integer array attribute should be this:
  //  element 0: the total number of objects in the mesh ( for sanity checking purposes )
  //  elements 1..n: the face indexes on the mesh that should be visualized.
  //
  // Right now, there should only ever be one CollisionModeDisplayNode in the scene.  You can
  // use all of the static methods below to help guarantee that only one CollisionModeDisplayNode
  // is ever created.  However, these are more for legacy.  The interface is
  // completely driven by MEL at this point.
  //
  class MAYAMESHSETUP_API CollisionModeDisplayNode : public MPxLocatorNode
  {
  public:
    CollisionModeDisplayNode();
    virtual ~CollisionModeDisplayNode(); 

    virtual MStatus compute( const MPlug& plug, MDataBlock& data );
    virtual void    draw( M3dView& view, const MDagPath& path, M3dView::DisplayStyle style, M3dView::DisplayStatus status );

    static  void*   Creator();
    static  MStatus Initialize();	

  public: 
    static MTypeId   m_NodeId;
    static MString   m_NodeName;

    bool             m_Active;

    // attributes
    static MObject   m_Set;    // input set to visualize
    static MObject   m_Color;  // color of visualized set objects

    // Not an attribute - just used by this class
    static std::string s_ConnectedSetStr;  // set the global mode node is currently connected to

  private:
    static MPointArray m_VertexBuffer;
  };

} // namespace Maya