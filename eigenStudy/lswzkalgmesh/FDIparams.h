#pragma once
#include <vector>
#include <alg/ialgwzk.h>

//��������FDI�������

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

	//Э�����������ʽ��ֵ���ų���2pi��14�η�
	std::vector<double>                        m_vUpperCovarMatVal;
	std::vector<double>                        m_vLowerCovarMatVal;

	//Э�������������
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
