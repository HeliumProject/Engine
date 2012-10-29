#pragma once

#include "Math/API.h"
#include "Math/FpuMatrix3.h"

namespace Helium
{
    ////////////////////////////////////////////////////////////////////////////////////////////////
    //
    //  CalculateBounds.H
    //
    //  written by: Rob Wyatt
    //
    //  Calcualtes various bounding volumes from a vertex point cloud
    //
    ////////////////////////////////////////////////////////////////////////////////////////////////

    class HELIUM_MATH_API BoundingVolumeGenerator
    {
    public:
        ////////////////////////////////////////////////////////////////////////////////////////////////
        //
        //  Method used to genrate bounding volume
        //
        ////////////////////////////////////////////////////////////////////////////////////////////////
        enum VolumeGenerateMethod
        {
            DEFAULT = -1,
            PRINCIPAL_AXIS = 0,
            BSPHERE_QUICK,
            BSPHERE_OPTIMIZED,
            NUM_METHODS
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////
        //
        //  Axis Aligned Bounding Box structure
        //  The extents within this structure are the half space extents from the center point.
        //
        ////////////////////////////////////////////////////////////////////////////////////////////////
        struct AABB
        {
            Vector3   m_Center;
            Vector3   m_Extents;    // extents are half the real extents as they are from the center point
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////
        //
        //  Orientated Bounding Box Structure
        //  The Axis vectors are the direction and Length of the box, like the AABB the axis are the half
        //  space Lengths from the center point.
        //
        ////////////////////////////////////////////////////////////////////////////////////////////////
        struct OBB
        {
            Vector3   m_Center;
            Vector3   m_Axis[3];
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////
        //
        //  Bounding Sphere Structure
        //
        ////////////////////////////////////////////////////////////////////////////////////////////////
        struct BSphere
        {
            Vector3   m_Center;
            float32_t     	m_Radius;
        };

        ////////////////////////////////////////////////////////////////////////////////////////////////
        //
        //  BoundingVolumeGenerator()
        //
        //  written by: Rob Wyatt
        //
        //  Contructor to which you pass the array of points and the count.
        //  If the point cloud is modified after this constructor is run it is most efficient to
        //  destroy the class and recreate it with the new vertex set.
        //
        ////////////////////////////////////////////////////////////////////////////////////////////////
        BoundingVolumeGenerator(Vector3* points, int32_t point_count, VolumeGenerateMethod method = BoundingVolumeGenerator::DEFAULT);
        BoundingVolumeGenerator(BSphere* bspheres, int32_t bsphere_count);
        ~BoundingVolumeGenerator();


        AABB    GetAABB();
        OBB     GetPrincipleAxisOBB();
        BSphere GetPrincipleAxisBoundingSphere();

    private:
        void CalculateSystem();
        void CalculateEigenSystem();

        int32_t       m_PointCnt;
        Vector3*  m_Points;

        Vector3*  m_BsphereCenters;
        float32_t*    	m_BsphereRads;

        Vector3   m_Mean;
        Matrix3   m_Covariant;
        Vector3   m_EigenValues;
        Matrix3   m_EigenVectors;   // R,S,T

        //-----
        VolumeGenerateMethod    m_volumeGenerationMethod;    // 
        V_Vector3               m_PointList;                 //  VolumeGenerateMethod::BSPHERE_OPTIMIZED
        Vector3                 m_Center;
        float32_t                     m_RadSqr;

        void    CalculateSystemMethod2(void);

        bool    SphereInside          (Vector3 &v);

        void    SphereInit            (void);
        void    SphereInit            (Vector3 &v);
        void    SphereInit            (Vector3 &v0, Vector3 &v1);
        void    SphereInit            (Vector3 &v0, Vector3 &v1, Vector3 &v2);
        void    SphereInit            (Vector3 &v0, Vector3 &v1, Vector3 &v2, Vector3 &v3);
        void    SphereInitSafe        (Vector3 &v0, Vector3 &v1, Vector3 &v2);

        void    MiniSphere            (int32_t c, Vector3 &v0, Vector3 &v1, Vector3 &v2);
        void    MiniSphere            (int32_t c, Vector3 &v0, Vector3 &v1);
        void    MiniSphere            (int32_t c, Vector3 &v);
        void    MiniSphere            (void);

        void    AverageSphere         (void);

        void    UpdateMethod2         (void);
    };
}