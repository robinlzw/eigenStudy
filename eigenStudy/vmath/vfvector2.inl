#include <utility>

// Implementation of hsmath
inline 
VFVector2::VFVector2()
{
}
 
inline 
VFVector2::VFVector2( float _x, float _y )
:x(_x),y(_y) 
{
}

inline 
VFVector2::VFVector2( const float * f )
{
   x=f[0];
   y=f[1]; 
}

inline 
VFVector2::VFVector2( const VFVector2 & src )
{
   x = src.x ; 
   y = src.y ;  
} 
//
//inline VFVector2::VFVector2( VFVector2 && src )                // move constructor  
//    : x( src.x ) , y(src.y) , z( src.z )
//{
//}

inline const VFVector2 & VFVector2::operator=( VFVector2 const& src )  // copy assignment operator  
{
    x = src.x ;
    y = src.y ; 

    return * this ;
}
   
inline 
const float& VFVector2::operator[](int i) const
{
//   ASSERT( i >=0 && i < 3 ) ;
   return ((float*)(&x))[i]  ;
}

inline 
float& VFVector2::operator[](int i)
{
//   ASSERT( i >=0 && i < 3 ) ;
   return ((float*)(&x))[i]  ;
} 
 
inline
VFVector2& VFVector2::operator += ( const VFVector2& v )
{
   x += v.x ;
   y += v.y ; 
   return * this ;
}

inline
VFVector2& VFVector2::operator -= (const VFVector2& v)
{
   x -= v.x ;
   y -= v.y ; 
   return * this ;
}

inline
VFVector2& VFVector2::operator *= (const VFVector2& v)
{
   x *= v.x ;
   y *= v.y ; 
   return * this ;
}

inline
VFVector2& VFVector2::operator /= (const VFVector2& v)
{
   x /= v.x ;
   y /= v.y ; 
   return * this ;
} ;

inline
VFVector2& VFVector2::operator *= (float s)
{
   x *= s ;
   y *= s ; 
   return * this ;
}

inline
VFVector2& VFVector2::operator /= (float s)
{
   x /= s ;
   y /= s ; 
   return * this ;
}
    
inline VFVector2 operator + ( const VFVector2  & v )
{
    return v ;
}
//
//inline VFVector2 && operator + ( VFVector2  && v ) 
//{
//    return std::move( v ) ;
//}

inline VFVector2 operator -( const VFVector2& v )
{
    return VFVector2( -v.x , -v.y ) ;
}
//
//inline VFVector2 && operator -( VFVector2 && v )
//{
//    v.x = -v.x ;
//    v.y = -v.y ;
//    v.z = -v.z ;
//    return std::move(v) ;
//}
 
inline VFVector2 operator + ( const VFVector2  & x , const VFVector2  & v ) 
{ 
    return VFVector2( x.x + v.x , x.y + v.y  ) ;
}
//
//inline  VFVector2 && operator +  ( VFVector2 && v , const VFVector2 & x )  
//{
//    v += x ; 
//    return std::move(v);
//}
//
//inline VFVector2 && operator + ( const VFVector2 & x , VFVector2 && v ) 
//{  
//    v += x ; 
//    return std::move(v);
//}
//
//inline VFVector2 && operator +  ( VFVector2 && v , VFVector2 && x )  
//{
//    v += x ; 
//    return std::move(v);
//}
// 
 
inline VFVector2 operator - ( const VFVector2  & x , const VFVector2  & v ) 
{ 
    return VFVector2( x.x - v.x , x.y - v.y  ) ;
}
//
//inline  VFVector2 && operator -  ( VFVector2 && v , const VFVector2 & x )  
//{
//    v -= x ; 
//    return std::move(v);
//}
//
//inline VFVector2 && operator - ( const VFVector2 & x , VFVector2 && v ) 
//{  
//    v -= x ; 
//    return std::move(v);
//}
//
//inline VFVector2 && operator -  ( VFVector2 && v , VFVector2 && x )  
//{
//    v -= x ; 
//    return std::move(v);
//}

inline VFVector2 VFVector2::operator *  ( float s ) const
{
   return VFVector2( x * s ,  y * s ) ;
}

// 注：非叉积 ,结果：( x1 * x2 , y1 * y2 , z1 * z2 ) ;
inline  VFVector2 VFVector2::operator * ( const VFVector2 & rhs) const  
{
    return VFVector2(
        x * rhs.x,
        y * rhs.y  );
}

inline VFVector2 VFVector2::operator /  ( float s ) const 
{ 
   return VFVector2( x / s , y / s ) ;
}  

inline void VFVector2::MakeFloor( const VFVector2& cmp )
{
    if( cmp.x < x ) x = cmp.x;
    if( cmp.y < y ) y = cmp.y; 
}

/** Sets this vector's components to the maximum of its own and the
ones of the passed in vector.
@remarks
'Maximum' in this case means the combination of the highest
value of x, y and z from both vectors. Highest is taken just
numerically, not magnitude, so 1 > -3.
*/
inline void VFVector2::MakeCeil( const VFVector2& cmp )
{
    if( cmp.x > x ) x = cmp.x;
    if( cmp.y > y ) y = cmp.y; 
}

inline float VFVector2::Dot( const VFVector2& v ) const // 点积
{
    return x * v.x + y * v.y ;
}
 
inline float VFVector2::AbsDot( const VFVector2& v ) const // |x1*x2| + |y1*y2| + |z1*z2|
{
    return fabs( x * v.x) + fabs(y * v.y) ;
}


inline bool VFVector2::operator < ( const VFVector2& ) const
{
   return true ;
}

inline bool operator <= (const VFVector2& , const VFVector2& )
{
   return true ;
}

inline float VFVector2::SqrMagnitude()                      const  // 模的平方
{
    return x*x + y*y ;
}

inline float VFVector2::Magnitude   ()                      const // 模长
{
    return sqrt( SqrMagnitude()) ;
}

inline VFVector2 VFVector2::Direction() const
{
    float len = 1.f / Magnitude() ;
    return VFVector2( x * len , y * len ) ;
}

inline void        VFVector2::Normalize   ()                            // 单位化
{
    float len = 1.f / Magnitude() ;

    x *= len ;
    y *= len ; 
}

inline VFVector2  VFVector2::ToNormal   ()                      const  // 生成单位化对象
{
    float len = 1.f / Magnitude() ;

    return VFVector2( x * len , y * len  ) ;
}

inline bool VFVector2::operator == ( const VFVector2 & v ) const
{
    return ( x == v.x && y == v.y  ) ; 
}   

inline bool VFVector2::operator != ( const VFVector2 & v ) const
{
    return ( x != v.x || y != v.y  ) ; 
}   

inline float VFVector2::Cross( const VFVector2 & v ) const
{
    return x * v.y - y * v.x ;
}


inline VFVector2 operator * ( float s, const VFVector2 &  v ) 
{
    return v * s ;
}

inline  bool VFVector2::IsZero( float threshold  ) const 
{
    return vf_appro_zero( SqrMagnitude() , threshold ) ;
}

inline  bool VFVector2::IsNormalized( float threshold ) const 
{
    return vf_appro_zero( 1.f - SqrMagnitude() , threshold ) ;
}

inline float VFVector2::GetRadiusFrom( VFVector2 dir ) const
{
    float radian = acos( Dot(dir) / ( Magnitude() * dir.Magnitude() ) );

    return dir.Cross( *this ) > 0 ? radian : -radian ;
}

inline VFVector2 VFVector2::Cartesian2Polar() const
{
    float r = x * x + y * y;

    if( vf_appro_zero( r ) )
        return VFVECTOR2::ZERO;

    r = sqrtf( r );

    float fValue = x / r ;
    float theta ;

    if( fValue >= 1.f ) 
        theta = 0 ;
    else if( fValue <= -1.f )
        theta = VF_PI ;
    else
        theta =  acos( fValue ) ; 

    if( y < 0 )
        return VFVECTOR2( -theta , r );

    return VFVECTOR2( theta , r ) ;
}

inline VFVector2 VFVector2::Polar2Cartesian() const
{
    return VFVECTOR2( cosf(x) , sinf(x) ) * y ;
}

