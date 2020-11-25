#ifndef __SCCMN_VMATH_VECTOR4_H__
#define __SCCMN_VMATH_VECTOR4_H__

#include "vmathpredef.h"
#include <math.h>

typedef struct VFVector4 
{
    float x ;
    float y ;
    float z ;
    float w ;

    // =====================================
    // Constructors
    // =====================================
    VFVector4() ; 
    VFVector4( float _x, float _y, float _z, float w ) ;
    VFVector4( const float * f );
    VFVector4( const VFVector4 & src ) ; 
    //VFVector4( VFVector4 && src );                 // move constructor  
    const float * Ptr() const ;
    const VFVector4 & operator=( VFVector4 const& src ) ; // copy assignment operator  

    // =====================================
    // Access grants
    // =====================================
    const float&operator[]( unsigned i) const;
    float&operator[]( unsigned i) ;  

    // =====================================
    // operators
    // =====================================
    VFVector4& operator += (const VFVector4& v);
    VFVector4& operator -= (const VFVector4& v);
    VFVector4& operator *= (const VFVector4& v);
    VFVector4& operator /= (const VFVector4& v);
    VFVector4& operator *= (float s);
    VFVector4& operator /= (float s); 
    
    friend VFVector4    operator + ( const VFVector4  & v ) ;
    //friend VFVector4 && operator + ( VFVector4 && v ) ; 
    friend VFVector4    operator - ( const VFVector4  & v ) ;  
    //friend VFVector4 && operator + ( VFVector4 && v ) ; 

    friend VFVector4    operator +  ( const VFVector4  & x , const VFVector4  & v )  ;
    //friend VFVector4 && operator +  ( const VFVector4 & x , VFVector4 && v )  ;
    //friend VFVector4 && operator +  ( VFVector4 && v , const VFVector4 & x )  ;
    //friend VFVector4 && operator +  ( VFVector4 && v , VFVector4 && x )  ;

    friend VFVector4    operator -  ( const VFVector4  & x , const VFVector4  & v )  ;
    //friend VFVector4 && operator -  ( const VFVector4 & x , VFVector4 && v )  ;
    //friend VFVector4 && operator -  ( VFVector4 && v , const VFVector4 & x )  ;
    //friend VFVector4 && operator -  ( VFVector4 && v , VFVector4 && x )  ;
       
    void MakeFloor( const VFVector4 & cmp ) ;
    void MakeCeil ( const VFVector4 & cmp ) ;

    VFVector4 operator *  ( float s ) const ;
    VFVector4 operator /  ( float s ) const ; 

    bool             operator <  ( const VFVector4 & v ) const ;
    bool             operator <= ( const VFVector4 & v ) const ;
    bool             operator >  ( const VFVector4 & v ) const ;
    bool             operator >= ( const VFVector4 & v ) const ;
    bool             operator == ( const VFVector4 & v ) const ;
    bool             operator != ( const VFVector4 & v ) const ;

    friend VFVector4    operator * ( float s, const VFVector4 &  v ) ;  
 
    float      Magnitude   ()                      const ; // 模长
    float      SqrMagnitude()                      const ; // 模的平方
    VFVector4  Direction    ()                      const ; // 方向, 单位化后的自己
    void       Normalize   ()                            ; // 单位化
    VFVector4  ToNormal    ()                      const ; // 生成单位化对象

    bool IsZero( float threshold = VF_EPS_2 ) const ;
    bool IsNormalized( float threshold = VF_EPS_2 ) const ; 

    static const VFVector4 ORIGIN     ;
    static const VFVector4 ZERO       ;
    static const VFVector4 AXIS_X     ;
    static const VFVector4 AXIS_Y     ;
    static const VFVector4 AXIS_Z     ;
    static const VFVector4 AXIS_W     ;
    static const VFVector4 AXIS_NEG_X ;
    static const VFVector4 AXIS_NEG_Y ;
    static const VFVector4 AXIS_NEG_Z ;
    static const VFVector4 AXIS_NEG_W ;
    static const VFVector4 UNIT_SCALE ;
    static const VFVector4 VTBL[4]    ;
} VFVECTOR4 , * LPVFVECTOR4 ;

typedef const VFVector4 *  LPCVFVECTOR4 ;

#include "VFVector4.inl"

#endif