#pragma once
#include "../types.h"
#include "../bb_util.h"
#include "detour/DetourNavMesh.h"
#include "detour/DetourNavMeshQuery.h"

//const unsigned int spathCol = m_hitResult ?
//BB::RGBA<64, 16, 0, 220>::VAL :
//duRGBA(240, 240, 240, 220);

class Navigation
{
	enum { MAX_POLYS = 256 };
public:
	Navigation();
	virtual ~Navigation();

	bool Initialize(const char* path);
	dtNavMesh* LoadAll(const char* path);
	void Render(struct duDebugDraw* dd);

public:
	_NODISCARD bool FindRandomPointInCircle(const Vec& center, float radius,
		float* dtrandom, const dtQueryFilter& ft = dtQueryFilter());
	_NODISCARD bool FindPath(const Vec& start, const Vec& end,
		Deque<Vec>& out_path, const dtQueryFilter& ft = dtQueryFilter());
	_NODISCARD bool FindPath(float* dtstart, float* dtend,
		Deque<Vec>& out_path, const dtQueryFilter& ft = dtQueryFilter());
	_NODISCARD bool Raycast(const Vec& start, dtPolyRef startref,
		const Vec& end, dtRaycastHit& hit,
		const dtQueryFilter& ft = dtQueryFilter());
	_NODISCARD bool Raycast(float* dtstart, dtPolyRef startref,
		float* dtend, dtRaycastHit& hit,
		const dtQueryFilter& ft = dtQueryFilter());

public:
	dtNavMesh* nav_mesh() { return nav_mesh_; }
	void nav_mesh(dtNavMesh* mesh) { nav_mesh_ = mesh; }
	dtNavMeshQuery* nav_query() { return nav_query_; }


protected:
    dtNavMesh* nav_mesh_;
    dtNavMeshQuery* nav_query_;
};