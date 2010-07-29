#pragma once

#include "Editor/API.h"

#include "Foundation/Math/Line.h"
#include "Foundation/Math/Frustum.h"

#include "Foundation/Memory/SmartPtr.h"

#include "Object.h"

#include "Visitor.h"

namespace Editor
{
  class Camera;
  class PickHit;
  typedef Helium::SmartPtr<PickHit> PickHitPtr;
  typedef std::vector<PickHitPtr> V_PickHitSmartPtr;

  //
  // Pick is an encapsulation of a user interaction with the screen
  //

  namespace PickTypes
  {
    enum PickType
    {
      Line,
      Frustum,
    };
  };

  namespace PickFlags
  {
    enum PickFlag
    {
      IgnoreNormal        = 1 << 0,
      IgnoreVertex        = 1 << 1,
      IgnoreIntersection  = 1 << 2,
    };
  }

  typedef PickFlags::PickFlag PickFlag;

  class PickVisitor : public Visitor
  {
  protected:
    // flags
    u32 m_Flags;

    // required for construction
    const Editor::Camera* m_Camera;

    // the results of the picking
    V_PickHitSmartPtr m_PickHits;

    // the object that will be associated with the hits generated in Pick() functions
    Object* m_CurrentObject;

    // matrices to map to and from world space and local space 
    //  - testing is performed in local space
    //  - results are stored in global space)
    Math::Matrix4 m_CurrentWorldTransform;
    Math::Matrix4 m_CurrentInverseWorldTransform;

  public: 
    PickVisitor(const Editor::Camera* camera);

  protected:
    PickVisitor(const PickVisitor& rhs) { }

  public:
    virtual const PickTypes::PickType GetPickType() const = 0;

    u32 GetFlags() const
    {
      return m_Flags;
    }

    bool HasFlags(u32 flags) const
    {
      return (m_Flags & flags) != 0;
    }

    void SetFlags(u32 flags)
    {
      m_Flags = flags;
    }

    void SetFlag(u32 flag, bool value)
    {
      if (value)
      {
        m_Flags |= flag;
      }
      else
      {
        m_Flags &= ~flag;
      }
    }

    const Editor::Camera* GetCamera() const
    {
      return m_Camera;
    }

  protected:
    PickHit* AddHit();

  public:
    void AddHit (PickHit* pickHit)
    {
      m_PickHits.push_back (pickHit);
    }

    const V_PickHitSmartPtr& GetHits() const
    {
      return m_PickHits;
    }

    bool HasHits() const
    {
      return !m_PickHits.empty();
    }

    void ClearHits()
    {
      m_PickHits.clear();
    }

    size_t GetHitCount() const
    {
      return m_PickHits.size();
    }

    void SetHitCount(size_t count)
    {
      HELIUM_ASSERT( count <= m_PickHits.size() );
      m_PickHits.resize( count );
    }

    void SetCurrentObject(Object* object)
    {
      m_CurrentObject = object;
    }

    void SetCurrentObject(Object* object, const Math::Matrix4& worldSpaceTransform)
    {
      m_CurrentObject = object;
      m_CurrentWorldTransform = worldSpaceTransform;
      m_CurrentInverseWorldTransform = worldSpaceTransform.Inverted();

      // working in local space
      Transform(); 
    }

    void SetCurrentObject(Object* object, const Math::Matrix4& worldSpaceTransform, const Math::Matrix4& inverseWorldSpaceTransform)
    {
      m_CurrentObject = object;
      m_CurrentWorldTransform = worldSpaceTransform;
      m_CurrentInverseWorldTransform = inverseWorldSpaceTransform;

      // working in local space
      Transform(); 
    }

    virtual void Transform() = 0;

    // picking functions (produce hits)
    virtual bool PickPoint(const Math::Vector3& p, const float err = Math::LinearIntersectionError) = 0;
    virtual bool PickSegment(const Math::Vector3& p1,const Math::Vector3& p2, const float err = Math::LinearIntersectionError) = 0;
    virtual bool PickTriangle(const Math::Vector3& v0,const Math::Vector3& v1,const Math::Vector3& v2, const float err = Math::LinearIntersectionError) = 0;
    virtual bool PickSphere(const Math::Vector3& center, const float radius) = 0;
    virtual bool PickBox(const Math::AlignedBox& box) = 0;

    // testing functions (no hits)
    virtual bool IntersectsBox(const Math::AlignedBox& box) const = 0;
  };

  class LinePickVisitor : virtual public PickVisitor
  {
  protected:
    Math::Line m_PickSpaceLine;
    Math::Line m_WorldSpaceLine;

  public:
    LinePickVisitor(const Editor::Camera* camera, const int x, const int y);
    LinePickVisitor(const Editor::Camera* camera, const Math::Line& line);

  protected:
    LinePickVisitor(const LinePickVisitor& rhs) : PickVisitor (rhs) { }

  public:
    virtual const PickTypes::PickType GetPickType() const HELIUM_OVERRIDE
    {
      return PickTypes::Line;
    }

    const Math::Line& GetPickSpaceLine() const
    {
      return m_PickSpaceLine;
    }

    const Math::Line& GetWorldSpaceLine() const
    {
      return m_WorldSpaceLine;
    }

    virtual void Transform() HELIUM_OVERRIDE;
    
    // picking functions (produce hits)
    virtual bool PickPoint(const Math::Vector3& p, const float err = Math::LinearIntersectionError) HELIUM_OVERRIDE;
    virtual bool PickSegment(const Math::Vector3& p1,const Math::Vector3& p2, const float err = Math::LinearIntersectionError) HELIUM_OVERRIDE;
    virtual bool PickTriangle(const Math::Vector3& v0,const Math::Vector3& v1,const Math::Vector3& v2, const float err = Math::LinearIntersectionError) HELIUM_OVERRIDE;
    virtual bool PickSphere(const Math::Vector3& center, const float radius) HELIUM_OVERRIDE;
    virtual bool PickBox(const Math::AlignedBox& box) HELIUM_OVERRIDE;

    // testing functions (no hits)
    virtual bool IntersectsBox(const Math::AlignedBox& box) const HELIUM_OVERRIDE;

  protected:
    // hit adding functions
    bool AddHitPoint(const Math::Vector3& p, Math::Vector3& offset);
    bool AddHitSegment(const Math::Vector3& p1,const Math::Vector3& p2, f32 mu, Math::Vector3& offset);
    bool AddHitTriangle(const Math::Vector3& v0,const Math::Vector3& v1,const Math::Vector3& v2, f32 u, f32 v, bool interior, Math::Vector3& vertex, Math::Vector3& intersection, float distance);
    bool AddHitTriangleClosestPoint(const Math::Vector3& v0,const Math::Vector3& v1,const Math::Vector3& v2, const Math::Vector3& point);
    bool AddHitBox(const Math::AlignedBox& box, Math::Vector3& intersection);
  };
  
  class FrustumPickVisitor : virtual public PickVisitor
  {
  protected:
    Math::Frustum m_PickSpaceFrustum;
    Math::Frustum m_WorldSpaceFrustum;

  public:
    FrustumPickVisitor(const Editor::Camera* camera, const int pixelX, const int pixelY, const float pixelBoxSize = -1.0f);
    FrustumPickVisitor(const Editor::Camera* camera, const Math::Frustum& worldSpaceFrustum);

  protected:
    FrustumPickVisitor(const FrustumPickVisitor& rhs) : PickVisitor (rhs) { }

  public:
    virtual const PickTypes::PickType GetPickType() const HELIUM_OVERRIDE
    {
      return PickTypes::Frustum;
    }

    const Math::Frustum& GetPickSpaceFrustum() const
    {
      return m_PickSpaceFrustum;
    }

    const Math::Frustum& GetWorldSpaceFrustum() const
    {
      return m_WorldSpaceFrustum;
    }

    virtual void Transform() HELIUM_OVERRIDE;

    // picking functions (produce hits)
    virtual bool PickPoint(const Math::Vector3& p, const float err = Math::LinearIntersectionError) HELIUM_OVERRIDE;
    virtual bool PickSegment(const Math::Vector3& p1,const Math::Vector3& p2, const float err = Math::LinearIntersectionError) HELIUM_OVERRIDE;
    virtual bool PickTriangle(const Math::Vector3& v0,const Math::Vector3& v1,const Math::Vector3& v2, const float err = Math::LinearIntersectionError) HELIUM_OVERRIDE;
    virtual bool PickSphere(const Math::Vector3& center, const float radius) HELIUM_OVERRIDE;
    virtual bool PickBox(const Math::AlignedBox& box) HELIUM_OVERRIDE;

    // testing functions (no hits)
    virtual bool IntersectsBox(const Math::AlignedBox& box) const HELIUM_OVERRIDE;

  protected:
    // hit adding functions
    bool AddHitPoint(const Math::Vector3& p);
    bool AddHitSegment(const Math::Vector3& p1,const Math::Vector3& p2);
    bool AddHitTriangle(const Math::Vector3& v0,const Math::Vector3& v1,const Math::Vector3& v2);
    bool AddHitSphere(const Math::Vector3& center);
    bool AddHitBox(const Math::AlignedBox& box);
  };

  class FrustumLinePickVisitor : virtual public LinePickVisitor, virtual public FrustumPickVisitor
  {
  public:
    FrustumLinePickVisitor(const Editor::Camera* camera, const int pixelX, const int pixelY, const float pixelBoxSize = -1.0f);
    FrustumLinePickVisitor(const Editor::Camera* camera, const Math::Line& line, const Math::Frustum& worldSpaceFrustum); 

  protected:
    FrustumLinePickVisitor(const FrustumLinePickVisitor& rhs) : PickVisitor (rhs), LinePickVisitor (rhs), FrustumPickVisitor(rhs) { }

  public:
    virtual const PickTypes::PickType GetPickType() const HELIUM_OVERRIDE
    {
      return PickTypes::Frustum;
    }

    virtual void Transform() HELIUM_OVERRIDE;

    // picking functions (produce hits)
    virtual bool PickPoint(const Math::Vector3& p, const float err = Math::LinearIntersectionError) HELIUM_OVERRIDE;
    virtual bool PickSegment(const Math::Vector3& p1,const Math::Vector3& p2, const float err = Math::LinearIntersectionError) HELIUM_OVERRIDE;
    virtual bool PickTriangle(const Math::Vector3& v0,const Math::Vector3& v1,const Math::Vector3& v2, const float err = Math::LinearIntersectionError) HELIUM_OVERRIDE;
    virtual bool PickSphere(const Math::Vector3& center, const float radius) HELIUM_OVERRIDE;
    virtual bool PickBox(const Math::AlignedBox& box) HELIUM_OVERRIDE;

    // testing functions (no hits)
    virtual bool IntersectsBox(const Math::AlignedBox& box) const HELIUM_OVERRIDE;
  };


  //
  // PickHit encapsulates a hit of a pick with an object
  //

  namespace PickSortTypes
  {
    enum PickSortType
    {
      Intersection, // General intersection... allows for slop... nearest to camera the better
      Surface,      // Must intersect a surface
      Vertex,       // Find the nearest vertex
    };
  }

  typedef PickSortTypes::PickSortType PickSortType;

  class PickHit : public Helium::RefCountBase<PickHit>
  {
    //
    // Object hit
    //

  private:
    Object* m_Object;

  public:
    Object* GetObject() const
    {
      return m_Object;
    }

    void SetObject(Object* object)
    {
      m_Object = object;
    }


    //
    // Normal at intersection
    //

  private:
    bool m_HasNormal;
    Math::Vector3 m_Normal;

  public:
    bool HasNormal() const
    {
      return m_HasNormal;
    }

    Math::Vector3 GetNormal() const
    {
      return m_Normal;
    }

    void SetNormal(const Math::Vector3& value)
    {
      m_Normal = value;
      m_HasNormal = true;
    }


    //
    // Vertex nearest intersection
    //

  private:
    bool m_HasVertex;
    f32 m_VertexDistance;
    Math::Vector3 m_Vertex;

  public:
    bool HasVertex() const
    {
      return m_HasVertex;
    }

    f32 GetVertexDistance() const
    {
      return m_VertexDistance;
    }

    const Math::Vector3& GetVertex() const
    {
      HELIUM_ASSERT(m_HasVertex);
      return m_Vertex;
    }

    void SetVertex(const Math::Vector3& value, f32 distance = Math::BigFloat)
    {
      m_HasVertex = true;
      m_VertexDistance = distance;
      m_Vertex = value;
    }


    //
    // Location of intersection
    //

  private:
    bool m_HasIntersection;
    f32 m_IntersectionDistance;
    Math::Vector3 m_Intersection;

  public:
    bool HasIntersection() const
    {
      return m_HasIntersection;
    }

    f32 GetIntersectionDistance() const
    {
      return m_IntersectionDistance;
    }

    Math::Vector3 GetIntersection() const
    {
      HELIUM_ASSERT(m_HasIntersection);
      return m_Intersection;
    }

    void SetIntersection(const Math::Vector3& value, f32 distance = Math::BigFloat)
    {
      m_Intersection = value;
      m_IntersectionDistance = distance;
      m_HasIntersection = true;
    }


    //
    // Implementation
    //

    PickHit(Object* o)
      : m_Object (o)
      , m_HasNormal (false)
      , m_HasVertex (false)
      , m_VertexDistance (Math::BigFloat)
      , m_HasIntersection (false)
      , m_IntersectionDistance (Math::BigFloat)
    {

    }

    static void Sort(Editor::Camera* camera, const V_PickHitSmartPtr& hits, V_PickHitSmartPtr& sorted, PickSortType sortType);
  };
}
