#include "stdafx.h"
#include "lsalgtools/ilsalgtoolsperfmesh.h"

namespace NM_PMMESH
{
	const float g_flReciprocalN[21] = {
		0.0f, 1.0f, 0.5f, 1.0f / 3.0f, 1.0f / 4.0f, 1.0f / 5.0f, 1.0f / 6.0f,
		1.0f / 7.0f, 1.0f / 8.0f, 1.0f / 9.0f,1.0f / 10.0f, 1.0f / 11.0f, 1.0f / 12.0f,
		1.0f / 13.0f, 1.0f / 14.0f, 1.0f / 15.0f, 1.0f / 16.0f, 1.0f / 17.0f, 1.0f / 18.0f,
		1.0f / 19.0f, 1.0f / 20.0f
	};

	const VFVECTOR3 g_vecConstB(1.0f, 1.0f, 1.0f);
	const VFMATRIX3 g_matA[21] = {
		MAT3_IDENTITY, MAT3_IDENTITY,
		VFMATRIX3(1.0f, -g_flReciprocalN[2], -g_flReciprocalN[2], -g_flReciprocalN[2], 1.0f, -g_flReciprocalN[2], -g_flReciprocalN[2], -g_flReciprocalN[2], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[3], -g_flReciprocalN[3], -g_flReciprocalN[3], 1.0f, -g_flReciprocalN[3], -g_flReciprocalN[3], -g_flReciprocalN[3], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[4], -g_flReciprocalN[4], -g_flReciprocalN[4], 1.0f, -g_flReciprocalN[4], -g_flReciprocalN[4], -g_flReciprocalN[4], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[5], -g_flReciprocalN[5], -g_flReciprocalN[5], 1.0f, -g_flReciprocalN[5], -g_flReciprocalN[5], -g_flReciprocalN[5], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[6], -g_flReciprocalN[6], -g_flReciprocalN[6], 1.0f, -g_flReciprocalN[6], -g_flReciprocalN[6], -g_flReciprocalN[6], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[7], -g_flReciprocalN[7], -g_flReciprocalN[7], 1.0f, -g_flReciprocalN[7], -g_flReciprocalN[7], -g_flReciprocalN[7], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[8], -g_flReciprocalN[8], -g_flReciprocalN[8], 1.0f, -g_flReciprocalN[8], -g_flReciprocalN[8], -g_flReciprocalN[8], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[9], -g_flReciprocalN[9], -g_flReciprocalN[9], 1.0f, -g_flReciprocalN[9], -g_flReciprocalN[9], -g_flReciprocalN[9], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[10], -g_flReciprocalN[10], -g_flReciprocalN[10], 1.0f, -g_flReciprocalN[10], -g_flReciprocalN[10], -g_flReciprocalN[10], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[11], -g_flReciprocalN[11], -g_flReciprocalN[11], 1.0f, -g_flReciprocalN[11], -g_flReciprocalN[11], -g_flReciprocalN[11], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[12], -g_flReciprocalN[12], -g_flReciprocalN[12], 1.0f, -g_flReciprocalN[12], -g_flReciprocalN[12], -g_flReciprocalN[12], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[13], -g_flReciprocalN[13], -g_flReciprocalN[13], 1.0f, -g_flReciprocalN[13], -g_flReciprocalN[13], -g_flReciprocalN[13], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[14], -g_flReciprocalN[14], -g_flReciprocalN[14], 1.0f, -g_flReciprocalN[14], -g_flReciprocalN[14], -g_flReciprocalN[14], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[15], -g_flReciprocalN[15], -g_flReciprocalN[15], 1.0f, -g_flReciprocalN[15], -g_flReciprocalN[15], -g_flReciprocalN[15], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[16], -g_flReciprocalN[16], -g_flReciprocalN[16], 1.0f, -g_flReciprocalN[16], -g_flReciprocalN[16], -g_flReciprocalN[16], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[17], -g_flReciprocalN[17], -g_flReciprocalN[17], 1.0f, -g_flReciprocalN[17], -g_flReciprocalN[17], -g_flReciprocalN[17], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[18], -g_flReciprocalN[18], -g_flReciprocalN[18], 1.0f, -g_flReciprocalN[18], -g_flReciprocalN[18], -g_flReciprocalN[18], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[19], -g_flReciprocalN[19], -g_flReciprocalN[19], 1.0f, -g_flReciprocalN[19], -g_flReciprocalN[19], -g_flReciprocalN[19], 1.0f),
		VFMATRIX3(1.0f, -g_flReciprocalN[20], -g_flReciprocalN[20], -g_flReciprocalN[20], 1.0f, -g_flReciprocalN[20], -g_flReciprocalN[20], -g_flReciprocalN[20], 1.0f)
	};
	const VFMATRIX3 g_matAInv[21] = {
		g_matA[0].Inverse(),g_matA[1].Inverse(), g_matA[2].Inverse(),g_matA[3].Inverse(),g_matA[4].Inverse(),
		g_matA[5].Inverse(),g_matA[6].Inverse(),g_matA[7].Inverse(),g_matA[8].Inverse(),g_matA[9].Inverse(),
		g_matA[10].Inverse(),g_matA[11].Inverse(),g_matA[12].Inverse(),g_matA[13].Inverse(),g_matA[14].Inverse(),
		g_matA[15].Inverse(),g_matA[16].Inverse(),g_matA[17].Inverse(),g_matA[18].Inverse(),g_matA[19].Inverse(),
		g_matA[20].Inverse() };

	const VFVECTOR3 g_vecBAInv[21] = {
		g_vecConstB * g_matAInv[0],g_vecConstB * g_matAInv[1],g_vecConstB * g_matAInv[2],g_vecConstB * g_matAInv[3],
		g_vecConstB * g_matAInv[4],g_vecConstB * g_matAInv[5],g_vecConstB * g_matAInv[6],g_vecConstB * g_matAInv[7],
		g_vecConstB * g_matAInv[8],g_vecConstB * g_matAInv[9],g_vecConstB * g_matAInv[10],g_vecConstB * g_matAInv[11],
		g_vecConstB * g_matAInv[12],g_vecConstB * g_matAInv[13],g_vecConstB * g_matAInv[14],g_vecConstB * g_matAInv[15],
		g_vecConstB * g_matAInv[16],g_vecConstB * g_matAInv[17],g_vecConstB * g_matAInv[18],g_vecConstB * g_matAInv[19],
		g_vecConstB * g_matAInv[20]
	};
	const float g_flBAInvAtBt[21] = {
		g_vecBAInv[0].Dot(g_matAInv[0].Transpose() * g_vecConstB), g_vecBAInv[1].Dot(g_matAInv[1].Transpose() * g_vecConstB),
		g_vecBAInv[2].Dot(g_matAInv[2].Transpose() * g_vecConstB), g_vecBAInv[3].Dot(g_matAInv[3].Transpose() * g_vecConstB),
		g_vecBAInv[4].Dot(g_matAInv[4].Transpose() * g_vecConstB), g_vecBAInv[5].Dot(g_matAInv[5].Transpose() * g_vecConstB),
		g_vecBAInv[6].Dot(g_matAInv[6].Transpose() * g_vecConstB), g_vecBAInv[7].Dot(g_matAInv[7].Transpose() * g_vecConstB),
		g_vecBAInv[8].Dot(g_matAInv[8].Transpose() * g_vecConstB), g_vecBAInv[9].Dot(g_matAInv[9].Transpose() * g_vecConstB),
		g_vecBAInv[10].Dot(g_matAInv[10].Transpose() * g_vecConstB), g_vecBAInv[11].Dot (g_matAInv[11].Transpose() * g_vecConstB),
		g_vecBAInv[12].Dot(g_matAInv[12].Transpose() * g_vecConstB), g_vecBAInv[13].Dot (g_matAInv[13].Transpose() * g_vecConstB),
		g_vecBAInv[14].Dot(g_matAInv[14].Transpose() * g_vecConstB), g_vecBAInv[15].Dot (g_matAInv[15].Transpose() * g_vecConstB),
		g_vecBAInv[16].Dot(g_matAInv[16].Transpose() * g_vecConstB), g_vecBAInv[17].Dot (g_matAInv[17].Transpose() * g_vecConstB),
		g_vecBAInv[18].Dot(g_matAInv[18].Transpose() * g_vecConstB), g_vecBAInv[19].Dot (g_matAInv[19].Transpose() * g_vecConstB),
		g_vecBAInv[20].Dot(g_matAInv[20].Transpose() * g_vecConstB)
	};
}
