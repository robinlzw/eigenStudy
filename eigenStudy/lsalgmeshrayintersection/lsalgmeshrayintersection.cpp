#include "stdafx.h"
#include "lsalgmeshrayintersection.h"
#include "interface/iobjreference.h"
#include "obj/lsalgtools/profile.h"
//#define RUNTIME_TEST
#ifdef RUNTIME_TEST
#ifdef VD_F_PROFILE_START
#undef  VD_F_PROFILE_START
#endif
#ifdef VD_F_PROFILE_FINISH
#undef  VD_F_PROFILE_FINISH
#endif
#define VD_F_PROFILE_START( profile ) VSFProfile::StartProfile(#profile,"E:\\testdata\\profile.txt");
#define VD_F_PROFILE_FINISH( profile ) VSFProfile::FinishProfile(#profile,"E:\\testdata\\profile.txt");
#endif

namespace NMALG_MESHRAYINTERSECTION
{
#pragma comment(lib,"Opengl32.lib")

	IVObject* GetGenerator(const unsigned nGuid)
	{
		return static_cast<IVMeshRayIntersection*>(new VSMeshRayIntersection(nGuid));
	}

	VSMeshRayIntersection::VSMeshRayIntersection(const unsigned nGuid)
		: NM_COMMON::VObjRef(nGuid)
		, cs_group_size_(1, 1, 1)
		, cs_block_size_((1 << 6), 1, 1)
		, max_group_x_((1 << 16) - 1)
		, max_group_y_((1 << 10))
	{
	}

	VSMeshRayIntersection::~VSMeshRayIntersection()
	{
		gl_env_->glDeleteProgram(handle_program_);
		gl_env_->glDeleteBuffers(1, &ssbo_verts_);
		gl_env_->glDeleteBuffers(1, &ssbo_surfs_);
		gl_env_->glDeleteBuffers(1, &ssbo_rays_);
		gl_env_->glDeleteBuffers(1, &ssbo_result_);
	}
	
	void VSMeshRayIntersection::Build(VSMeshRayIntersectOutput& output, const VSMeshRayIntersectInput& input)
	{
		if (input.simpMesh.nVertCount == 0 ||
			input.simpMesh.nTriangleCount == 0 ||
			input.rays.len == 0)
			return;
		VD_F_PROFILE_START(VSMeshRayIntersection::Build);
		ReSet(output);
		while (input.simpMesh.nTriangleCount / cs_block_size_.x > max_group_x_)
			cs_block_size_.x *= 2;

		CalcInsert(input.simpMesh, input.rays);

		output.cbRayLen = VD_V2CB(out_dst_len_);
		output.cbSurfIdx = VD_V2CB(out_dst_idx_);
		output.cbOpRayLen = VD_V2CB(out_dst_len_op_);
		output.cbOpSurfIdx = VD_V2CB(out_dst_idx_op_);
		VD_F_PROFILE_FINISH(VSMeshRayIntersection::Build);
	}

	void VSMeshRayIntersection::ReSet(VSMeshRayIntersectOutput& output)
	{
		output.cbRayLen.len = 0;
		output.cbRayLen.pData = nullptr;
		output.cbSurfIdx.len = 0;
		output.cbSurfIdx.pData = nullptr;
		output.cbOpRayLen.len = 0;
		output.cbOpRayLen.pData = nullptr;
		output.cbOpSurfIdx.len = 0;
		output.cbOpSurfIdx.pData = nullptr;

		gl_env_->glDeleteProgram(handle_program_);
		gl_env_->glDeleteBuffers(1, &ssbo_verts_);
		gl_env_->glDeleteBuffers(1, &ssbo_surfs_);
		gl_env_->glDeleteBuffers(1, &ssbo_rays_);
		gl_env_->glDeleteBuffers(1, &ssbo_result_);

		shader_compute_src_.resize(0);
		ori_rays_.resize(0);
		ori_verts_.resize(0);
		ori_surfs_.resize(0);
		dst_len_.resize(0);
		dst_idx_.resize(0);
		dst_len_op_.resize(0);
		dst_idx_op_.resize(0);
		out_dst_len_.resize(0);
		out_dst_idx_.resize(0);
		out_dst_len_op_.resize(0);
		out_dst_idx_op_.resize(0);
	}

	void VSMeshRayIntersection::CalcInsert(const VSSimpleMeshF& mesh, const VSConstBuffer<VFRay>& rays)
	{
		VD_F_PROFILE_START(VSMeshRayIntersection::CalcInsert);
		Init(mesh, rays);
		if (!GetProgram())
			return;
		GenBuffer();
		GetResult();
		VD_F_PROFILE_FINISH(VSMeshRayIntersection::CalcInsert);
	}

	void VSMeshRayIntersection::Init(const VSSimpleMeshF& mesh, const VSConstBuffer<VFRay>& rays)
	{
		ori_rays_.resize(rays.len * 2);
		for (size_t i = 0; i < rays.len; i++) {
			const auto& ori = rays.pData[i].GetOrigin();
			const auto& dir = rays.pData[i].GetDirection();
			ori_rays_[i * 2].x = ori.x;
			ori_rays_[i * 2].y = ori.y;
			ori_rays_[i * 2].z = ori.z;
			ori_rays_[i * 2].w = 0.0f;
			ori_rays_[i * 2 + 1].x = dir.x;
			ori_rays_[i * 2 + 1].y = dir.y;
			ori_rays_[i * 2 + 1].z = dir.z;
			ori_rays_[i * 2 + 1].w = 0.0f;
		}

		ori_verts_.resize(mesh.nVertCount);
		for (size_t i = 0; i < mesh.nVertCount; i++) {
			ori_verts_[i].x = mesh.pVertices[i].x;
			ori_verts_[i].y = mesh.pVertices[i].y;
			ori_verts_[i].z = mesh.pVertices[i].z;
			ori_verts_[i].w = 0.0f;
		}

		const auto surf_fix_size = cs_block_size_.x - (mesh.nTriangleCount % cs_block_size_.x);
		ori_surfs_.resize(mesh.nTriangleCount + surf_fix_size);
		for (size_t i = 0; i < mesh.nTriangleCount; i++) {
			ori_surfs_[i].x = mesh.pTriangles[i].x;
			ori_surfs_[i].y = mesh.pTriangles[i].y;
			ori_surfs_[i].z = mesh.pTriangles[i].z;
			ori_surfs_[i].w = 0;
		}
		if (surf_fix_size != cs_block_size_.x)
			memset(&ori_surfs_[mesh.nTriangleCount], 0, surf_fix_size * sizeof(VNVECTOR4UI));
	}

	bool VSMeshRayIntersection::GetProgram()
	{
		handle_program_ = gl_env_->glCreateProgram();

		GetShaderSource();
		const GLchar* compute_src = shader_compute_src_.c_str();
		const auto shader_compute = gl_env_->glCreateShader(GL_COMPUTE_SHADER);
		gl_env_->glShaderSource(shader_compute, 1, &compute_src, nullptr);
		gl_env_->glCompileShader(shader_compute);

		GLint status;
		gl_env_->glGetShaderiv(shader_compute, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE)
			return false;

		gl_env_->glAttachShader(handle_program_, shader_compute);
		gl_env_->glLinkProgram(handle_program_);

		GLint link_status;
		gl_env_->glGetProgramiv(handle_program_, GL_LINK_STATUS, &link_status);
		if (link_status == GL_FALSE)
			return false;

		gl_env_->glDeleteShader(shader_compute);
		
		return true;
	}

	void VSMeshRayIntersection::GetShaderSource()
	{
		char block_str[MAX_PATH] = "\0";
		sprintf_s(block_str,
			"layout(local_size_x = %d, local_size_y = %d, local_size_z = %d) in;\n",
			cs_block_size_.x, cs_block_size_.y, cs_block_size_.z);

		shader_compute_src_ =
			"#version 430 core \n";
		shader_compute_src_ += block_str;
		shader_compute_src_ +=
			"layout(std430, binding = 1) buffer input_vert  { vec4  verts[] ; }; \n\
			layout(std430, binding = 2) buffer input_surf  { uvec4 surfs[] ; }; \n\
			layout(std430, binding = 3) buffer input_ray   { vec4  rays[]  ; }; \n\
			layout(std430, binding = 4) buffer output_data { float result[]; }; \n\
			void main(void) \n\
			{ \n\
				uint rstIdx = uint(gl_GlobalInvocationID.x + gl_GlobalInvocationID.y * gl_WorkGroupSize.x * gl_NumWorkGroups.x); \n\
				result[rstIdx] = 0.0f; \n\
				vec3 ray_ori = rays[gl_GlobalInvocationID.y * 2].xyz; \n\
				vec3 ray_dir = rays[gl_GlobalInvocationID.y * 2 + 1].xyz; \n\
				uvec3 surf = surfs[gl_GlobalInvocationID.x].xyz; \n\
				vec3 v0 = verts[surf.x].xyz; \n\
				vec3 v1 = verts[surf.y].xyz; \n\
				vec3 v2 = verts[surf.z].xyz; \n\
				vec3 e1 = v1 - v0; \n\
				vec3 e2 = v2 - v0; \n\
				vec3 p = cross(ray_dir, e2); \n\
				float det = dot(e1, p); \n\
				vec3 t; \n\
				if (det > 0) { \n\
					t = ray_ori - v0; \n\
				} \n\
				else { \n\
					t = v0 - ray_ori; \n\
					det = -det; \n\
				} \n\
				if (det < 0.0001f) return; \n\
				float u = dot(t, p); \n\
				if (u < 0 || u > det) return; \n\
				vec3 q = cross(t, e1); \n\
				float v = dot(ray_dir, q); \n\
				if (v < 0 || u + v > det) return; \n\
				result[rstIdx] = dot(e2, q) * (1.0f / det); \n\
			} \n\
		\0";
	}

	void VSMeshRayIntersection::GenBuffer()
	{
		VD_F_PROFILE_START(VSMeshRayIntersection::GenBuffer);
		gl_env_->glGenBuffers(1, &ssbo_verts_);
		gl_env_->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_verts_);
		gl_env_->glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(decltype(ori_verts_[0])) * ori_verts_.size(), &ori_verts_[0], GL_STATIC_DRAW);
		gl_env_->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssbo_verts_);
		gl_env_->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		gl_env_->glGenBuffers(1, &ssbo_surfs_);
		gl_env_->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_surfs_);
		gl_env_->glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(decltype(ori_surfs_[0])) * ori_surfs_.size(), &ori_surfs_[0], GL_STATIC_DRAW);
		gl_env_->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssbo_surfs_);
		gl_env_->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		gl_env_->glGenBuffers(1, &ssbo_rays_);
		gl_env_->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_rays_);
		gl_env_->glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(decltype(ori_rays_[0])) * ori_rays_.size(), &ori_rays_[0], GL_STATIC_DRAW);
		gl_env_->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssbo_rays_);
		gl_env_->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		gl_env_->glGenBuffers(1, &ssbo_result_);
		gl_env_->glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssbo_result_);
		gl_env_->glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float) * ori_rays_.size() * ori_surfs_.size() / 2, nullptr, GL_DYNAMIC_READ);
		gl_env_->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssbo_result_);
		gl_env_->glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);
		VD_F_PROFILE_FINISH(VSMeshRayIntersection::GenBuffer);
	}

	void VSMeshRayIntersection::GetResult()
	{
// 	 	GLint cntx, cnty, cntz;
// 	 	gl_env_->glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &cntx);
// 	 	gl_env_->glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &cnty);
// 	 	gl_env_->glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &cntz);
		VD_F_PROFILE_START(VSMeshRayIntersection::GetResult);
		VD_F_PROFILE_START(VSMeshRayIntersection::GetResult_glUseProgram);
		cs_group_size_.x = ori_surfs_.size() / cs_block_size_.x;
		cs_group_size_.y = ori_rays_.size();

		gl_env_->glUseProgram(handle_program_);
		VD_F_PROFILE_FINISH(VSMeshRayIntersection::GetResult_glUseProgram);
		VD_F_PROFILE_START(VSMeshRayIntersection::GetResult_glDispatchCompute);
		gl_env_->glDispatchCompute(cs_group_size_.x, cs_group_size_.y, cs_group_size_.z);
		VD_F_PROFILE_FINISH(VSMeshRayIntersection::GetResult_glDispatchCompute);
		VD_F_PROFILE_START(VSMeshRayIntersection::GetResult_glFlush);
		gl_env_->glFlush();
		VD_F_PROFILE_FINISH(VSMeshRayIntersection::GetResult_glFlush);
		VD_F_PROFILE_START(VSMeshRayIntersection::GetResult_glMapNamedBuffer);
		const auto rst_ptr = static_cast<float*>(gl_env_->glMapNamedBuffer(ssbo_result_, GL_READ_ONLY));
		VD_F_PROFILE_FINISH(VSMeshRayIntersection::GetResult_glMapNamedBuffer);
		VD_F_PROFILE_START(VSMeshRayIntersection::GetResult2);
		const auto ray_cnt = ori_rays_.size() / 2;
		const auto surf_cnt = ori_surfs_.size();
		dst_len_.resize(ray_cnt);
		dst_idx_.resize(ray_cnt);
		dst_len_op_.resize(ray_cnt);
		dst_idx_op_.resize(ray_cnt);

		for (size_t ray_idx = 0; ray_idx < ray_cnt; ray_idx++) {
			const auto rst_idx = ray_idx * surf_cnt;
			for (size_t j = 0; j < surf_cnt; j++) {
				if (rst_ptr[rst_idx + j] > VF_EPS) {
 					dst_len_[ray_idx].push_back(rst_ptr[rst_idx + j]);
 					dst_idx_[ray_idx].push_back(j);
 				}
				else if (rst_ptr[rst_idx + j] < -VF_EPS) {
					dst_len_op_[ray_idx].push_back(rst_ptr[rst_idx + j]);
					dst_idx_op_[ray_idx].push_back(j);
				}
			}
			SortResult(dst_len_[ray_idx], dst_idx_[ray_idx]);
			SortResult(dst_len_op_[ray_idx], dst_idx_op_[ray_idx], false);
		}

		out_dst_len_.resize(dst_len_.size());
		out_dst_idx_.resize(dst_idx_.size());
		out_dst_len_op_.resize(dst_len_op_.size());
		out_dst_idx_op_.resize(dst_idx_op_.size());
		for (size_t i = 0; i < dst_len_.size(); i++)
			out_dst_len_[i] = VD_V2CB(dst_len_[i]);
		for (size_t i = 0; i < dst_idx_.size(); i++)
			out_dst_idx_[i] = VD_V2CB(dst_idx_[i]);
		for (size_t i = 0; i < dst_len_op_.size(); i++)
			out_dst_len_op_[i] = VD_V2CB(dst_len_op_[i]);
		for (size_t i = 0; i < dst_idx_op_.size(); i++)
			out_dst_idx_op_[i] = VD_V2CB(dst_idx_op_[i]);
		VD_F_PROFILE_FINISH(VSMeshRayIntersection::GetResult2);
		VD_F_PROFILE_FINISH(VSMeshRayIntersection::GetResult);
	}

	void VSMeshRayIntersection::SortResult(std::vector<float>& len, std::vector<unsigned>& idx, bool greater)
	{
		if (len.size() != idx.size())
			return;
		
		for (size_t i = 0; i < len.size(); i++) {
			size_t id = i;
			for (size_t j = i + 1; j < len.size(); j++) {
				if (greater && len[id] > len[j]) id = j;
				else if (!greater && len[id] < len[j]) id = j;
			}
			if (id != i) {
				std::swap(len[id], len[i]);
				std::swap(idx[id], idx[i]);
			}
		}		
	}
}
