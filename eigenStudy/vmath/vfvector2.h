#ifndef __EASPRT_VFVECTOR2_H__
#define __EASPRT_VFVECTOR2_H__

#include "vfdef.h"
#include <math.h>
#include "../vstd/dstrm.h"

typedef struct VFVector2 
{
    float x ;
    float y ; 

    // =====================================
    // Constructors
    // =====================================
    VFVector2() ; 
    VFVector2( float _x, float _y ) ;
    VFVector2( const float * f );
    VFVector2( const VFVector2 & src ) ; 
    //VFVector2( VFVector2 && src );                 // move constructor  

    const VFVector2 & operator=( VFVector2 const& src ) ; // copy assignment operator  

    // =====================================
    // Access grants
    // =====================================
    const float&operator[](int i) const;
    float&operator[](int i) ;  

    // =====================================
    // operators
    // =====================================
    VFVector2& operator += (const VFVector2& v);
    VFVector2& operator -= (const VFVector2& v);
    VFVector2& operator *= (const VFVector2& v);
    VFVector2& operator /= (const VFVector2& v);
    VFVector2& operator *= (float s);
    VFVector2& operator /= (float s); 
    
    friend VFVector2    operator + ( const VFVector2  & v ) ;
    //friend VFVector2 && operator + ( VFVector2 && v ) ; 
    friend VFVector2    operator - ( const VFVector2  & v ) ;  
    //friend VFVector2 && operator + ( VFVector2 && v ) ; 

    friend VFVector2    operator +  ( const VFVector2  & x , const VFVector2  & v )  ;
    //friend VFVector2 && operator +  ( const VFVector2 & x , VFVector2 && v )  ;
    //friend VFVector2 && operator +  ( VFVector2 && v , const VFVector2 & x )  ;
    //friend VFVector2 && operator +  ( VFVector2 && v , VFVector2 && x )  ;

    friend VFVector2    operator -  ( const VFVector2  & x , const VFVector2  & v )  ;
    //friend VFVector2 && operator -  ( const VFVector2 & x , VFVector2 && v )  ;
    //friend VFVector2 && operator -  ( VFVector2 && v , const VFVector2 & x )  ;
    //friend VFVector2 && operator -  ( VFVector2 && v , VFVector2 && x )  ;
       
    void MakeFloor( const VFVector2 & cmp ) ;
    void MakeCeil ( const VFVector2 & cmp ) ;

    VFVector2 operator *  ( float s ) const ;
    VFVector2 operator * ( const VFVector2 & rhs) const ; // 注：非叉积 ,结果：( x1 * x2 , y1 * y2 , z1 * z2 ) ; 
    VFVector2 operator /  ( float s ) const ; 

    bool             operator <  ( const VFVector2 & v ) const ;
    bool             operator <= ( const VFVector2 & v ) const ;
    bool             operator >  ( const VFVector2 & v ) const ;
    bool             operator >= ( const VFVector2 & v ) const ;
    bool             operator == ( const VFVector2 & v ) const ;
    bool             operator != ( const VFVector2 & v ) const ;

    friend VFVector2    operator * ( float s, const VFVector2 &  v ) ;  
    //friend VFVector2 && operator * ( float s, VFVector2 && v ) ;  

    float      Dot         ( const VFVector2& v ) const ; // 点积
    float      AbsDot      ( const VFVector2& v ) const ; // |x1*x2| + |y1*y2| + |z1*z2| 

    float       Cross( const VFVector2 & v )     const ; // 叉积 
    VFVector2  operator / ( const VFVector2& v )  const ; // 叉积 

    float      Magnitude   ()                      const ; // 模长
    float      SqrMagnitude()                      const ; // 模的平方
    VFVector2  Direction    ()                      const ; // 方向, 单位化后的自己
    void        Normalize   ()                            ; // 单位化
    VFVector2  ToNormal    ()                      const ; // 生成单位化对象
     
    bool IsZero( float threshold = 0 ) const ;
    bool IsNormalized( float threshold = VF_EPS_2 ) const ; 

    float GetRadiusFrom( VFVector2 dir = VFVector2( 1 , 0 ) ) const;
    VFVector2 Cartesian2Polar() const ;
    VFVector2 Polar2Cartesian() const ;

    static const VFVector2 ORIGIN     ;
    static const VFVector2 ZERO       ;
    static const VFVector2 AXIS_X     ;
    static const VFVector2 AXIS_Y     ; 
    static const VFVector2 AXIS_NEG_X ;
    static const VFVector2 AXIS_NEG_Y ; 
    static const VFVector2 UNIT_SCALE ;
    static const VFVector2 VTBL[2]    ;
} VFVECTOR2 , * LPVFVECTOR2 ;

typedef const VFVECTOR2 *  LPCVFVECTOR2 ;


inline VDataOutput & operator <<( VDataOutput & dOut , const VFVECTOR2 & v )
{
    dOut << v.x << v.y ;

    return dOut ;
}

inline VDataInput & operator >>( VDataInput & dIn , VFVECTOR2 & v )
{
    dIn >> v.x >> v.y ;

    return dIn ;
}

#include "vfvector2.inl"

#endif //__EASPRT_VFVECTOR2_H__