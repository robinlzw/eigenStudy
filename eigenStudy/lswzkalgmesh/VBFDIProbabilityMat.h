#pragma once
#include <sys/alg/isysalgwxj.h>
#include <pkg/ldcktest.h>
#include <alg/ialgwzk.h>

#include "VBFDINameCal.h"
#include "FDIparams.h"

#include "VBMeshBarycenter.h"
#include "VBMeshGap.h"
#include "VBMeshHogFeature.h"
#include "VBMeshSurfaceArea.h"
#include "VBEllipsesFit.h"
#include "VBVerticesPCA.h"
#include "VBFindHole.h"

struct VSFDIProbabilityMatRslt
{
	VSConstBuffer<unsigned>        meshOrder;
	VSConstBuffer<double>          probMat;  //每颗牙齿属于16类牙齿类型分布的概率
	VSConstBuffer<double>          gapMat;   //相邻牙齿间距属于2个缺牙分布的概率
	VNWZKALG::VSTranDistribution   tranProb; //转换概率
};

// 这个结构体可能应该拿出去
struct VSProbInfoInput
{
	VSConstBuffer<VNWZKALG::VSFeatureGaussDistribution>    gaussProb;
	VSConstBuffer<VNWZKALG::VSGapDistribution>             gapProb;
	VSConstBuffer<double>                                  GaussCoeffVal;    //高斯分布系数
	VSConstBuffer<VSConstBuffer<double>>                   CovarMatInv;      //高斯协方差矩阵的逆矩阵
	VNWZKALG::VSTranDistribution                           tranProb;
};

class VBFDIProbabilityMat
{
public:
	VBFDIProbabilityMat() {}
	~VBFDIProbabilityMat() {}

	void Build(VSFDIProbabilityMatRslt & probMat, const VNALGWXJ::VSFDIGeneratorDep & meshs, const VSProbInfoInput & distribuData)
	{
		ProbMatCal(meshs, distribuData);

		probMat.meshOrder = VD_V2CB(m_vMeshOrder);
		probMat.probMat = VD_V2CB(m_vProbMat);
		probMat.gapMat = VD_V2CB(m_vGapMat);
		probMat.tranProb.TranProbCol = VD_V2CB(m_vTranCol);
		probMat.tranProb.TranProbRow = VD_V2CB(m_vTranRow);
	}

private:
	void   ToothPlanePos(const VNALGWXJ::VSFDIGeneratorDep & meshs);
	void   ToothFeatureCal(const VNALGWXJ::VSFDIGeneratorDep & meshs);
	void   ProbMatCal(const VNALGWXJ::VSFDIGeneratorDep & meshs, const VSProbInfoInput & distribuData);
	void   SingleToothFeature(std::vector<float>& vec, unsigned pos);

private:
	//void   CoordinateTran(VFVECTOR3 & xDirect, VFVECTOR3 & yDirect, VFVECTOR3 & zDirect, const VNALGWXJ::VSFDIGeneratorDep & meshs,
	//	                  const std::vector<VFVECTOR3> & bary, const unsigned & isUpper);

private:
	std::vector<unsigned>               m_vMeshOrder;
	std::vector<double>                 m_vProbMat;      //n*16的矩阵，按行存储
	std::vector<double>                 m_vGapMat;
	std::vector<double>                 m_vTranRow;
	std::vector<double>                 m_vTranCol;
								        
	std::vector<VFVECTOR2>              m_vEllipseCoord;        //牙齿网格投影点在椭圆坐标系内坐标（无序）
	std::vector<float>                  m_vSurfaceArea;         //牙齿的表面积（有序）
	std::vector<VFVECTOR3>              m_vToothVertPCAvalue;   //牙齿顶点主成分特征值（有序）
	std::vector<std::vector<float>>     m_vToothHogFeature;     //牙齿的hog特征（有序）
	std::vector<VFVECTOR2>              m_vToothBaryDist;       //牙齿表面距离重心的距离最大值和最小值（有序）(x:min, y:max)
	std::vector<float>                  m_vToothGap;            //牙齿的间距（有序）
	std::vector<VFVECTOR3>              m_vBarycenter;          //牙齿重心坐标（无序）
	std::vector<VFVECTOR2>              m_vProjectCoord;        //牙齿重心投影坐标(无序）
	std::vector<std::vector<double>>    m_vProbMatTemp;         //有序

	std::vector<VFVECTOR3>              m_vBndryBaryDist;
	std::vector<unsigned>               m_vMeshOrderTemp;       //用于计算相邻牙齿间距
};

class VBFDICalculatorBuilder
{
public:
    VBFDICalculatorBuilder(const VSProbInfoInput & input ):m_Input(input){}
    ~VBFDICalculatorBuilder() {}

    void Build(VNALGWXJ::VSFDIBuilderOutput & out, const VNALGWXJ::VSFDIGeneratorDep & teethMeshs)
    {
        // 根据 mesh 生成 顺序 和 概率数据
		VSFDIProbabilityMatRslt   m_FDImat;
		VBFDIProbabilityMat       m_probMat;
		m_probMat.Build(m_FDImat, teethMeshs, m_Input);

        VSConstBuffer< unsigned > order;
		order = m_FDImat.meshOrder;
        // 生成顺序后
        m_Order.Build(order);
        out.m_Order = m_Order.Get();
        
        VSFDICalInput generated;
		generated.gapMat = m_FDImat.gapMat;
		generated.probMat = m_FDImat.probMat;
		generated.tranProb = m_Input.tranProb;
        // 生成概率数据后
        m_SysDepData.Build(generated);

        // 再生成系统
        m_OutSys.reset(new VSysFDICalculator(m_SysDepData.Get()));
        out.m_essSysFDICal.pSysProv = m_OutSys.get();
    }

private:
    const VSProbInfoInput & m_Input;

    TVPersist< VSConstBuffer< unsigned > > m_Order;
    TVPersist< VSFDICalInput > m_SysDepData;
    std::shared_ptr< VSysFDICalculator > m_OutSys;
};

VD_BEGIN_PIPELINE( VPLFDICalculatorBuilder, VSProbInfoInput)
    VDDP_NOD2(FDICalculatorBuilder, VBFDICalculatorBuilder);
VD_END_PIPELINE()

class VMFDICalculatorBuilder
{
public:
    VMFDICalculatorBuilder(VPLFDICalculatorBuilder & pl
        , IVSlot< VNALGWXJ::VSFDIGeneratorDep > & slt
        , IVInputPort< VNALGWXJ::VSFDIBuilderOutput > & ip)
    {
        pl.CreateFDICalculatorBuilder(m_Builder, pl.GetEnv().Get<VSProbInfoInput>());
        VLNK(m_Builder, slt);
        VLNK(ip, m_Builder);
    }
    ~VMFDICalculatorBuilder() {}

private:
    VPLFDICalculatorBuilder::FDICalculatorBuilder m_Builder;
};

struct VSysTraitFDICalculatorBuilder
{
    typedef VPLFDICalculatorBuilder PIPELINE;
    template< typename TR > struct TRTrait;
    template<> struct TRTrait< VNALGWXJ::VRFDICalculatorBuilder > { typedef VMFDICalculatorBuilder mngr_type; };
};

using VSysFDICalculatorBuilder = TVSystemProviderImp< VSysTraitFDICalculatorBuilder, IVSYS< VNALGWXJ::VRFDICalculatorBuilder > >;

class VBFDIGeneratorBuilderCreator
{
public:
    VBFDIGeneratorBuilderCreator() {}
    ~VBFDIGeneratorBuilderCreator() {}

    void Build(VNALGWXJ::VSFDIBuilderCreatorOutput & out, const VNALGWXJ::VSFDIGeneratorCreatorDep & inputEsss)
    {
        // consturuct prob info input
		m_params.InitParam();
		m_vUpperGaussDistribution.resize(16);
		m_vLowerGaussDistribution.resize(16);
		m_vUpInvTemp.resize(16);
		m_vLowInvTemp.resize(16);
		for (unsigned i = 0; i < 16; i++)
		{
			m_vUpperGaussDistribution[i].m_Mu = VD_V2CB(m_params.m_vUpperMu[i]);
			m_vUpperGaussDistribution[i].m_Sigma = VD_V2CB(m_params.m_vUpperSigma[i]);
			m_vUpperGaussDistribution[i].m_Ptype = m_params.m_vUpperPtype[i];
			m_vUpInvTemp[i] = VD_V2CB(m_params.m_vUpperCovarMatInv[i]);

			m_vLowerGaussDistribution[i].m_Mu = VD_V2CB(m_params.m_vLowerMu[i]);
			m_vLowerGaussDistribution[i].m_Sigma = VD_V2CB(m_params.m_vLowerSigma[i]);
			m_vLowerGaussDistribution[i].m_Ptype = m_params.m_vLowerPtype[i];
			m_vLowInvTemp[i] = VD_V2CB(m_params.m_vLowerCovarMatInv[i]);
		}
		m_GapDis = VD_V2CB(m_params.m_vGapDistribution);
		m_UpperGaussDis = VD_V2CB(m_vUpperGaussDistribution);
		m_LowerGaussDis = VD_V2CB(m_vLowerGaussDistribution);
		m_TranProb.TranProbCol = VD_V2CB(m_params.m_vTranProbCol);
		m_TranProb.TranProbRow = VD_V2CB(m_params.m_vTranProbRow);
		m_UpperGaussCoeff = VD_V2CB(m_params.m_vUpperCovarMatVal);
		m_LowerGaussCoeff = VD_V2CB(m_params.m_vLowerCovarMatVal);
		m_UpperInv = VD_V2CB(m_vUpInvTemp);
		m_LowerInv = VD_V2CB(m_vLowInvTemp);

		m_upperInput.gapProb = m_GapDis;
		m_upperInput.gaussProb = m_UpperGaussDis;
		m_upperInput.tranProb = m_TranProb;
		m_upperInput.GaussCoeffVal = m_UpperGaussCoeff;
		m_upperInput.CovarMatInv = m_UpperInv;

		m_lowerInput.gapProb = m_GapDis;
		m_lowerInput.gaussProb = m_LowerGaussDis;
		m_lowerInput.tranProb = m_TranProb;
		m_lowerInput.GaussCoeffVal = m_LowerGaussCoeff;
		m_lowerInput.CovarMatInv = m_LowerInv;

		///////////////////
        m_OutSysUpper.reset(new VSysFDICalculatorBuilder(m_upperInput));
        m_OutSysLower.reset(new VSysFDICalculatorBuilder(m_lowerInput));

        out.m_FDIUpperBuilder.pSysProv = m_OutSysUpper.get();
        out.m_FDILowerBuilder.pSysProv = m_OutSysLower.get();
    }

private:
    VSProbInfoInput m_upperInput;
    VSProbInfoInput m_lowerInput;

    std::shared_ptr< VSysFDICalculatorBuilder > m_OutSysUpper;
    std::shared_ptr< VSysFDICalculatorBuilder > m_OutSysLower;

private:
	FDIparams      m_params;

	std::vector<VNWZKALG::VSFeatureGaussDistribution>     m_vUpperGaussDistribution;
	std::vector<VNWZKALG::VSFeatureGaussDistribution>     m_vLowerGaussDistribution;
	VSConstBuffer<double>                                 m_UpperGaussCoeff;
	VSConstBuffer<double>                                 m_LowerGaussCoeff;
	VNWZKALG::VSTranDistribution                          m_TranProb;
	VSConstBuffer<VNWZKALG::VSFeatureGaussDistribution>   m_UpperGaussDis;
	VSConstBuffer<VNWZKALG::VSFeatureGaussDistribution>   m_LowerGaussDis;
	VSConstBuffer<VNWZKALG::VSGapDistribution>            m_GapDis;
	std::vector<VSConstBuffer<double>>                    m_vUpInvTemp;
	std::vector<VSConstBuffer<double>>                    m_vLowInvTemp;
	VSConstBuffer<VSConstBuffer<double>>                  m_UpperInv;
	VSConstBuffer<VSConstBuffer<double>>                  m_LowerInv;
};

VD_BEGIN_PIPELINE(VPLFDIGeneratorBuilderCreator)
    VDDP_NOD2(FDIGeneratorBuilderCreator, VBFDIGeneratorBuilderCreator);
VD_END_PIPELINE()

class VMFDIGeneratorBuilderCreator
{
public:
    VMFDIGeneratorBuilderCreator(VPLFDIGeneratorBuilderCreator & pl
        , IVSlot< VNALGWXJ::VSFDIGeneratorCreatorDep > & slt
        , IVInputPort< VNALGWXJ::VSFDIBuilderCreatorOutput > & ip)
    {
        pl.CreateFDIGeneratorBuilderCreator(m_Builder);
        VLNK(m_Builder, slt);
        VLNK(ip, m_Builder);
    }
    ~VMFDIGeneratorBuilderCreator() {}

private:
    VPLFDIGeneratorBuilderCreator::FDIGeneratorBuilderCreator m_Builder;
};

struct VSysTraitFDIGeneratorBuilderCreator
{
    typedef VPLFDIGeneratorBuilderCreator PIPELINE;
    template< typename TR > struct TRTrait;
    template<> struct TRTrait< VNALGWXJ::VRFDIGeneratorCreator > { typedef VMFDIGeneratorBuilderCreator mngr_type; };
};

using VSysFDIGeneratorBuilderCreator = TVSystemProviderImp< VSysTraitFDIGeneratorBuilderCreator, IVSYS< VNALGWXJ::VRFDIGeneratorCreator > >;