#ifndef __SCCMN_VMATH_DEF_H__
#define __SCCMN_VMATH_DEF_H__

#include <limits>

// 基础的数学支持头文件。



const float VF_EPS           = 0.0001f                         ;
const float VF_EPS_2         = 0.000001f                       ;
const float VF_PI            = 3.1415926535897932384626f       ;
const float VF_HALF_PI       = VF_PI / 2.f                     ;
const float VF_QUARTER_PI    = VF_PI / 4.f                     ;
const float VF_DBL_PI        = VF_PI * 2.f                     ;
const float VF_COEFF_DEGREE  = 180.f / VF_PI                   ;
const float VF_COEFF_RADIAN  = VF_PI / 180.f                   ;
const float VF_MAX			 = 10000.0f;
const float VF_MIN			 = -10000.0f;
const double VD_EPS           = 0.0001                         ;
const double VD_EPS_2         = 0.000001                       ;
const double VD_PI            = 3.1415926535897932384626       ;
const double VD_HALF_PI       = VD_PI / 2.                     ;
const double VD_DBL_PI        = VD_PI * 2.                     ;
const double VD_COEFF_DEGREE  = 180.f / VD_PI                  ;
const double VD_COEFF_RADIAN  = VD_PI / 180.f                  ;

const unsigned VD_INVALID_INDEX = (~((unsigned)0))            ;

inline float vf_squre( float d ) { return d * d ; } 


inline float vf_sign( float f ) { return f < 0.f ? -1.f : 1.f ; }


template< typename T >
inline T vf_max( T a , T b ) { return (((a) > (b)) ? (a) : (b)) ; }


template< typename T >
inline T vf_min( T a , T b ) { return (((a) < (b)) ? (a) : (b)) ; } 



enum EV_AXISTYPE
{
    AT_I = 0 ,
    AT_J ,
    AT_K
} ;



const EV_AXISTYPE  VTBL_AXISTYPES[] = { AT_I , AT_J , AT_K } ;



enum EV_DIR_AXI_TYPE
{
    AT_I_PST = 0,
    AT_J_PST ,
    AT_K_PST ,
	AT_I_NGT ,
	AT_J_NGT ,
	AT_K_NGT
} ;



const EV_DIR_AXI_TYPE  VTBL_DIRAXISTYPES[] = {  AT_I_PST , AT_J_PST ,AT_K_PST ,AT_I_NGT , AT_J_NGT , AT_K_NGT } ;



inline bool vf_appro_zero( float r , float dThreshold = VF_EPS ) { return r >= -dThreshold && r <= dThreshold ; } // 是否约等于零



inline bool vf_appro_zero_2( float  r ) { return vf_appro_zero( r , VF_EPS_2 ) ; } // 是否约等于零



inline bool vf_equal_real( float r1 , float r2 , float dThreshold = VF_EPS ) { return vf_appro_zero( r1 - r2 , dThreshold ) ; }

#endif
