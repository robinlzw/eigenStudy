#include <utility>

// Implementation of hsmath
inline 
VFVector3::VFVector3()
{
}
 
inline 
VFVector3::VFVector3( float _x, float _y, float _z)
:x(_x),y(_y),z(_z)
{
}

inline 
VFVector3::VFVector3( const float * f )
{
   x=f[0];
   y=f[1];
   z=f[2];
}

inline 
VFVector3::VFVector3( const VFVector3 & src )
{
   x = src.x ; 
   y = src.y ; 
   z = src.z ;
} 
//
//inline VFVector3::VFVector3( VFVector3 && src )                // move constructor  
//    : x( src.x ) , y(src.y) , z( src.z )
//{
//}

inline const VFVector3 & VFVector3::operator=( VFVector3 const& src )  // copy assignment operator  
{
    x = src.x ;
    y = src.y ;
    z = src.z ;

    return * this ;
}

inline
const float * VFVector3::Ptr() const
{
    return &x ;
}

inline
float * VFVector3::Ptr() 
{
    return &x ;
}

inline 
const float& VFVector3::operator[](unsigned i) const
{
//   ASSERT( i >=0 && i < 3 ) ;
   return ((float*)(&x))[i]  ;
}

inline 
float& VFVector3::operator[](unsigned i)
{
//   ASSERT( i >=0 && i < 3 ) ;
   return ((float*)(&x))[i]  ;
} 
 
inline
VFVector3& VFVector3::operator += ( const VFVector3& v )
{
   x += v.x ;
   y += v.y ;
   z += v.z ;
   return * this ;
}

inline
VFVector3& VFVector3::operator -= (const VFVector3& v)
{
   x -= v.x ;
   y -= v.y ;
   z -= v.z ;
   return * this ;
}

inline
VFVector3& VFVector3::operator *= (const VFVector3& v)
{
   x *= v.x ;
   y *= v.y ;
   z *= v.z ;
   return * this ;
}

inline
VFVector3& VFVector3::operator /= (const VFVector3& v)
{
   x /= v.x ;
   y /= v.y ;
   z /= v.z ;
   return * this ;
} ;

inline
VFVector3& VFVector3::operator *= (float s)
{
   x *= s ;
   y *= s ;
   z *= s ;
   return * this ;
}

inline
VFVector3& VFVector3::operator /= (float s)
{
   x /= s ;
   y /= s ;
   z /= s ;
   return * this ;
}
    
inline VFVector3 operator + ( const VFVector3  & v )
{
    return v ;
}
//
//inline VFVector3 && operator + ( VFVector3  && v ) 
//{
//    return std::move( v ) ;
//}

inline VFVector3 operator -( const VFVector3& v )
{
    return VFVector3( -v.x , -v.y , -v.z ) ;
}
//
//inline VFVector3 && operator -( VFVector3 && v )
//{
//    v.x = -v.x ;
//    v.y = -v.y ;
//    v.z = -v.z ;
//    return std::move(v) ;
//}
 
inline VFVector3 operator + ( const VFVector3  & x , const VFVector3  & v ) 
{ 
    return VFVector3( x.x + v.x , x.y + v.y , x.z + v.z ) ;
}
//
//inline  VFVector3 && operator +  ( VFVector3 && v , const VFVector3 & x )  
//{
//    v += x ; 
//    return std::move(v);
//}
//
//inline VFVector3 && operator + ( const VFVector3 & x , VFVector3 && v ) 
//{  
//    v += x ; 
//    return std::move(v);
//}
//
//inline VFVector3 && operator +  ( VFVector3 && v , VFVector3 && x )  
//{
//    v += x ; 
//    return std::move(v);
//}
// 
 
inline VFVector3 operator - ( const VFVector3  & x , const VFVector3  & v ) 
{ 
    return VFVector3( x.x - v.x , x.y - v.y , x.z - v.z ) ;
}
//
//inline  VFVector3 && operator -  ( VFVector3 && v , const VFVector3 & x )  
//{
//    v -= x ; 
//    return std::move(v);
//}
//
//inline VFVector3 && operator - ( const VFVector3 & x , VFVector3 && v ) 
//{  
//    v -= x ; 
//    return std::move(v);
//}
//
//inline VFVector3 && operator -  ( VFVector3 && v , VFVector3 && x )  
//{
//    v -= x ; 
//    return std::move(v);
//}

inline VFVector3 VFVector3::operator *  ( float s ) const
{
   return VFVector3( x * s ,  y * s ,  z * s ) ;
}

// 注：非叉积 ,结果：( x1 * x2 , y1 * y2 , z1 * z2 ) ;
inline  VFVector3 VFVector3::operator * ( const VFVector3 & rhs) const  
{
    return VFVector3(
        x * rhs.x,
        y * rhs.y,
        z * rhs.z );
}

inline VFVector3 VFVector3::operator /  ( float s ) const 
{ 
   return VFVector3( x / s , y / s , z / s ) ;
}  

inline VFVector3 VFVector3::operator /  ( const VFVector3& v ) const 
{ 
   return VFVector3( x / v.x , y / v.y , z / v.z ) ;
}  

inline void VFVector3::MakeFloor( const VFVector3& cmp )
{
    if( cmp.x < x ) x = cmp.x;
    if( cmp.y < y ) y = cmp.y;
    if( cmp.z < z ) z = cmp.z;
}

/** Sets this vector's components to the maximum of its own and the
ones of the passed in vector.
@remarks
'Maximum' in this case means the combination of the highest
value of x, y and z from both vectors. Highest is taken just
numerically, not magnitude, so 1 > -3.
*/
inline void VFVector3::MakeCeil( const VFVector3& cmp )
{
    if( cmp.x > x ) x = cmp.x;
    if( cmp.y > y ) y = cmp.y;
    if( cmp.z > z ) z = cmp.z;
}

inline float VFVector3::Dot( const VFVector3& v ) const // 点积
{
    return x * v.x + y * v.y + z * v.z ;
}
 
inline float VFVector3::AbsDot( const VFVector3& v ) const // |x1*x2| + |y1*y2| + |z1*z2|
{
    return fabs( x * v.x) + fabs(y * v.y) + fabs(z * v.z);
}


inline bool VFVector3::operator < ( const VFVector3& ) const
{
   return true ;
}

inline bool operator <= (const VFVector3& , const VFVector3& )
{
   return true ;
}

inline float VFVector3::SqrMagnitude()                      const  // 模的平方
{
    return x*x + y*y + z*z ;
}

inline float VFVector3::Magnitude   ()                      const // 模长
{
    return sqrt( SqrMagnitude()) ;
}

inline VFVector3 VFVector3::Direction() const
{
    float len = 1.f / Magnitude() ;
    return VFVector3( x * len , y * len , z * len ) ;
}

inline void        VFVector3::Normalize   ()                            // 单位化
{
	float flVal = Magnitude() + 1.0e-9f;
    float len = 1.f / flVal ;

    x *= len ;
    y *= len ;
    z *= len ; 
}
//
//inline VFVector3  VFVector3::ToNormal   ()                      const  // 生成单位化对象
//{
//    float len = 1.f / Magnitude() ;
//
//    return VFVector3( x * len , y * len , z * len ) ;
//}
   
inline VFVector3  VFVector3::GetOrthoVector() const // 生成一个与自己垂直的向量( 任意 )
{ 
    if( fabs(x) > fabs(y) )
    {
        if( fabs(x) < fabs(z) )
            return Cross( VFVECTOR3::AXIS_Y ) ;

        return Cross( VFVECTOR3::AXIS_Z ) ;
    }
    else
    {
        if( fabs(y) < fabs(z) )
            return Cross( VFVECTOR3::AXIS_X ) ;

        return Cross( VFVECTOR3::AXIS_X ) ;
    }
}
   
inline VFVector3  VFVector3::GetOrthoNormal() const // 生成一个与自己垂直的向量( 任意 )
{ 
    return GetOrthoVector().Direction() ;
}

inline bool VFVector3::operator == ( const VFVector3 & v ) const
{
    return ( x == v.x && y == v.y && z == v.z ) ; 
}   

inline bool VFVector3::operator != ( const VFVector3 & v ) const
{
    return ( x != v.x || y != v.y || z != v.z ) ; 
}   

inline VFVector3 VFVector3::Cross( const VFVector3 & v ) const
{
    return VFVector3(  y * v.z - z * v.y , 
                        z * v.x - x * v.z ,
                        x * v.y - y * v.x ) ;
}


inline VFVector3 operator * ( float s, const VFVector3 &  v ) 
{
    return v * s ;
}
 

inline  bool VFVector3::IsZero( float threshold  ) const 
{
    return vf_appro_zero( SqrMagnitude() , threshold ) ;
}

inline  bool VFVector3::IsNormalized( float threshold ) const 
{
    return vf_appro_zero( 1.f - SqrMagnitude() , threshold ) ;
}