#include "navi_mesh.h"

#include <stdio.h>
#include "SDL_opengl.h"
#include "DetourDebugDraw.h"

static const int NAVMESHSET_MAGIC = 'M' << 24 | 'S' << 16 | 'E' << 8 | 'T'; //'MSET';
static const int NAVMESHSET_VERSION = 1;

struct NavMeshSetHeader
{
	int magic;
	int version;
	int numTiles;
	dtNavMeshParams params;
};

struct NavMeshTileHeader
{
	dtTileRef tileRef;
	int dataSize;
};

unsigned int DebugDrawGLBB::areaToCol(unsigned int area)
{
	switch (area)
	{
		// Ground (0) : light blue
	case SAMPLE_POLYAREA_GROUND: return duRGBA(0, 192, 255, 255);
		// Water : blue
	case SAMPLE_POLYAREA_WATER: return duRGBA(0, 0, 255, 255);
		// Road : brown
	case SAMPLE_POLYAREA_ROAD: return duRGBA(50, 20, 12, 255);
		// Door : cyan
	case SAMPLE_POLYAREA_DOOR: return duRGBA(0, 255, 255, 255);
		// Grass : green
	case SAMPLE_POLYAREA_GRASS: return duRGBA(0, 255, 0, 255);
		// Jump : yellow
	case SAMPLE_POLYAREA_JUMP: return duRGBA(255, 255, 0, 255);
		// Unexpected : red
	default: return duRGBA(255, 0, 0, 255);
	}
}

void DebugDrawGLBB::depthMask(bool state)
{
	glDepthMask(state ? GL_TRUE : GL_FALSE);
}

void DebugDrawGLBB::texture(bool state)
{
	if (state)
	{
		glEnable(GL_TEXTURE_2D);
		//g_tex.bind();
	}
	else
	{
		glDisable(GL_TEXTURE_2D);
	}
}

void DebugDrawGLBB::begin(duDebugDrawPrimitives prim, float size)
{
	switch (prim)
	{
	case DU_DRAW_POINTS:
		glPointSize(size);
		glBegin(GL_POINTS);
		break;
	case DU_DRAW_LINES:
		glLineWidth(size);
		glBegin(GL_LINES);
		break;
	case DU_DRAW_TRIS:
		glBegin(GL_TRIANGLES);
		break;
	case DU_DRAW_QUADS:
		glBegin(GL_QUADS);
		break;
	};
}

void DebugDrawGLBB::vertex(const dtReal* pos, unsigned int color)
{
	glColor4ubv((GLubyte*)&color);
	glVertex3dv(pos);
}

void DebugDrawGLBB::vertex(const dtReal x, const dtReal y, const dtReal z, unsigned int color)
{
	glColor4ubv((GLubyte*)&color);
	glVertex3d(x, y, z);
}

void DebugDrawGLBB::vertex(const dtReal* pos, unsigned int color, const dtReal* uv)
{
	glColor4ubv((GLubyte*)&color);
	glTexCoord2dv(uv);
	glVertex3dv(pos);
}

void DebugDrawGLBB::vertex(const dtReal x, const dtReal y, const dtReal z, unsigned int color, const dtReal u, const dtReal v)
{
	glColor4ubv((GLubyte*)&color);
	glTexCoord2d(u, v);
	glVertex3d(x, y, z);
}

void DebugDrawGLBB::vertex(const float* pos, unsigned int color)
{
	glColor4ubv((GLubyte*)&color);
	glVertex3fv(pos);
}

void DebugDrawGLBB::vertex(const float x, const float y, const float z, unsigned int color)
{
	glColor4ubv((GLubyte*)&color);
	glVertex3f(x, y, z);
}

void DebugDrawGLBB::vertex(const float* pos, unsigned int color, const float* uv)
{
	glColor4ubv((GLubyte*)&color);
	glTexCoord2fv(uv);
	glVertex3fv(pos);
}

void DebugDrawGLBB::vertex(const float x, const float y, const float z, unsigned int color, const float u, const float v)
{
	glColor4ubv((GLubyte*)&color);
	glTexCoord2f(u, v);
	glVertex3f(x, y, z);
}

void DebugDrawGLBB::end()
{
	glEnd();
	glLineWidth(1.0f);
	glPointSize(1.0f);
}

NaviMesh::NaviMesh()
	:nav_query_(dtAllocNavMeshQuery())
	, nav_mesh_(nullptr)
{
}

NaviMesh::~NaviMesh()
{
	dtFreeNavMeshQuery(nav_query_);
	dtFreeNavMesh(nav_mesh_);
}

void NaviMesh::Start(const char* path)
{
	dtFreeNavMesh(nav_mesh_);
	nav_mesh_ = LoadAll(path);
	if (nav_mesh_)
		nav_query_->init(nav_mesh_, 2048);
}

dtNavMesh* NaviMesh::LoadAll(const char* path)
{
	FILE* fp = fopen(path, "rb");
	if (!fp) return 0;

	// Read header.
	NavMeshSetHeader header;
	size_t readLen = fread(&header, sizeof(NavMeshSetHeader), 1, fp);
	if (readLen != 1)
	{
		fclose(fp);
		return 0;
	}
	if (header.magic != NAVMESHSET_MAGIC)
	{
		fclose(fp);
		return 0;
	}
	if (header.version != NAVMESHSET_VERSION)
	{
		fclose(fp);
		return 0;
	}

	dtNavMesh* mesh = dtAllocNavMesh();
	if (!mesh)
	{
		fclose(fp);
		return 0;
	}
	dtStatus status = mesh->init(&header.params);
	if (dtStatusFailed(status))
	{
		fclose(fp);
		return 0;
	}

	// Read tiles.
	for (int i = 0; i < header.numTiles; ++i)
	{
		NavMeshTileHeader tileHeader;
		readLen = fread(&tileHeader, sizeof(tileHeader), 1, fp);
		if (readLen != 1)
		{
			fclose(fp);
			return 0;
		}

		if (!tileHeader.tileRef || !tileHeader.dataSize)
			break;

		unsigned char* data = (unsigned char*)dtAlloc(tileHeader.dataSize, DT_ALLOC_PERM);
		if (!data) break;
		memset(data, 0, tileHeader.dataSize);
		readLen = fread(data, tileHeader.dataSize, 1, fp);
		if (readLen != 1)
		{
			dtFree(data);
			fclose(fp);
			return 0;
		}

		mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0);
	}

	fclose(fp);

	return mesh;
}

void NaviMesh::Render()
{
	duDebugDrawNavMeshWithClosedList(&dd_, *nav_mesh_, *nav_query_, DU_DRAWNAVMESH_CLOSEDLIST);
}
