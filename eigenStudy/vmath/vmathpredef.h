#ifndef __EASPRT_VMATHPREDEF_H__
#define __EASPRT_VMATHPREDEF_H__

#include "vfdef.h"
#include <math.h>
#include "vfangle.h"

struct  VFVector3           ;
struct  VFQuaternion        ;
struct  VFMatrix3           ;
struct  VFRay               ;
struct  VFPlane             ;
class   VFMatrix4           ;
class   VFAxisAlignedBox    ;
class   VFSphere            ;
struct  VFLineSeg           ;
struct  VFRaySeg            ;
struct  VFLocation          ;



// VFMath类——提供了一些float类型代数运算的接口。
class VFMath
{
public :
    static inline float Sin( float fValue )
    {
        return float( sin( fValue )) ;
    } 
    static inline float Cos( float fValue )
    {
        return float( cos( fValue )) ;
    } 
    static inline VFRadian ACos( float fValue )
    {
        if( fValue >= 1.f ) 
            return VFRadian( 0.f ) ;
        else if( fValue <= -1.f )
            return VFRadian( VF_PI ) ;

        return VFRadian( float( acos( fValue )) ) ;
    } 
    static inline VFRadian ASin( float fValue )
    {
        if( fValue >= 1.f ) 
            return VFRadian( VF_HALF_PI ) ;
        else if( fValue <= -1.f )
            return VFRadian( -VF_HALF_PI ) ;

        return VFRadian( float( asin( fValue )) ) ;
    } 
    static inline float Abs( float fValue )
    {
        return float( fabs( fValue ) ) ;
    }
    static inline float Sqr( float fValue )
    {
        return fValue * fValue ;
    }
    static inline float Sqrt( float fValue )
    {
        return float( sqrt( fValue ) ) ;
    }
    static inline float InvSqrt( float vValue ) 
    {
//////////ASM/////////////////
        //__asm {
        //    fld1 // r0 = 1.f
        //        fld r // r1 = r0, r0 = r
        //        fsqrt // r0 = sqrtf( r0 )
        //        fdiv // r0 = r1 / r0
        //} // returns r0
/////////////////////
	    return 1.f / sqrtf( vValue ) ;
    }

    static bool intersects( const VFSphere & s , const VFAxisAlignedBox & aab ) ;
    static bool intersects( const VFPlane  & p , const VFAxisAlignedBox & aab ) ; 
    static bool intersects( const VFSphere & s , const VFPlane  & p ) ;

    static const float POS_INFINITY ;
    static const float NEG_INFINITY ;

    static bool IsZero( const VFVector3 & v , float fThreshold ) ;

    static VFLocation GetPlaneLocation( const VFPlane & plane )  ;
} ;



#endif //__EASPRT_VMATHPREDEF_H__