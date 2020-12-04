#include "stdafx.h"
#include "VBFDINameCal.h"

unsigned VBFDINameCal::SupernumerraryCheck(const VSConstBuffer<VNALGWXJ::VEFDI>& initInfo)
{
	unsigned count = 0;
	for (unsigned i = 0; i < initInfo.len; i++)
	{
		if (initInfo.pData[i] == VNALGWXJ::FDI_Supernumerary_Tooth)
		{
			m_vSupernumerrary.push_back(i);
			count++;
		}
	}

	return count;
}

void VBFDINameCal::FixedToothCheck(std::vector<VNVECTOR2UI>& vec, const VSConstBuffer<VNALGWXJ::VEFDI>& initInfo)
{
	VNVECTOR2UI temp;
	for (unsigned i = 0; i < initInfo.len; i++)
	{
		if (initInfo.pData[i] >= VNALGWXJ::FDIL1 && initInfo.pData[i] <= VNALGWXJ::FDIR8 || initInfo.pData[i] == VNALGWXJ::FDI_Supernumerary_Tooth)
		{
			temp.x = i;                    //pos
			temp.y = initInfo.pData[i];    //FDI name
			vec.push_back(temp);
		}
	}
}

int VBFDINameCal::Filter(const std::vector<VNVECTOR2UI> & fixedTooth, const std::vector<unsigned>& permuVec)
{
	//排除不符合要求的组合
	for (unsigned i = 0; i < permuVec.size() - 1; i++)
	{
		if (permuVec[i] == 1 && permuVec[i + 1] == 1)
			return 1;
	}
	//将有牙齿的位置顺序填充
	memcpy_s(&m_vPermuTemp.front(), sizeof(unsigned) * 16, &permuVec.front(), sizeof(unsigned) * 16);
	unsigned count = 0;
	for (unsigned i = 0; i < permuVec.size(); i++)
	{
		if (permuVec[i] == 0)
		{
			m_vPermuTemp[i] = count;
			count ++;
		}
		else
			m_vPermuTemp[i] = 20;              //表示缺牙
	}

	if (fixedTooth.empty())  //未指定牙齿名称
	{
		double temp = ProbCal(m_vPermuTemp);
		//ttt.push_back(temp);
		if (prob < temp)
		{ 
			prob = temp;
			memcpy_s(&m_vPermu.front(), sizeof(unsigned) * m_vPermuTemp.size(), &m_vPermuTemp.front(), sizeof(unsigned) * m_vPermuTemp.size());
			return 0;
		}
	}
	else   //指定牙齿名称
	{
		//排除不符合指定名称的组合情况
		//for (unsigned i = 0; i < fixedTooth.size(); i++)
		//{
		//	unsigned remainToothNum = count - fixedTooth[i].x - 1, num = 0, permuRemain = 0;
		//	unsigned preNum = fixedTooth[i].x;
		//	for (unsigned j = 0; j < m_vPermuTemp.size(); j++)
		//	{
		//		if (m_vPermuTemp[j] != 20 && num <= preNum)
		//		{
		//			num++;
		//			continue;
		//		}
		//		//if (num > 0 && (num - 1) == preNum && m_vPermuTemp[j] != 20)
		//		//	permuRemain++;
		//		if (num > 0)
		//		{
		//			permuRemain = count - num;             //指定所在位置牙位之后实际的牙齿数量（不包括缺牙位）
		//			unsigned remainTotalNum = 16 - j;      //当前所在位置之后实际的牙齿数量（包括缺牙位）
		//			unsigned maxNum = 0;                   //指定所在位置牙位之后允许的最大牙齿数量（包括缺牙位）
		//			if (fixedTooth[i].y > VNALGWXJ::FDIR1)
		//				maxNum = 17 - fixedTooth[i].y;
		//			else
		//				maxNum = 8 + fixedTooth[i].y - 2;
		//			if (remainToothNum > maxNum || permuRemain != remainToothNum)
		//				return 1;
		//		}
		//	}
		//	//if (permuRemain != remainToothNum)
		//	//	return 1;
		//}

		double temp = ProbCal(m_vPermuTemp);
		if (prob < temp)
		{
			prob = temp;
			memcpy_s(&m_vPermu.front(), sizeof(unsigned) * m_vPermuTemp.size(), &m_vPermuTemp.front(), sizeof(unsigned) * m_vPermuTemp.size());
			if (m_vFDInameModify.size() != 0)
				memcpy_s(&m_vModifyTemp.front(), sizeof(VNALGWXJ::VEFDI) * m_vFDInameModify.size(), &m_vFDInameModify.front(), sizeof(VNALGWXJ::VEFDI) * m_vFDInameModify.size());
			FDIModify(supNum, toothNum);
			for (unsigned i = 0; i < fixedTooth.size(); i++)
			{
				if (fixedTooth[i].y != m_vFDInameModify[fixedTooth[i].x])
				{
					memcpy_s(&m_vFDInameModify.front(), sizeof(VNALGWXJ::VEFDI) * m_vFDInameModify.size(), &m_vModifyTemp.front(), sizeof(VNALGWXJ::VEFDI) * m_vFDInameModify.size());
					return 1;
				}
			}
			return 0;
		}
	}
	
	return 1;
}

int VBFDINameCal::FilterEnds(const std::vector<VNVECTOR2UI>& fixedTooth, unsigned emptyNum)
{
	double sum1 = -FLT_MAX, sum2 = -FLT_MAX;
	std::vector<unsigned>    m_PermuSpecial1, m_PermuSpecial2;
	m_PermuSpecial1.resize(16);
	m_PermuSpecial2.resize(16);
	for (unsigned i = 0; i < emptyNum; i++)
	{
		m_PermuSpecial1[i] = 20;
		m_PermuSpecial2[16 - emptyNum + i] = 20;
	}
	for (unsigned i = emptyNum; i < 16; i++)
	{
		m_PermuSpecial1[i] = i - emptyNum;
		m_PermuSpecial2[i - emptyNum] = i - emptyNum;
	}

	if (fixedTooth.empty())    //未指定牙齿名称
	{
		sum1 = ProbCal(m_PermuSpecial1);
		sum2 = ProbCal(m_PermuSpecial2);
		if (sum1 < sum2 && sum2 > prob)
		{
			prob = sum2;
			memcpy_s(&m_vPermu.front(), sizeof(unsigned) * m_vPermu.size(), &m_PermuSpecial2.front(), sizeof(unsigned) * m_vPermu.size());
			return 0;
		}
		if (sum1 > sum2 && sum1 > prob)
		{
			prob = sum1;
			memcpy_s(&m_vPermu.front(), sizeof(unsigned) * m_vPermu.size(), &m_PermuSpecial1.front(), sizeof(unsigned) * m_vPermu.size());
			return 0;
		}
	}
	else
	{
		bool flag1 = true, flag2 = true;
		for (unsigned i = 0; i < fixedTooth.size(); i++)   //排除不符合指定名称的组合情况
		{
			if ((fixedTooth[i].y - 2) != m_PermuSpecial1[fixedTooth[i].x] && 
				(fixedTooth[i].y - 2) != m_PermuSpecial1[fixedTooth[i].x]  )    //指定位置牙齿名称与组合中相同位置的名称不相同
				return 1;
			if ((fixedTooth[i].y - 2) != m_PermuSpecial1[fixedTooth[i].x])
				flag1 = false;
			if ((fixedTooth[i].y - 2) != m_PermuSpecial1[fixedTooth[i].x])
				flag2 = false;
		}
		if (flag1)
			sum1 = ProbCal(m_PermuSpecial1);
		if (flag2)
			sum2 = ProbCal(m_PermuSpecial2);
		if (sum1 < sum2 && sum2 > prob)
		{
			prob = sum2;
			memcpy_s(&m_vPermu.front(), sizeof(unsigned) * m_vPermu.size(), &m_PermuSpecial2.front(), sizeof(unsigned) * m_vPermu.size());
			return 0;
		}
		if (sum1 > sum2 && sum1 > prob)
		{
			prob = sum1;
			memcpy_s(&m_vPermu.front(), sizeof(unsigned) * m_vPermu.size(), &m_PermuSpecial1.front(), sizeof(unsigned) * m_vPermu.size());
			return 0;
		}
	}
	return 1;
}

double VBFDINameCal::ProbCal(const std::vector<unsigned>& permuVec)
{
	num++;
	double typeP = 0., gapP = 0., tranR = 0., tranC = 0.;
	unsigned count = 0;
	//类型概率
	for (unsigned i = 0; i < permuVec.size(); i++)
	{
		if (permuVec[i] == 20)
		{
			count++;
			continue;
		}

		double a1 = m_ProbMat.probMat.pData[(i - count) * 16 + i];

		if (permuVec[i] < 20)
			typeP += m_ProbMat.probMat.pData[(i - count) * 16 + i];
	}

	//间距概率
	count = 0;
	unsigned len = permuVec.size() - 1;
	for (unsigned i = 0; i < permuVec.size() - 1; i++)
	{
		if (permuVec[i] == 20)
		{
			count++;
			continue;
		}
		if (i == (len - 1) && permuVec[len] == 20)    //倒数第二颗牙缺失，计入缺牙个数，防止间隙概率取值越界
			count++;

		double a1 = m_ProbMat.gapMat.pData[(i - count) * 2 + 1];
		double a0 = m_ProbMat.gapMat.pData[(i - count) * 2 + 0];

		if (permuVec[i] < 20 && permuVec[i + 1] < 20)
			gapP += m_ProbMat.gapMat.pData[(i - count) * 2 + 0];       //间隙概率不考虑缺牙位置
		else if(permuVec[i] < 20 && permuVec[i + 1] == 20)
			gapP += m_ProbMat.gapMat.pData[(i - count) * 2 + 1];
	}

	//转换概率 行
	for (unsigned i = 1; i < permuVec.size(); i++)
	{
		if (permuVec[i] == 20)
			continue;

		double a = m_ProbMat.tranProb.TranProbRow.pData[16 + i];
		double b = m_ProbMat.tranProb.TranProbRow.pData[i];

		if (permuVec[i] < 20 && permuVec[i - 1] < 20)
			tranR += m_ProbMat.tranProb.TranProbRow.pData[ i ];
		else if(permuVec[i] < 20 && permuVec[i - 1] == 20)
			tranR += m_ProbMat.tranProb.TranProbRow.pData[ 16 + i ];
	}

	//转换概率 列
	for (unsigned i = 0; i < permuVec.size() - 1; i++)
	{
		if (permuVec[i] == 20)
			continue;

		double a1 = m_ProbMat.tranProb.TranProbCol.pData[i * 2 + 1];
		double a0 = m_ProbMat.tranProb.TranProbCol.pData[i * 2 + 0];

		if (permuVec[i] < 20 && permuVec[i + 1] < 20)
			tranC += m_ProbMat.tranProb.TranProbCol.pData[i * 2 + 0];
		else if (permuVec[i] < 20 && permuVec[i + 1] == 20)
			tranC += m_ProbMat.tranProb.TranProbCol.pData[i * 2 + 1];
	}

	return .5 * typeP + gapP + tranR + tranC;
}

bool VBFDINameCal::FDInameCalc(const VSConstBuffer<VNALGWXJ::VEFDI>& initInfo)
{
	std::vector<unsigned>    m_permuV;    //排列
	FixedToothCheck(m_vFixedTooth, initInfo);
	supNum = SupernumerraryCheck(initInfo);
	toothNum = initInfo.len - supNum;

	if (toothNum > 16 || toothNum < 8)
		return false;

	if (toothNum == 16)     //不缺牙
	{
		FullTooth(supNum);
		return true;
	}

	m_permuV.resize(16);
	m_vPermuTemp.resize(16);
	unsigned emptyNum = 16 - toothNum;
	for (unsigned i = 0; i < emptyNum; i++)
		m_permuV[i] = 1;                        //缺牙位置设为1

	num = 0;
	int flag = 0, res = 1;
	prob = -FLT_MAX;
	m_vPermu.resize(m_permuV.size());
	m_vFDIname.resize(m_permuV.size());
	m_vModifyTemp.resize(initInfo.len);
	do
	{
		flag = Filter(m_vFixedTooth, m_permuV);
		if (flag == 0)
			res = 0;
	} while (prev_permutation(m_permuV.begin(), m_permuV.end()));

	flag = FilterEnds(m_vFixedTooth, emptyNum);
	if (flag == 0)
		res = 0;

	if (res == 0)
	{
		if (m_vFixedTooth.size() == 0)
			FDIModify(supNum, toothNum);
		//FDIModify(supNum, toothNum);
		return true;
	}

	return false;
}

void VBFDINameCal::FDIModify(unsigned supNum, unsigned toothNum)
{
	unsigned nLen = m_vPermu.size(), count = 0;
	for (unsigned i = 0; i < 8; i++)   //左侧
	{
		m_vFDIname[i] = VNALGWXJ::FDI_NONE;
		count++;
		if (m_vPermu[i] < 20)
		{
			*(int*)&m_vFDIname[i] = VNALGWXJ::FDIL8 - count + 1;
			break;
		}
	}
	for (unsigned i = count; i < 8; i++)
	{
		if (m_vPermu[i] < 20 && m_vFDIname[i - 1] != VNALGWXJ::FDI_NONE)
			*(int*)&(m_vFDIname[i]) = m_vFDIname[i - 1] - 1;
		if (m_vPermu[i] < 20 && m_vFDIname[i - 1] == VNALGWXJ::FDI_NONE && i > 1)
			*(int*)&(m_vFDIname[i]) = m_vFDIname[i - 2] - 2;
		if (m_vPermu[i] < 20 && m_vFDIname[i - 1] == VNALGWXJ::FDI_NONE && i == 1)
			*(int*)&(m_vFDIname[i]) = VNALGWXJ::FDIL7;
		if (m_vPermu[i] == 20)
			m_vFDIname[i] = VNALGWXJ::FDI_NONE;
	}

	count = 0;
	for (unsigned i = 8; i < 16; i++)   //右侧
	{
		m_vFDIname[i] = VNALGWXJ::FDI_NONE;
		count++;
		if (m_vPermu[i] < 20)
		{
			*(int*)&m_vFDIname[i] = VNALGWXJ::FDIR1 - count + 1;
			break;
		}
	}
	for (unsigned i = count + 8; i < 16; i++)
	{
		if (m_vPermu[i] < 20 && m_vFDIname[i - 1] != VNALGWXJ::FDI_NONE)
			*(int*)&(m_vFDIname[i]) = m_vFDIname[i - 1] + 1;
		if (m_vPermu[i] < 20 && m_vFDIname[i - 1] == VNALGWXJ::FDI_NONE && i > 9)
			*(int*)&(m_vFDIname[i]) = m_vFDIname[i - 2] + 2;
		if (m_vPermu[i] < 20 && m_vFDIname[i - 1] == VNALGWXJ::FDI_NONE && i == 9)
			*(int*)&(m_vFDIname[i]) = VNALGWXJ::FDIR2;
		if (m_vPermu[i] == 20)
			m_vFDIname[i] = VNALGWXJ::FDI_NONE;
	}

	m_vFDInameModify.resize(0);
	m_vFDInameModify.reserve(toothNum + supNum);
	count = 0;
	for (unsigned i = 0; i < 16; i++)
	{
		if (supNum > 0 && count < m_vSupernumerrary.size() && (i == m_vSupernumerrary[count]))
		{
			m_vFDInameModify.push_back(VNALGWXJ::FDI_Supernumerary_Tooth);
			count ++;
		}
		else if (m_vFDIname[i] != VNALGWXJ::FDI_NONE)
			m_vFDInameModify.push_back(m_vFDIname[i]);
	}
}

void VBFDINameCal::FullTooth(unsigned supNum)
{
	m_vFDIname.resize(16);
	m_vFDIname[0] = VNALGWXJ::FDIL8;   //左侧
	for (unsigned i = 1; i < 8; i++)
		*(int*)&m_vFDIname[i] = m_vFDIname[i - 1] - 1;

	m_vFDIname[8] = VNALGWXJ::FDIR1;   //右侧
	for (unsigned i = 9; i < 16; i++)
		*(int*)&m_vFDIname[i] = m_vFDIname[i - 1] + 1;

	unsigned count = 0;
	if (supNum > 0)
	{
		m_vFDInameModify.resize(16 + supNum);
		for (unsigned i = 0; i < 16; i++)
		{
			if (i == m_vSupernumerrary[count])
			{
				count++;
				m_vFDInameModify[i + count] = VNALGWXJ::FDI_Supernumerary_Tooth;
			}
			else
				m_vFDInameModify[i + count] = m_vFDIname[i];
		}
	}
	else
	{
		m_vFDInameModify.resize(16);
		memcpy_s(&m_vFDInameModify.front(), sizeof(VNALGWXJ::FDI_NONE) * 16, &m_vFDIname.front(), sizeof(VNALGWXJ::FDI_NONE) * 16);
	}
}
