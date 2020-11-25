
inline
    VFMatrix4::VFMatrix4() 
{
}

inline VFMatrix4::VFMatrix4( const VFMatrix3 & m3x3 ) 
{
    m[0][0] = m3x3[0][0]  ;
    m[0][1] = m3x3[0][1]  ;
    m[0][2] = m3x3[0][2]  ;
    m[0][3] = 0           ;

    m[1][0] = m3x3[1][0]  ;
    m[1][1] = m3x3[1][1]  ;
    m[1][2] = m3x3[1][2]  ;
    m[1][3] = 0           ;

    m[2][0] = m3x3[2][0]  ;
    m[2][1] = m3x3[2][1]  ;
    m[2][2] = m3x3[2][2]  ;
    m[2][3] = 0           ;

    m[3][0] = 0  ;
    m[3][1] = 0  ;
    m[3][2] = 0  ;
    m[3][3] = 1  ; 
}

inline
    VFMatrix4::VFMatrix4( float m00, float m01, float m02, float m03 ,
    float m10, float m11, float m12, float m13 ,
    float m20, float m21, float m22, float m23 ,
    float m30, float m31, float m32, float m33   ) 
{
    m[0][0] = m00;
    m[0][1] = m01;
    m[0][2] = m02;
    m[0][3] = m03;
    m[1][0] = m10;
    m[1][1] = m11;
    m[1][2] = m12;
    m[1][3] = m13;
    m[2][0] = m20;
    m[2][1] = m21;
    m[2][2] = m22;
    m[2][3] = m23;
    m[3][0] = m30;
    m[3][1] = m31;
    m[3][2] = m32;
    m[3][3] = m33;
}

inline float * VFMatrix4::operator [] ( size_t iRow )
{ 
    return m[iRow];
}

inline const float * VFMatrix4::operator [] ( size_t iRow ) const
{ 
    return m[iRow];
}

inline VFMatrix4 VFMatrix4::operator *( const VFMatrix4 & m2 ) const
{
    VFMatrix4 r;
    r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
    r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
    r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
    r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

    r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
    r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
    r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
    r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

    r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
    r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
    r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
    r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

    r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
    r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
    r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
    r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];

    return r;
}

inline VFVector3 VFMatrix4::operator * ( const VFVector3 &v ) const 
{
    VFVector3 r;

    float fInvW = 1.0f / ( m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] );

    r.x = ( m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] ) * fInvW;
    r.y = ( m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] ) * fInvW;
    r.z = ( m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] ) * fInvW;

    return r;
}

inline VFVector3 VFMatrix4::MltipleNorm( const VFVector3 &v ) const
{
    VFVector3 r;

    float fInvW = 1.0f / ( m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] );

    r.x = ( m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z ) * fInvW;
    r.y = ( m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z ) * fInvW;
    r.z = ( m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z ) * fInvW;

    return r;
}
inline VFMatrix4 VFMatrix4::operator + ( const VFMatrix4 &m2 ) const
{
    VFMatrix4 r;

    r.m[0][0] = m[0][0] + m2.m[0][0];
    r.m[0][1] = m[0][1] + m2.m[0][1];
    r.m[0][2] = m[0][2] + m2.m[0][2];
    r.m[0][3] = m[0][3] + m2.m[0][3];

    r.m[1][0] = m[1][0] + m2.m[1][0];
    r.m[1][1] = m[1][1] + m2.m[1][1];
    r.m[1][2] = m[1][2] + m2.m[1][2];
    r.m[1][3] = m[1][3] + m2.m[1][3];

    r.m[2][0] = m[2][0] + m2.m[2][0];
    r.m[2][1] = m[2][1] + m2.m[2][1];
    r.m[2][2] = m[2][2] + m2.m[2][2];
    r.m[2][3] = m[2][3] + m2.m[2][3];

    r.m[3][0] = m[3][0] + m2.m[3][0];
    r.m[3][1] = m[3][1] + m2.m[3][1];
    r.m[3][2] = m[3][2] + m2.m[3][2];
    r.m[3][3] = m[3][3] + m2.m[3][3];

    return r;
}
inline VFMatrix4 VFMatrix4::operator - ( const VFMatrix4 &m2 ) const
{
    VFMatrix4 r;
    r.m[0][0] = m[0][0] - m2.m[0][0];
    r.m[0][1] = m[0][1] - m2.m[0][1];
    r.m[0][2] = m[0][2] - m2.m[0][2];
    r.m[0][3] = m[0][3] - m2.m[0][3];

    r.m[1][0] = m[1][0] - m2.m[1][0];
    r.m[1][1] = m[1][1] - m2.m[1][1];
    r.m[1][2] = m[1][2] - m2.m[1][2];
    r.m[1][3] = m[1][3] - m2.m[1][3];

    r.m[2][0] = m[2][0] - m2.m[2][0];
    r.m[2][1] = m[2][1] - m2.m[2][1];
    r.m[2][2] = m[2][2] - m2.m[2][2];
    r.m[2][3] = m[2][3] - m2.m[2][3];

    r.m[3][0] = m[3][0] - m2.m[3][0];
    r.m[3][1] = m[3][1] - m2.m[3][1];
    r.m[3][2] = m[3][2] - m2.m[3][2];
    r.m[3][3] = m[3][3] - m2.m[3][3];

    return r;
}    
inline bool VFMatrix4::operator == ( const VFMatrix4& m2 ) const
{
    if( 
        m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
        m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
        m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
        m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3] )
        return false;
    return true;
}
inline bool VFMatrix4::operator != ( const VFMatrix4& m2 ) const
{
    if( 
        m[0][0] != m2.m[0][0] || m[0][1] != m2.m[0][1] || m[0][2] != m2.m[0][2] || m[0][3] != m2.m[0][3] ||
        m[1][0] != m2.m[1][0] || m[1][1] != m2.m[1][1] || m[1][2] != m2.m[1][2] || m[1][3] != m2.m[1][3] ||
        m[2][0] != m2.m[2][0] || m[2][1] != m2.m[2][1] || m[2][2] != m2.m[2][2] || m[2][3] != m2.m[2][3] ||
        m[3][0] != m2.m[3][0] || m[3][1] != m2.m[3][1] || m[3][2] != m2.m[3][2] || m[3][3] != m2.m[3][3] )
        return true;
    return false;
}

inline VFMatrix4 VFMatrix4::Transpose(void) const
{
    return VFMatrix4(m[0][0], m[1][0], m[2][0], m[3][0],
        m[0][1], m[1][1], m[2][1], m[3][1],
        m[0][2], m[1][2], m[2][2], m[3][2],
        m[0][3], m[1][3], m[2][3], m[3][3]);
}    
inline void VFMatrix4::SetTrans( const VFVector3 & v )
{
    m[0][3] = v.x;
    m[1][3] = v.y;
    m[2][3] = v.z;
}       
inline VFVector3 VFMatrix4::GetTrans() const
{
    return VFVector3(m[0][3], m[1][3], m[2][3]);
}

inline void VFMatrix4::MakeTrans( const VFVector3 & v )
{
    m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = v.x;
    m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = v.y;
    m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = v.z;
    m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 1.0;
}

inline void VFMatrix4::MakeTrans( float tx, float ty, float tz )
{
    m[0][0] = 1.0; m[0][1] = 0.0; m[0][2] = 0.0; m[0][3] = tx;
    m[1][0] = 0.0; m[1][1] = 1.0; m[1][2] = 0.0; m[1][3] = ty;
    m[2][0] = 0.0; m[2][1] = 0.0; m[2][2] = 1.0; m[2][3] = tz;
    m[3][0] = 0.0; m[3][1] = 0.0; m[3][2] = 0.0; m[3][3] = 1.0;
}

/** Gets a translation matrix.
*/
inline VFMatrix4 VFMatrix4::CreateTrans( const VFVector3 & v )
{
    VFMatrix4 r;

    r.m[0][0] = 1.0; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = v.x;
    r.m[1][0] = 0.0; r.m[1][1] = 1.0; r.m[1][2] = 0.0; r.m[1][3] = v.y;
    r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = 1.0; r.m[2][3] = v.z;
    r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

    return r;
}

/** Gets a translation matrix - variation for not using a vector.
*/
inline VFMatrix4 VFMatrix4::CreateTrans( float t_x, float t_y, float t_z )
{
    VFMatrix4 r;

    r.m[0][0] = 1.0; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = t_x;
    r.m[1][0] = 0.0; r.m[1][1] = 1.0; r.m[1][2] = 0.0; r.m[1][3] = t_y;
    r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = 1.0; r.m[2][3] = t_z;
    r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

    return r;
}

inline void VFMatrix4::SetScale( const VFVector3& v )
{
    m[0][0] = v.x;
    m[1][1] = v.y;
    m[2][2] = v.z;
}

/** Gets a scale matrix.
*/
inline VFMatrix4 VFMatrix4::CreateScale( const VFVector3& v )
{
    VFMatrix4 r;
    r.m[0][0] = v.x; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = 0.0;
    r.m[1][0] = 0.0; r.m[1][1] = v.y; r.m[1][2] = 0.0; r.m[1][3] = 0.0;
    r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = v.z; r.m[2][3] = 0.0;
    r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

    return r;
}

/** Gets a scale matrix - variation for not using a vector.
*/
inline VFMatrix4 VFMatrix4::CreateScale( float s_x, float s_y, float s_z )
{
    VFMatrix4 r;
    r.m[0][0] = s_x; r.m[0][1] = 0.0; r.m[0][2] = 0.0; r.m[0][3] = 0.0;
    r.m[1][0] = 0.0; r.m[1][1] = s_y; r.m[1][2] = 0.0; r.m[1][3] = 0.0;
    r.m[2][0] = 0.0; r.m[2][1] = 0.0; r.m[2][2] = s_z; r.m[2][3] = 0.0;
    r.m[3][0] = 0.0; r.m[3][1] = 0.0; r.m[3][2] = 0.0; r.m[3][3] = 1.0;

    return r;
}
inline VFMatrix4 VFMatrix4::operator*(float scalar) const
{
    return VFMatrix4(
        scalar*m[0][0], scalar*m[0][1], scalar*m[0][2], scalar*m[0][3],
        scalar*m[1][0], scalar*m[1][1], scalar*m[1][2], scalar*m[1][3],
        scalar*m[2][0], scalar*m[2][1], scalar*m[2][2], scalar*m[2][3],
        scalar*m[3][0], scalar*m[3][1], scalar*m[3][2], scalar*m[3][3]);
}

VFMatrix4 VFMatrix4::Inverse() const
{
    float m00 = m[0][0], m01 = m[0][1], m02 = m[0][2], m03 = m[0][3];
    float m10 = m[1][0], m11 = m[1][1], m12 = m[1][2], m13 = m[1][3];
    float m20 = m[2][0], m21 = m[2][1], m22 = m[2][2], m23 = m[2][3];
    float m30 = m[3][0], m31 = m[3][1], m32 = m[3][2], m33 = m[3][3];

    float v0 = m20 * m31 - m21 * m30;
    float v1 = m20 * m32 - m22 * m30;
    float v2 = m20 * m33 - m23 * m30;
    float v3 = m21 * m32 - m22 * m31;
    float v4 = m21 * m33 - m23 * m31;
    float v5 = m22 * m33 - m23 * m32;

    float t00 = + (v5 * m11 - v4 * m12 + v3 * m13);
    float t10 = - (v5 * m10 - v2 * m12 + v1 * m13);
    float t20 = + (v4 * m10 - v2 * m11 + v0 * m13);
    float t30 = - (v3 * m10 - v1 * m11 + v0 * m12);

    float invDet = 1 / (t00 * m00 + t10 * m01 + t20 * m02 + t30 * m03);

    float d00 = t00 * invDet;
    float d10 = t10 * invDet;
    float d20 = t20 * invDet;
    float d30 = t30 * invDet;

    float d01 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d11 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d21 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d31 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    v0 = m10 * m31 - m11 * m30;
    v1 = m10 * m32 - m12 * m30;
    v2 = m10 * m33 - m13 * m30;
    v3 = m11 * m32 - m12 * m31;
    v4 = m11 * m33 - m13 * m31;
    v5 = m12 * m33 - m13 * m32;

    float d02 = + (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d12 = - (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d22 = + (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d32 = - (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    v0 = m21 * m10 - m20 * m11;
    v1 = m22 * m10 - m20 * m12;
    v2 = m23 * m10 - m20 * m13;
    v3 = m22 * m11 - m21 * m12;
    v4 = m23 * m11 - m21 * m13;
    v5 = m23 * m12 - m22 * m13;

    float d03 = - (v5 * m01 - v4 * m02 + v3 * m03) * invDet;
    float d13 = + (v5 * m00 - v2 * m02 + v1 * m03) * invDet;
    float d23 = - (v4 * m00 - v2 * m01 + v0 * m03) * invDet;
    float d33 = + (v3 * m00 - v1 * m01 + v0 * m02) * invDet;

    return VFMatrix4(
        d00, d01, d02, d03,
        d10, d11, d12, d13,
        d20, d21, d22, d23,
        d30, d31, d32, d33);
}

inline  VFMatrix4 VFMatrix4::InverseAffine(void) const
{ 
    float m10 = m[1][0], m11 = m[1][1], m12 = m[1][2];
    float m20 = m[2][0], m21 = m[2][1], m22 = m[2][2];

    float t00 = m22 * m11 - m21 * m12;
    float t10 = m20 * m12 - m22 * m10;
    float t20 = m21 * m10 - m20 * m11;

    float m00 = m[0][0], m01 = m[0][1], m02 = m[0][2];

    float invDet = 1 / (m00 * t00 + m01 * t10 + m02 * t20);

    t00 *= invDet; t10 *= invDet; t20 *= invDet;

    m00 *= invDet; m01 *= invDet; m02 *= invDet;

    float r00 = t00;
    float r01 = m02 * m21 - m01 * m22;
    float r02 = m01 * m12 - m02 * m11;

    float r10 = t10;
    float r11 = m00 * m22 - m02 * m20;
    float r12 = m02 * m10 - m00 * m12;

    float r20 = t20;
    float r21 = m01 * m20 - m00 * m21;
    float r22 = m00 * m11 - m01 * m10;

    float m03 = m[0][3], m13 = m[1][3], m23 = m[2][3];

    float r03 = - (r00 * m03 + r01 * m13 + r02 * m23);
    float r13 = - (r10 * m03 + r11 * m13 + r12 * m23);
    float r23 = - (r20 * m03 + r21 * m13 + r22 * m23);

    return VFMatrix4(
        r00, r01, r02, r03,
        r10, r11, r12, r13,
        r20, r21, r22, r23,
        0,   0,   0,   1 );
}

inline void VFMatrix4::InsertMatrix3( const VFMatrix3 & mat3 )
{
    m[0][0] = mat3.m[0][0]; m[0][1] = mat3.m[0][1]; m[0][2] = mat3.m[0][2];
    m[1][0] = mat3.m[1][0]; m[1][1] = mat3.m[1][1]; m[1][2] = mat3.m[1][2];
    m[2][0] = mat3.m[2][0]; m[2][1] = mat3.m[2][1]; m[2][2] = mat3.m[2][2];
}

inline void VFMatrix4::ExtractMatrix3( VFMatrix3 & m3x3) const
{
    m3x3.m[0][0] = m[0][0];
    m3x3.m[0][1] = m[0][1];
    m3x3.m[0][2] = m[0][2];
    m3x3.m[1][0] = m[1][0];
    m3x3.m[1][1] = m[1][1];
    m3x3.m[1][2] = m[1][2];
    m3x3.m[2][0] = m[2][0];
    m3x3.m[2][1] = m[2][1];
    m3x3.m[2][2] = m[2][2];

} 

inline
    void VFMatrix4::MakeTransform( const VFVector3 & position, const VFVector3 & scale, const VFQuaternion & orientation )
{
    VFMatrix3 rot3x3 ;
    orientation.ToRotationMatrix(rot3x3);

    // Set up final matrix with scale, rotation and translation
    m[0][0] = scale.x * rot3x3[0][0]; m[0][1] = scale.y * rot3x3[0][1]; m[0][2] = scale.z * rot3x3[0][2]; m[0][3] = position.x;
    m[1][0] = scale.x * rot3x3[1][0]; m[1][1] = scale.y * rot3x3[1][1]; m[1][2] = scale.z * rot3x3[1][2]; m[1][3] = position.y;
    m[2][0] = scale.x * rot3x3[2][0]; m[2][1] = scale.y * rot3x3[2][1]; m[2][2] = scale.z * rot3x3[2][2]; m[2][3] = position.z;

    // No projection term
    m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
}
        
inline
    void VFMatrix4::MakeInverseTransform(const VFVector3& position, const VFVector3& scale, const VFQuaternion& orientation) 
{
    // Invert the parameters
    VFVector3 invTranslate = -position;
    VFVector3 invScale(1 / scale.x, 1 / scale.y, 1 / scale.z);
    VFQuaternion invRot = orientation.Inverted();

    // Because we're inverting, order is translation, rotation, scale
    // So make translation relative to scale & rotation
    invTranslate = invRot * invTranslate; // rotate
    invTranslate *= invScale; // scale

    // Next, make a 3x3 rotation matrix
    VFMatrix3 rot3x3;
    invRot.ToRotationMatrix(rot3x3);

    // Set up final matrix with scale, rotation and translation
    m[0][0] = invScale.x * rot3x3[0][0]; m[0][1] = invScale.x * rot3x3[0][1]; m[0][2] = invScale.x * rot3x3[0][2]; m[0][3] = invTranslate.x;
    m[1][0] = invScale.y * rot3x3[1][0]; m[1][1] = invScale.y * rot3x3[1][1]; m[1][2] = invScale.y * rot3x3[1][2]; m[1][3] = invTranslate.y;
    m[2][0] = invScale.z * rot3x3[2][0]; m[2][1] = invScale.z * rot3x3[2][1]; m[2][2] = invScale.z * rot3x3[2][2]; m[2][3] = invTranslate.z;		

    // No projection term
    m[3][0] = 0; m[3][1] = 0; m[3][2] = 0; m[3][3] = 1;
}

inline
    void VFMatrix4::Decomposition(VFVector3& position, VFVector3& scale, VFQuaternion& orientation) const 
{ 
    VFMatrix3 m3x3;

    ExtractMatrix3(m3x3);

    VFMatrix3 matQ ;

    VFVector3 vecU ;

    m3x3.QDUDecomposition( matQ, scale, vecU ); 

    orientation = VFQuaternion( matQ );

    position = VFVector3( m[0][3], m[1][3], m[2][3] );
}

inline VFMatrix4 VFMatrix4::Concatenate(const VFMatrix4 &m2) const
{
    VFMatrix4 r;
    r.m[0][0] = m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0] + m[0][3] * m2.m[3][0];
    r.m[0][1] = m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1] + m[0][3] * m2.m[3][1];
    r.m[0][2] = m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2] + m[0][3] * m2.m[3][2];
    r.m[0][3] = m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3] * m2.m[3][3];

    r.m[1][0] = m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0] + m[1][3] * m2.m[3][0];
    r.m[1][1] = m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1] + m[1][3] * m2.m[3][1];
    r.m[1][2] = m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2] + m[1][3] * m2.m[3][2];
    r.m[1][3] = m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3] * m2.m[3][3];

    r.m[2][0] = m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0] + m[2][3] * m2.m[3][0];
    r.m[2][1] = m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1] + m[2][3] * m2.m[3][1];
    r.m[2][2] = m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2] + m[2][3] * m2.m[3][2];
    r.m[2][3] = m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3] * m2.m[3][3];

    r.m[3][0] = m[3][0] * m2.m[0][0] + m[3][1] * m2.m[1][0] + m[3][2] * m2.m[2][0] + m[3][3] * m2.m[3][0];
    r.m[3][1] = m[3][0] * m2.m[0][1] + m[3][1] * m2.m[1][1] + m[3][2] * m2.m[2][1] + m[3][3] * m2.m[3][1];
    r.m[3][2] = m[3][0] * m2.m[0][2] + m[3][1] * m2.m[1][2] + m[3][2] * m2.m[2][2] + m[3][3] * m2.m[3][2];
    r.m[3][3] = m[3][0] * m2.m[0][3] + m[3][1] * m2.m[1][3] + m[3][2] * m2.m[2][3] + m[3][3] * m2.m[3][3];

    return r;
}

inline VFMatrix4 VFMatrix4::ConcatenateAffine(const VFMatrix4 &m2) const
{ 
    return VFMatrix4(
        m[0][0] * m2.m[0][0] + m[0][1] * m2.m[1][0] + m[0][2] * m2.m[2][0],
        m[0][0] * m2.m[0][1] + m[0][1] * m2.m[1][1] + m[0][2] * m2.m[2][1],
        m[0][0] * m2.m[0][2] + m[0][1] * m2.m[1][2] + m[0][2] * m2.m[2][2],
        m[0][0] * m2.m[0][3] + m[0][1] * m2.m[1][3] + m[0][2] * m2.m[2][3] + m[0][3],

        m[1][0] * m2.m[0][0] + m[1][1] * m2.m[1][0] + m[1][2] * m2.m[2][0],
        m[1][0] * m2.m[0][1] + m[1][1] * m2.m[1][1] + m[1][2] * m2.m[2][1],
        m[1][0] * m2.m[0][2] + m[1][1] * m2.m[1][2] + m[1][2] * m2.m[2][2],
        m[1][0] * m2.m[0][3] + m[1][1] * m2.m[1][3] + m[1][2] * m2.m[2][3] + m[1][3],

        m[2][0] * m2.m[0][0] + m[2][1] * m2.m[1][0] + m[2][2] * m2.m[2][0],
        m[2][0] * m2.m[0][1] + m[2][1] * m2.m[1][1] + m[2][2] * m2.m[2][1],
        m[2][0] * m2.m[0][2] + m[2][1] * m2.m[1][2] + m[2][2] * m2.m[2][2],
        m[2][0] * m2.m[0][3] + m[2][1] * m2.m[1][3] + m[2][2] * m2.m[2][3] + m[2][3],

        0, 0, 0, 1);
}
inline VFQuaternion VFMatrix4::ExtractQuaternion() const
{
    VFMatrix3 m3x3;

    ExtractMatrix3( m3x3 ) ;

    return VFQuaternion(m3x3);
}

inline bool VFMatrix4::IsAffine(void) const
{
    return m[3][0] == 0 && m[3][1] == 0 && m[3][2] == 0 && m[3][3] == 1;
}       

inline VFVector3 VFMatrix4::TransformAffine(const VFVector3& v) const
{ 
    return VFVector3(
        m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3], 
        m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3],
        m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3]);
}
