#include "stdafx.h"
#include "VBRangeEncode.h"

void VBRangeEncode::RangeEncode(const VSConstBuffer<unsigned>& value, unsigned RangeSize)
{
	btree.Build(Range, RangeSize);

	for (unsigned i = 0; i < value.len; i++)
	{
		Encode(value.pData[i]);
	}

	FinishEncode(d, n, L);
}

void VBRangeEncode::Encode(unsigned symbol)
{	
	unsigned Fs = btree.GetCumulativeFrequency(symbol);
	unsigned fs = btree.GetCumulativeFrequency(symbol + 1) - Fs;
	
	/*if (1)
	{
		unsigned temp = R;
		long long unsigned r = (long long unsigned)R * (long long unsigned)fs;
		R = (unsigned)(r / (long long unsigned)btree.GetTotalFreq());
		L = L + temp / btree.GetTotalFreq() * Fs;
	}
	else
	{
		R = R / btree.GetTotalFreq();
		L = L + R * Fs;
		R = R * fs;
	}*/
	
	R = R / btree.GetTotalFreq();
	L = L + R * Fs;
	R = R * fs;
	while (R <= Rmin)
	{
		H = L + R - 1;
		if (n != 0)     //���ӳ�����
		{
			if (H <= Rmax)         //H<0����������
			{
				m_vCompress.push_back(d);
				for (unsigned i = 0; i < n - 1; i++)
				{
					m_vCompress.push_back(0xFF);
				}
				n = 0;
				L = L + Rmax;
			}
			else if (L >= Rmax)      //L>0����������
			{
				m_vCompress.push_back(d + 1);
				for (unsigned i = 0; i < n - 1; i++)
				{
					m_vCompress.push_back(0x00);
				}
				n = 0;
			}
			else      //����δ��
			{
				n ++;
				//��չ����
				L = (L << 8) & (Rmax - 1);      //L = L * b % Rmax
				R = R << 8;                     //R = R * b
				continue;
			}
			//continue;
		}

		if (((L ^ H) & (0xff << 23)) == 0)
		{
			m_vCompress.push_back(L >> 23 & 0x000000FF);        //��������
		}
		else    //�ӳ�����
		{
			L = L - Rmax;
			d = L >> 23;
			n = 1;
		}

		//��չ����
		L = (L << 8) & (Rmax - 1) | (L & Rmax);   //L = L * b % Rmax, if L < Rmax, shift L to [L + Rmax, H + Rmax - 1]
		R = R << 8;                               //R = R * b
	}
	btree.UpdateFreq(symbol);
}

void VBRangeEncode::FinishEncode(unsigned d, unsigned n, unsigned L)
{
	//����ӳ�����
	if (n != 0)
	{
		if (L < Rmax)   //��������
		{
			m_vCompress.push_back(d);
			for (unsigned i = 0; i < n - 1; i++)
			{
				m_vCompress.push_back(0xFF);
			}
		}
		else     //��������
		{
			m_vCompress.push_back(d + 1);
			for (unsigned i = 0; i < n - 1; i++)
			{
				m_vCompress.push_back(0x00);
			}
		}
	}

	//���ʣ�����
	L = L << 1;
	unsigned temp = 32;
	while (temp > 0)
	{
		temp -= 8;
		m_vCompress.push_back((L >> temp) & 0x000000FF);
	}
}