
#include "vfquaternion.h"
#include "vfmatrix3.h" 
#include <cassert>

VFQuaternion VFQuaternion::ZERO(0.0,0.0,0.0,0.0)        ;
VFQuaternion VFQuaternion::IDENTITY(1.0,0.0,0.0,0.0)    ;

VFQuaternion::VFQuaternion( const VFMatrix3 & rot )
{ 
    VFQuaternion f = FromRotationMatrix( rot ) ;
    x = f.x ;
    y = f.y ;
    z = f.z ;
    w = f.w ;
}

VFQuaternion::operator VFMatrix3() const 
{
    // Assert(length() > 0.9999 && length() < 1.0001, "VFQuaternion is not normalized") ;		
    return VFMATRIX3(1-2*(v.y*v.y+v.z*v.z), 2*(v.x*v.y-s*v.z),   2*(v.x*v.z+s*v.y),   
        2*(v.x*v.y+s*v.z),   1-2*(v.x*v.x+v.z*v.z), 2*(v.y*v.z-s*v.x),   
        2*(v.x*v.z-s*v.y),   2*(v.y*v.z+s*v.x),   1-2*(v.x*v.x+v.y*v.y));
}

void VFQuaternion::ToRotationMatrix( VFMatrix3 & kRot ) const
{     
    float fTx  = x+x;
    float fTy  = y+y;
    float fTz  = z+z;
    float fTwx = fTx*w;
    float fTwy = fTy*w;
    float fTwz = fTz*w;
    float fTxx = fTx*x;
    float fTxy = fTy*x;
    float fTxz = fTz*x;
    float fTyy = fTy*y;
    float fTyz = fTz*y;
    float fTzz = fTz*z;

    kRot[0][0] = 1.0f-(fTyy+fTzz);
    kRot[0][1] = fTxy-fTwz;
    kRot[0][2] = fTxz+fTwy;
    kRot[1][0] = fTxy+fTwz;
    kRot[1][1] = 1.0f-(fTxx+fTzz);
    kRot[1][2] = fTyz-fTwx;
    kRot[2][0] = fTxz-fTwy;
    kRot[2][1] = fTyz+fTwx;
    kRot[2][2] = 1.0f-(fTxx+fTyy); 
} ;

VFQuaternion VFQuaternion::FromRotationMatrix ( const VFMatrix3 & kRot)
{     
    VFQuaternion qRtn ;

    // Algorithm in Ken Shoemake's article in 1987 SIGGRAPH course notes
    // article "Quaternion Calculus and Fast Animation". 
    float fTrace = kRot[0][0]+kRot[1][1]+kRot[2][2];
    float fRoot;

    if ( fTrace > 0.0 )
    {
        // |w| > 1/2, may as well choose w > 1/2
        fRoot = sqrt(fTrace + 1.0f);  // 2w
        qRtn.w = 0.5f*fRoot;
        fRoot = 0.5f/fRoot;  // 1/(4w)
        qRtn.x = (kRot[2][1]-kRot[1][2])*fRoot;
        qRtn.y = (kRot[0][2]-kRot[2][0])*fRoot;
        qRtn.z = (kRot[1][0]-kRot[0][1])*fRoot;
    }
    else
    {
        // |w| <= 1/2
        static size_t s_iNext[3] = { 1, 2, 0 };
        size_t i = 0;
        if ( kRot[1][1] > kRot[0][0] )
            i = 1;
        if ( kRot[2][2] > kRot[i][i] )
            i = 2;
        size_t j = s_iNext[i];
        size_t k = s_iNext[j];

        fRoot = sqrt(kRot[i][i]-kRot[j][j]-kRot[k][k] + 1.0f);
        float* apkQuat[3] = { &qRtn.x, &qRtn.y, &qRtn.z };
        *apkQuat[i] = 0.5f*fRoot;
        fRoot = 0.5f/fRoot;
        qRtn.w = (kRot[k][j]-kRot[j][k])*fRoot;
        *apkQuat[j] = (kRot[j][i]+kRot[i][j])*fRoot;
        *apkQuat[k] = (kRot[k][i]+kRot[i][k])*fRoot;
    }

	qRtn.normalize() ;

    return qRtn ;
}
	


void VFQuaternion::Invert() 
{ 
    float fNorm = length_squared() ; 

    if( fNorm > 0.0f )
    {
        conjugate() ; 

        *this/=length_squared(); 
    }
    else
        *this = VFQuaternion::ZERO ;
}



VFQuaternion VFQuaternion::Inverted() const
{      
    float fNorm = length_squared() ;

    if ( fNorm > 0.0 )
    {
        float fInvNorm = 1.0f / fNorm ;

        return VFQuaternion( w*fInvNorm , -x*fInvNorm , -y*fInvNorm , -z*fInvNorm );
    }
    else
    {
        // return an invalid result to flag the error
        return VFQuaternion::ZERO ;
    }
}
    


// FromRotateAxis()输入旋转前、旋转后的向量，输出旋转操作对应的四元数
//		旋转轴为vo,vd的差积，旋转角就是他俩的夹角。
VFQuaternion VFQuaternion::FromRotateAxis( const VFVECTOR3 & orign , const VFVECTOR3 & dest ) 
{
    VFVECTOR3 vo( orign.Direction() ) , vd( dest.Direction() ) ;
    VFVECTOR3 vAxis = vo.Cross( vd ) ;
    float     dot   = vo.Dot( vd ) ;

    if( vAxis.IsZero() )		// 若vo, vd平行
    {
        if( dot > 0 )
            return IDENTITY ;
        else
        {
            VFVECTOR3 vtemp( vo * vo ) ;

            if( vtemp.y >= vtemp.x && vtemp.z >= vtemp.x ) 
                vtemp = VFVECTOR3::AXIS_X ;
            else if( vtemp.x >= vtemp.y && vtemp.z >= vtemp.y )
                vtemp = VFVECTOR3::AXIS_Y ;
            else
                vtemp = VFVECTOR3::AXIS_Z ;

            return VFQuaternion::FromAxisAngle( vo.Cross( vtemp ) , VF_PI ) ;
        }
    }

    return VFQuaternion::FromAxisAngle( vAxis.Direction() , VFMath::ACos( dot ).valueRadians() ) ;
}



/*
Function: FromAxes
Description:
	通过三个局部坐标系轴xaxis,yaxis,zaxis,来构造一个旋转四元素，作用是把局部坐标转换到全局坐标，
	即把在坐标系(xaxis,yaxis,zaxis)下的点坐标，转换到全部坐标系下。
*/
VFQuaternion VFQuaternion::FromAxes (const VFVECTOR3& xaxis, const VFVECTOR3& yaxis, const VFVECTOR3& zaxis)
{
    VFMatrix3 kRot;

    assert( xaxis.IsNormalized( VF_EPS ) ) ;
    assert( yaxis.IsNormalized( VF_EPS ) ) ;
    assert( zaxis.IsNormalized( VF_EPS ) ) ;
    assert( vf_appro_zero( xaxis.Dot( yaxis ) , VF_EPS ) ) ;
    assert( vf_appro_zero( yaxis.Dot( zaxis ) , VF_EPS ) ) ;
    assert( vf_appro_zero( zaxis.Dot( xaxis ) , VF_EPS ) ) ;

    kRot[0][0] = xaxis.x;
    kRot[1][0] = xaxis.y;
    kRot[2][0] = xaxis.z;

    kRot[0][1] = yaxis.x;
    kRot[1][1] = yaxis.y;
    kRot[2][1] = yaxis.z;

    kRot[0][2] = zaxis.x;
    kRot[1][2] = zaxis.y;
    kRot[2][2] = zaxis.z;

    return FromRotationMatrix(kRot); 
}	

VFQuaternion VFQuaternion::FromAxesXY ( const VFVECTOR3 & xaxis , const VFVECTOR3 & yaxis ) 
{
    return FromAxes( xaxis , yaxis , xaxis.Cross( yaxis ) ) ;
}

VFQuaternion VFQuaternion::FromAxesYZ ( const VFVECTOR3 & yaxis , const VFVECTOR3 & zaxis ) 
{
    return FromAxes( yaxis.Cross( zaxis ) , yaxis , zaxis ) ;
}


VFQuaternion VFQuaternion::FromAxesZX ( const VFVECTOR3 & zaxis , const VFVECTOR3 & xaxis ) 
{
    return FromAxes( xaxis , zaxis.Cross( xaxis ) , zaxis ) ;
}


void VFQuaternion::ToAxisAngle( VFVector3 & rkAxis , VFRadian & rfAngle ) const  
{
        // The quaternion representing the rotation is
        //   q = cos(A/2)+sin(A/2)*(x*i+y*j+z*k)
        float fSqrLength = x*x+y*y+z*z ;

        if ( fSqrLength > 0.0 )
        {
            rfAngle = 2.0*VFMath::ACos(w);
            float fInvLength = VFMath::InvSqrt(fSqrLength);
            rkAxis.x = x*fInvLength;
            rkAxis.y = y*fInvLength;
            rkAxis.z = z*fInvLength;
        }
        else
        {
            // angle is 0 (mod 2*pi), so any axis will do
            rfAngle = VFRadian(0.0);
            rkAxis.x = 1.0;
            rkAxis.y = 0.0;
            rkAxis.z = 0.0;
        }
}
