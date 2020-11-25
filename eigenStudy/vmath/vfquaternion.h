
#ifndef _SCCMN_MATH_QUATERNION_H_
#define _SCCMN_MATH_QUATERNION_H_

#include "vmathpredef.h"
#include "vfvector3.h"

#include "../vstd/dstrm.h"



// VFQuaternion类——四元数类
struct VFQuaternion
{
public :
	union {
		struct {
			float    s;			//!< the real component
			VFVector3 v;		//!< the imaginary components
		};
        struct {
            float    w ;
            float    x ;
            float    y ; 
            float    z ;
        } ;
		struct { float elem[4]; }; //! the raw elements of the VFQuaternion
	}; 

    static VFQuaternion ZERO        ;
    static VFQuaternion IDENTITY    ; 

public :
	//1. 构造函数
	VFQuaternion() {}
	VFQuaternion(float real, float x, float y, float z): s(real), v(x,y,z) {}
	VFQuaternion(float real, const VFVector3 &i): s(real), v(i) {}

	//		拷贝构造函数
    VFQuaternion(const VFQuaternion & val) :s(val.s), v(val.v) {}
   
	//		？？？参数为变换矩阵的构造函数
	VFQuaternion( const VFMatrix3 & rot ) ;

    //		参数为欧拉角的构造函数1
	VFQuaternion(float theta_z, float theta_y, float theta_x)
	{
		float cos_z_2 = VFMath::Cos(0.5f*theta_z);
		float cos_y_2 = VFMath::Cos(0.5f*theta_y);
		float cos_x_2 = VFMath::Cos(0.5f*theta_x);

		float sin_z_2 = VFMath::Sin(0.5f*theta_z);
		float sin_y_2 = VFMath::Sin(0.5f*theta_y);
		float sin_x_2 = VFMath::Sin(0.5f*theta_x);

		// and now compute VFQuaternion
		s   = cos_z_2*cos_y_2*cos_x_2 + sin_z_2*sin_y_2*sin_x_2;
		v.x = cos_z_2*cos_y_2*sin_x_2 - sin_z_2*sin_y_2*cos_x_2;
		v.y = cos_z_2*sin_y_2*cos_x_2 + sin_z_2*cos_y_2*sin_x_2;
		v.z = sin_z_2*cos_y_2*cos_x_2 - cos_z_2*sin_y_2*sin_x_2;

	}
	
	//		参数为欧拉角的构造函数2 
	VFQuaternion(const VFVector3 &angles)
	{	
		float cos_z_2 = VFMath::Cos(0.5f*angles.z);
		float cos_y_2 = VFMath::Cos(0.5f*angles.y);
		float cos_x_2 = VFMath::Cos(0.5f*angles.x);

		float sin_z_2 = VFMath::Sin(0.5f*angles.z);
		float sin_y_2 = VFMath::Sin(0.5f*angles.y);
		float sin_x_2 = VFMath::Sin(0.5f*angles.x);

		// and now compute VFQuaternion
		s   = cos_z_2*cos_y_2*cos_x_2 + sin_z_2*sin_y_2*sin_x_2;
		v.x = cos_z_2*cos_y_2*sin_x_2 - sin_z_2*sin_y_2*cos_x_2;
		v.y = cos_z_2*sin_y_2*cos_x_2 + sin_z_2*cos_y_2*sin_x_2;
		v.z = sin_z_2*cos_y_2*cos_x_2 - cos_z_2*sin_y_2*sin_x_2;		
	} 
		



	// 2.运算符重载
	VFQuaternion &operator =(const VFQuaternion &q)		
	{ s= q.s; v= q.v; return *this; }

	const VFQuaternion operator +(const VFQuaternion &q) const	
	{ return VFQuaternion(s+q.s, v+q.v); }

	const VFQuaternion operator -(const VFQuaternion &q) const	
	{ return VFQuaternion(s-q.s, v-q.v); }

	const VFQuaternion operator *(const VFQuaternion &q) const	
    {	return VFQuaternion( s*q.s - v.Dot( q.v ) , //VALG_D::DotProduct( v , q.v ) ,
				  v.y*q.v.z - v.z*q.v.y + s*q.v.x + v.x*q.s,
				  v.z*q.v.x - v.x*q.v.z + s*q.v.y + v.y*q.s,
				  v.x*q.v.y - v.y*q.v.x + s*q.v.z + v.z*q.s );
	}

	const VFQuaternion operator /(const VFQuaternion &q) const	
	{
        VFQuaternion p = q.Inverted() ;
		return *this * p;
	}

	const VFQuaternion operator *(float scale) const
	{ return VFQuaternion(s*scale,v*scale); }

	const VFQuaternion operator /(float scale) const
	{ return VFQuaternion(s/scale,v/scale); }

	const VFQuaternion operator -() const
	{ return VFQuaternion(-s, -v); }
	
	const VFQuaternion &operator +=(const VFQuaternion &q)		
	{ v+=q.v; s+=q.s; return *this; }

	const VFQuaternion &operator -=(const VFQuaternion &q)		
	{ v-=q.v; s-=q.s; return *this; }
    
    inline bool operator == ( const VFQuaternion & v ) const
    {
        return ( x == v.x && y == v.y && z == v.z && w == v.w ) ; 
    }   
    
    inline bool operator != ( const VFQuaternion & v ) const
    {
        return ( x != v.x || y != v.y || z != v.z || w != v.w ) ; 
    }   

	const VFQuaternion& operator *=(const VFQuaternion &q)		
	{			
        float x= v.x, y= v.y, z= v.z, sn= s*q.s - v.Dot( q.v ) ; // VALG_D::DotProduct( v , q.v );
		v.x= y*q.v.z - z*q.v.y + s*q.v.x + x*q.s;
		v.y= z*q.v.x - x*q.v.z + s*q.v.y + y*q.s;
		v.z= x*q.v.y - y*q.v.x + s*q.v.z + z*q.s;
		s= sn;
		return *this;
	} 
	
	const VFQuaternion& operator *= (float scale)			
	{ v*=scale; s*=scale; return *this; }


	//		四元数乘以三维向量——表示该向量v被该四元素施加旋转操作。
	VFVector3 operator * (const VFVector3 & v) const
	{
		// nVidia SDK implementation
		VFVector3 uv, uuv;
		VFVector3 qvec(x, y, z);
		uv = qvec.Cross(v);
		uuv = qvec.Cross(uv);
		uv *= (2.0f  * w);
		uuv *= 2.0f;

		return v + uv + uuv;
	}



	const VFQuaternion& operator /= (float scale)			
	{ v/=scale; s/=scale; return *this; }
	

	// 3. 一些数学方法。
	//			获取四元数的长度
	float length() const
    { return sqrt( s*s + v.SqrMagnitude() ); }

	//			获取四元数长度的平方
	float length_squared() const
	{ return (s*s + v.SqrMagnitude() ); }

	//			! normalizes this VFQuaternion
	void normalize()
	{ *this/=length(); }

	//			! returns the normalized version of this VFQuaternion
	VFQuaternion normalized() const
	{ return  *this/length(); }

	//			! computes the conjugate of this VFQuaternion
	void conjugate()
	{ v=-v; }
    VFQuaternion conjugate() const
    { return VFQuaternion( s , -v ) ; }

	//			! inverts this VFQuaternion
	void Invert() ; 
    
    VFQuaternion Inverted() const ;
	
	//			! returns the logarithm of a VFQuaternion = v*a where q = [cos(a),v*sin(a)]
	VFQuaternion log() const
	{
		float a = acos(s);
		float sina = VFMath::Sin(a);
		VFQuaternion ret;

		ret.s = 0;
		if (sina > 0)
		{
			ret.v.x = a*v.x/sina;
			ret.v.y = a*v.y/sina;
			ret.v.z = a*v.z/sina;
		} else {
			ret.v.x= ret.v.y= ret.v.z= 0;
		}
		return ret;
	}

	//			! returns e^VFQuaternion = exp(v*a) = [cos(a),vsin(a)]
	VFQuaternion exp() const
	{
        float a = v.Magnitude() ;
		float sina = VFMath::Sin(a);
		float cosa = VFMath::Cos(a);
		VFQuaternion ret;

		ret.s = cosa;
		if (a > 0)
		{
			ret.v.x = sina * v.x / a;
			ret.v.y = sina * v.y / a;
			ret.v.z = sina * v.z / a;
		} else {
			ret.v.x = ret.v.y = ret.v.z = 0;
		}
		return ret;
	}
      
	
	//			转换为表示旋转操作的3*3变换矩阵
	operator VFMatrix3() const ; 

    void ToRotationMatrix( VFMatrix3 & kRot ) const ; 

	//			! computes the dot product of 2 VFQuaternions
	static inline float dot(const VFQuaternion &q1, const VFQuaternion &q2) 
    { return q1.v.Dot( q2.v )  + q1.s * q2.s ; }

	//			! linear VFQuaternion interpolation
	static VFQuaternion Lerp(const VFQuaternion &q1, const VFQuaternion &q2, float t) 
	{ return (q1*(1-t) + q2*t).normalized(); }

	//			! spherical linear interpolation
	static VFQuaternion SLerp(const VFQuaternion &q1, const VFQuaternion &q2, float t) 
	{
		VFQuaternion q3;
		float dot = VFQuaternion::dot(q1, q2);

		/*	dot = cos(theta)
			if (dot < 0), q1 and q2 are more than 90 degrees apart,
			so we can invert one to reduce spinning	*/
		if (dot < 0)
		{
			dot = -dot;
			q3 = -q2;
		} else q3 = q2;
		
		if (dot < 0.95f)
		{
			float angle = acos(dot);
			return (q1*VFMath::Sin(angle*(1-t)) + q3*VFMath::Sin(angle*t))/VFMath::Sin(angle);
		} else // if the angle is small, use linear interpolation								
			return Lerp(q1,q3,t);		
	}

	//! This version of slerp, used by squad, does not check for theta > 90.
	static VFQuaternion slerpNoInvert(const VFQuaternion &q1, const VFQuaternion &q2, float t) 
	{
		float dot = VFQuaternion::dot(q1, q2);

		if (dot > -0.95f && dot < 0.95f)
		{
			float angle = acos(dot);			
			return (q1*VFMath::Sin(angle*(1-t)) + q2*VFMath::Sin(angle*t))/VFMath::Sin(angle);
		} else  // if the angle is small, use linear interpolation								
			return Lerp(q1,q2,t);			
	}

	//! spherical cubic interpolation
	static VFQuaternion squad(const VFQuaternion &q1,const VFQuaternion &q2,const VFQuaternion &a,const VFQuaternion &b,float t)
	{
		VFQuaternion c= slerpNoInvert(q1,q2,t),
			       d= slerpNoInvert(a,b,t);		
		return slerpNoInvert(c,d,2*t*(1-t));
	}

	//! Shoemake-Bezier interpolation using De Castlejau algorithm
	static VFQuaternion bezier(const VFQuaternion &q1,const VFQuaternion &q2,const VFQuaternion &a,const VFQuaternion &b,float t)
	{
		// level 1
		VFQuaternion q11= slerpNoInvert(q1,a,t),
				q12= slerpNoInvert(a,b,t),
				q13= slerpNoInvert(b,q2,t);		
		// level 2 and 3
		return slerpNoInvert(slerpNoInvert(q11,q12,t), slerpNoInvert(q12,q13,t), t);
	}

	//! Given 3 VFQuaternions, qn-1,qn and qn+1, calculate a control point to be used in spline interpolation
	static VFQuaternion spline(const VFQuaternion &qnm1,const VFQuaternion &qn,const VFQuaternion &qnp1)
	{
		VFQuaternion qni(qn.s, -qn.v);	
		return qn * (( (qni*qnm1).log()+(qni*qnp1).log() )/-4).exp();
	}

	//! converts from a normalized axis - angle pair rotation to a VFQuaternion
	static inline VFQuaternion FromAxisAngle(const VFVector3 &axis, float angle)
	{ return VFQuaternion(VFMath::Cos(angle/2), axis*VFMath::Sin(angle/2)); }

	//! returns the axis and angle of this unit VFQuaternion
	void ToAxisAngle( VFVector3 &axis , VFRadian & angle ) const ;

    //! rotates v by this VFQuaternion (VFQuaternion must be unit)
	VFQuaternion rotate(const VFQuaternion &V )
	{    
		VFQuaternion conjugate(*this);
		conjugate.conjugate();
        VFQuaternion vt = (*this * V * conjugate) ;
        return vt ;
	} 





	//! returns the euler angles from a rotation VFQuaternion
	VFVector3 euler_angles(bool homogenous=true) const
	{
		float sqw = s*s;    
		float sqx = v.x*v.x;    
		float sqy = v.y*v.y;    
		float sqz = v.z*v.z;    

		VFVector3 euler;
		if (homogenous) {
			euler.x = atan2(2.f * (v.x*v.y + v.z*s), sqx - sqy - sqz + sqw);    		
			euler.y = asin(-2.f * (v.x*v.z - v.y*s));
			euler.z = atan2(2.f * (v.y*v.z + v.x*s), -sqx - sqy + sqz + sqw);    
		} else {
			euler.x = atan2(2.f * (v.z*v.y + v.x*s), 1 - 2*(sqx + sqy));
			euler.y = asin(-2.f * (v.x*v.z - v.y*s));
			euler.z = atan2(2.f * (v.x*v.y + v.z*s), 1 - 2*(sqy + sqz));
		}
		return euler;
	}

    static VFQuaternion FromRotationMatrix (const VFMatrix3 & kRot) ;
    


    // staticAxis ：不变轴方向
    // axisInplane：可变轴方向
    // normType: staticAxis和另一个轴只能是EV_AXISTYPE中的一个时，第三个轴
    //           ( staticAxis × axisInplane ) )作为新坐标系下的哪一个轴
    // static VFQuaternion FromPlane( const VFVECTOR3 & staticAxis 
    //                              , const VFVECTOR3 & axisInplane 
    //                              , EV_DIR_AXI_TYPE mainType 
    //                              , EV_DIR_AXI_TYPE normType ) ;

    //
    static VFQuaternion FromRotateAxis( const VFVECTOR3 & orign , const VFVECTOR3 & dest ) ;
  
    // 以下函数中参数需要是单位向量，且相互正交 
    static VFQuaternion FromAxes  ( const VFVECTOR3 & xaxis , const VFVECTOR3 & yaxis , const VFVECTOR3& zaxis ) ;
    static VFQuaternion FromAxesXY( const VFVECTOR3 & xaxis , const VFVECTOR3 & yaxis ) ;
    static VFQuaternion FromAxesYZ( const VFVECTOR3 & yaxis , const VFVECTOR3 & zaxis ) ;
    static VFQuaternion FromAxesZX( const VFVECTOR3 & zaxis , const VFVECTOR3 & xaxis ) ;
};


inline VDataOutput & operator <<( VDataOutput & dOut , const VFQuaternion & v )
{
    dOut << v.x << v.y << v.z << v.w ;

    return dOut ;
}

inline VDataInput & operator >>( VDataInput & dIn , VFQuaternion & v )
{
    dIn >> v.x >> v.y >> v.z >> v.w ;

    return dIn ;
}

#endif
