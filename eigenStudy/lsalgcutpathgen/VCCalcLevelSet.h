#pragma once
#include <obj/alg/ialgcutpathgen.h>

namespace NMALG_CUTPATHGEN
{
	class VCCalcLevelSet
	{
	public:
		VCCalcLevelSet();
		~VCCalcLevelSet();

		void Calc(VSConstBuffer<double>& cbLevelSet, const VSPerfectMesh& perfMesh,
			const VSConstBuffer<unsigned>& cbFrontPointSet, const VSConstBuffer<unsigned>& cbBgkPointSet);

	private:
		std::vector<double> m_vLevelSet;
	};
}
