#pragma once

#include "Math/Line.h"
#include "Math/Frustum.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Math/Point.h"

#include "Common/Automation/Event.h"

#include "wx/wx.h"

const extern Math::Axis SideAxis;
const extern Math::Vector3 SideVector;

const extern Math::Axis UpAxis;
const extern Math::Vector3 UpVector;

const extern Math::Axis OutAxis;
const extern Math::Vector3 OutVector;

inline Math::Vector3 SetupVector(float sideValue, float upValue, float outValue)
{
  return (SideVector * sideValue) + (UpVector * upValue) + (OutVector * outValue);
}

namespace ProjectionModes
{
  enum ProjectionMode
  {
    Perspective,
    Orthographic,
  };
}

typedef ProjectionModes::ProjectionMode ProjectionMode;

namespace ShadingModes
{
  enum ShadingMode
  {
    Wireframe,
    Material,
    Texture,
  };
}

typedef ShadingModes::ShadingMode ShadingMode;

namespace MovementModes
{
  enum MovementMode
  {
    None,
    Orbit,
    Track,
    Dolly,
    Free
  };
}

typedef MovementModes::MovementMode MovementMode;

namespace IntersectionPlanes
{
  enum IntersectionPlane
  {
    View,
    Ground,
  };
}

typedef IntersectionPlanes::IntersectionPlane IntersectionPlane;

//
// Moved event
//

struct CameraMovedArgs
{
  Math::Matrix4 m_Transform;

  CameraMovedArgs( const Math::Matrix4& transform )
    : m_Transform( transform )
  {

  }
};

typedef Nocturnal::Signature< void, const CameraMovedArgs& > CameraMovedSignature;


//
// Camera
//

class Camera
{
private:
  // total control size
  int m_ControlWidth;
  int m_ControlHeight;

  // projection information
  ProjectionMode m_ProjectionMode;
  Math::Matrix4 m_Projection;
  Math::Matrix4 m_InverseProjection;

  // view information
  Math::Matrix4 m_View;
  Math::Matrix4 m_InverseView;
  Math::Frustum m_ViewFrustum;

  // current movement mode
  MovementMode m_MovementMode;

  // movement members
  Math::Vector3 m_Pivot;
  float m_Offset;
  float m_Sensitivity;
  Math::Matrix4 m_Orientation;

  // previous 2d coordinates of mouse
  Math::Point m_Prev;

  // surface rendering
  bool m_WireframeOnMesh;
  bool m_WireframeOnShaded;
  ShadingMode m_ShadingMode;

  // optimizations
  bool m_ViewFrustumCulling;
  bool m_BackFaceCulling;

public:

  //
  // Statics
  //

  const static float NearClipDistance;
  const static float FarClipDistance;
  const static float FieldOfView;


  //
  // Constructor
  //

  Camera();

  void Setup(ProjectionMode mode = ProjectionModes::Perspective, const Math::Vector3& dir = Math::Vector3::Zero, const Math::Vector3& up = Math::Vector3::Zero);

  int GetWidth() const
  {
    return m_ControlWidth;
  }

  int GetHeight() const
  {
    return m_ControlHeight;
  }


  //
  // Projection Mode
  //

  ProjectionMode GetProjectionMode() const
  {
    return m_ProjectionMode;
  }
  void SetProjectionMode(ProjectionMode mode)
  {
    m_ProjectionMode = mode;
  }

  const Math::Matrix4& GetProjection() const
  {
    return m_Projection;
  }

  const Math::Matrix4& GetInverseProjection() const
  {
    return m_InverseProjection;
  }


  //
  // Position and Orientation
  //

  // the center point and rotation m_Pivot of the camera
  const Math::Vector3& GetPivot() const
  {
    return m_Pivot;
  }
  void SetPivot(const Math::Vector3& value)
  {
    m_Pivot = value;
  }

  // the m_Offset back from the m_Pivot point (camera's z value to the m_Pivot)
  float GetOffset() const
  {
    return m_Offset;
  }
  void SetOffset(float value)
  {
    m_Offset = value;
  }

  // the movement sensitivity
  float GetSensitivity() const
  {
    return m_Sensitivity;
  }
  void SetSensitivity(float value)
  {
    m_Sensitivity = value;
  }

  const Math::Matrix4 GetOrientation() const
  {
    return m_Orientation;
  }


  //
  // View
  //

  const Math::Matrix4& GetView() const
  {
    return m_View;
  }

  const Math::Matrix4& GetInverseView() const
  {
    return m_InverseView;
  }

  const Math::Frustum& GetViewFrustum() const
  {
    return m_ViewFrustum;
  }


  //
  // Polygonal Surfaces
  //

  bool GetWireframeOnMesh() const
  {
    return m_WireframeOnMesh;
  }
  void SetWireframeOnMesh(bool value)
  {
    m_WireframeOnMesh = value;
  }

  bool GetWireframeOnShaded() const
  {
    return m_WireframeOnShaded;
  }
  void SetWireframeOnShaded(bool value)
  {
    m_WireframeOnShaded = value;
  }

  ShadingMode GetShadingMode() const
  {
    return m_ShadingMode;
  }
  void SetShadingMode(ShadingMode value)
  {
    m_ShadingMode = value;
  }


  //
  // Culling
  //

  bool IsViewFrustumCulling() const
  {
    return m_ViewFrustumCulling;
  }
  void SetViewFrustumCulling(bool value)
  {
    m_ViewFrustumCulling = value;
  }

  bool IsBackFaceCulling() const
  {
    return m_BackFaceCulling;
  }
  void SetBackFaceCulling(bool value)
  {
    m_BackFaceCulling = value;
  }


  //
  // Implementation
  //


  void Reset();

  void GetUpAxisTransform(Math::Matrix4& m) const;

  Math::Matrix4& SetProjection(int w, int h);
  void GetPerspectiveProjection(Math::Matrix4& m) const;
  void GetOrthographicProjection(Math::Matrix4& m) const;

  void GetDirection(Math::Vector3& dir) const;
  void GetPosition(Math::Vector3& pos) const;

  float ScalingTo(const Math::Vector3& pos) const;

  void MouseDown(wxMouseEvent& e);
  void MouseUp(wxMouseEvent& e);
  void MouseMove(wxMouseEvent& e);
  void MouseScroll(wxMouseEvent& e);

  void Update( bool updateRemote = false );

  void WorldToScreen(const Math::Vector3& v, float& x, float& y);

  void ViewportToScreen(const Math::Vector3& v, float& x, float& y);
  void ScreenToViewport(float x, float y, Math::Vector3& v) const;

  void ViewportToWorldVertex(float x, float y, Math::Vector3& v) const;
  void ViewportToWorldNormal(float x, float y, Math::Vector3& n) const;
  void ViewportToPlaneVertex(float x, float y, IntersectionPlane p, Math::Vector3& v) const;
  void ViewportToLine(float x, float y, Math::Line& l) const;
  bool ViewportToFrustum(float startx, float starty, float endx, float endy, Math::Frustum& f) const;

  Math::AxesFlags ParallelAxis(const Math::Matrix4& m, float criticalDotProduct) const;

  void Frame(const Math::AlignedBox& box);
  void SetTransform( const Math::Matrix4& transform );

  MovementMode GetMovementMode() const
  {
    return m_MovementMode;
  }
  
  void SetMovementMode( const MovementMode mode )
  {
    m_MovementMode = mode;
  }

protected:
  CameraMovedSignature::Event m_Moved;

public:
  void AddMovedListener( const CameraMovedSignature::Delegate& listener )
  {
    m_Moved.Add( listener );
  }
  void RemoveMovedListener( const CameraMovedSignature::Delegate& listener )
  {
    m_Moved.Remove( listener );
  }
};