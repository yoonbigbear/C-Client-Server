#pragma once

#define _CRT_SECURE_NO_WARNINGS
#include "DebugDraw.h"
#include "DetourNavMesh.h"
#include "DetourNavMeshQuery.h"

enum SamplePolyAreas
{
	SAMPLE_POLYAREA_GROUND,
	SAMPLE_POLYAREA_WATER,
	SAMPLE_POLYAREA_ROAD,
	SAMPLE_POLYAREA_DOOR,
	SAMPLE_POLYAREA_GRASS,
	SAMPLE_POLYAREA_JUMP,
};

enum duDebugDrawPrimitives;

static const unsigned int startCol = duRGBA(128, 25, 0, 192);
static const unsigned int endCol = duRGBA(51, 102, 0, 129);
static const unsigned int pathCol = duRGBA(0, 0, 0, 64);

class DebugDrawGLBB : public duDebugDraw
{
public:
    virtual unsigned int areaToCol(unsigned int area);
	virtual void depthMask(bool state);
	virtual void texture(bool state);
	virtual void begin(duDebugDrawPrimitives prim, float size = 1.0f);
	virtual void vertex(const float* pos, unsigned int color);
	virtual void vertex(const float x, const float y, const float z, unsigned int color);
	virtual void vertex(const float* pos, unsigned int color, const float* uv);
	virtual void vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v);

	virtual void end();

	void vertex(const dtReal* pos, unsigned int color);
	void vertex(const dtReal x, const dtReal y, const dtReal z, unsigned int color);
	void vertex(const dtReal* pos, unsigned int color, const dtReal* uv);
	void vertex(const dtReal x, const dtReal y, const dtReal z, unsigned int color, const dtReal u, const dtReal v);

};

class NaviMesh
{
public:
	NaviMesh();
	~NaviMesh();

	void Start(const char* path);
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