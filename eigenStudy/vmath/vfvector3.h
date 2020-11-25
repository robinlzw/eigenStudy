#ifndef __SCCMN_VMATH_VECTOR3_H__
#define __SCCMN_VMATH_VECTOR3_H__

#include "vmathpredef.h"
#include <math.h>
#include "../vstd/dstrm.h"



// ��ʾ��ά�ռ����������ݽṹ��Ҳ���Ա�ʾ��ά�ռ�ĵ㣨���������һ���£�
typedef struct VFVector3 
{
    float x ;
    float y ;
    float z ;


	// ��̬��Ա���ݡ�����Ϊ����������Ϊ�����Ա���¼����������ã�����
	static const VFVector3 ORIGIN;
	static const VFVector3 ZERO;			// ����������ԭ�㣿
	static const VFVector3 AXIS_X;			// ��������ʾ����ϵX���������
	static const VFVector3 AXIS_Y;
	static const VFVector3 AXIS_Z;
	static const VFVector3 AXIS_NEG_X;
	static const VFVector3 AXIS_NEG_Y;
	static const VFVector3 AXIS_NEG_Z;
	static const VFVector3 UNIT_SCALE;
	static const VFVector3 VTBL[3];
	static const VFVector3 NEG_VTBL[3];
	static const VFVector3 DIR_VTBL[6];			// ��EV_DIR_AXI_TYPE����

	// ��Ԫ����
	friend VFVector3    operator + (const VFVector3  & v);
	friend VFVector3    operator - (const VFVector3  & v);
	friend VFVector3    operator +  (const VFVector3  & x, const VFVector3  & v);
	friend VFVector3    operator -  (const VFVector3  & x, const VFVector3  & v);
	friend VFVector3    operator * (float s, const VFVector3 &  v);

	// ���캯��
    VFVector3() ; 
    VFVector3( float _x, float _y, float _z) ;
    VFVector3( const float * f );
    VFVector3( const VFVector3 & src ) ; 
 





 

    //���������
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


	// ��Ԫ����ˡ�
    VFVector3 operator * ( const VFVector3 & rhs) const ; // ע���ǲ�� ,�����( x1 * x2 , y1 * y2 , z1 * z2 ) ; 
    VFVector3 operator /  ( float s ) const ; 

    bool             operator <  ( const VFVector3 & v ) const ;
    bool             operator <= ( const VFVector3 & v ) const ;
    bool             operator >  ( const VFVector3 & v ) const ;
    bool             operator >= ( const VFVector3 & v ) const ;
    bool             operator == ( const VFVector3 & v ) const ;
    bool             operator != ( const VFVector3 & v ) const ;
	const float& operator[](unsigned i) const;
	float& operator[](unsigned i);


		
	const float * Ptr() const;			// ���ط�װ���������ݵ��׵�ַ����&x
	float * Ptr();


	// ��������
    float      Dot         ( const VFVector3& v ) const ;	// ���
    float      AbsDot      ( const VFVector3& v ) const ;	// |x1*x2| + |y1*y2| + |z1*z2| 

    VFVector3  Cross( const VFVector3 & v )     const ;		// ��� 
    VFVector3  operator / ( const VFVector3& v )  const ;	// (x1/x2,y1/y2,z1/z2)

    float      Magnitude   ()                      const ;	// ģ��
    float      SqrMagnitude()                      const ;	// ģ��ƽ��
    VFVector3  Direction    ()                      const ;	// ����, ��λ������Լ�
    void        Normalize  ()                            ;	// ��λ��
    VFVector3  GetOrthoVector()            const ;			// ����һ�����Լ���ֱ������
    VFVector3  GetOrthoNormal()            const ;			// ����һ�����Լ���ֱ�ĵ�λ����

    bool IsZero( float threshold = VF_EPS_2 ) const ;		// �ж��Ƿ���0��������������ֵ��
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