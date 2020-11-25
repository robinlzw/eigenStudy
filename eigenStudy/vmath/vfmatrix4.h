#ifndef _SCCMN_MATH_MATRIX4_H_
#define _SCCMN_MATH_MATRIX4_H_

#include "vfvector3.h"
#include "vfmatrix3.h"
#include "vfquaternion.h"



class VFMatrix4 // 此处是按行写的44矩阵 Opengl需要按列的矩阵 所以要先做转置再交给Opengl
{ 
protected:
    /// The matrix entries, indexed by [row][col].
    union {
        float m[4][4] ;
        float _m[16]  ;
    }; 

public :
    VFMatrix4() ;
    VFMatrix4( 
            float m00, float m01, float m02, float m03,
            float m10, float m11, float m12, float m13,
            float m20, float m21, float m22, float m23,
            float m30, float m31, float m32, float m33 ) ;

     VFMatrix4(const VFMatrix3& m3x3 ) ; 

public : 
    inline float * operator [] ( size_t iRow ) ;
    inline const float *operator [] ( size_t iRow ) const ;
    inline VFMatrix4 operator * ( const VFMatrix4 &m2 ) const ;   
    inline VFVector3 operator * ( const VFVector3 &v ) const ; 
    inline VFVector3 MltipleNorm( const VFVector3 &v ) const ;     
    inline VFMatrix4 operator + ( const VFMatrix4 &m2 ) const ;
    inline VFMatrix4 operator - ( const VFMatrix4 &m2 ) const ; 
    inline bool operator == ( const VFMatrix4& m2 ) const ; 
    inline bool operator != ( const VFMatrix4& m2 ) const ; 
    inline VFMatrix4 Transpose(void) const ;
    inline void SetTrans( const VFVector3 & v ) ;
    inline VFVector3 GetTrans() const ;

    inline void MakeTrans( const VFVector3 & v ) ;
    inline void MakeTrans( float tx, float ty, float tz ) ;

    /** Gets a translation matrix.
    */
    inline static VFMatrix4 CreateTrans( const VFVector3 & v ) ; 

    /** Gets a translation matrix - variation for not using a vector.
    */
    inline static VFMatrix4 CreateTrans( float t_x, float t_y, float t_z ) ;

    inline void SetScale( const VFVector3& v ) ; 

    /** Gets a scale matrix.
    */
    inline static VFMatrix4 CreateScale( const VFVector3& v ) ; 

    /** Gets a scale matrix - variation for not using a vector.
    */
    inline static VFMatrix4 CreateScale( float s_x, float s_y, float s_z ) ; 
    inline VFMatrix4 operator*(float scalar) const ; 
    inline VFMatrix4 Inverse() const;
    inline VFMatrix4 InverseAffine(void) const ; 
       
    inline void InsertMatrix3( const VFMatrix3 & mat3 ) ;
    inline void ExtractMatrix3( VFMatrix3 & m3x3) const ;
    
    inline void MakeTransform( const VFVector3 & position , const VFVector3 & scale , const VFQuaternion & orientation ) ;
    inline void MakeInverseTransform(const VFVector3& position, const VFVector3& scale, const VFQuaternion & orientation) ;   
    inline void Decomposition( VFVector3& position, VFVector3& scale, VFQuaternion & orientation) const ;
           
    inline VFMatrix4 Concatenate(const VFMatrix4 &m2) const ;
    inline VFMatrix4 ConcatenateAffine(const VFMatrix4 &m2) const ;
    inline VFQuaternion ExtractQuaternion() const ;
    inline bool IsAffine(void) const ;
    inline VFVector3 TransformAffine(const VFVector3& v) const ;
} ;

typedef VFMatrix4 VFMATRIX4 , * LPVFMATRIX4 ;  

const VFMatrix4 MAT4_ZERO(
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0,
    0, 0, 0, 0 );

const VFMatrix4 MAT4_IDENTITY(
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1 );

#include "vfmatrix4.inl"

#endif // _SCCMN_MATH_MATRIX4_H_