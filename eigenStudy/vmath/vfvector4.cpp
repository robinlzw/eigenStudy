#include "VFVector4.h"

const VFVector4 VFVector4::ORIGIN( 0 , 0 , 0 , 0 ) ;
const VFVector4 VFVector4::ZERO( 0 , 0 , 0 , 0 ) ;
const VFVector4 VFVector4::AXIS_X( 1 , 0 , 0 , 0 ) ;
const VFVector4 VFVector4::AXIS_Y( 0 , 1 , 0 , 0 ) ;
const VFVector4 VFVector4::AXIS_Z( 0 , 0 , 1 , 0 ) ;
const VFVector4 VFVector4::AXIS_W( 0 , 0 , 0 , 1 ) ;
const VFVector4 VFVector4::AXIS_NEG_X( -1 , 0 , 0 ,  0 ) ;
const VFVector4 VFVector4::AXIS_NEG_Y( 0 , -1 , 0 ,  0 ) ;
const VFVector4 VFVector4::AXIS_NEG_Z( 0 , 0 , -1 ,  0 ) ;
const VFVector4 VFVector4::AXIS_NEG_W( 0 , 0 ,  0 , -1 ) ;
const VFVector4 VFVector4::UNIT_SCALE( 1 , 1 , 1 , 1 );
const VFVector4 VFVector4::VTBL[] = { VFVector4::AXIS_X , VFVector4::AXIS_Y , VFVector4::AXIS_Z , VFVector4::AXIS_W } ;
