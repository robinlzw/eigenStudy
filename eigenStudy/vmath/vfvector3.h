#ifndef __SCCMN_VMATH_VECTOR3_H__
#define __SCCMN_VMATH_VECTOR3_H__

#include "vmathpredef.h"
#include <math.h>
#include "../vstd/dstrm.h"



// 表示三维空间向量的数据结构，也可以表示三维空间的点（点和向量是一回事）
typedef struct VFVector3 
{
    float x ;
    float y ;
    float z ;


	// 静态成员数据——因为常用所以作为类属性被记录下来方便调用？？？
	static const VFVector3 ORIGIN;
	static const VFVector3 ZERO;			// ？？？坐标原点？
	static const VFVector3 AXIS_X;			// ？？？表示坐标系X轴的向量？
	static const VFVector3 AXIS_Y;
	static const VFVector3 AXIS_Z;
	static const VFVector3 AXIS_NEG_X;
	static const VFVector3 AXIS_NEG_Y;
	static const VFVector3 AXIS_NEG_Z;
	static const VFVector3 UNIT_SCALE;
	static const VFVector3 VTBL[3];
	static const VFVector3 NEG_VTBL[3];
	static const VFVector3 DIR_VTBL[6];			// 用EV_DIR_AXI_TYPE索引

	// 友元函数
	friend VFVector3    operator + (const VFVector3  & v);
	friend VFVector3    operator - (const VFVector3  & v);
	friend VFVector3    operator +  (const VFVector3  & x, const VFVector3  & v);
	friend VFVector3    operator -  (const VFVector3  & x, const VFVector3  & v);
	friend VFVector3    operator * (float s, const VFVector3 &  v);

	// 构造函数
    VFVector3() ; 
    VFVector3( float _x, float _y, float _z) ;
    VFVector3( const float * f );
    VFVector3( const VFVector3 & src ) ; 
 





 

    //运算符重载
	const VFVector3 & operator=(VFVector3 const& src); // copy assignment operator  
    VFVector3& operator += (const VFVector3& v);
    VFVector3& operator -= (const VFVector3& v);
    VFVector3& operator *= (const VFVector3& v);
    VFVector3& operator /= (const VFVector3& v);
    VFVector3& operator *= (float s);
    VFVector3& operator /= (float s); 
    

    void MakeFloor( const VFVector3 & cmp ) ;
    void MakeCeil ( const VFVector3 & cmp ) ;

    VFVector3 operator *  ( float s ) const ;


	// 按元素相乘。
    VFVector3 operator * ( const VFVector3 & rhs) const ; // 注：非叉积 ,结果：( x1 * x2 , y1 * y2 , z1 * z2 ) ; 
    VFVector3 operator /  ( float s ) const ; 

    bool             operator <  ( const VFVector3 & v ) const ;
    bool             operator <= ( const VFVector3 & v ) const ;
    bool             operator >  ( const VFVector3 & v ) const ;
    bool             operator >= ( const VFVector3 & v ) const ;
    bool             operator == ( const VFVector3 & v ) const ;
    bool             operator != ( const VFVector3 & v ) const ;
	const float& operator[](unsigned i) const;
	float& operator[](unsigned i);


		
	const float * Ptr() const;			// 返回封装的坐标数据的首地址——&x
	float * Ptr();


	// 向量运算
    float      Dot         ( const VFVector3& v ) const ;	// 点积
    float      AbsDot      ( const VFVector3& v ) const ;	// |x1*x2| + |y1*y2| + |z1*z2| 

    VFVector3  Cross( const VFVector3 & v )     const ;		// 叉积 
    VFVector3  operator / ( const VFVector3& v )  const ;	// (x1/x2,y1/y2,z1/z2)

    float      Magnitude   ()                      const ;	// 模长
    float      SqrMagnitude()                      const ;	// 模的平方
    VFVector3  Direction    ()                      const ;	// 方向, 单位化后的自己
    void        Normalize  ()                            ;	// 单位化
    VFVector3  GetOrthoVector()            const ;			// 生成一个与自己垂直的向量
    VFVector3  GetOrthoNormal()            const ;			// 生成一个与自己垂直的单位向量

    bool IsZero( float threshold = VF_EPS_2 ) const ;		// 判断是否是0向量，可设置阈值。
    bool IsNormalized( float threshold = VF_EPS_2 ) const ; 


} VFVECTOR3 , * LPVFVECTOR3 ;




inline VDataOutput & operator <<( VDataOutput & dOut , const VFVECTOR3 & v )
{
    dOut << v.x << v.y << v.z ;

    return dOut ;
}


inline VDataInput & operator >>( VDataInput & dIn , VFVECTOR3 & v )
{
    dIn >> v.x >> v.y >> v.z ;

    return dIn ;
}


typedef const VFVECTOR3 *  LPCVFVECTOR3 ;

#include "vfvector3.inl"

#endif