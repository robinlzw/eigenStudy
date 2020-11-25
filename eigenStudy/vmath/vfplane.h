#pragma once

#include "vfvector3.h"

/** \addtogroup Core
*  @{
*/
/** \addtogroup Math
*  @{
*/



// VFPlane类————三维空间的平面类
/** 
	三维空间平面方程——Ax + By + Cz + D = 0
@par
	This equates to a vector (the normal of the plane, whose x, y
	and z components equate to the coefficients A, B and C
	respectively), and a constant (D) which is the distance along
	the normal you have to go to move the plane back to the origin.
*/
struct VFPlane
{
public:
    VFPlane ();
    VFPlane ( const VFPlane & rhs );
    /** Construct a plane through a normal, and a distance to move the plane along the normal.*/
    VFPlane ( const VFVECTOR3& rkNormal, float fConstant )   ;
    /** Construct a plane using the 4 constants directly **/
    VFPlane ( float a , float b , float c , float d ) ;
    VFPlane ( const VFVECTOR3& rkNormal, const VFVECTOR3& rkPoint );
    VFPlane ( const VFVECTOR3& rkPoint0, const VFVECTOR3& rkPoint1, const VFVECTOR3& rkPoint2 ) ;



    /** The "positive side" of the plane is the half space to which the
    plane normal points. The "negative side" is the other half
    space. The flag "no side" indicates the plane itself.
    */
    enum Side
    {
        NO_SIDE,
        POSITIVE_SIDE,
        NEGATIVE_SIDE,
        BOTH_SIDE
    };

public :
    Side getSide (const VFVECTOR3& rkPoint) const;

    /** Returns which side of the plane that the given box lies on.
    The box is defined as centre/half-size pairs for effectively.
    @param centre The centre of the box.
    @param halfSize The half-size of the box.
    @returns
    POSITIVE_SIDE if the box complete lies on the "positive side" of the plane,
    NEGATIVE_SIDE if the box complete lies on the "negative side" of the plane,
    and BOTH_SIDE if the box intersects the plane.
    */
    Side getSide (const VFVECTOR3& centre, const VFVECTOR3& halfSize) const;

    /** This is a pseudodistance. The sign of the return value is
    positive if the point is on the positive side of the plane,
    negative if the point is on the negative side, and zero if the
    point is on the plane.
    @par
    The absolute value of the return value is the true distance only
    when the plane normal is a unit length vector.
    */
    float getDistance (const VFVECTOR3& rkPoint) const;

    /** Redefine this plane based on 3 points. */
    void redefine(const VFVECTOR3& rkPoint0, const VFVECTOR3& rkPoint1,
        const VFVECTOR3& rkPoint2);

    /** Redefine this plane based on a normal and a point. */
    void redefine(const VFVECTOR3& rkNormal, const VFVECTOR3& rkPoint);

    /** Project a vector onto the plane. 
    @remarks This gives you the element of the input vector that is perpendicular 
    to the normal of the plane. You can get the element which is parallel
    to the normal of the plane by subtracting the result of this method
    from the original vector, since parallel + perpendicular = original.
    @param v The input vector
    */
    VFVECTOR3 projectVector(const VFVECTOR3& v) const;
	VFVECTOR3 projectPoint(const VFVECTOR3& p) const;
    
    /// Comparison operator
    bool operator==(const VFPlane& rhs) const
    {
        return (rhs.d == d && rhs.normal == normal);
    }
    bool operator!=(const VFPlane& rhs) const
    {
        return (rhs.d != d && rhs.normal != normal);
    }

    /** Normalises the plane.
    @remarks
    This method normalises the plane's normal and the length scale of d
    is as well.
    @note
    This function will not crash for zero-sized vectors, but there
    will be no changes made to their components.
    @returns The previous length of the plane's normal.
    */
    float normalise(void);

public :
    VFVECTOR3 normal    ;
    float    d         ;	// d: 表示原点到平面的距离，正负号表示过原点O的法线，和平面的交点V，
							//	  如果向量 dir = O-V 和法线方向一致，则d的符号是正号；否则d的符号为负，即d是负数。
};



//-----------------------------------------------------------------------
inline VFPlane::VFPlane ()
{
    normal = VFVECTOR3::ZERO ;
    d = 0.0;
}
//-----------------------------------------------------------------------
inline VFPlane::VFPlane ( const VFPlane& rhs )
{
    normal = rhs.normal;
    d = rhs.d;
}
//-----------------------------------------------------------------------
inline VFPlane::VFPlane (const VFVECTOR3& rkNormal, float fConstant)
{
    normal = rkNormal;
    d = -fConstant;
}
//---------------------------------------------------------------------
inline VFPlane::VFPlane (float a, float b, float c, float _d)
    : normal(a, b, c), d(_d)
{
}
//-----------------------------------------------------------------------
inline VFPlane::VFPlane (const VFVECTOR3& rkNormal, const VFVECTOR3& rkPoint)
{
    redefine(rkNormal, rkPoint);
}
//-----------------------------------------------------------------------
inline VFPlane::VFPlane (const VFVECTOR3& rkPoint0, const VFVECTOR3& rkPoint1,const VFVECTOR3& rkPoint2)
{
    redefine(rkPoint0, rkPoint1, rkPoint2);
}
//-----------------------------------------------------------------------
inline float VFPlane::getDistance (const VFVECTOR3& rkPoint) const
{
    return normal.Dot(rkPoint) + d;
}
