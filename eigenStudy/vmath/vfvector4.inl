#include <utility>

// Implementation of hsmath
inline 
VFVector4::VFVector4()
{
}
 
inline 
VFVector4::VFVector4( float _x, float _y, float _z,float _w)
:x(_x),y(_y),z(_z),w(_w)
{
}

inline 
VFVector4::VFVector4( const float * f )
{
   x=f[0];
   y=f[1];
   z=f[2];
   w=f[3];
}

inline 
VFVector4::VFVector4( const VFVector4 & src )
{
   x = src.x ; 
   y = src.y ; 
   z = src.z ;
   w = src.w ;
} 
//
//inline VFVector4::VFVector4( VFVector4 && src )                // move constructor  
//    : x( src.x ) , y(src.y) , z( src.z )
//{
//}  
inline 
const float * VFVector4::Ptr() const 
{
    return &x ;
}


inline const VFVector4 & VFVector4::operator=( VFVector4 const& src )  // copy assignment operator  
{
    x = src.x ;
    y = src.y ;
    z = src.z ;
    w = src.w ;

    return * this ;
}
   
inline 
const float& VFVector4::operator[](unsigned i) const
{
//   ASSERT( i >=0 && i < 3 ) ;
   return ((float*)(&x))[i]  ;
}

inline 
float& VFVector4::operator[](unsigned i)
{
//   ASSERT( i >=0 && i < 3 ) ;
   return ((float*)(&x))[i]  ;
} 
 
inline
VFVector4& VFVector4::operator += ( const VFVector4& v )
{
   x += v.x ;
   y += v.y ;
   z += v.z ;
   w += v.w ;
   return * this ;
}

inline
VFVector4& VFVector4::operator -= (const VFVector4& v)
{
   x -= v.x ;
   y -= v.y ;
   z -= v.z ;
   w -= v.w ;
   return * this ;
}

inline
VFVector4& VFVector4::operator *= (const VFVector4& v)
{
   x *= v.x ;
   y *= v.y ;
   z *= v.z ;
   w *= v.w ;
   return * this ;
}

inline
VFVector4& VFVector4::operator /= (const VFVector4& v)
{
   x /= v.x ;
   y /= v.y ;
   z /= v.z ;
   w /= v.w ;
   return * this ;
} ;

inline
VFVector4& VFVector4::operator *= (float s)
{
   x *= s ;
   y *= s ;
   z *= s ;
   w *= s ;
   return * this ;
}

inline
VFVector4& VFVector4::operator /= (float s)
{
   x /= s ;
   y /= s ;
   z /= s ;
   w /= s ;
   return * this ;
}
    
inline VFVector4 operator + ( const VFVector4  & v )
{
    return v ;
}
//
//inline VFVector4 && operator + ( VFVector4  && v ) 
//{
//    return std::move( v ) ;
//}

inline VFVector4 operator -( const VFVector4& v )
{
    return VFVector4( -v.x , -v.y , -v.z , -v.w ) ;
}
//
//inline VFVector4 && operator -( VFVector4 && v )
//{
//    v.x = -v.x ;
//    v.y = -v.y ;
//    v.z = -v.z ;
//    return std::move(v) ;
//}
 
inline VFVector4 operator + ( const VFVector4  & x , const VFVector4  & v ) 
{ 
    return VFVector4( x.x + v.x , x.y + v.y , x.z + v.z , x.w + v.w ) ;
}
//
//inline  VFVector4 && operator +  ( VFVector4 && v , const VFVector4 & x )  
//{
//    v += x ; 
//    return std::move(v);
//}
//
//inline VFVector4 && operator + ( const VFVector4 & x , VFVector4 && v ) 
//{  
//    v += x ; 
//    return std::move(v);
//}
//
//inline VFVector4 && operator +  ( VFVector4 && v , VFVector4 && x )  
//{
//    v += x ; 
//    return std::move(v);
//}
// 
 
inline VFVector4 operator - ( const VFVector4  & x , const VFVector4  & v ) 
{ 
    return VFVector4( x.x - v.x , x.y - v.y , x.z - v.z , x.w - v.w ) ;
}
//
//inline  VFVector4 && operator -  ( VFVector4 && v , const VFVector4 & x )  
//{
//    v -= x ; 
//    return std::move(v);
//}
//
//inline VFVector4 && operator - ( const VFVector4 & x , VFVector4 && v ) 
//{  
//    v -= x ; 
//    return std::move(v);
//}
//
//inline VFVector4 && operator -  ( VFVector4 && v , VFVector4 && x )  
//{
//    v -= x ; 
//    return std::move(v);
//}

inline VFVector4 VFVector4::operator *  ( float s ) const
{
   return VFVector4( x * s ,  y * s ,  z * s , w * s ) ;
}
 
inline VFVector4 VFVector4::operator /  ( float s ) const 
{ 
   return VFVector4( x / s , y / s , z / s , w / s ) ;
}  

inline void VFVector4::MakeFloor( const VFVector4& cmp )
{
    if( cmp.x < x ) x = cmp.x;
    if( cmp.y < y ) y = cmp.y;
    if( cmp.z < z ) z = cmp.z;
    if( cmp.w < w ) w = cmp.w;
}

/** Sets this vector's components to the maximum of its own and the
ones of the passed in vector.
@remarks
'Maximum' in this case means the combination of the highest
value of x, y and z from both vectors. Highest is taken just
numerically, not magnitude, so 1 > -3.
*/
inline void VFVector4::MakeCeil( const VFVector4& cmp )
{
    if( cmp.x > x ) x = cmp.x;
    if( cmp.y > y ) y = cmp.y;
    if( cmp.z > z ) z = cmp.z;
    if( cmp.w > w ) w = cmp.w;
}
 

inline bool VFVector4::operator < ( const VFVector4& ) const
{
   return true ;
}

inline bool operator <= (const VFVector4& , const VFVector4& )
{
   return true ;
}

inline float VFVector4::SqrMagnitude()                      const  // 模的平方
{
    return x*x + y*y + z*z + w * w ;
}

inline float VFVector4::Magnitude   ()                      const // 模长
{
    return sqrt( SqrMagnitude()) ;
}

inline VFVector4 VFVector4::Direction() const
{
    float len = 1.f / Magnitude() ;
    return VFVector4( x * len , y * len , z * len , w * len ) ;
}

inline void        VFVector4::Normalize   ()                            // 单位化
{
    float len = 1.f / Magnitude() ;

    x *= len ;
    y *= len ;
    z *= len ; 
    w *= len ;
}

inline VFVector4  VFVector4::ToNormal   ()                      const  // 生成单位化对象
{
    float len = 1.f / Magnitude() ;

    return VFVector4( x * len , y * len , z * len , w * len ) ;
}

inline bool VFVector4::operator == ( const VFVector4 & v ) const
{
    return ( x == v.x && y == v.y && z == v.z && w == v.w ) ; 
}   

inline bool VFVector4::operator != ( const VFVector4 & v ) const
{
    return ( x != v.x || y != v.y || z != v.z || w != v.w ) ; 
}   
 

inline VFVector4 operator * ( float s, const VFVector4 &  v ) 
{
    return v * s ;
}
 

inline  bool VFVector4::IsZero( float threshold  ) const 
{
    return vf_appro_zero( SqrMagnitude() , threshold ) ;
}

inline  bool VFVector4::IsNormalized( float threshold ) const 
{
    return vf_appro_zero( 1.f - SqrMagnitude() , threshold ) ;
}