#pragma once

#include "Undo/ExistenceCommand.h"
#include "math/Vector3.h"
#include "math/Matrix4.h"

namespace Luna
{
  // Forwards
  class NavMesh;
  typedef Nocturnal::SmartPtr< Luna::NavMesh > NavMeshPtr;

  class AddNavMeshCommand : public Undo::Command
  {
  private:
    u32 m_Mode;
    NavMeshPtr m_Mesh;
    Math::V_Vector3 m_OldPositions;
    V_u32 m_SelectedVerts;
    Math::Matrix4 m_ManipulatorPos;
    Math::Vector3 m_RotationStartValue;

    // used for merging verts when we try to add
    bool m_Merged;
    V_u32 m_TriAdded;

    // mesh cache
    V_u32 m_WireframeVertexIndices;
    V_u32 m_TriangleVertexIndices;
    Math::V_Vector3 m_Positions;
    V_u32 m_SelectedTris;
    u32 m_SelectedEdge;

    Math::Matrix4 m_CubeTransform;
    Math::Matrix4 m_CubeInvTransform;

  public:
    AddNavMeshCommand( Luna::NavMeshPtr mesh, const Math::V_Vector3& positions, u32 mode );
    AddNavMeshCommand( Luna::NavMeshPtr mesh, const Math::Matrix4& cube_transform,  const Math::Matrix4& cube_inv_transform, u32 mode );
    virtual void Undo() NOC_OVERRIDE;
    virtual void Redo() NOC_OVERRIDE;

    enum
    {
      kAddVert,
      kAddTri,
      kModify,
      kDeleteVerts,
      kDeleteTris,
      kDeleteEdges,
      kSnapMergeToNearestVert,
      kPunchCubeHole,
      kPaste,
    };
  };
}
