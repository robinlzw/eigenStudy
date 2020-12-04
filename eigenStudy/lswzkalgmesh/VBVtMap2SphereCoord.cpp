#include "stdafx.h"
#include "VBVtMap2SphereCoord.h"

void VBVtMap2SphereCoord::BuildTree(const VSConstBuffer<VFRay>& curveVt)
{
	m_vCurveVt.resize(curveVt.len);
	for (unsigned i = 0; i < curveVt.len; i++)
	{
		m_vCurveVt[i] = curveVt.pData[i].GetOrigin();
	}
	cbf = VD_V2CB(m_vCurveVt);

	m_KdTree.Build(vst, cbf);
}

void VBVtMap2SphereCoord::SphereCoordMap(const VSConstBuffer<VFRay>& curveVt, const VSConstBuffer<VFVECTOR3>& mapVt)
{
	BuildTree(curveVt);
	m_vIdx.resize(mapVt.len);
	m_vNorm2.resize(mapVt.len);
	m_vTheta.resize(mapVt.len);
	m_vPhi.resize(mapVt.len);

	LocalCoord    localC;
	m_GenCoord.Build(localC, curveVt);

	std::pair<unsigned, float>  temp;
	VFVECTOR3  curveV2mapV, vecXY;
	float projX, projY, projZ;
	for (unsigned i = 0; i < mapVt.len; i++)
	{
		m_KNN.Build(temp, vst, cbf, mapVt.pData[i]);
		m_vIdx[i] = (float)temp.first;

		curveV2mapV = mapVt.pData[i] - curveVt.pData[temp.first].GetOrigin();
		m_vNorm2[i] = curveV2mapV.Magnitude();

		projX = ( curveV2mapV.Dot(localC.coordX.pData[temp.first]) );
		projY = ( curveV2mapV.Dot(localC.coordY.pData[temp.first]) );
		projZ = ( curveV2mapV.Dot(localC.coordZ.pData[temp.first]) );
		//vecXY = projX * localC.coordX.pData[temp.first] + projY * localC.coordY.pData[temp.first];
		vecXY = curveV2mapV - projZ * localC.coordZ.pData[temp.first];
		m_vTheta[i] = atan2f(projY, projX);
		m_vPhi[i] = atan2f(projZ, vecXY.Magnitude());
	}
}

VD_EXPORT_SYSTEM_SIMPLE(VBVtMap2SphereCoord, VNALGMESH::VRVtMap2SphereCoord);
 

// Added By chakai 
namespace VNWZKALG
{
    struct VSLSCOORDBaseData
    {
        // 
		VNALGMESH::VSKDTree        vst;
		VSConstBuffer<VFRay>       curveVt;
		VSConstBuffer<VFVECTOR3>   cbf;
		LocalCoord                 localC;
    } ;

    namespace VNLSCOORD_VT2SPHERE
    {
        typedef VRVtMap2SphereCoord2 VRIMP;

        class VBuilder
        {
        public:
            void Build( VNWZKALG::VSSphereCoordMap<float> & ou , const VSLSCOORDBaseData & frm , const VSConstBuffer<VFVECTOR3> & vInput )
            {
                /////
				m_vIdx.resize(vInput.len);
				m_vNorm2.resize(vInput.len);
				m_vTheta.resize(vInput.len);
				m_vPhi.resize(vInput.len);

				//LocalCoord    localC;
				//m_GenCoord.Build(localC, frm.curveVt);

				std::pair<unsigned, float>  temp;
				VFVECTOR3  curveV2mapV, vecXY;
				float projX, projY, projZ;
				for (unsigned i = 0; i < vInput.len; i++)
				{
					m_KNN.Build(temp, frm.vst, frm.cbf, vInput.pData[i]);
					m_vIdx[i] = (float)temp.first;

					curveV2mapV = vInput.pData[i] - frm.curveVt.pData[temp.first].GetOrigin();
					m_vNorm2[i] = curveV2mapV.Magnitude();

					projX = (curveV2mapV.Dot(frm.localC.coordX.pData[temp.first]));
					projY = (curveV2mapV.Dot(frm.localC.coordY.pData[temp.first]));
					projZ = (curveV2mapV.Dot(frm.localC.coordZ.pData[temp.first]));
					//vecXY = projX * localC.coordX.pData[temp.first] + projY * localC.coordY.pData[temp.first];
					vecXY = curveV2mapV - projZ * frm.localC.coordZ.pData[temp.first];
					m_vTheta[i] = atan2f(projY, projX);
					m_vPhi[i] = atan2f(projZ, vecXY.Magnitude());
				}

				ou.idx = VD_V2CB(m_vIdx);
				ou.norm2 = VD_V2CB(m_vNorm2);
				ou.theta = VD_V2CB(m_vTheta);
				ou.phi = VD_V2CB(m_vPhi);
            }

		private:
			std::vector<float>         m_vIdx;
			std::vector<float>         m_vNorm2;
			std::vector<float>         m_vTheta;
			std::vector<float>         m_vPhi;

			VBKNearestNeighbor     m_KNN;
			VBGenCoord             m_GenCoord;
        };

        VD_BEGIN_PIPELINE( VPL , VSLSCOORDBaseData )
            VDDP_NOD( Node , VBuilder );
        VD_END_PIPELINE() ;

        class VM
        {
        public:
            VM( VPL & pl 
                , IVSlot< VSConstBuffer<VFVECTOR3> > & sltInput
                , IVInputPort< VNWZKALG::VSSphereCoordMap<float> > & ipOut
            ): m_data( pl.GetEnv().Get< VSLSCOORDBaseData >() )
            { 
                pl.CreateNode( m_node , m_data , sltInput ) ;
                VLNK( ipOut , m_node ) ;
            }
            ~VM()
            {
            }
        private:
            TVSource< VSLSCOORDBaseData > m_data ;
            VPL::Node m_node;
        };

        struct VTRAIT
        {
            typedef VPL PIPELINE;
            template< typename TR > struct TRTrait;
            template<> struct TRTrait< VRIMP > { typedef VM mngr_type; };
        };

        typedef TVDynamicSystemBuilder< VTRAIT , IVSysDynamic< VRIMP > > VBSystem;
    }

    namespace VNLSCOORD_SPHERE2VT
    {
        typedef VRSphereCoordMap2Vt2 VRIMP;

        class VBuilder
        {
        public:
            void Build( VSConstBuffer<VFVECTOR3> & ou , const VSLSCOORDBaseData & frm , const VNWZKALG::VSSphereCoordMap<float> & vInput )
            {
                /////
				unsigned mapNum = vInput.idx.len;
				m_vMapVt.resize(mapNum);
				VFVECTOR3 temp;
				for (unsigned i = 0; i < mapNum; i++)
				{
					unsigned idPos = IDproc(vInput.idx.pData[i], frm.curveVt.len);
					temp.x = vInput.norm2.pData[ i ] * cosf(vInput.phi.pData[ i ]) * cosf(vInput.theta.pData[ i ]);
					temp.y = vInput.norm2.pData[ i ] * cosf(vInput.phi.pData[ i ]) * sinf(vInput.theta.pData[ i ]);
					temp.z = vInput.norm2.pData[ i ] * sinf(vInput.phi.pData[ i ]);

					m_vMapVt[i] = temp.x * frm.localC.coordX.pData[idPos] + temp.y * frm.localC.coordY.pData[idPos] + temp.z * frm.localC.coordZ.pData[idPos] + frm.curveVt.pData[idPos].GetOrigin();
				}

				ou = VD_V2CB(m_vMapVt);
            }

		private:
			unsigned IDproc(float idx, unsigned vNum)
			{
				if (idx < 0.f)
					return 0;
				else if (idx > static_cast<float>(vNum))
					return vNum;
				else
					return static_cast<unsigned>(roundf(idx));
			}

		private:
			std::vector<VFVECTOR3>     m_vMapVt;
        };

        VD_BEGIN_PIPELINE( VPL , VSLSCOORDBaseData )
            VDDP_NOD( Node , VBuilder );
        VD_END_PIPELINE() ;

        class VM
        {
        public:
            VM( VPL & pl 
                , IVSlot< VNWZKALG::VSSphereCoordMap<float> > & sltInput
                , IVInputPort< VSConstBuffer<VFVECTOR3> > & ipOut
            ) : m_data( pl.GetEnv().Get< VSLSCOORDBaseData >() )
            { 
                pl.CreateNode( m_node , m_data , sltInput ) ;
                VLNK( ipOut , m_node ) ;
            }
            ~VM()
            {
            }
        private:
            TVSource< VSLSCOORDBaseData > m_data ;
            VPL::Node m_node;
        };

        struct VTRAIT
        {
            typedef VPL PIPELINE;
            template< typename TR > struct TRTrait;
            template<> struct TRTrait< VRIMP > { typedef VM mngr_type; };
        };

        typedef TVDynamicSystemBuilder< VTRAIT , IVSysDynamic< VRIMP > > VBSystem;
    }

    namespace VNLINESPHERECOORD
    {
        typedef VREntry< VSLSCoordTransPair > VRIMP;

        class VBData
        {
        public:
            void Build( VSLSCOORDBaseData & ou , const VSConstBuffer< VFRay > & cbRay )
            {
                  // do somthing
				m_vCurveVt.resize(cbRay.len);
				for (unsigned i = 0; i < cbRay.len; i++)
				{
					m_vCurveVt[i] = cbRay.pData[i].GetOrigin();
				}
				cbf = VD_V2CB(m_vCurveVt);

				m_KdTree.Build(m_kdout, cbf);
				m_GenCoord.Build(m_LC, cbRay);

				ou.vst = m_kdout;
				ou.curveVt = cbRay;
				ou.cbf = cbf;
				ou.localC = m_LC;
            } 

		private:
			std::vector<VFVECTOR3>     m_vCurveVt;
			VSConstBuffer<VFVECTOR3>   cbf;

			VBKDTree               m_KdTree;
			VNALGMESH::VSKDTree    m_kdout;
			VBGenCoord             m_GenCoord;
			LocalCoord             m_LC;
        };

        class VBOutput
        {
        public:
            void Build( VSLSCoordTransPair & ou , const VSESS< VRVtMap2SphereCoord2 > & esVt2Sphere 
                                                , const VSESS< VRSphereCoordMap2Vt2 > & esSphere2Vt )
            {
                ou.esVt2Sphere = esVt2Sphere ;
                ou.esSphere2Vt = esSphere2Vt ;
            } 
        };

        VD_BEGIN_PIPELINE( VPL , VSConstBuffer< VFRay > )
            VDDP_NOD( Data   , VBData );
            VDDP_NOD( Vt2Sp  , VNLSCOORD_VT2SPHERE::VBSystem );
            VDDP_NOD( Sp2Vt  , VNLSCOORD_SPHERE2VT::VBSystem );
            VDDP_NOD( Output , VBOutput );
        VD_END_PIPELINE() ;

        class VM
        {
        public:
            VM( VPL & pl 
                , IVInputPort< VSLSCoordTransPair > & ipOut
            ) : m_srcInput( pl.GetEnv().Get< VSConstBuffer< VFRay > >() )
            { 
                pl.CreateData( m_data , m_srcInput ) ; 
                pl.CreateVt2Sp ( m_Vt2Sp  , m_data ) ; 
                pl.CreateSp2Vt ( m_Sp2Vt  , m_data ) ; 
                pl.CreateOutput( m_Output , m_Vt2Sp , m_Sp2Vt ) ; 
                VLNK( ipOut , m_Output ) ;
            }
            ~VM()
            {
            }
        private:
            TVSource< VSConstBuffer< VFRay > > m_srcInput ;
            VPL::Data   m_data ;
            VPL::Vt2Sp  m_Vt2Sp  ;
            VPL::Sp2Vt  m_Sp2Vt  ;
            VPL::Output m_Output ;
        };

        struct VTRAIT
        {
            typedef VPL PIPELINE;
            template< typename TR > struct TRTrait;
            template<> struct TRTrait< VRIMP > { typedef VM mngr_type; };
        };

        typedef TVDynamicSystemBuilder< VTRAIT , IVSysDynamic< VRIMP > > VBSystem;
    }
}  

VD_EXPORT_SYSTEM_SYSTEM_SIMPLE_NAME( LSCoordSystem , VNWZKALG::VNLINESPHERECOORD::VTRAIT , VNALGMESH::VRVtLineSphereCoordBuilder ) ;
