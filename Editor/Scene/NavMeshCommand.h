#pragma once

#include "Application/Undo/ExistenceCommand.h"
#include "Foundation/Math/Vector3.h"
#include "Foundation/Math/Matrix4.h"

namespace Editor
{
  // Forwards
  class NavMesh;
  typedef Helium::SmartPtr< Editor::NavMesh > NavMeshPtr;

  class AddNavMeshCommand : public Undo::Command
  {
  private:
    u32 m_Mode;
    NavMeshPtr m_Mesh;
    Math::V_Vector3 m_OldPositions;
    std::vector< u32 > m_SelectedVerts;
    Math::Matrix4 m_ManipulatorPos;
    Math::Vector3 m_RotationStartValue;

    // used for merging verts when we try to add
    bool m_Merged;
    std::vector< u32 > m_TriAdded;

    // mesh cache
    std::vector< u32 > m_WireframeVertexIndices;
    std::vector< u32 > m_TriangleVertexIndices;
    Math::V_Vector3 m_Positions;
    std::vector< u32 > m_SelectedTris;
    u32 m_SelectedEdge;

    Math::Matrix4 m_CubeTransform;
    Math::Matrix4 m_CubeInvTransform;

  public:
    AddNavMeshCommand( Editor::NavMeshPtr mesh, const Math::V_Vector3& positions, u32 mode );
    AddNavMeshCommand( Editor::NavMeshPtr mesh, const Math::Matrix4& cube_transform,  const Math::Matrix4& cube_inv_transform, u32 mode );
    virtual void Undo() HELIUM_OVERRIDE;
    virtual void Redo() HELIUM_OVERRIDE;

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
