#pragma once
#include <interface/iobjreference.h>
#include <obj/alg/ialgcutpathgen.h>
namespace NMALG_CUTPATHGEN
{
	class VCFindCutPath
	{
	public:
		VCFindCutPath();
		~VCFindCutPath();

		void Gen(VSCutPathInfo& pathInfo, std::vector<unsigned>& vCutVertIndex, const VSPerfectMesh& mesh,
			const VSConstBuffer<float>& cbLevelSet, const float flThreshold = 0.95f);

	private:
		std::vector<VFVECTOR3> m_vCutVertices;
		std::vector<VFVECTOR3> m_vCutNormals;
	};
}
