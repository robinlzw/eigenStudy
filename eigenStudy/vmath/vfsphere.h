#ifndef __EASPRT_VFSPHERE_H__
#define __EASPRT_VFSPHERE_H__

#include "vfvector3.h"
#include "vmathpredef.h"

class VFSphere
{   
protected:
    float mRadius;
    VFVector3 mCenter;

public:
    VFSphere() : mRadius(1.0), mCenter(VFVector3::ZERO) {}
    VFSphere(const VFVector3& center, float radius)
        : mRadius(radius), mCenter(center) {}

    float GetRadius(void) const { return mRadius; }

    void SetRadius(float radius) { mRadius = radius; }

    const VFVector3& GetCenter(void) const { return mCenter; }

    void SetCenter(const VFVector3& center) { mCenter = center; }

    bool Intersects(const VFSphere& s) const
    {
        return (s.mCenter - mCenter).SqrMagnitude() <=
            VFMath::Sqr(s.mRadius + mRadius);
    }

    bool Intersects(const VFAxisAlignedBox& box) const
    {
        return VFMath::intersects(*this, box);
    }
    bool Intersects(const VFPlane& plane) const
    {
        return VFMath::intersects(*this, plane);
    }
    bool Intersects(const VFVector3& v) const
    {
        return ((v - mCenter).SqrMagnitude() <= VFMath::Sqr(mRadius));
    }

    void Merge( const VFVector3 & v )
    {
        VFVector3 diff   =  v - mCenter;
        float lengthSq   = diff.SqrMagnitude(); 

        // Early-out
        if ( VFMath::Sqr( mRadius ) < lengthSq ) 
        { 
            float length = VFMath::Sqrt( lengthSq );

            float t = ( length - mRadius ) / ( 2.0f * length ) ;
            mCenter = mCenter + diff * t;
            mRadius = 0.5f * ( length + mRadius ) ; 
        }
    }

    void Merge(const VFSphere& oth)
    {
        VFVector3 diff =  oth.GetCenter() - mCenter;
        float lengthSq = diff.SqrMagnitude();
        float radiusDiff = oth.GetRadius() - mRadius;

        // Early-out
        if (VFMath::Sqr(radiusDiff) >= lengthSq) 
        {
            // One fully contains the other
            if (radiusDiff <= 0.0f) 
                return; // no change
            else 
            {
                mCenter = oth.GetCenter();
                mRadius = oth.GetRadius();
                return;
            }
        }

        float length = VFMath::Sqrt(lengthSq);

        VFVector3 newCenter;
        float newRadius;
        if ((length + oth.GetRadius()) > mRadius) 
        {
            float t = (length + radiusDiff) / (2.0f * length);
            newCenter = mCenter + diff * t;
        } 
        else
            newCenter = mCenter ;

        newRadius = 0.5f * (length + mRadius + oth.GetRadius());

        mCenter = newCenter;
        mRadius = newRadius;
    }


}; 

#endif //__EASPRT_VFSPHERE_H__