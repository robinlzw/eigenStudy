#pragma once
#include <vector>
#include <alg/ialgwzk.h>

//计算牙齿FDI所需参数

class FDIparams
{
public:
	FDIparams() {}
	~FDIparams() {}

	void InitParam();

	//gauss distribution
	std::vector<std::vector<double>>           m_vUpperSigma;
	std::vector<std::vector<double>>           m_vLowerSigma;
	std::vector<std::vector<double>>           m_vUpperMu;
	std::vector<std::vector<double>>           m_vLowerMu;
	std::vector<double>                        m_vUpperPtype;
	std::vector<double>                        m_vLowerPtype;
	//tran  distribution
	std::vector<double>                        m_vTranProbCol;
	std::vector<double>                        m_vTranProbRow;
	//gap distribution
	std::vector<VNWZKALG::VSGapDistribution>   m_vGapDistribution;

	//协方差矩阵行列式的值开放乘以2pi的14次方
	std::vector<double>                        m_vUpperCovarMatVal;
	std::vector<double>                        m_vLowerCovarMatVal;

	//协方差矩阵的逆矩阵
	std::vector<std::vector<double>>           m_vUpperCovarMatInv;
	std::vector<std::vector<double>>           m_vLowerCovarMatInv;

private:
	void GaussSigma();
	void GaussMu();
	void GaussPtype();
	void TranDistribution();
	void GapDistribution();
	void CovarMatVal();
	void CovarMatInv();
};
