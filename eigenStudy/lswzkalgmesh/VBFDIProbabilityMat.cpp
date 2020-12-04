#include "stdafx.h"
#include "VBFDIProbabilityMat.h"

bool comp(const std::pair<unsigned, float> & v1, const std::pair<unsigned, float> & v2)
{
    return (v1.second < v2.second);
}
bool dirComp(const std::pair<float, float> & a, const std::pair<float, float> & b )
{
    return (a.first < b.first);
}

void vf_lswzkalgmesh_get_system(VSESS< VNALGWXJ::VRFDIGeneratorCreator > & out)
{
    static VSysFDIGeneratorBuilderCreator sys;
    out.pSysProv = &sys;
}

void VBFDIProbabilityMat::ToothPlanePos(const VNALGWXJ::VSFDIGeneratorDep & meshs)
{
    //barycenter
    std::vector<VFVECTOR3>  baryTemp;
    VBMeshBarycenter     m_Barycenter;
    m_vBarycenter.resize(meshs.m_TeethMeshs.len);
    VFVECTOR3 mean = VFVECTOR3::ZERO;
    for (unsigned i = 0; i < meshs.m_TeethMeshs.len; i++)
    {
        VFVECTOR3   temp;
        m_Barycenter.Build(temp, meshs.m_TeethMeshs.pData[i]);
        m_vBarycenter[i] = temp;
        mean += temp;
    }
    baryTemp.resize(m_vBarycenter.size());
    memcpy_s(&baryTemp.front(), sizeof(VFVECTOR3) * m_vBarycenter.size(), &m_vBarycenter.front(), sizeof(VFVECTOR3) * m_vBarycenter.size());
    mean /= static_cast<float>(m_vBarycenter.size());
    for (unsigned i = 0; i < m_vBarycenter.size(); i++)
        m_vBarycenter[i] -= mean;

    //PCA
    VSConstBuffer<VFVECTOR3>    bary;
    bary = VD_V2CB(m_vBarycenter);
    VBVerticesPCA   m_pca;
    VSPCAUDVmat   pcaValue;
    m_pca.Build(pcaValue, bary);

    //coordinate direction
    VFVECTOR3 xDirect(pcaValue.U.pData[0], pcaValue.U.pData[3], pcaValue.U.pData[6]), 
              yDirect(pcaValue.U.pData[1], pcaValue.U.pData[4], pcaValue.U.pData[7]), 
              zDirect(pcaValue.U.pData[2], pcaValue.U.pData[5], pcaValue.U.pData[8]),
              xDTemp(1.f, 0.f, 0.f), yDTemp(0.f, 1.f, 0.f);
    if (xDirect.Dot(xDTemp) < 0)
        xDirect = -xDirect;
    if (yDirect.Dot(yDTemp) < 0)
        yDirect = -yDirect;

    //projection
    VFVECTOR2 temp = VFVECTOR2::ZERO;
    m_vProjectCoord.resize(m_vBarycenter.size());

    for (unsigned i = 0; i < m_vBarycenter.size(); i++)
    {
        temp.x = m_vBarycenter[i].Dot(xDirect);
        temp.y = m_vBarycenter[i].Dot(yDirect);

        m_vProjectCoord[i] = temp;
    }

    //ellipse fit
    VSConstBuffer<VFVECTOR2>   fitIpt;
    fitIpt = VD_V2CB(m_vProjectCoord);
    VBEllipsesFit    m_ecllipse;
    VSConstBuffer<float>    coeff;
    m_ecllipse.Build(coeff, fitIpt);

    VFVECTOR2 ellipseCenter = VFVECTOR2::ZERO;
    //x = (2CD - BE) / (BB - 4AC)
    ellipseCenter.x = (2.f * coeff.pData[2] * coeff.pData[4] - coeff.pData[1] * coeff.pData[4]) /
                      (coeff.pData[1] * coeff.pData[1] - 4.f * coeff.pData[0] * coeff.pData[4]);
    //y = (2AE - BD) / (BB - 4AC)
    ellipseCenter.y = (2.f * coeff.pData[0] * coeff.pData[2] - coeff.pData[1] * coeff.pData[3]) /
                      (coeff.pData[1] * coeff.pData[1] - 4.f * coeff.pData[0] * coeff.pData[2]);

    VFVECTOR2 ellipseXDirect(coeff.pData[1], coeff.pData[2] - coeff.pData[0] - sqrtf((coeff.pData[0] - coeff.pData[2]) * (coeff.pData[0] - coeff.pData[2])) + coeff.pData[1] * coeff.pData[1]);
    if (ellipseXDirect[1] < 0.f)
        ellipseXDirect = -ellipseXDirect;
    VFVECTOR2 ellipseYDirect(-ellipseXDirect[1], ellipseXDirect[0]);
    ellipseXDirect.Normalize();
    ellipseYDirect.Normalize();

    //translate project point to ellipse coordinate
    m_vEllipseCoord.resize(m_vProjectCoord.size());
    VFVECTOR2 t = VFVECTOR2::ZERO, m = VFVECTOR2::ZERO;
    for (unsigned i = 0; i < m_vProjectCoord.size(); i++)
    {
        t.x = m_vProjectCoord[i].Dot(ellipseXDirect);
        t.y = m_vProjectCoord[i].Dot(ellipseYDirect);
        m_vEllipseCoord[i] = t;
        m += m_vProjectCoord[i];
    }
    m /= (float)m_vProjectCoord.size();
    for (unsigned i = 0; i < m_vEllipseCoord.size(); i++)
        m_vEllipseCoord[i].y -= m.y;

    std::vector<std::pair<unsigned, float>>   m_vAngle;
    m_vAngle.resize(m_vEllipseCoord.size());
    for (unsigned i = 0; i < m_vEllipseCoord.size(); i++)
    {
        VFVECTOR2 angleDirect = m_vEllipseCoord[i] - ellipseCenter;
        m_vAngle[i].first = i;
        m_vAngle[i].second = atan2(angleDirect.y, angleDirect.x);
    }

    std::sort(m_vAngle.begin(), m_vAngle.end(), comp);

    m_vMeshOrder.resize(m_vEllipseCoord.size());
    m_vMeshOrderTemp.resize(m_vEllipseCoord.size());
    for (unsigned i = 0; i < m_vEllipseCoord.size(); i++)
    {
        m_vMeshOrder[m_vAngle[i].first] = i;            //原始网格的第i个在排序后网格中的位置
        m_vMeshOrderTemp[i] = m_vAngle[i].first;        //第i个位置放置的是第几个原始网格
    }
}

void VBFDIProbabilityMat::ToothFeatureCal(const VNALGWXJ::VSFDIGeneratorDep & meshs)
{
    //牙齿的表面积（有序）
    m_vSurfaceArea.resize(meshs.m_TeethMeshs.len);
    VBMeshSurfaceArea   m_area;

    //牙齿顶点主成分特征值（有序）
    m_vToothVertPCAvalue.resize(meshs.m_TeethMeshs.len);
    VBVerticesPCA    m_pca;
    VSPCAUDVmat      udv;

    //牙齿的hog特征（有序）
    VBMeshHogFeature   m_hog;
    unsigned blockNum = 20;
    m_vToothHogFeature.resize(meshs.m_TeethMeshs.len);
    for (unsigned i = 0; i < meshs.m_TeethMeshs.len; i++)
        m_vToothHogFeature[i].resize(blockNum);

    //牙齿表面距离重心的距离最大值和最小值（有序）
    m_vToothBaryDist.resize(meshs.m_TeethMeshs.len);

    for (unsigned i = 0; i < meshs.m_TeethMeshs.len; i++)
    {
        //牙齿的表面积
        float area = 0.f;
        m_area.Build(area, meshs.m_TeethMeshs.pData[ i ]);
        m_vSurfaceArea[ m_vMeshOrder[i] ] = area;

        //牙齿顶点主成分特征值
        std::vector<VFVECTOR3>      vert;
        VSConstBuffer<VFVECTOR3>    ipt;
        unsigned vlen = meshs.m_TeethMeshs.pData[ i ].nVertCount;
        vert.resize(vlen);
        memcpy_s(&vert.front(), sizeof(VFVECTOR3) * vlen, meshs.m_TeethMeshs.pData[ i ].pVertices, sizeof(VFVECTOR3) * vlen);
        VFVECTOR3 mean = VFVECTOR3::ZERO;
        for (unsigned j = 0; j < vlen; j++)
            mean += vert[j];
        mean /= static_cast<float>(vlen);
        for (unsigned j = 0; j < vlen; j++)
            vert[j] -= mean;
        ipt = VD_V2CB(vert);
        m_pca.Build(udv, ipt);

        m_vToothVertPCAvalue[ m_vMeshOrder[i] ].x = udv.D.pData[0] * udv.D.pData[0] / (float)(vlen - 1);
        m_vToothVertPCAvalue[ m_vMeshOrder[i] ].y = udv.D.pData[1] * udv.D.pData[1] / (float)(vlen - 1);
        m_vToothVertPCAvalue[ m_vMeshOrder[i] ].z = udv.D.pData[2] * udv.D.pData[2] / (float)(vlen - 1);

        //牙齿的hog特征
        /*TVExtSource< VNALGMESH::VRPerfMesh > extsysEC;
        TVR2B< VNALGMESH::VRPerfMesh >  m_PerfMesh(*extsysEC);
        VSPerfectMesh gm = m_PerfMesh.Run(meshs.m_TeethMeshs.pData[ i ]).Get<VSPerfectMesh>();*/
        VSConstBuffer<float>   hog;
        m_hog.Build(hog, meshs.m_TeethVertsCurv.pData[i], blockNum);
        memcpy_s(&m_vToothHogFeature[ m_vMeshOrder[i] ].front(), sizeof(float) * blockNum, hog.pData, sizeof(float) * blockNum);
        for (unsigned j = 0; j < blockNum; j++)
            m_vToothHogFeature[m_vMeshOrder[i]][j] *= 10.f;

        //牙齿表面距离重心的距离最大值和最小值
        std::vector<float>    m_vDist;
        m_vDist.resize(vlen);
        for (unsigned j = 0; j < vlen; j++)
            m_vDist[j] = (meshs.m_TeethMeshs.pData[i].pVertices[j] - mean).Magnitude();
        auto minmax = std::minmax_element(m_vDist.begin(), m_vDist.end());
        m_vToothBaryDist[ m_vMeshOrder[i] ].x = *minmax.first;
        m_vToothBaryDist[ m_vMeshOrder[i] ].y = *minmax.second;
    }

    //牙齿的间距（有序）
    //m_vToothGap.resize(meshs.m_TeethMeshs.len - 1);
    //VBMeshGap   m_gap;
    //for (unsigned i = 0; i < m_vMeshOrder.size() - 1; i++)
    //{
    //    /*float dist = 0.f;
    //    m_gap.Build(dist, meshs.m_TeethMeshs.pData[ m_vMeshOrder[i] ], meshs.m_TeethMeshs.pData[ m_vMeshOrder[i + 1] ]);
    //    
    //    m_vToothGap[ i ] = dist;*/
    //    
    //    float dist = 0.f;
    //    m_gap.Build(dist, meshs.m_TeethMeshs.pData[m_vMeshOrderTemp[i]], meshs.m_TeethMeshs.pData[m_vMeshOrderTemp[i + 1]]);
    //    
    //    m_vToothGap[i] = dist;
    //}

    m_vToothGap.resize(m_vMeshOrder.size() * m_vMeshOrder.size(), 0.f);
    VBMeshGap   m_gap;
    for (unsigned i = 0; i < m_vMeshOrder.size(); i++)
    {
        for (unsigned j = 0; j < i; j++)
        {
            float dist = 0.f;
            m_gap.Build(dist, meshs.m_TeethMeshs.pData[ m_vMeshOrderTemp[ i ] ], meshs.m_TeethMeshs.pData[ m_vMeshOrderTemp[ j ] ]);

            m_vToothGap[ i * m_vMeshOrder.size() + j ] = dist;
        }
    }
}

void VBFDIProbabilityMat::ProbMatCal(const VNALGWXJ::VSFDIGeneratorDep & meshs, const VSProbInfoInput & distribuData)
{
    ToothPlanePos(meshs);
    ToothFeatureCal(meshs);

    m_vProbMatTemp.resize(meshs.m_TeethMeshs.len);
    for (unsigned i = 0; i < meshs.m_TeethMeshs.len; i++)
        m_vProbMatTemp[i].resize(16);
    std::vector<float>   feature;
    feature.resize(28);
    std::vector<double>   m_vProbSum;
    m_vProbSum.resize(meshs.m_TeethMeshs.len);
    for (unsigned i = 0; i < meshs.m_TeethMeshs.len; i++)
    {
        SingleToothFeature(feature, i);

        double probSum = 0.0;
        for (unsigned j = 0; j < 16; j++)
        {
            double sum = 0.0;
            for (unsigned k = 0; k < 28; k++)
            {
                double  ss = 0.;
                for (unsigned m = 0; m < 28; m++)
                {
                    double xmu = feature[m] - distribuData.gaussProb.pData[j].m_Mu.pData[m];
                    double cinv = distribuData.CovarMatInv.pData[j].pData[m * 28 + k];          //colume
                     ss += (xmu * cinv);
                }
                sum += (ss * (feature[k] - distribuData.gaussProb.pData[j].m_Mu.pData[k]));
            }
            sum *= 0.5;
            m_vProbMatTemp[i][j] = distribuData.gaussProb.pData[j].m_Ptype * (exp(-sum) / distribuData.GaussCoeffVal.pData[j]);
            probSum += m_vProbMatTemp[i][j];
        }
        m_vProbSum[i] = probSum;
    }
    m_vProbMat.resize(m_vProbMatTemp.size() * 16);
    for (unsigned i = 0; i < m_vProbSum.size(); i++)
    {
        for (unsigned j = 0; j < 16; j++)
        {
            /*m_vProbMatTemp[ m_vMeshOrder[i] ][j] /= m_vProbSum[ m_vMeshOrder[i] ];
            if (m_vProbMatTemp[ m_vMeshOrder[i] ][j] < 1e-9)
                m_vProbMatTemp[ m_vMeshOrder[i] ][j] = 1e-9;
            m_vProbMatTemp[ m_vMeshOrder[i] ][j] = log(m_vProbMatTemp[ m_vMeshOrder[i] ][j]);
            m_vProbMat[m_vMeshOrder[i] * 16 + j] = m_vProbMatTemp[ m_vMeshOrder[i] ][j];*/

            m_vProbMatTemp[i][j] /= m_vProbSum[i];
            if (m_vProbMatTemp[i][j] < 1e-9)
                m_vProbMatTemp[i][j] = 1e-9;
            m_vProbMatTemp[i][j] = log(m_vProbMatTemp[i][j]);
            m_vProbMat[m_vMeshOrder[i] * 16 + j] = m_vProbMatTemp[i][j];
        }
    }

    m_vGapMat.resize(m_vToothGap.size(), 0.0);
    for (unsigned i = 0; i < m_vMeshOrder.size(); i++)
    {
        for (unsigned j = 0; j < i; j++)
        {
            float gapVal = m_vToothGap[ i * m_vMeshOrder.size() + j ];
            unsigned miss0Idx = j * m_vMeshOrder.size() + i;
            unsigned miss1Idx = i * m_vMeshOrder.size() + j;

            double sumGap = 0.0;
            double  sigmaD = distribuData.gapProb.pData[ 0 ].m_Sigma * distribuData.gapProb.pData[ 0 ].m_Sigma;
            double  muD = (gapVal - distribuData.gapProb.pData[ 0 ].m_Mu) * (gapVal - distribuData.gapProb.pData[ 0 ].m_Mu);
            double probMiss0 = distribuData.gapProb.pData[ 0 ].m_Ptype * exp(-muD / (2. * sigmaD)) / sqrt(2. * VD_PI * sigmaD);
            sumGap += probMiss0;

            sigmaD = distribuData.gapProb.pData[ 1 ].m_Sigma * distribuData.gapProb.pData[ 1 ].m_Sigma;
            muD = (gapVal - distribuData.gapProb.pData[ 1 ].m_Mu) * (gapVal - distribuData.gapProb.pData[ 1 ].m_Mu);
            double probMiss1 = 0.8 * exp(-muD / (2. * sigmaD)) / sqrt(2. * VD_PI * sigmaD);
            if (gapVal > 1.f && gapVal < 11.f)
                probMiss1 = probMiss1 + 0.2 * 0.1;
            probMiss1 *= distribuData.gapProb.pData[ 1 ].m_Ptype;
            sumGap += probMiss1;
            
            probMiss0 /= sumGap;
            probMiss1 /= sumGap;

            m_vGapMat[ miss0Idx ] = log(probMiss0 < 1e-9 ? 1e-9 : probMiss0);
            m_vGapMat[ miss1Idx ] = log(probMiss1 < 1e-9 ? 1e-9 : probMiss1);
        }
    }

    m_vTranRow.resize(distribuData.tranProb.TranProbRow.len);
    for (unsigned i = 0; i < distribuData.tranProb.TranProbRow.len; i++)
        m_vTranRow[i] = log(distribuData.tranProb.TranProbRow.pData[i]);
    m_vTranCol.resize(distribuData.tranProb.TranProbCol.len);
    for (unsigned i = 0; i < distribuData.tranProb.TranProbCol.len; i++)
        m_vTranCol[i] = log(distribuData.tranProb.TranProbCol.pData[i]);
}

void VBFDIProbabilityMat::SingleToothFeature(std::vector<float>& vec, unsigned pos)
{
    vec[0] = m_vEllipseCoord[ pos ].x;
    vec[1] = m_vEllipseCoord[ pos ].y;

    vec[2] = m_vSurfaceArea[ m_vMeshOrder[pos] ];

    vec[3] = m_vToothVertPCAvalue[ m_vMeshOrder[pos] ].x;
    vec[4] = m_vToothVertPCAvalue[ m_vMeshOrder[pos] ].y;
    vec[5] = m_vToothVertPCAvalue[ m_vMeshOrder[pos] ].z;

    for (unsigned i = 0; i < 20; i++)
        vec[i + 6] = m_vToothHogFeature[ m_vMeshOrder[pos] ][i];

    vec[26] = m_vToothBaryDist[ m_vMeshOrder[pos] ].x;
    vec[27] = m_vToothBaryDist[ m_vMeshOrder[pos] ].y;
}

//void VBFDIProbabilityMat::CoordinateTran(VFVECTOR3 & xDirect, VFVECTOR3 & yDirect, VFVECTOR3 & zDirect, const VNALGWXJ::VSFDIGeneratorDep & meshs, const std::vector<VFVECTOR3>& bary, const unsigned & isUpper)
//{
//	std::vector<VFVECTOR3>  m_vBndryCenter;    //boundary vert
//	m_vBndryCenter.resize(meshs.m_TeethMeshs.len);
//	for (unsigned i = 0; i < meshs.m_TeethMeshs.len; i++)
//	{
//		TVExtSource< VNALGMESH::VRPerfMesh > extsysEC;
//		TVR2B< VNALGMESH::VRPerfMesh >  m_PerfMesh(*extsysEC);
//		VSPerfectMesh gm = m_PerfMesh.Run(meshs.m_TeethMeshs.pData[i]).Get<VSPerfectMesh>();
//
//		VNWZKALG::VSHoleBoundary  Bndry;
//		VBFindHole    m_hole;
//		m_hole.Build(Bndry, gm);
//
//		VFVECTOR3   bndryCenter = VFVECTOR3::ZERO;
//		unsigned bLen = Bndry.bndry.pData[0].orderedVert.len;
//		for (unsigned j = 0; j < bLen; j++)
//			bndryCenter += meshs.m_TeethMeshs.pData[i].pVertices[Bndry.bndry.pData[0].orderedVert.pData[j]];
//		bndryCenter /= static_cast<float>(bLen);
//		m_vBndryCenter[i] = bndryCenter;
//	}
//
//	VFVECTOR3 sum = VFVECTOR3::ZERO;
//	for (unsigned i = 0; i < m_vBndryCenter.size(); i++)
//	{
//		m_vBndryCenter[i] = bary[i] - m_vBndryCenter[i];
//		m_vBndryCenter[i].Normalize();
//
//		sum += m_vBndryCenter[i];
//	}
//	VFVECTOR3 wDirect = sum / sum.Magnitude();
//
//	// z direction
//	if (isUpper == 0)
//	{
//		if (wDirect.Dot(zDirect) > 0.f)
//			zDirect = -zDirect;
//	}
//	else
//	{
//		if (wDirect.Dot(zDirect) < 0.f)
//			zDirect = -zDirect;
//	}
//
//	//x direction
//	std::pair<unsigned, float>   maxProj(0, -FLT_MAX), minProj(0, FLT_MAX);
//	for (unsigned i = 0; i < bary.size(); i++)
//	{
//		float projLen = bary[i].Dot(xDirect);
//		if (maxProj.second < projLen)
//		{
//			maxProj.first = i;
//			maxProj.second = projLen;
//		}
//		if (minProj.second > projLen)
//		{
//			minProj.first = i;
//			minProj.second = projLen;
//		}
//	}
//	if (abs(bary[maxProj.first].y) > abs(bary[minProj.first].y))
//		xDirect = -xDirect;
//
//	yDirect = zDirect.Cross(xDirect);
//}

