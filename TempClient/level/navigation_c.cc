#include "navigation_c.h"

#ifdef __APPLE__
#	include <OpenGL/glu.h>
#else
#	include <GL/glu.h>
#endif

#include "share/detour/Recast.h"


static bool isectSegAABB(const float* sp, const float* sq,
	const float* amin, const float* amax,
	float& tmin, float& tmax)
{
	static const float EPS = 1e-6f;

	float d[3];
	d[0] = sq[0] - sp[0];
	d[1] = sq[1] - sp[1];
	d[2] = sq[2] - sp[2];
	tmin = 0.0;
	tmax = 1.0f;

	for (int i = 0; i < 3; i++)
	{
		if (fabsf(d[i]) < EPS)
		{
			if (sp[i] < amin[i] || sp[i] > amax[i])
				return false;
		}
		else
		{
			const float ood = 1.0f / d[i];
			float t1 = (amin[i] - sp[i]) * ood;
			float t2 = (amax[i] - sp[i]) * ood;
			if (t1 > t2) { float tmp = t1; t1 = t2; t2 = tmp; }
			if (t1 > tmin) tmin = t1;
			if (t2 < tmax) tmax = t2;
			if (tmin > tmax) return false;
		}
	}

	return true;
}
static bool intersectSegmentTriangle(const float* sp, const float* sq,
	const float* a, const float* b, const float* c,
	float& t)
{
	float v, w;
	float ab[3], ac[3], qp[3], ap[3], norm[3], e[3];
	rcVsub(ab, b, a);
	rcVsub(ac, c, a);
	rcVsub(qp, sp, sq);

	// Compute triangle normal. Can be precalculated or cached if
	// intersecting multiple segments against the same triangle
	rcVcross(norm, ab, ac);

	// Compute denominator d. If d <= 0, segment is parallel to or points
	// away from triangle, so exit early
	float d = rcVdot(qp, norm);
	if (d <= 0.0f) return false;

	// Compute intersection t value of pq with plane of triangle. A ray
	// intersects iff 0 <= t. Segment intersects iff 0 <= t <= 1. Delay
	// dividing by d until intersection has been found to pierce triangle
	rcVsub(ap, sp, a);
	t = rcVdot(ap, norm);
	if (t < 0.0f) return false;
	if (t > d) return false; // For segment; exclude this code line for a ray test

	// Compute barycentric coordinate components and test if within bounds
	rcVcross(e, qp, ap);
	v = rcVdot(ac, e);
	if (v < 0.0f || v > d) return false;
	w = -rcVdot(ab, e);
	if (w < 0.0f || v + w > d) return false;

	// Segment/ray intersects triangle. Perform delayed division
	t /= d;

	return true;
}

bool Navigation_C::ScreenRay(float* src, float* dst, float& tmin)
{
	bool hit = false;
	auto max_tiles = nav_mesh_->getMaxTiles();
	tmin = 1.0f;
	float t = 1;

	for (int i = 0; i < max_tiles; ++i)
	{
		const dtMeshTile* tile = nav_mesh_->getTile(i);
		if (!tile->header) continue;

		//if tilemesh
		float btmin, btmax;
		if (!isectSegAABB(src, dst, tile->header->bmin,
			tile->header->bmax, btmin, btmax))
			continue;

		for (int j = 0; j < tile->header->polyCount; ++j)
		{
			const dtPoly* p = &tile->polys[j];
			const dtPolyDetail* pd = &tile->detailMeshes[j];

			for (int k = 0; k < pd->triCount; ++k)
			{
				const unsigned char* tris = &tile->detailTris[(pd->triBase + k) * 4];

				if (intersectSegmentTriangle(src, dst,
					&tile->verts[p->verts[tris[0]] * 3],
					&tile->verts[p->verts[tris[0 + 1]] * 3],
					&tile->verts[p->verts[tris[0 + 2]] * 3], t))
				{
					if (t < tmin)
						tmin = t;
					hit = true;
				}
			}

		}
	}
	return hit;
}
