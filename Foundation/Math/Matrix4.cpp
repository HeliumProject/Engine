#include "Matrix4.h"

#include "Matrix3.h"
#include "AngleAxis.h"
#include "EulerAngles.h"

using namespace Helium;

const Matrix4 Matrix4::Identity (Vector4 (1, 0, 0, 0), Vector4 (0, 1, 0, 0), Vector4 (0, 0, 1, 0), Vector4 (0, 0, 0, 1));
const Matrix4 Matrix4::Zero (Vector4 (0, 0, 0, 0), Vector4 (0, 0, 0, 0), Vector4 (0, 0, 0, 0), Vector4 (0, 0, 0, 0));

Matrix4::Matrix4(const Matrix3& v)
{
    (*this) = v;
}

Matrix4& Matrix4::operator=(const Matrix3& v)
{
    x = v.x; x.w = 0;
    y = v.y; y.w = 0;
    z = v.z; z.w = 0;
    t = Vector4 (0, 0, 0, 1);

    return *this;
}

Matrix4::Matrix4(const EulerAngles& v)
{
    (*this) = v;
}

Matrix4& Matrix4::operator=(const EulerAngles& v)
{
    return *this = Matrix3(v);
}

Matrix4::Matrix4(const AngleAxis& v)
{
    (*this) = v;
}

Matrix4& Matrix4::operator=(const AngleAxis& v)
{
    return *this = Matrix3(v);
}

Matrix4::Matrix4(const Scale& v)
{
    (*this) = v;
}

Matrix4& Matrix4::operator=(const Scale& v)
{
    (*this) = Matrix4 ();

    x[0]=v.x;
    y[1]=v.y;
    z[2]=v.z;

    return *this;
}

Matrix4::Matrix4(const Shear& v)
{
    (*this) = v;
}

Matrix4& Matrix4::operator=(const Shear& v)
{
    *this = Matrix4 ();

    y[0]=v.xy;
    z[0]=v.xz;
    z[1]=v.yz;

    return *this;
}



void Matrix4::Decompose(Scale& scale, Shear& shear, Matrix3& rotate, Vector3& translate) const
{
    translate = Vector3 (t[0], t[1], t[2]);

    // this code HIGHLY influenced by: 
    //   http://tog.acm.org/GraphicsGems/gemsii/unmatrix.c
    // 
    // and the associated graphics gems

    // okay, first fill out rotate completely with our upper-left 3x3
    // we will normalize parts of rotate through this function, and the order/timing is important
    //
    rotate = *this; 

    // scale and normalize
    scale.x = sqrt ((rotate[0][0]*rotate[0][0] + rotate[0][1]*rotate[0][1] + rotate[0][2]*rotate[0][2]));

    rotate[0][0] /= scale.x;
    rotate[0][1] /= scale.x;
    rotate[0][2] /= scale.x;

    // compute first draft of shear xy
    // this is 'too big' by a factor of scale.y and will be rescaled 
    shear.xy = rotate[0][0]*rotate[1][0] + rotate[0][1]*rotate[1][1] + rotate[0][2]*rotate[1][2];

    // make r[1] orthogonal to r[0]
    rotate[1][0] = rotate[1][0] - shear.xy*rotate[0][0];
    rotate[1][1] = rotate[1][1] - shear.xy*rotate[0][1];
    rotate[1][2] = rotate[1][2] - shear.xy*rotate[0][2];

    // scale and normalize
    scale.y = sqrt ((rotate[1][0]*rotate[1][0] + rotate[1][1]*rotate[1][1] + rotate[1][2]*rotate[1][2]));

    rotate[1][0] /= scale.y;
    rotate[1][1] /= scale.y;
    rotate[1][2] /= scale.y;

    // rescale shear.xy
    shear.xy /= scale.y; 

    // compute shears and make 3rd row orthogonal to both other rows
    shear.xz = rotate[0][0]*rotate[2][0] + rotate[0][1]*rotate[2][1] + rotate[0][2]*rotate[2][2];

    rotate[2][0] = rotate[2][0] - shear.xz*rotate[0][0];
    rotate[2][1] = rotate[2][1] - shear.xz*rotate[0][1];
    rotate[2][2] = rotate[2][2] - shear.xz*rotate[0][2];

    shear.yz = rotate[1][0]*rotate[2][0] + rotate[1][1]*rotate[2][1] + rotate[1][2]*rotate[2][2];

    rotate[2][0] = rotate[2][0] - shear.yz*rotate[1][0];
    rotate[2][1] = rotate[2][1] - shear.yz*rotate[1][1];
    rotate[2][2] = rotate[2][2] - shear.yz*rotate[1][2];

    // get z scale and normalize
    scale.z = sqrt ((rotate[2][0]*rotate[2][0]+rotate[2][1]*rotate[2][1]+rotate[2][2]*rotate[2][2]));

    rotate[2][0] /= scale.z;
    rotate[2][1] /= scale.z;
    rotate[2][2] /= scale.z;

    // rescale shears
    shear.xz /= scale.z; 
    shear.yz /= scale.z; 

    if (rotate.Determinant() < 0)
    {
        rotate[0][0]*=-1;rotate[0][1]*=-1;rotate[0][2]*=-1;
        rotate[1][0]*=-1;rotate[1][1]*=-1;rotate[1][2]*=-1;
        rotate[2][0]*=-1;rotate[2][1]*=-1;rotate[2][2]*=-1;

        scale *= -1;
    }
}

void Matrix4::Decompose(Scale& scale, Matrix3& rotate, Vector3& translate) const
{
    Shear discardedShear; 
    return Decompose(scale, discardedShear, rotate, translate); 
}

void Matrix4::Decompose (Scale& scale, EulerAngles& rotate, Vector3& translate) const
{
    Matrix3 rotateMatrix;
    Decompose( scale, rotateMatrix, translate );
    rotate = rotateMatrix;
}

void Matrix4::Decompose (Scale& scale, Shear& shear, EulerAngles& rotate, Vector3& translate) const
{
    Matrix3 rotateMatrix;
    Decompose( scale, shear, rotateMatrix, translate );

    rotate = rotateMatrix;
}

Matrix4 Matrix4::RotateX(f32 theta)
{
    Matrix4 m = Identity;
    m[1][1] = m[2][2] = cos(theta);
    m[1][2] = sin(theta);
    m[2][1] = -m[1][2];
    return m;
}

Matrix4 Matrix4::RotateY(f32 theta)
{
    Matrix4 m = Identity;
    m[0][0] = m[2][2] = cos(theta);
    m[2][0] = sin(theta);
    m[0][2] = -m[2][0];
    return m;
}

Matrix4 Matrix4::RotateZ(f32 theta)
{
    Matrix4 m = Identity;
    m[0][0] = m[1][1] = cos(theta);
    m[1][0] = sin(theta);
    m[0][1] = -m[1][0];
    return m;
}