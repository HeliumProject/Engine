#include "AngleAxis.h"
#include "Quaternion.h"
#include "Matrix3.h"
#include "Matrix4.h"

using namespace Helium;

const AngleAxis AngleAxis::Zero ( 0.0f, 0.0f, 0.0f, 0.0f );
const AngleAxis AngleAxis::Identity ( 0.0f, 1.0f, 0.0f, 0.0f );

AngleAxis::AngleAxis(const Matrix3& v)
{
    (*this) = v;
}

AngleAxis& AngleAxis::operator=(const Matrix3& v)
{
    return *this = Quaternion (v);
}

AngleAxis::AngleAxis(const Matrix4& v)
{
    (*this) = v;
}

AngleAxis& AngleAxis::operator=(const Matrix4& v)
{
    return *this = Quaternion (v);
}

AngleAxis::AngleAxis(const EulerAngles& v)
{
    (*this) = v;  
}

AngleAxis& AngleAxis::operator=(const EulerAngles& v)
{
    return *this = Quaternion (v);
}

AngleAxis::AngleAxis(const Quaternion& v)
{
    (*this) = v;
}

AngleAxis& AngleAxis::operator=(const Quaternion& v)
{
    Quaternion aQuat(v);
    aQuat.Normalize();

    f32 cos_a = aQuat.values.w;
    angle = (acos(cos_a) * 2.0f);
    f32 sin_a = sqrt(1.0f - cos_a * cos_a);

    if (fabs(sin_a) < 0.0005f) sin_a = 1.0f;

    axis.x = aQuat.values.x / sin_a;
    axis.y = aQuat.values.y / sin_a;
    axis.z = aQuat.values.z / sin_a;

    if (axis.x == 0 && axis.y == 0 && axis.z == 0)
    {
        angle = 0;
        axis.x = 1;
    }

    return *this;
}