#include "vfplane.h"
#include "vfmatrix3.h"
 
//-----------------------------------------------------------------------
VFPlane::Side VFPlane::getSide (const VFVECTOR3& rkPoint) const
{
    float fDistance = getDistance(rkPoint);

    if ( fDistance < 0.0 )
        return VFPlane::NEGATIVE_SIDE;

    if ( fDistance > 0.0 )
        return VFPlane::POSITIVE_SIDE;

    return VFPlane::NO_SIDE;
}

//-----------------------------------------------------------------------
VFPlane::Side VFPlane::getSide (const VFVECTOR3& centre, const VFVECTOR3& halfSize) const
{
    // Calculate the distance between box centre and the plane
    float dist = getDistance(centre);

    // Calculate the maximise allows absolute distance for
    // the distance between box centre and plane
    float maxAbsDist = normal.AbsDot( halfSize ) ;

    if (dist < -maxAbsDist)
        return VFPlane::NEGATIVE_SIDE;

    if (dist > +maxAbsDist)
        return VFPlane::POSITIVE_SIDE;

    return VFPlane::BOTH_SIDE;
}

//-----------------------------------------------------------------------
float VFPlane::normalise( void )
{
    float fLength = normal.Magnitude();

    // Will also work for zero-sized vectors, but will change nothing
    if (fLength > VF_EPS_2 )
    {
        float fInvLength = 1.0f / fLength;
        normal *= fInvLength;
        d *= fInvLength;
    }

    return fLength;
} 

//-----------------------------------------------------------------------
void VFPlane::redefine(const VFVECTOR3& rkNormal, const VFVECTOR3& rkPoint)
{
    normal = rkNormal;
    d = -rkNormal.Dot(rkPoint);
}

//-----------------------------------------------------------------------
void VFPlane::redefine(const VFVECTOR3& rkPoint0, const VFVECTOR3& rkPoint1,
    const VFVECTOR3& rkPoint2)
{
    VFVECTOR3 kEdge1 = rkPoint1 - rkPoint0;
    VFVECTOR3 kEdge2 = rkPoint2 - rkPoint0;
    normal = kEdge1.Cross(kEdge2);
    normal.Normalize();
    d = -normal.Dot(rkPoint0) ;
}

//-----------------------------------------------------------------------
VFVECTOR3 VFPlane::projectVector(const VFVECTOR3& v) const
{
    // We know plane normal is unit length, so use simple method
    VFMATRIX3 xform;

    xform[0][0] = 1.0f - normal.x * normal.x;
    xform[0][1] = -normal.x * normal.y;
    xform[0][2] = -normal.x * normal.z;
    xform[1][0] = -normal.y * normal.x;
    xform[1][1] = 1.0f - normal.y * normal.y;
    xform[1][2] = -normal.y * normal.z;
    xform[2][0] = -normal.z * normal.x;
    xform[2][1] = -normal.z * normal.y;
    xform[2][2] = 1.0f - normal.z * normal.z;

    return xform * v ;
}

VFVECTOR3 VFPlane::projectPoint(const VFVECTOR3& p) const
{
	// We know plane normal is unit length, so use simple method
	VFVECTOR3 dotInPlane = -d * normal;
	VFVECTOR3 dif = dotInPlane - p;
	dif = dif.Dot(normal) * normal;
	return p + dif;
}
