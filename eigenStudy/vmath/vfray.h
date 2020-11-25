#pragma once

#include <utility>
#include "vmathpredef.h" 
#include "vfvector3.h"

struct VFRay
{
protected:
    VFVector3 mOrigin    ;
    VFVector3 mDirection ;

public:
    VFRay():mOrigin( VFVector3::ZERO ), mDirection( VFVector3::AXIS_Z ) {}


    VFRay(const VFVector3& origin, const VFVector3& direction)
        :mOrigin(origin), mDirection(direction) {}

    /** Sets the origin of the ray. */
    void SetOrigin(const VFVector3& origin) { mOrigin = origin; } 


    /** Gets the origin of the ray. */
    const VFVector3& GetOrigin(void) const {return mOrigin;} 

    /** Sets the direction of the ray. */
    void SetDirection(const VFVector3& dir) {mDirection = dir;} 


    /** Gets the direction of the ray. */
    const VFVector3& GetDirection(void) const {return mDirection;} 


    /** Gets the position of a point t units along the ray. */
    VFVector3 GetPoint( float t ) const { 
        return VFVector3(mOrigin + (mDirection * t));
    }


	float GetUnitsLenth(const VFVECTOR3& point) const {
		return mDirection.Dot(point - mOrigin);
	}


    /** Gets the position of a point t units along the ray. */
    VFVector3 operator*( float t ) const { 
        return GetPoint(t);
    }

    /** Tests whether this ray intersects the given plane. 
    @returns A pair structure where the first element indicates whether
    an intersection occurs, and if true, the second element will
    indicate the distance along the ray at which it intersects. 
    This can be converted to a point in space by calling getPoint().
    */
    std::pair< bool, float > Intersects( const VFPlane & plane ) const ;



    /** Tests whether this ray intersects the given plane bounded volume. 
    @returns A pair structure where the first element indicates whether
    an intersection occurs, and if true, the second element will
    indicate the distance along the ray at which it intersects. 
    This can be converted to a point in space by calling getPoint().
    */
    //std::pair<bool, float> intersects(const PlaneBoundedVolume& p) const
    //{
    //    return Math::intersects(*this, p.planes, p.outside == Plane::POSITIVE_SIDE);
    //}
    /** Tests whether this ray intersects the given sphere. 
    @returns A pair structure where the first element indicates whether
    an intersection occurs, and if true, the second element will
    indicate the distance along the ray at which it intersects. 
    This can be converted to a point in space by calling getPoint().
    */
    //std::pair<bool, float> intersects(const Sphere& s) const
    //{
    //	return Math::intersects(*this, s);
    //}
    /** Tests whether this ray intersects the given box. 
    @returns A pair structure where the first element indicates whether
    an intersection occurs, and if true, the second element will
    indicate the distance along the ray at which it intersects. 
    This can be converted to a point in space by calling getPoint().
    */
    //std::pair<bool, float> intersects(const AxisAlignedBox& box) const
    //{
    //	return Math::intersects(*this, box);
    //} 

    friend VDataOutput & operator <<( VDataOutput & dOut , const VFRay & r ) ;



    friend VDataInput  & operator >>( VDataInput & dIn   , VFRay & r ) ;
};
 


inline VDataOutput & operator <<( VDataOutput & dOut , const VFRay & r )
{
    dOut << r.mOrigin << r.mDirection ;

    return dOut ;
}



inline VDataInput & operator >>( VDataInput & dIn , VFRay & r )
{
    dIn >> r.mOrigin >> r.mDirection ;

    return dIn ;
}
