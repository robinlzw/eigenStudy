#pragma once
#include <vmath/vfray.h>
#include <vmath/vfmesh.h>

struct LocalCoord
{
	VSConstBuffer<VFVECTOR3>      coordX;
	VSConstBuffer<VFVECTOR3>      coordY;
	VSConstBuffer<VFVECTOR3>      coordZ;
};

class VBGenCoord
{
public:
	VBGenCoord() {}
	~VBGenCoord() {}

	void Build(LocalCoord & out, const VSConstBuffer<VFRay> & vert)
	{
		GenCoordinate(vert);

		out.coordX = VD_V2CB(m_vCoordinateX);
		out.coordY = VD_V2CB(m_vCoordinateY);
		out.coordZ = VD_V2CB(m_vCoordinateZ);
	}

private:
	void     GenCoordinate(const VSConstBuffer<VFRay>& curveVt)
	{
		m_vCoordinateX.resize(curveVt.len);
		m_vCoordinateY.resize(curveVt.len);
		m_vCoordinateZ.resize(curveVt.len);
		VFVECTOR3  temp, temp1, temp2;

		//中间点
		for (unsigned i = 1; i < curveVt.len - 1; i++)
		{
			temp1 = curveVt.pData[i].GetOrigin() - curveVt.pData[i - 1].GetOrigin();
			temp2 = curveVt.pData[i + 1].GetOrigin() - curveVt.pData[i].GetOrigin();
			temp1.Normalize();
			temp2.Normalize();
			temp = temp1 + temp2;
			temp.Normalize();
#if 0
			m_vCoordinateZ[i] = temp;
			temp = m_vCoordinateZ[i].Cross(curveVt.pData[i].GetDirection());
			temp.Normalize();
			m_vCoordinateY[i] = temp;
			temp = m_vCoordinateY[i].Cross(m_vCoordinateZ[i]);
			temp.Normalize();
			m_vCoordinateX[i] = temp;
#else
			m_vCoordinateZ[i] = temp;
			m_vCoordinateX[i] = curveVt.pData[i].GetDirection();			
			m_vCoordinateY[i] = m_vCoordinateZ[i].Cross(m_vCoordinateX[i]);
			m_vCoordinateY[i].Normalize();
			m_vCoordinateZ[i] = m_vCoordinateX[i].Cross(m_vCoordinateY[i]);
			m_vCoordinateZ[i].Normalize();			
#endif
		}
#if 0
		//两端顶点
		temp = curveVt.pData[1].GetOrigin() - curveVt.pData[0].GetOrigin();
		temp.Normalize();
		m_vCoordinateZ[0] = temp;
		temp = m_vCoordinateZ[0].Cross(curveVt.pData[0].GetDirection());
		temp.Normalize();
		m_vCoordinateY[0] = temp;
		temp = m_vCoordinateY[0].Cross(m_vCoordinateZ[0]);
		temp.Normalize();
		m_vCoordinateX[0] = temp;

		temp = curveVt.pData[curveVt.len - 1].GetOrigin() - curveVt.pData[curveVt.len - 2].GetOrigin();
		temp.Normalize();
		m_vCoordinateZ[curveVt.len - 1] = temp;
		temp = m_vCoordinateZ[curveVt.len - 1].Cross(curveVt.pData[curveVt.len - 1].GetDirection());
		temp.Normalize();
		m_vCoordinateY[curveVt.len - 1] = temp;
		temp = m_vCoordinateY[curveVt.len - 1].Cross(m_vCoordinateZ[curveVt.len - 1]);
		temp.Normalize();
		m_vCoordinateX[curveVt.len - 1] = temp;
#else
		//两端顶点
		temp = curveVt.pData[1].GetOrigin() - curveVt.pData[0].GetOrigin();
		temp.Normalize();
		m_vCoordinateZ[0] = temp;
		m_vCoordinateX[0] = curveVt.pData[0].GetDirection();
		m_vCoordinateY[0] = m_vCoordinateZ[0].Cross(m_vCoordinateX[0]);
		m_vCoordinateY[0].Normalize();
		m_vCoordinateZ[0] = m_vCoordinateX[0].Cross(m_vCoordinateY[0]);
		m_vCoordinateZ[0].Normalize();

		temp = curveVt.pData[curveVt.len - 1].GetOrigin() - curveVt.pData[curveVt.len - 2].GetOrigin();
		temp.Normalize();
		m_vCoordinateZ[curveVt.len - 1] = temp;
		m_vCoordinateX[curveVt.len - 1] = curveVt.pData[curveVt.len - 1].GetDirection();
		m_vCoordinateY[curveVt.len - 1] = m_vCoordinateZ[curveVt.len - 1].Cross(m_vCoordinateX[curveVt.len - 1]);
		m_vCoordinateY[curveVt.len - 1].Normalize();
		m_vCoordinateZ[curveVt.len - 1] = m_vCoordinateX[curveVt.len - 1].Cross(m_vCoordinateY[curveVt.len - 1]);
		m_vCoordinateZ[curveVt.len - 1].Normalize();
#endif
	}
private:
	std::vector<VFVECTOR3>     m_vCoordinateX;
	std::vector<VFVECTOR3>     m_vCoordinateY;
	std::vector<VFVECTOR3>     m_vCoordinateZ;
};
