#pragma once

#include "Pipeline/API.h"

#include "Math/FpuLine.h"
#include "Math/FpuFrustum.h"

#include "Foundation/Memory/SmartPtr.h"

#include "Foundation/Reflect/Object.h"
#include "Pipeline/SceneGraph/Visitor.h"

namespace Helium
{
    namespace SceneGraph
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
            uint32_t m_Flags;

            // required for construction
            const SceneGraph::Camera* m_Camera;

            // the results of the picking
            V_PickHitSmartPtr m_PickHits;

            // the object that will be associated with the hits generated in Pick() functions
            Reflect::Object* m_CurrentObject;

            // matrices to map to and from world space and local space 
            //  - testing is performed in local space
            //  - results are stored in global space)
            Matrix4 m_CurrentWorldTransform;
            Matrix4 m_CurrentInverseWorldTransform;

        public: 
            PickVisitor(const SceneGraph::Camera* camera);

        protected:
            PickVisitor(const PickVisitor& rhs) { }

        public:
            virtual const PickTypes::PickType GetPickType() const = 0;

            uint32_t GetFlags() const
            {
                return m_Flags;
            }

            bool HasFlags(uint32_t flags) const
            {
                return (m_Flags & flags) != 0;
            }

            void SetFlags(uint32_t flags)
            {
                m_Flags = flags;
            }

            void SetFlag(uint32_t flag, bool value)
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

            const SceneGraph::Camera* GetCamera() const
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

            void SetCurrentObject(Reflect::Object* object)
            {
                m_CurrentObject = object;
            }

            void SetCurrentObject(Reflect::Object* object, const Matrix4& worldSpaceTransform)
            {
                m_CurrentObject = object;
                m_CurrentWorldTransform = worldSpaceTransform;
                m_CurrentInverseWorldTransform = worldSpaceTransform.Inverted();

                // working in local space
                Transform(); 
            }

            void SetCurrentObject(Reflect::Object* object, const Matrix4& worldSpaceTransform, const Matrix4& inverseWorldSpaceTransform)
            {
                m_CurrentObject = object;
                m_CurrentWorldTransform = worldSpaceTransform;
                m_CurrentInverseWorldTransform = inverseWorldSpaceTransform;

                // working in local space
                Transform(); 
            }

            virtual void Transform() = 0;

            // picking functions (produce hits)
            virtual bool PickPoint(const Vector3& p, const float err = HELIUM_LINEAR_INTERSECTION_ERROR) = 0;
            virtual bool PickSegment(const Vector3& p1,const Vector3& p2, const float err = HELIUM_LINEAR_INTERSECTION_ERROR) = 0;
            virtual bool PickTriangle(const Vector3& v0,const Vector3& v1,const Vector3& v2, const float err = HELIUM_LINEAR_INTERSECTION_ERROR) = 0;
            virtual bool PickSphere(const Vector3& center, const float radius) = 0;
            virtual bool PickBox(const AlignedBox& box) = 0;

            // testing functions (no hits)
            virtual bool IntersectsBox(const AlignedBox& box) const = 0;
        };

        class LinePickVisitor : virtual public PickVisitor
        {
        protected:
            Line m_PickSpaceLine;
            Line m_WorldSpaceLine;

        public:
            LinePickVisitor(const SceneGraph::Camera* camera, const int x, const int y);
            LinePickVisitor(const SceneGraph::Camera* camera, const Line& line);

        protected:
            LinePickVisitor(const LinePickVisitor& rhs) : PickVisitor (rhs) { }

        public:
            virtual const PickTypes::PickType GetPickType() const HELIUM_OVERRIDE
            {
                return PickTypes::Line;
            }

            const Line& GetPickSpaceLine() const
            {
                return m_PickSpaceLine;
            }

            const Line& GetWorldSpaceLine() const
            {
                return m_WorldSpaceLine;
            }

            virtual void Transform() HELIUM_OVERRIDE;

            // picking functions (produce hits)
            virtual bool PickPoint(const Vector3& p, const float err = HELIUM_LINEAR_INTERSECTION_ERROR) HELIUM_OVERRIDE;
            virtual bool PickSegment(const Vector3& p1,const Vector3& p2, const float err = HELIUM_LINEAR_INTERSECTION_ERROR) HELIUM_OVERRIDE;
            virtual bool PickTriangle(const Vector3& v0,const Vector3& v1,const Vector3& v2, const float err = HELIUM_LINEAR_INTERSECTION_ERROR) HELIUM_OVERRIDE;
            virtual bool PickSphere(const Vector3& center, const float radius) HELIUM_OVERRIDE;
            virtual bool PickBox(const AlignedBox& box) HELIUM_OVERRIDE;

            // testing functions (no hits)
            virtual bool IntersectsBox(const AlignedBox& box) const HELIUM_OVERRIDE;

        protected:
            // hit adding functions
            bool AddHitPoint(const Vector3& p, Vector3& offset);
            bool AddHitSegment(const Vector3& p1,const Vector3& p2, float32_t mu, Vector3& offset);
            bool AddHitTriangle(const Vector3& v0,const Vector3& v1,const Vector3& v2, float32_t u, float32_t v, bool interior, Vector3& vertex, Vector3& intersection, float distance);
            bool AddHitTriangleClosestPoint(const Vector3& v0,const Vector3& v1,const Vector3& v2, const Vector3& point);
            bool AddHitBox(const AlignedBox& box, Vector3& intersection);
        };

        class FrustumPickVisitor : virtual public PickVisitor
        {
        protected:
            Frustum m_PickSpaceFrustum;
            Frustum m_WorldSpaceFrustum;

        public:
            FrustumPickVisitor(const SceneGraph::Camera* camera, const int pixelX, const int pixelY, const float pixelBoxSize = -1.0f);
            FrustumPickVisitor(const SceneGraph::Camera* camera, const Frustum& worldSpaceFrustum);

        protected:
            FrustumPickVisitor(const FrustumPickVisitor& rhs) : PickVisitor (rhs) { }

        public:
            virtual const PickTypes::PickType GetPickType() const HELIUM_OVERRIDE
            {
                return PickTypes::Frustum;
            }

            const Frustum& GetPickSpaceFrustum() const
            {
                return m_PickSpaceFrustum;
            }

            const Frustum& GetWorldSpaceFrustum() const
            {
                return m_WorldSpaceFrustum;
            }

            virtual void Transform() HELIUM_OVERRIDE;

            // picking functions (produce hits)
            virtual bool PickPoint(const Vector3& p, const float err = HELIUM_LINEAR_INTERSECTION_ERROR) HELIUM_OVERRIDE;
            virtual bool PickSegment(const Vector3& p1,const Vector3& p2, const float err = HELIUM_LINEAR_INTERSECTION_ERROR) HELIUM_OVERRIDE;
            virtual bool PickTriangle(const Vector3& v0,const Vector3& v1,const Vector3& v2, const float err = HELIUM_LINEAR_INTERSECTION_ERROR) HELIUM_OVERRIDE;
            virtual bool PickSphere(const Vector3& center, const float radius) HELIUM_OVERRIDE;
            virtual bool PickBox(const AlignedBox& box) HELIUM_OVERRIDE;

            // testing functions (no hits)
            virtual bool IntersectsBox(const AlignedBox& box) const HELIUM_OVERRIDE;

        protected:
            // hit adding functions
            bool AddHitPoint(const Vector3& p);
            bool AddHitSegment(const Vector3& p1,const Vector3& p2);
            bool AddHitTriangle(const Vector3& v0,const Vector3& v1,const Vector3& v2);
            bool AddHitSphere(const Vector3& center);
            bool AddHitBox(const AlignedBox& box);
        };

        class FrustumLinePickVisitor : virtual public LinePickVisitor, virtual public FrustumPickVisitor
        {
        public:
            FrustumLinePickVisitor(const SceneGraph::Camera* camera, const int pixelX, const int pixelY, const float pixelBoxSize = -1.0f);
            FrustumLinePickVisitor(const SceneGraph::Camera* camera, const Line& line, const Frustum& worldSpaceFrustum); 

        protected:
            FrustumLinePickVisitor(const FrustumLinePickVisitor& rhs) : PickVisitor (rhs), LinePickVisitor (rhs), FrustumPickVisitor(rhs) { }

        public:
            virtual const PickTypes::PickType GetPickType() const HELIUM_OVERRIDE
            {
                return PickTypes::Frustum;
            }

            virtual void Transform() HELIUM_OVERRIDE;

            // picking functions (produce hits)
            virtual bool PickPoint(const Vector3& p, const float err = HELIUM_LINEAR_INTERSECTION_ERROR) HELIUM_OVERRIDE;
            virtual bool PickSegment(const Vector3& p1,const Vector3& p2, const float err = HELIUM_LINEAR_INTERSECTION_ERROR) HELIUM_OVERRIDE;
            virtual bool PickTriangle(const Vector3& v0,const Vector3& v1,const Vector3& v2, const float err = HELIUM_LINEAR_INTERSECTION_ERROR) HELIUM_OVERRIDE;
            virtual bool PickSphere(const Vector3& center, const float radius) HELIUM_OVERRIDE;
            virtual bool PickBox(const AlignedBox& box) HELIUM_OVERRIDE;

            // testing functions (no hits)
            virtual bool IntersectsBox(const AlignedBox& box) const HELIUM_OVERRIDE;
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
            Reflect::Object* m_HitObject;

        public:
            Reflect::Object* GetHitObject() const
            {
                return m_HitObject;
            }

            void SetHitObject(Reflect::Object* object)
            {
                m_HitObject = object;
            }


            //
            // Normal at intersection
            //

        private:
            bool m_HasNormal;
            Vector3 m_Normal;

        public:
            bool HasNormal() const
            {
                return m_HasNormal;
            }

            Vector3 GetNormal() const
            {
                return m_Normal;
            }

            void SetNormal(const Vector3& value)
            {
                m_Normal = value;
                m_HasNormal = true;
            }


            //
            // Vertex nearest intersection
            //

        private:
            bool m_HasVertex;
            float32_t m_VertexDistance;
            Vector3 m_Vertex;

        public:
            bool HasVertex() const
            {
                return m_HasVertex;
            }

            float32_t GetVertexDistance() const
            {
                return m_VertexDistance;
            }

            const Vector3& GetVertex() const
            {
                HELIUM_ASSERT(m_HasVertex);
                return m_Vertex;
            }

            void SetVertex(const Vector3& value, float32_t distance = FLT_MAX)
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
            float32_t m_IntersectionDistance;
            Vector3 m_Intersection;

        public:
            bool HasIntersection() const
            {
                return m_HasIntersection;
            }

            float32_t GetIntersectionDistance() const
            {
                return m_IntersectionDistance;
            }

            Vector3 GetIntersection() const
            {
                HELIUM_ASSERT(m_HasIntersection);
                return m_Intersection;
            }

            void SetIntersection(const Vector3& value, float32_t distance = FLT_MAX)
            {
                m_Intersection = value;
                m_IntersectionDistance = distance;
                m_HasIntersection = true;
            }


            //
            // Implementation
            //

            PickHit(Reflect::Object* o)
                : m_HitObject (o)
                , m_HasNormal (false)
                , m_HasVertex (false)
                , m_VertexDistance ((float)FLT_MAX)
                , m_HasIntersection (false)
                , m_IntersectionDistance ((float)FLT_MAX)
            {

            }

            static void Sort(SceneGraph::Camera* camera, const V_PickHitSmartPtr& hits, V_PickHitSmartPtr& sorted, PickSortType sortType);
        };
    }
}