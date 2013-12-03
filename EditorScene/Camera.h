#pragma once

#include "Math/Line.h"
#include "Math/Frustum.h"
#include "Math/Vector3.h"
#include "Math/Matrix4.h"
#include "Math/Point.h"

#include "EditorScene/API.h"
#include "EditorScene/Input.h"
#include "EditorScene/CameraSettings.h"

namespace Helium
{
	namespace Editor
	{
		namespace ProjectionModes
		{
			enum ProjectionMode
			{
				Perspective,
				Orthographic,
			};
		}

		typedef ProjectionModes::ProjectionMode ProjectionMode;

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
				Viewport,
				Ground,
			};
		}

		typedef IntersectionPlanes::IntersectionPlane IntersectionPlane;

		//
		// Moved event
		//

		struct CameraMovedArgs
		{
			Matrix4 m_Transform;

			CameraMovedArgs( const Matrix4& transform )
				: m_Transform( transform )
			{

			}
		};

		typedef Helium::Signature< const CameraMovedArgs& > CameraMovedSignature;


		//
		// Camera
		//

		class HELIUM_EDITOR_SCENE_API Camera
		{
		private:
			// total control size
			Point m_Size;

			// projection information
			ProjectionMode m_ProjectionMode;
			Matrix4 m_Projection;
			Matrix4 m_InverseProjection;

			// view information
			Matrix4 m_View;
			Matrix4 m_InverseView;
			Frustum m_ViewFrustum;

			// current movement mode
			MovementMode m_MovementMode;

			// movement members
			Vector3 m_Pivot;
			float m_Offset;
			float m_Sensitivity;
			Matrix4 m_Orientation;

			// previous 2d coordinates of mouse
			Point m_Prev;

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

			void LoadSettings(CameraSettings* prefs);
			void SaveSettings(CameraSettings* prefs);

			void Setup(ProjectionMode mode = ProjectionModes::Perspective, const Vector3& dir = Vector3::Zero, const Vector3& up = Vector3::Zero);

			Point GetSize() const
			{
				return m_Size;
			}

			int GetWidth() const
			{
				return m_Size.x;
			}

			int GetHeight() const
			{
				return m_Size.y;
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

			const Matrix4& GetProjection() const
			{
				return m_Projection;
			}

			const Matrix4& GetInverseProjection() const
			{
				return m_InverseProjection;
			}


			//
			// Position and Orientation
			//

			// the center point and rotation m_Pivot of the camera
			const Vector3& GetPivot() const
			{
				return m_Pivot;
			}
			void SetPivot(const Vector3& value)
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

			const Matrix4& GetOrientation() const
			{
				return m_Orientation;
			}


			//
			// Viewport
			//

			const Matrix4& GetView() const
			{
				return m_View;
			}

			const Matrix4& GetInverseView() const
			{
				return m_InverseView;
			}

			const Frustum& GetViewFrustum() const
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

			void GetUpAxisTransform(Matrix4& m) const;

			Matrix4& SetProjection(int w, int h);
			void GetPerspectiveProjection(Matrix4& m) const;
			void GetOrthographicProjection(Matrix4& m) const;

			void GetDirection(Vector3& dir) const;
			void GetPosition(Vector3& pos) const;

			float ScalingTo(const Vector3& pos) const;

			void MouseDown( const MouseButtonInput& e );
			void MouseUp( const MouseButtonInput& e );
			void MouseMove( const MouseMoveInput& e );
			void MouseScroll( const MouseScrollInput& e );

			void Update( bool updateRemote = false );

			void WorldToScreen(const Vector3& v, float& x, float& y);

			void ViewportToScreen(const Vector3& v, float& x, float& y);
			void ScreenToViewport(float x, float y, Vector3& v) const;

			void ViewportToWorldVertex(float x, float y, Vector3& v) const;
			void ViewportToWorldNormal(float x, float y, Vector3& n) const;
			void ViewportToPlaneVertex(float x, float y, IntersectionPlane p, Vector3& v) const;
			void ViewportToLine(float x, float y, Line& l) const;
			bool ViewportToFrustum(float startx, float starty, float endx, float endy, Frustum& f) const;

			AxesFlags ParallelAxis(const Matrix4& m, float criticalDotProduct) const;

			void Frame(const AlignedBox& box);
			void SetTransform( const Matrix4& transform );

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
	}
}