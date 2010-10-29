////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CalculateBounds.cpp
//
//  written by: Rob Wyatt
//
//  Calcualtes various bounding volumes from a vertex point cloud
//
////////////////////////////////////////////////////////////////////////////////////////////////
#include "CalculateBounds.h"
#include "Macros.h"

using namespace Helium;

static const f32 epsilon = 1.0e-10F;
static const i32 sweeps = 32;


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
BoundingVolumeGenerator::BoundingVolumeGenerator(Vector3* points, i32 point_count, VolumeGenerateMethod method )
{
    m_volumeGenerationMethod  = method;
    m_PointCnt        = point_count;
    m_Points          = points;
    m_BsphereCenters  = NULL;
    m_BsphereRads     = NULL;

    switch( method )
    {
    case DEFAULT:
    case PRINCIPAL_AXIS:
        CalculateSystem();
        break;
    case BSPHERE_QUICK:
        AverageSphere();
        break;
    case BSPHERE_OPTIMIZED:
        CalculateSystemMethod2();
        break;
    }
}

////////////////////////////////////////////////////////////////////////////////////////////////
BoundingVolumeGenerator::BoundingVolumeGenerator(BSphere* bspheres, i32 bsphere_count)
{

    m_volumeGenerationMethod = BoundingVolumeGenerator::DEFAULT;
    m_BsphereCenters  = new Vector3 [bsphere_count];
    m_BsphereRads     = new f32 [bsphere_count];
    m_PointCnt        = bsphere_count;
    m_Points          = m_BsphereCenters;

    for(i32 i = 0; i < bsphere_count; i++)
    {
        m_BsphereCenters[i] = bspheres[i].m_Center;
        m_BsphereRads[i]    = bspheres[i].m_Radius;
    }

    CalculateSystem();
}



////////////////////////////////////////////////////////////////////////////////////////////////
//
//  ~BoundingVolumeGenerator()
//
//  written by: Rob Wyatt
//
//  Destroys the current volume class
//
////////////////////////////////////////////////////////////////////////////////////////////////
BoundingVolumeGenerator::~BoundingVolumeGenerator()
{
    delete [] m_BsphereCenters;
    m_BsphereCenters = NULL;

    delete [] m_BsphereRads;
    m_BsphereRads = NULL;
}



////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CalculateSystem()
//
//  written by: Rob Wyatt
//
//  PCA of the point cloud gives us the principle axis of the space of the point cloud, this
//  info is used by most of the volume generators so this function is called from the constructor.
//
////////////////////////////////////////////////////////////////////////////////////////////////
void BoundingVolumeGenerator::CalculateSystem()
{
    if (m_PointCnt==0)
        return;

    // First Calculate the average Position
    m_Mean = Vector3(0,0,0);
    for (i32 i=0;i<m_PointCnt;i++)
    {
        m_Mean+=m_Points[i];
    }
    m_Mean/=(f32)m_PointCnt;

    // Calculate the covariance matrix  
    m_Covariant = Matrix3(Vector3 (0, 0, 0), Vector3 (0, 0, 0), Vector3 (0, 0, 0));
    for (i32 i=0;i<m_PointCnt;i++)
    {
        m_Covariant[0].x += (m_Points[i].x - m_Mean.x)*(m_Points[i].x - m_Mean.x);
        m_Covariant[1].y += (m_Points[i].y - m_Mean.y)*(m_Points[i].y - m_Mean.y);
        m_Covariant[2].z += (m_Points[i].z - m_Mean.z)*(m_Points[i].z - m_Mean.z);

        m_Covariant[0].y += (m_Points[i].x - m_Mean.x)*(m_Points[i].y-m_Mean.y);
        m_Covariant[0].z += (m_Points[i].x - m_Mean.x)*(m_Points[i].z-m_Mean.z);
        m_Covariant[1].z += (m_Points[i].y - m_Mean.y)*(m_Points[i].z-m_Mean.z);
    }

    // covariance matrix is symmetric so copy the elements
    m_Covariant[1].x = m_Covariant[0].y;
    m_Covariant[2].x = m_Covariant[0].z;
    m_Covariant[2].y = m_Covariant[1].z;

    // divide the matrix by the point count
    m_Covariant/=(f32)m_PointCnt;

    CalculateEigenSystem();
}



////////////////////////////////////////////////////////////////////////////////////////////////
//
//  CalculateEigenSystem()
//
//  written by: Rob Wyatt
//
//  Calculates the Eigenvalues and Eigenvectors for the covariant matrix of the point cloud, the
//  Eigenvectors are sorted into order based on their respective Eigenvalue to give the 3 component
//  axis of the point cloud.
//
////////////////////////////////////////////////////////////////////////////////////////////////
void BoundingVolumeGenerator::CalculateEigenSystem()
{
    f32 m11 = m_Covariant(0,0);
    f32 m12 = m_Covariant(0,1);
    f32 m13 = m_Covariant(0,2);
    f32 m22 = m_Covariant(1,1);
    f32 m23 = m_Covariant(1,2);
    f32 m33 = m_Covariant(2,2);

    m_EigenVectors = Matrix3::Identity;

    for (i32 a = 0; a < sweeps; a++)
    {
        // stop if off-diagonal entries small enough
        if ((fabs(m12) < epsilon) && (fabs(m13) < epsilon) &&
            (fabs(m23) < epsilon)) break;

        if (m12 != 0.0F)
        {
            f32 u = (m22 - m11) * 0.5F / m12;
            f32 u2 = u * u;
            f32 u2p1 = u2 + 1.0F;
            f32 t = (u2p1 != u2) ?
                ((u < 0.0F) ? -1.0F : 1.0F) * (sqrt(u2p1) - fabs(u)) : 0.5F / u;
            f32 c = 1.0F / sqrt(t * t + 1.0F);
            f32 s = c * t;

            m11 -= t * m12;
            m22 += t * m12;
            m12 = 0.0F;

            f32 temp = c * m13 - s * m23;
            m23 = s * m13 + c * m23;
            m13 = temp;

            for (i32 i = 0; i < 3; i++)
            {
                f32 temp = c * m_EigenVectors(i,0) - s * m_EigenVectors(i,1);
                m_EigenVectors(i,1) = s * m_EigenVectors(i,0) + c * m_EigenVectors(i,1);
                m_EigenVectors(i,0) = temp;
            }
        }

        if (m13 != 0.0F)
        {
            f32 u = (m33 - m11) * 0.5F / m13;
            f32 u2 = u * u;
            f32 u2p1 = u2 + 1.0F;
            f32 t = (u2p1 != u2) ?
                ((u < 0.0F) ? -1.0F : 1.0F) * (sqrt(u2p1) - fabs(u)) : 0.5F / u;
            f32 c = 1.0F / sqrt(t * t + 1.0F);
            f32 s = c * t;

            m11 -= t * m13;
            m33 += t * m13;
            m13 = 0.0F;

            f32 temp = c * m12 - s * m23;
            m23 = s * m12 + c * m23;
            m12 = temp;

            for (i32 i = 0; i < 3; i++)
            {
                f32 temp = c * m_EigenVectors(i,0) - s * m_EigenVectors(i,2);
                m_EigenVectors(i,2) = s * m_EigenVectors(i,0) + c * m_EigenVectors(i,2);
                m_EigenVectors(i,0) = temp;
            }
        }

        if (m23 != 0.0F)
        {
            f32 u = (m33 - m22) * 0.5F / m23;
            f32 u2 = u * u;
            f32 u2p1 = u2 + 1.0F;
            f32 t = (u2p1 != u2) ?
                ((u < 0.0F) ? -1.0F : 1.0F) * (sqrt(u2p1) - fabs(u)) : 0.5F / u;
            f32 c = 1.0F / sqrt(t * t + 1.0F);
            f32 s = c * t;

            m22 -= t * m23;
            m33 += t * m23;
            m23 = 0.0F;

            f32 temp = c * m12 - s * m13;
            m13 = s * m12 + c * m13;
            m12 = temp;

            for (i32 i = 0; i < 3; i++)
            {
                f32 temp = c * m_EigenVectors(i,1) - s * m_EigenVectors(i,2);
                m_EigenVectors(i,2) = s * m_EigenVectors(i,1) + c * m_EigenVectors(i,2);
                m_EigenVectors(i,1) = temp;
            }
        }
    }

    m_EigenValues[0] = m11;
    m_EigenValues[1] = m22;
    m_EigenValues[2] = m33;
}



////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetPrincipleAxisOBB()
//
//  written by: Rob Wyatt
//
//  Calculates a very tight fitting orientated bounding box based on PCA of the vertex point cloud.
//  The result is a center point and 3 axis, the Length of the axis is only half of the volume
//  extents, to get the full extents you need to perform center point +/- extents.
//
////////////////////////////////////////////////////////////////////////////////////////////////
BoundingVolumeGenerator::OBB BoundingVolumeGenerator::GetPrincipleAxisOBB()
{
    // not supported for bspheres yet
    HELIUM_ASSERT(m_BsphereCenters == NULL);

    OBB result;

    // To calculate the bounding box we need the extents of the point
    // cloud in the PCA axis, this gives us the bounds of the bounding
    // box. From this we can calculate the center of the bounding box
    // which is all the info we need.
    f32 mina,maxa;
    f32 minb,maxb;
    f32 minc,maxc;
    mina = maxa = m_Points[0].Dot(m_EigenVectors[0]);
    minb = maxb = m_Points[0].Dot(m_EigenVectors[1]);
    minc = maxc = m_Points[0].Dot(m_EigenVectors[2]);

    for (i32 i=1;i<m_PointCnt;i++)
    {
        f32 a = m_Points[i].Dot(m_EigenVectors[0]);
        mina = MIN(mina,a);
        maxa = MAX(maxa,a);

        f32 b = m_Points[i].Dot(m_EigenVectors[1]);
        minb = MIN(minb,b);
        maxb = MAX(maxb,b);

        f32 c = m_Points[i].Dot(m_EigenVectors[2]);
        minc = MIN(minc,c);
        maxc = MAX(maxc,c);
    }

    // calculate the average extents to get the center point
    f32 a = (mina+maxa)/2.0f;
    f32 b = (minb+maxb)/2.0f;
    f32 c = (minc+maxc)/2.0f;
    result.m_Center = (m_EigenVectors[0]*a) + (m_EigenVectors[1]*b) + (m_EigenVectors[2]*c);

    // calculate the half of the total extent for the cube axis
    a = (maxa-mina)/2.0f;
    b = (maxb-minb)/2.0f;
    c = (maxc-minc)/2.0f;
    result.m_Axis[0] = m_EigenVectors[0]*a;
    result.m_Axis[1] = m_EigenVectors[1]*b;
    result.m_Axis[2] = m_EigenVectors[2]*c;

    return result;
}



////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetPrincipleAxisBoundingSphere()
//
//  written by: Rob Wyatt
//
//  Generates the PCA bounding sphere, only the principle axis is used. This gives a very good
//  bounding sphere but it is not perfect, however this can handle very large numbers of vertices
//  as the execution time is linear with the number of vertices. Most algorithms that generate
//  near optimal spheres have squared execution time with the number of vertices.
//
////////////////////////////////////////////////////////////////////////////////////////////////
BoundingVolumeGenerator::BSphere BoundingVolumeGenerator::GetPrincipleAxisBoundingSphere()
{
    BSphere result;

    if ( (m_volumeGenerationMethod == BSPHERE_OPTIMIZED) || (m_volumeGenerationMethod == BSPHERE_QUICK) )
    {
        result.m_Center = m_Center;
        result.m_Radius = sqrtf(m_RadSqr);
        return result;
    }


    if (m_BsphereCenters == NULL)
    {
        // using the primary priciple axis calculate the extents of the point cloud along that axis
        f32 mina,maxa;
        i32 max_idx = 0;
        i32 min_idx = 0;
        mina = maxa = m_Points[0].Dot(m_EigenVectors[0]);

        for (i32 i=1;i<m_PointCnt;i++)
        {
            f32 a = m_Points[i].Dot(m_EigenVectors[0]);
            if (a<mina)
            {
                mina = a;
                min_idx = i;
            }
            if (a>maxa)
            {
                maxa=a;
                max_idx = i;
            }
        }  

        // the center point is the half way point between the two max extent vertices
        result.m_Center = (m_Points[min_idx]+m_Points[max_idx])/2.0f;

        // the radius is the distance from this point to the max point
        result.m_Radius = (m_Points[max_idx] - result.m_Center).Length();

        // This sphere may not enclose all points, for points that fall inside the current sphere we
        // accept as they are, for points that are outside we try to optimally adjust the sphere
        for (i32 i=0;i<m_PointCnt;i++)
        {
            if ( (m_Points[i] - result.m_Center).LengthSquared()>result.m_Radius*result.m_Radius)
            {
                // this vert is outside of the current bouinding sphere, we calculate the new center of the
                // sphere by placing it on the line from the current center to the new point. We calculate a
                // point on the line where the new sphere and old sphere are tengent to each other (point G)
                // and from this we calculate the new center as half way between point G and the new point.
                // From the new center a new radius is calculated using the position of the new point.
                Vector3 g = result.m_Center - ( (m_Points[i]-result.m_Center).Normalize() * result.m_Radius);
                result.m_Center = (g+m_Points[i])/2.0f;
                result.m_Radius = (m_Points[i] - result.m_Center).Length();
            }
        }
    }
    else
    {
        // using the primary priciple axis calculate the extents of the point cloud along that axis
        Vector3 principal_axis = m_EigenVectors[0];
        f32     mina           =  9999.0f;
        f32     maxa           = -9999.0f;
        Vector3 min_pt( 9999.0f,  9999.0f,  9999.0f);
        Vector3 max_pt(-9999.0f, -9999.0f, -9999.0f);

        for(i32 i = 0; i < m_PointCnt; i++)
        {
            const Vector3&  p  = m_BsphereCenters[i];
            Vector3         d  = (principal_axis * m_BsphereRads[i]);
            Vector3         p1 = (p - d);
            Vector3         p2 = (p + d);
            f32             a1 = p1.Dot(principal_axis);
            f32             a2 = p2.Dot(principal_axis);

            if (a1 < mina) { mina = a1; min_pt = p1; }
            if (a1 > maxa) { maxa = a1; max_pt = p1; }
            if (a2 < mina) { mina = a2; min_pt = p2; }
            if (a2 > maxa) { maxa = a2; max_pt = p2; }
        }

        // center is the half way point between the two max extent vertices
        // radius is the distance from this point to the max point
        result.m_Center = (min_pt + max_pt) / 2.0f;
        result.m_Radius = (max_pt - result.m_Center).Length();

        // This sphere may not enclose all points, for points that fall inside the current sphere we
        // accept as they are, for points that are outside we try to optimally adjust the sphere
        for(i32 i = 0; i < m_PointCnt; i++)
        {
            f32             res_r2  = (result.m_Radius * result.m_Radius);
            const Vector3&  p       = m_BsphereCenters[i];
            f32             rad     = m_BsphereRads[i];
            Vector3         dp      = (p - result.m_Center).Normalize();
            Vector3         pa      = (p - (dp * rad));
            Vector3         pb      = (p + (dp * rad));

            if ((pa - result.m_Center).LengthSquared() > res_r2)
            {
                Vector3 g = result.m_Center - (dp * result.m_Radius);
                result.m_Center = (g + pa)/2.0f;
                result.m_Radius = (pa - result.m_Center).Length();
            }

            if ((pb - result.m_Center).LengthSquared() > res_r2)
            {
                Vector3 g = result.m_Center - (dp * result.m_Radius);
                result.m_Center = (g + pb)/2.0f;
                result.m_Radius = (pb - result.m_Center).Length();
            }
        }
    }

    return result;
}



////////////////////////////////////////////////////////////////////////////////////////////////
//
//  GetAABB()
//
//  written by: Rob Wyatt
//
//  Generates a basic axis aligned bounding volume based on the extents in each axis. The returned
//  volume is the center point and the half extents of the volume.
//
////////////////////////////////////////////////////////////////////////////////////////////////
BoundingVolumeGenerator::AABB BoundingVolumeGenerator::GetAABB()
{
    // not supported for bspheres yet
    HELIUM_ASSERT(m_BsphereCenters == NULL);

    AABB result;

    f32 minx,maxx;
    f32 miny,maxy;
    f32 minz,maxz;
    minx = maxx = m_Points[0].x;
    miny = maxy = m_Points[0].y;
    minz = maxz = m_Points[0].z;

    for (i32 i=1;i<m_PointCnt;i++)
    {
        minx = MIN(minx,m_Points[i].x);
        maxx = MAX(maxx,m_Points[i].x);

        miny = MIN(miny,m_Points[i].y);
        maxy = MAX(maxy,m_Points[i].y);

        minz = MIN(minz,m_Points[i].z);
        maxz = MAX(maxz,m_Points[i].z);
    }

    // center is the middle of the extents
    result.m_Center.x = (minx+maxx)/2.0f;
    result.m_Center.y = (miny+maxy)/2.0f;
    result.m_Center.z = (minz+maxz)/2.0f;

    result.m_Extents.x = (maxx-minx)/2.0f;
    result.m_Extents.y = (maxy-miny)/2.0f;
    result.m_Extents.z = (maxz-minz)/2.0f;

    return result;
}



////////////////////////////////////////////////////////////////////////////////////////////////
//
// al's brute-force bsphere code - somewhat hacked in - it should probably be tidied up
//
////////////////////////////////////////////////////////////////////////////////////////////////


/***********************************************************************************************************************
*  BoundingVolumeGenerator::SphereInside
*   - returns true if 'v' is inside the sphere
***********************************************************************************************************************/
bool BoundingVolumeGenerator::SphereInside(Vector3 &v)
{
    if (m_RadSqr >= 0.0)
    {
        f32 lhs = (v - m_Center).LengthSquared();
        f32 rhs = m_RadSqr + (m_RadSqr * 0.000001f);

        return (lhs <= rhs);
    }

    return false;
}


/***********************************************************************************************************************
*  BoundingVolumeGenerator::SphereInit()
*   - inits sphere with no m_PointList
***********************************************************************************************************************/
void BoundingVolumeGenerator::SphereInit(void)
{
    m_Center = Vector3(0.0, 0.0, 0.0);
    m_RadSqr = -1.0;
}


/***********************************************************************************************************************
*  BoundingVolumeGenerator::SphereInit()
*   - inits sphere with 1 vert
***********************************************************************************************************************/
void BoundingVolumeGenerator::SphereInit(Vector3 &v)
{
    m_Center = v;
    m_RadSqr = 0.0;
}


/***********************************************************************************************************************
*  BoundingVolumeGenerator::SphereInit()
*   - inits sphere with 2 m_PointList
***********************************************************************************************************************/
void BoundingVolumeGenerator::SphereInit(Vector3 &v0, Vector3 &v1)
{
    m_Center = (v0 + v1) * 0.5;
    m_RadSqr = (v0 - m_Center).LengthSquared();
}


/***********************************************************************************************************************
*  BoundingVolumeGenerator::SphereInit()
*   - inits sphere with 3 m_PointList
***********************************************************************************************************************/
void BoundingVolumeGenerator::SphereInitSafe(Vector3 &v0, Vector3 &v1, Vector3 &v2)
{
    // compute aabb around points and derive bsphere from this
    f32 min_x = MIN( MIN( v0.x, v1.x ), v2.x );
    f32 min_y = MIN( MIN( v0.y, v1.y ), v2.y );
    f32 min_z = MIN( MIN( v0.z, v1.z ), v2.z );
    f32 max_x = MAX( MAX( v0.x, v1.x ), v2.x );
    f32 max_y = MAX( MAX( v0.y, v1.y ), v2.y );
    f32 max_z = MAX( MAX( v0.z, v1.z ), v2.z );

    m_Center.x = (min_x + max_x) * 0.5f;
    m_Center.y = (min_y + max_y) * 0.5f;
    m_Center.z = (min_z + max_z) * 0.5f;

    m_RadSqr = MAX( (v0 - m_Center).LengthSquared(), (v1 - m_Center).LengthSquared() );
    m_RadSqr = MAX( (v2 - m_Center).LengthSquared(), m_RadSqr );
}


/***********************************************************************************************************************
*  BoundingVolumeGenerator::SphereInit()
*   - inits sphere with 3 m_PointList
***********************************************************************************************************************/
void BoundingVolumeGenerator::SphereInit(Vector3 &v0, Vector3 &v1, Vector3 &v2)
{
    // The intersection of two lines defines the m_Center of the sphere. 
    // The two lines are: 
    // the line that passes through the point midway between vrta & vrtb, and perpendicular to line segment vrta to vrtb. 
    // the line that passes through the point midway between vrta & vrtc, and perpendicular to line segment vrta to vrtc. 
    // Both lines line in the plane that contains {vrta}, {vrtb}, and {vrtc}. 

    double v0x = v0.x;  double v0y = v0.y;  double v0z = v0.z;  
    double v1x = v1.x;  double v1y = v1.y;  double v1z = v1.z;  
    double v2x = v2.x;  double v2y = v2.y;  double v2z = v2.z;  

    // normalized vector from a to b
    // Vector3 vab = v1 - v0;
    double vabx = v1x - v0x;
    double vaby = v1y - v0y;
    double vabz = v1z - v0z;
    double vabinvlen = 1.0 / sqrt( (vabx * vabx) + (vaby * vaby) + (vabz * vabz) );
    vabx *= vabinvlen;
    vaby *= vabinvlen;
    vabz *= vabinvlen;

    // normalized vector from a to c
    // Vector3 vac = v2 - v0;
    double vacx = v2x - v0x;
    double vacy = v2y - v0y;
    double vacz = v2z - v0z;
    double vacinvlen = 1.0 / sqrt( (vacx * vacx) + (vacy * vacy) + (vacz * vacz) );
    vacx *= vacinvlen;
    vacy *= vacinvlen;
    vacz *= vacinvlen;

    // cross these two vectors
    // Vector3 n = vab.Cross(vac);
    double nx = vaby * vacz - vabz * vacy;
    double ny = vabz * vacx - vabx * vacz;
    double nz = vabx * vacy - vaby * vacx;

    double nlensqr = (nx * nx) + (ny * ny) + (nz * nz);
    if (nlensqr < 1.0e-5)
    {
        return SphereInitSafe(v0, v1, v2);
    }

    // n.Normalize();
    double ninvlen = 1.0 / sqrt( nlensqr );
    nx *= ninvlen;
    ny *= ninvlen;
    nz *= ninvlen;

    // Vector3 nab = vab.Cross(n);
    // nab.Normalize();                // normal vector to line segment vrta, vrtb
    double nabx = vaby * nz - vabz * ny;
    double naby = vabz * nx - vabx * nz;
    double nabz = vabx * ny - vaby * nx;
    double nabinvlen = 1.0 / sqrt( (nabx * nabx) + (naby * naby) + (nabz * nabz) );
    nabx *= nabinvlen;
    naby *= nabinvlen;
    nabz *= nabinvlen;

    // Vector3 nac = vac.Cross(n);
    // nac.Normalize();                // normal vector to line segment vrta, vrtc
    double nacx = vacy * nz - vacz * ny;
    double nacy = vacz * nx - vacx * nz;
    double nacz = vacx * ny - vacy * nx;
    double nacinvlen = 1.0 / sqrt( (nacx * nacx) + (nacy * nacy) + (nacz * nacz) );
    nacx *= nacinvlen;
    nacy *= nacinvlen;
    nacz *= nacinvlen;

    // Vector3 pab = (v0 + v1) * 0.5;
    double pabx = (v0x + v1x) * 0.5;
    double paby = (v0y + v1y) * 0.5;
    double pabz = (v0z + v1z) * 0.5;

    // Vector3 pac = (v0 + v2) * 0.5;
    double pacx = (v0x + v2x) * 0.5;
    double pacy = (v0y + v2y) * 0.5;
    double pacz = (v0z + v2z) * 0.5;

    double crx = naby * nacz - nabz * nacy;
    double cry = nabz * nacx - nabx * nacz;
    double crz = nabx * nacy - naby * nacx;
    double crzdot = (crx * crx) + (cry * cry) + (crz * crz);
    if (crzdot < 1.0e-5)
    {
        return SphereInitSafe(v0, v1, v2);
    }

    double pdx = pacx - pabx;
    double pdy = pacy - paby;
    double pdz = pacz - pabz;

    double px = pdy * nacz - pdz * nacy;
    double py = pdz * nacx - pdx * nacz;
    double pz = pdx * nacy - pdy * nacx;

    double t = ((px * crx) + (py * cry) + (pz * crz)) / crzdot;

    m_Center.x = f32(pabx + (nabx * t));
    m_Center.y = f32(paby + (naby * t));
    m_Center.z = f32(pabz + (nabz * t));

    m_RadSqr = (v0 - m_Center).LengthSquared();
}


/***********************************************************************************************************************
*  BoundingVolumeGenerator::SphereInit()
*   - inits sphere with 4 m_PointList
***********************************************************************************************************************/
void BoundingVolumeGenerator::SphereInit(Vector3 &v0, Vector3 &v1, Vector3 &v2, Vector3 &v3)
{

    // The intersection of three planes defines the m_Center of the sphere. 
    // The three planes are: 
    // the plane that passes through the point midway between vrta & vrtb, and perpendicular to line segment vrta to vrtb. 
    // the plane that passes through the point midway between vrtb & vrtc, and perpendicular to line segment vrtb to vrtc. 
    // the plane that passes through the point midway between vrtc & vrtd, and perpendicular to line segment vrtc to vrtd. 

    double v0x = v0.x;  double v0y = v0.y;  double v0z = v0.z;  
    double v1x = v1.x;  double v1y = v1.y;  double v1z = v1.z;  
    double v2x = v2.x;  double v2y = v2.y;  double v2z = v2.z;  
    double v3x = v3.x;  double v3y = v3.y;  double v3z = v3.z;  

    // normalized vector from a to b
    //  Vector3 nab = v1 - v0;
    //  nab.Normalize();
    double nabx = v1x - v0x;
    double naby = v1y - v0y;
    double nabz = v1z - v0z;
    double nabinvlen = 1.0 / sqrt( (nabx * nabx) + (naby * naby) + (nabz * nabz) );
    nabx *= nabinvlen;
    naby *= nabinvlen;
    nabz *= nabinvlen;

    // normalized vector from b to c
    //  Vector3 nbc = v2 - v1;
    //  nbc.Normalize();
    double nbcx = v2x - v1x;
    double nbcy = v2y - v1y;
    double nbcz = v2z - v1z;
    double nbcinvlen = 1.0 / sqrt( (nbcx * nbcx) + (nbcy * nbcy) + (nbcz * nbcz) );
    nbcx *= nbcinvlen;
    nbcy *= nbcinvlen;
    nbcz *= nbcinvlen;

    // normalized vector from c to d
    //  Vector3 ncd = v3 - v2;
    //  ncd.Normalize();
    double ncdx = v2x - v1x;
    double ncdy = v2y - v1y;
    double ncdz = v2z - v1z;
    double ncdinvlen = 1.0 / sqrt( (ncdx * ncdx) + (ncdy * ncdy) + (ncdz * ncdz) );
    ncdx *= ncdinvlen;
    ncdy *= ncdinvlen;
    ncdz *= ncdinvlen;

    // intersection plane normal for a->b and b->c
    //  Vector3 n = nab.Cross(nbc);
    double nx = naby * nbcz - nabz * nbcy;
    double ny = nabz * nbcx - nabx * nbcz;
    double nz = nabx * nbcy - naby * nbcx;

    //  f32 d = n.Dot(ncd);
    double d = (nx * ncdx) + (ny * ncdy) + (nz * ncdz);

    if (d < 1.0e-5)
    {
        // compute aabb around points and derive bsphere from this
        f32 min_x = MIN( MIN( MIN( v0.x, v1.x ), v2.x ), v3.x );
        f32 min_y = MIN( MIN( MIN( v0.y, v1.y ), v2.y ), v3.y );
        f32 min_z = MIN( MIN( MIN( v0.z, v1.z ), v2.z ), v3.z );
        f32 max_x = MAX( MAX( MAX( v0.x, v1.x ), v2.x ), v3.x );
        f32 max_y = MAX( MAX( MAX( v0.y, v1.y ), v2.y ), v3.y );
        f32 max_z = MAX( MAX( MAX( v0.z, v1.z ), v2.z ), v3.z );

        m_Center.x = (min_x + max_x) * 0.5f;
        m_Center.y = (min_y + max_y) * 0.5f;
        m_Center.z = (min_z + max_z) * 0.5f;

        m_RadSqr = MAX( (v0 - m_Center).LengthSquared(), (v1 - m_Center).LengthSquared() );
        m_RadSqr = MAX( (v2 - m_Center).LengthSquared(), m_RadSqr );
        m_RadSqr = MAX( (v3 - m_Center).LengthSquared(), m_RadSqr );
        return;
    }

    //  Vector3 pab = (v0 + v1) * 0.5;
    double pabx = (v0x + v1x) * 0.5;
    double paby = (v0y + v1y) * 0.5;
    double pabz = (v0z + v1z) * 0.5;

    //  Vector3 pbc = (v1 + v2) * 0.5;
    double pbcx = (v1x + v2x) * 0.5;
    double pbcy = (v1y + v2y) * 0.5;
    double pbcz = (v1z + v2z) * 0.5;

    //  Vector3 pcd = (v2 + v3) * 0.5;
    double pcdx = (v2x + v3x) * 0.5;
    double pcdy = (v2y + v3y) * 0.5;
    double pcdz = (v2z + v3z) * 0.5;

    //  f32 f;
    double f;
    double cx, cy, cz;

    //  f = pab.Dot(nab);
    f = (pabx * nabx) + (paby * naby) + (pabz * nabz);
    //  n = nbc.Cross(ncd);
    nx = nbcy * ncdz - nbcz * ncdy;
    ny = nbcz * ncdx - nbcx * ncdz;
    nz = nbcx * ncdy - nbcy * ncdx;
    //  m_Center = n * f;
    cx = nx * f;
    cy = ny * f;
    cz = nz * f;

    //  f = pbc.Dot(nbc);
    f = (pbcx * nbcx) + (pbcy * nbcy) + (pbcz * nbcz);
    //  n = ncd.Cross(nab);
    nx = ncdy * nabz - ncdz * naby;
    ny = ncdz * nabx - ncdx * nabz;
    nz = ncdx * naby - ncdy * nabx;
    //  m_Center += n * f;
    cx += nx * f;
    cy += ny * f;
    cz += nz * f;

    //  f = pcd.Dot(ncd);
    f = (pcdx * ncdx) + (pcdy * ncdy) + (pcdz * ncdz);
    //  n = nab.Cross(nbc);
    nx = naby * nbcz - nabz * nbcy;
    ny = nabz * nbcx - nabx * nbcz;
    nz = nabx * nbcy - naby * nbcx;
    //  m_Center += n * f;
    cx += nx * f;
    cy += ny * f;
    cz += nz * f;

    //  m_Center /= d;
    m_Center.x = f32(cx / d);
    m_Center.y = f32(cy / d);
    m_Center.z = f32(cz / d);

    m_RadSqr = (v0 - m_Center).LengthSquared();
}


/***********************************************************************************************************************
*  BoundingVolumeGenerator::MiniSphere()
*   - compute the smallest enclosing sphere for first c m_PointList (v0, v1, v2 must lie on boundary)
***********************************************************************************************************************/
void BoundingVolumeGenerator::MiniSphere(i32 c, Vector3 &v0, Vector3 &v1, Vector3 &v2)
{
    SphereInit(v0, v1, v2);

    for (i32 i = 0; i < c; i++)
    {
        if (!SphereInside(m_PointList[i]))
        {
            SphereInit(m_PointList[i], v0, v1, v2);
        }
    }
}


/***********************************************************************************************************************
*  BoundingVolumeGenerator::MiniSphere()
*   - compute the smallest enclosing sphere for first c m_PointList (v0, v1 must lie on boundary)
***********************************************************************************************************************/
void BoundingVolumeGenerator::MiniSphere(i32 c, Vector3 &v0, Vector3 &v1)
{
    SphereInit(v0, v1);

    for (i32 i = 0; i < c; i++)
    {
        if (!SphereInside(m_PointList[i]))
        {
            MiniSphere(i, m_PointList[i], v0, v1);
        }
    }
}


/***********************************************************************************************************************
*  BoundingVolumeGenerator::MiniSphere()
*   - compute the smallest enclosing sphere for first c m_PointList (v must lie on boundary)
***********************************************************************************************************************/
void BoundingVolumeGenerator::MiniSphere(i32 c, Vector3 &v)
{
    if (c >= 1)
    {
        SphereInit(v, m_PointList[0]);
        for (i32 i = 1; i < c; i++)
        {
            if (!SphereInside(m_PointList[i]))
            {
                MiniSphere(i, m_PointList[i], v);
            }
        }
    }
    else
    {
        SphereInit(v);
    }
}


/***********************************************************************************************************************
*  BoundingVolumeGenerator::MiniSphere()
*   - compute the smallest enclosing sphere
***********************************************************************************************************************/
void BoundingVolumeGenerator::MiniSphere(void)
{
    i32 c = (i32)m_PointList.size();

    if (c >= 2)
    {
        SphereInit(m_PointList[0], m_PointList[1]);
        for (i32 i = 2; i < c; i++)
        {
            if (!SphereInside(m_PointList[i]))
            {
                MiniSphere(i, m_PointList[i]);
            }
        }
    }
    else if (c == 1)
    {
        SphereInit(m_PointList[0]);
    }
    else
    {
        SphereInit();
    }

    // safety -- make sure bsphere fully encompasses the geometry
    for (i32 i = 0; i < c; i++)
    {
        f32 distsqr = (m_PointList[i] - m_Center).LengthSquared();
        m_RadSqr = MAX( m_RadSqr, distsqr );
    }
}


////////////////////////////////////////////////////////////////////////////////////////////////
void BoundingVolumeGenerator::CalculateSystemMethod2(void)
{
    if (m_PointCnt <= 0)
        return;

    m_PointList.reserve(m_PointCnt);

    Vector3 brick_min, brick_max;

    // determine a pseudo-random (but still deterministic) order for the points to be added
    u32* pointOrder = (u32*)malloc( sizeof(u32) * m_PointCnt );
    u32 pointOrderCnt = m_PointCnt;
    for(i32 i = 0; i < m_PointCnt; i++)
    {
        pointOrder[i] = i;
    }

    // reorder the points because this supposedly helps the algorithm avoid worst-case performance
    i32 pointOrderIndex = 0;
    for (i32 i = 0; i < m_PointCnt; i++)
    {
        pointOrderIndex = (pointOrderIndex + 104729) % pointOrderCnt; // increment by the 10000th smallest prime number

        m_PointList.push_back( m_Points[pointOrder[pointOrderIndex]] );
        pointOrder[pointOrderIndex] = pointOrder[pointOrderCnt-1];
        pointOrderCnt--;
    }

    free(pointOrder);

    // compute it
    MiniSphere();
}

////////////////////////////////////////////////////////////////////////////////////////////////



void BoundingVolumeGenerator::AverageSphere(void)
{
    SphereInit( );

    for( i32 i = 0; i < m_PointCnt; i++ )
    {
        m_Center += m_Points[i];
    }

    m_Center *= ( 1.0f/(f32)m_PointCnt);    // get the avg, divide by number of points

    // make sure bsphere fully encompasses the geometry
    for (i32 i = 0; i < m_PointCnt; i++)
    {
        f32 distsqr = (m_Points[i] - m_Center).LengthSquared();
        m_RadSqr = MAX( m_RadSqr, distsqr );
    }
}