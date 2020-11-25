#include "stdafx.h"
#include "lsalgtools/ilsalgtoolsperfmesh.h"


void FitCircle(const std::list<VFVECTOR3>& circle, VFVECTOR3& center, float& radix)
{
	if (circle.empty())
	{
		center = VFVECTOR3::ZERO;
		radix = 0;
		return;
	}
	if (circle.size() == 1)
	{
		center = circle.front();
		radix = 0;
		return;
	}
	if (circle.size() == 2)
	{
		center = (circle.front() + circle.back()) / 2;
		radix = (center - circle.front()).Magnitude();
		return;
	}
	if (circle.size() > 3)
	{
		center = VFVECTOR3::ZERO;
		radix = 0;
		return;
	}

	auto iter = circle.begin();
	const auto& v0 = *iter++;
	const auto& v1 = *iter++;
	const auto& v2 = *iter++;

	{ //is colinear ?
		auto v10 = v1 - v0;
		auto v20 = v2 - v0;
		v10.Normalize();
		v20.Normalize();

		if (acos(v10.AbsDot(v20) / (v10.Magnitude() * v20.Magnitude())) * (180 / VF_PI) < VF_EPS)
		{
			center = VFVECTOR3::ZERO;
			radix = 0;
			return;
		}
	}

	{ //is obtuse triangle ?
		auto v10 = (v1 - v0).SqrMagnitude();
		auto v20 = (v2 - v0).SqrMagnitude();
		auto v21 = (v2 - v1).SqrMagnitude();

		if (v10 >= v20 + v21)
		{
			center = (v1 + v0) / 2;
			radix = (center - v0).Magnitude();
			return;
		}
		if (v20 >= v10 + v21)
		{
			center = (v2 + v0) / 2;
			radix = (center - v0).Magnitude();
			return;
		}
		if (v21 >= v20 + v10)
		{
			center = (v2 + v1) / 2;
			radix = (center - v1).Magnitude();
			return;
		}
	}

	float a0 = 2 * (v1.x - v0.x);
	float b0 = 2 * (v1.y - v0.y);
	float c0 = v1.x * v1.x - v0.x * v0.x + v1.y * v1.y - v0.y * v0.y;
	float a1 = 2 * (v2.x - v0.x);
	float b1 = 2 * (v2.y - v0.y);
	float c1 = v2.x * v2.x - v0.x * v0.x + v2.y * v2.y - v0.y * v0.y;

	center.x = (c0 * b1 - c1 * b0) / (a0 * b1 - a1 * b0);
	center.y = (a0 * c1 - a1 * c0) / (a0 * b1 - a1 * b0);
	radix = (float)sqrt((double)(
		(center.x - v0.x) * (center.x - v0.x) +
		(center.y - v0.y) * (center.y - v0.y)));
}

std::list<VFVECTOR3> Mincircle(std::list<VFVECTOR3> circle, const std::list<VFVECTOR3>& points, VFVECTOR3& center, float& radix)
{
	if (circle.size() == 3 || points.empty())
	{
		FitCircle(circle, center, radix);
		return points;
	}

	auto points_new = points;
	points_new.pop_back();
	auto p = points.back();

	points_new = Mincircle(circle, points_new, center, radix);

	bool chk = false;
	if (radix < VF_EPS_2)
		chk = true;
	else
		chk = (p - center).Magnitude() > radix + VF_EPS_2;

	if (chk)
	{
		circle.push_front(p);
		Mincircle(circle, points_new, center, radix);
		points_new.push_front(p);
	}

	return points_new;
}

bool PMCalcMinBoundCircle(const VSConstBuffer<VFVECTOR3>& verts, VFVECTOR3* center, float* radius, const VFPlane* projPlane)
{
	if (verts.len < 3)
		return false;

	std::list<VFVECTOR3> hullmesh;
	std::list<VFVECTOR3> circle;

	VFVECTOR3 ctrl_pnts[3];
	for (size_t i = 0; i < verts.len; i++) {
		auto v = verts.pData[i];		
		if (projPlane == nullptr)
			v.z = 0.0f;
		else
			v = projPlane->projectPoint(v);
		hullmesh.push_back(v);
		if (i == 0) ctrl_pnts[0] = v;
		if (i == verts.len / 3) ctrl_pnts[1] = v;
		if (i == verts.len / 2) ctrl_pnts[2] = v;
	}

	auto xaxis = ctrl_pnts[0] - ctrl_pnts[2];
	xaxis.Normalize();
	auto norm = (ctrl_pnts[0] - ctrl_pnts[1]).Cross(ctrl_pnts[1] - ctrl_pnts[2]);
	norm.Normalize();
	if (xaxis.IsZero() || norm.IsZero())
		return false;
	VFLocation loc(ctrl_pnts[0], VFQuaternion::FromAxesZX(norm, xaxis));

	if (norm != VFVECTOR3::AXIS_Z) {
		for (auto& vert : hullmesh)
			vert = loc.TransGlobalVertexToLocal(vert);
	}

	Mincircle(circle, hullmesh, *center, *radius);

	if (norm != VFVECTOR3::AXIS_Z)
		*center = loc.TransLocalVertexToGlobal(*center);
	return true;
}

bool PMCalcMaxInscribedCircle(const VSConstBuffer<VFVECTOR3>& verts, const VFVECTOR3& center, float* radius, const VFPlane* projPlane)
{
	if (verts.len <= 0)
		return false;

	float flMaxRadix = FLT_MAX;
	for (size_t i = 0; i < verts.len; ++i)
	{
		auto x = verts.pData[i].x;
		auto y = verts.pData[i].y;
		if (projPlane != nullptr)
		{
			x = projPlane->projectPoint(verts.pData[i]).x;
			y = projPlane->projectPoint(verts.pData[i]).y;
		}
		float dis1 = sqrt(x * x + y * y);
		float dis2 = sqrt((x - center.x) * (x - center.x) + (y - center.y) * (y - center.y));
		// 			if (flMaxRadix > dis1)
		// 				flMaxRadix = dis1;
		if (flMaxRadix > dis2)
			flMaxRadix = dis2;
	}

	*radius = flMaxRadix;

	return true;
}

bool PMCalcMinMaxCircle(const VSConstBuffer<VFVECTOR3>& verts, const float& outterRadius, const float& innerRadius, VFVECTOR3* center, const VFPlane* projPlane)
{
	float flCenterYMax = VF_MAX;
	float flCenterYMin = VF_MIN;
	for (size_t i = 0; i < verts.len; i++)
	{
		auto vert = verts.pData[i];
		if (projPlane != nullptr)
			vert = projPlane->projectPoint(vert);
	 	float flCYMin = 0.0f;
	 	float flCYMax = 0.0f;
	 	float flXDist = vert.x - (center == nullptr ? 0 : center->x);
	 	float flYDist = vert.y - (center == nullptr ? 0 : center->y);
	 	float flOutYSquare = outterRadius * outterRadius - flXDist * flXDist;
	 	float flInYSquare  = innerRadius * innerRadius - flXDist * flXDist;
	 
	 	if (flOutYSquare <= 0)
	 		return false;
	 	if (flInYSquare < 0)
	 	{
	 		flCYMax = flYDist + sqrt(flOutYSquare);
	 		flCYMin = flYDist - sqrt(flOutYSquare);
	 	}
	 	else
	 	{
	 		flCYMax = flYDist - sqrt(flInYSquare);
	 		flCYMin = flYDist - sqrt(flOutYSquare);
	 	}
	 
	 	if (flCenterYMin > flCYMax || flCenterYMax < flCYMin)
	 		return false;
	 	if (flCenterYMax > flCYMax)
	 		flCenterYMax = flCYMax;
	 	if (flCenterYMin < flCYMin)
	 		flCenterYMin = flCYMin;
	}

	if (center != nullptr)
		center->y += (flCenterYMax + flCenterYMin) / 2;

	return true;
}