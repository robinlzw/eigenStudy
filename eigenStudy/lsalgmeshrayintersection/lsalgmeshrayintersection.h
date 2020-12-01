#pragma once
#include "LibStatic/include/objreference.h"
#include "../include/algmeshrayintersection.h"

#include "vmath/vfvector4.h"
#include "vmath/vfray.h"
#include "obj/iglnew/VGlEnvCreator.h"





namespace NMALG_MESHRAYINTERSECTION
{
	class VSMeshRayIntersection:public NM_COMMON::VObjRef, public IVMeshRayIntersection
	{
	public:
		VSMeshRayIntersection(const unsigned nGuid);
		virtual ~VSMeshRayIntersection();

		virtual void Build(VSMeshRayIntersectOutput& output, const VSMeshRayIntersectInput& input);

	private:
		void ReSet(VSMeshRayIntersectOutput& output);
		void CalcInsert(const VSSimpleMeshF& mesh, const VSConstBuffer<VFRay>& rays);
		void Init(const VSSimpleMeshF& mesh, const VSConstBuffer<VFRay>& rays);
		bool GetProgram();
		void GetShaderSource();
		void GenBuffer();
		void GetResult();
		void SortResult(std::vector<float>& len, std::vector<unsigned>& idx, bool greater = true);
	private:
		VGlEnvCreator<4, 5> gl_env_;

		GLuint handle_program_;
		GLuint ssbo_verts_;
		GLuint ssbo_surfs_;
		GLuint ssbo_rays_;
		GLuint ssbo_result_;
		std::string shader_compute_src_;

		VNVECTOR3UI cs_group_size_;
		VNVECTOR3UI cs_block_size_;
		const unsigned max_group_x_;
		const unsigned max_group_y_;

		std::vector<VFVECTOR4>   ori_rays_;
		std::vector<VFVECTOR4>   ori_verts_;
		std::vector<VNVECTOR4UI> ori_surfs_;

		std::vector<std::vector<float>>    dst_len_;
		std::vector<std::vector<unsigned>> dst_idx_;
		std::vector<std::vector<float>>    dst_len_op_;
		std::vector<std::vector<unsigned>> dst_idx_op_;

		std::vector<VSConstBuffer<float>>    out_dst_len_;
		std::vector<VSConstBuffer<unsigned>> out_dst_idx_;
		std::vector<VSConstBuffer<float>>    out_dst_len_op_;
		std::vector<VSConstBuffer<unsigned>> out_dst_idx_op_;
	};
}
