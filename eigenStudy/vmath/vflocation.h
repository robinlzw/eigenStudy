#pragma once

#include "vfvector3.h"
#include "vfquaternion.h"
#include "VFMatrix4.h"


// location��
//			��һ��location��������һ�����壬����ά����position��ʾ������ȫ������ϵ�е�λ�ã�orienta��ʾ��������ȫ������ϵ�еĳ���
//			���Ա�ʾȫ�����ꡢ����Ҳ���Ա�ʾ������ꡢ����
//			��һ������A��location����locA����toGlocal��������������B�Ŀռ���Ϣ���ݶ���DataBʱ���˳�����locA��������ȫ�����ݣ�DataB������������B���������A�ľֲ����ݡ�
struct VFLocation
{
public:
    // construction
    VFLocation(){}

    VFLocation( const VFVECTOR3 & pos , const VFQuaternion & q )
					:position( pos ) , orientation( q ){}
    ~VFLocation(){}

public :

	// ������һ��location����
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



	// �ֲ��������ά���� �� ȫ���������ά����
	//		���location������������Ӧ������ȫ�������е�λ�á�������Ϣ��
	//		����ֲ������Ǹ�location�����Ӧ������ľֲ����ꡣ
    VFVECTOR3 TransLocalVertexToGlobal( const VFVECTOR3 & v ) const
    {
		// ����ת��ƽ�ơ�
        return position + orientation * v ;					// orientation.Inverted() * ( v - position ) + position ;
    }



	// ȫ���������ά���� �� �ֲ��������ά����
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

	// ����������4��4����
    VFMatrix4 ToMatrix4() const
    {
        VFMatrix4 mat4( orientation ) ;		// ��Ԫ��orientation��4X4ʵ�����ʾ
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



	// ��Ԫ�����Բ�ֵ
	static VFLocation Lerp( const VFLocation & L1 , const VFLocation & L2 , float t) 
	{ 
        VFLocation loc ;
        loc.position    = L1.position * ( 1 - t ) + L2.position * t ;
        loc.orientation = VFQuaternion::Lerp( L1.orientation , L2.orientation , t ) ;

        return loc ;
    }


	// ��Ԫ���������Բ�ֵ
    static VFLocation SLerp( const VFLocation & L1 , const VFLocation & L2 , float t )
    {
        VFLocation loc;
        loc.position = L1.position * ( 1 - t ) + L2.position * t;
        loc.orientation = VFQuaternion::SLerp( L1.orientation , L2.orientation , t );

        return loc;
    }




public :
	// �������ɶȱ�ʾһ�������λ����Ϣ����3�����ɶȱ�ʾ���ĵ�����λ�ã�3�����ɶȱ�ʾ���峯��
    VFVECTOR3    position    ;			// ��ʾλ�õ����ά�ռ����ꡣ
    VFQuaternion orientation ;			// ��Ԫ��������������ʾ����

    static const VFLocation ORIGIN ;		// ԭ�㣬Ĭ�Ϲ��캯�����죬������Ԫ�ض���0��
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

