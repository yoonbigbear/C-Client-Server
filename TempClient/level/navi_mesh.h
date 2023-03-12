#pragma once

#include "pre.h"
#include "detour/DetourNavMesh.h"
#include "detour/DetourNavMeshQuery.h"

class NaviMesh
{
public:
	NaviMesh();
	~NaviMesh();

	void Initialize(const char* path);
	dtNavMesh* LoadAll(const char* path);
	void Render();

public:
	dtNavMesh* nav_mesh() { return nav_mesh_; }
	void nav_mesh(dtNavMesh* mesh) { nav_mesh_ = mesh; }
	dtNavMeshQuery* nav_query() { return nav_query_; }

private:
	DebugDrawGLBB dd_;
    dtNavMesh* nav_mesh_;
    dtNavMeshQuery* nav_query_;
};