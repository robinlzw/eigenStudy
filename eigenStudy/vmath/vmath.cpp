
#ifdef _WIN32 
#ifdef _DEBUG
#include <windows.h>
#endif
#endif

#include "vmath.h"
#include "vflocation.h"
#include "vfplane.h"

const float VFMath::POS_INFINITY  = std::numeric_limits<float>::infinity()  ;
const float VFMath::NEG_INFINITY  = -std::numeric_limits<float>::infinity() ;

bool VFMath::intersects( const VFSphere& s , const VFAxisAlignedBox & aab ) 
{
    return false ;
}

bool VFMath::intersects( const VFPlane & p , const VFAxisAlignedBox & aab ) 
{
    return false ;
}
    
bool VFMath::intersects( const VFSphere & s , const VFPlane  & p ) 
{
    return false ;
}


float VFAxisAlignedBox::distance(const VFVector3& v) const 
{ 
    if (this->contains(v))
        return 0;
    else
    {
        assert(0) ;
        //float maxDist = std::numeric_limits<float>::min();

        //if (v.x < mMinimum.x)
        //    maxDist = mMinimum.x - v.x ; // std::max(maxDist, mMinimum.x - v.x);
        //if (v.y < mMinimum.y)
        //    maxDist = std::max( maxDist, mMinimum.y - v. y);
        //if (v.z < mMinimum.z)
        //    maxDist = std::max(maxDist, mMinimum.z - v.z);

        //if (v.x > mMaximum.x)
        //    maxDist = std::max(maxDist, v.x - mMaximum.x);
        //if (v.y > mMaximum.y)
        //    maxDist = std::max(maxDist, v.y - mMaximum.y);
        //if (v.z > mMaximum.z)
        //    maxDist = std::max(maxDist, v.z - mMaximum.z);

        return 0 ; // maxDist;
    }
}    

bool VFMath::IsZero( const VFVector3 & v , float fThreshold )
{
    return vf_appro_zero( v.SqrMagnitude() , fThreshold * fThreshold ) ;
}
    
VFLocation VFMath::GetPlaneLocation( const VFPlane & plane )  
{
    const VFVECTOR3 & n = plane.normal ;
    VFVECTOR3 y = n.GetOrthoNormal() ;

    VFQuaternion q = VFQuaternion::FromAxesYZ( y , plane.normal ) ;

    return VFLocation( n * - plane.d , q ) ;
}
