#pragma once

#include "vfvector3.h"
#include "vfquaternion.h"
#include "VFMatrix4.h"


// location类
//			若一个location对象描述一个物体，则三维向量position表示物体在全局坐标系中的位置，orienta表示该物体在全局坐标系中的朝向
//			可以表示全局坐标、朝向，也可以表示相对坐标、朝向。
//			当一个物体A的location对象locA调用toGlocal方法作用于物体B的空间信息数据对象DataB时，此场景下locA的数据是全局数据，DataB的数据是物体B相对于物体A的局部数据。
struct VFLocation
{
public:
    // construction
    VFLocation(){}

    VFLocation( const VFVECTOR3 & pos , const VFQuaternion & q )
					:position( pos ) , orientation( q ){}
    ~VFLocation(){}

public :

	// 输入另一个location对象，
    VFLocation TransLocalPosToGlobal( const VFLocation & v ) const
    {
        VFLocation vRtn ;
        VFVECTOR3 vx , vy , vz ;

        vRtn.position = TransLocalVertexToGlobal( v.position ) ;
        
        vx = v.orientation * VFVECTOR3::AXIS_X ;
        vy = v.orientation * VFVECTOR3::AXIS_Y ;
        vz = v.orientation * VFVECTOR3::AXIS_Z ;


        vx = TransLocalNormalToGlobal( vx ) ;
        vy = TransLocalNormalToGlobal( vy ) ;
        vz = TransLocalNormalToGlobal( vz ) ;

        vRtn.orientation = VFQuaternion::FromAxes( vx , vy , vz ) ;

        return vRtn ;
    }



    VFLocation TransGlobalPosToLocal( const VFLocation & v ) const
    {
        VFLocation vRtn ;
        VFVECTOR3 vx , vy , vz ;

        vRtn.position = TransGlobalVertexToLocal( v.position ) ;
        
        vx = v.orientation * VFVECTOR3::AXIS_X ;
        vy = v.orientation * VFVECTOR3::AXIS_Y ;
        vz = v.orientation * VFVECTOR3::AXIS_Z ;

        vx = TransGlobalNormalToLocal( vx ) ;
        vy = TransGlobalNormalToLocal( vy ) ;
        vz = TransGlobalNormalToLocal( vz ) ;

       vRtn.orientation = VFQuaternion::FromAxes( vx , vy , vz ) ;

        return vRtn ;
    }



	// 局部坐标的三维向量 → 全局坐标的三维向量
	//		这个location对象包含了其对应物体在全局坐标中的位置、朝向信息。
	//		这个局部坐标是该location对象对应的物体的局部坐标。
    VFVECTOR3 TransLocalVertexToGlobal( const VFVECTOR3 & v ) const
    {
		// 先旋转再平移。
        return position + orientation * v ;					// orientation.Inverted() * ( v - position ) + position ;
    }



	// 全局坐标的三维向量 → 局部坐标的三维向量
    VFVECTOR3 TransGlobalVertexToLocal( const VFVECTOR3 & v ) const
    {
        return orientation.Inverted() * ( v - position ) ; // orientation * ( v + position ) - position ;
    }




    VFVECTOR3 TransLocalNormalToGlobal( const VFVECTOR3 & v ) const
    {
        return orientation * v ;
    }

    VFVECTOR3 TransGlobalNormalToLocal( const VFVECTOR3 & v ) const
    {
        return orientation.Inverted() * v ;
    }

	// ？？？返回4×4矩阵
    VFMatrix4 ToMatrix4() const
    {
        VFMatrix4 mat4( orientation ) ;		// 四元数orientation的4X4实矩阵表示
        mat4.SetTrans( position ); 

        return mat4 ;
    }

    VFLocation Inverted() const
    {
        VFVECTOR3 v = orientation.Inverted() * (-position) ;
        return VFLocation( v , orientation.Inverted() ) ;
    }

    bool IsZero( float threshold = VF_EPS_2 ) const
    {
        return vf_equal_real( 1.f , orientation.w , threshold ) && position.IsZero( threshold ) ;
    }
       
    bool operator == ( const VFLocation & v ) const
    {
        return ( position == v.position && orientation == v.orientation ) ;
    }
       
    bool operator != ( const VFLocation & v ) const
    {
        return ( position != v.position || orientation != v.orientation ) ;
    }



	// 四元数线性插值
	static VFLocation Lerp( const VFLocation & L1 , const VFLocation & L2 , float t) 
	{ 
        VFLocation loc ;
        loc.position    = L1.position * ( 1 - t ) + L2.position * t ;
        loc.orientation = VFQuaternion::Lerp( L1.orientation , L2.orientation , t ) ;

        return loc ;
    }


	// 四元数球面线性插值
    static VFLocation SLerp( const VFLocation & L1 , const VFLocation & L2 , float t )
    {
        VFLocation loc;
        loc.position = L1.position * ( 1 - t ) + L2.position * t;
        loc.orientation = VFQuaternion::SLerp( L1.orientation , L2.orientation , t );

        return loc;
    }




public :
	// 六个自由度表示一个物体的位置信息——3个自由度表示中心点所在位置，3个自由度表示物体朝向。
    VFVECTOR3    position    ;			// 表示位置点的三维空间坐标。
    VFQuaternion orientation ;			// 四元数——？？？表示朝向？

    static const VFLocation ORIGIN ;		// 原点，默认构造函数构造，即所有元素都是0；
};




inline VDataOutput & operator <<( VDataOutput & stm , const VFLocation & ti )
{
    stm << ti.position << ti.orientation ;

    return stm ;
}

inline VDataInput & operator >>( VDataInput & stm , VFLocation & ti )
{
    stm >> ti.position >> ti.orientation ;

    return stm ;
}

