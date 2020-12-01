#pragma once
#include <LibStatic/include/objreference.h>
#include "../include/algmeshrayintersection.h"

#include "vmath/vfvector4.h"
#include "vmath/vfray.h"

namespace NMALG_MESHRAYINTERSECTION2
{
	class VCRayMeshIntersection2 : public NM_COMMON::VObjRef, public IVMeshRayIntersection
	{
	public:
		VCRayMeshIntersection2(const unsigned nGuid);
		virtual ~VCRayMeshIntersection2();

		virtual void Build(VSMeshRayIntersectOutput& output, const VSMeshRayIntersectInput& input);


	private:
		void CalcInsert(const VSSimpleMeshF& mesh, const VSConstBuffer<VFRay>& rays);
	private:
		std::vector<std::vector<float> >		m_vvRayLen;		// ���������������񽻵㣺ray.origin + ray.direction * len
		std::vector<std::vector<float> >		m_vvOpRayLen;	// ���߷����������񽻵㣺ray.origin + ray.direction * len
		std::vector<std::vector<unsigned> >		m_vvSurfIdx;	// �����������������ཻ������Ƭ����cbRayLenһһ��Ӧ
		std::vector<std::vector<unsigned> >		m_vvOpSurfIdx;	// ���߷������������ཻ������Ƭ����cbOpRayLenһһ��Ӧ

		std::vector<VSConstBuffer<float> >		m_vCbRayLen;		
		std::vector<VSConstBuffer<float> >		m_vCbOpRayLen; 
		std::vector<VSConstBuffer<unsigned> >	m_vCbSurfIdx;
		std::vector<VSConstBuffer<unsigned> >	m_vCbOpSurfIdx;
	};
}
