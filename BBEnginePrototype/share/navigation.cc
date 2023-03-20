#include "navigation.h"
#include <stdio.h>
#include "detour/DebugDraw.h"
#include "detour/DetourDebugDraw.h"


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

Navigation::Navigation()
	:nav_query_(dtAllocNavMeshQuery())
	, nav_mesh_(nullptr)
{

}

Navigation::~Navigation()
{
	dtFreeNavMeshQuery(nav_query_);
	dtFreeNavMesh(nav_mesh_);
}

void Navigation::Initialize(const char* path)
{
	dtFreeNavMesh(nav_mesh_);
	nav_mesh_ = LoadAll(path);
	if (nav_mesh_)
		nav_query_->init(nav_mesh_, 2048);
}

dtNavMesh* Navigation::LoadAll(const char* path)
{
	FILE* fp;
	auto err = fopen_s(&fp, path, "rb");
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

		dtStatusSucceed(mesh->addTile(data, tileHeader.dataSize, DT_TILE_FREE_DATA, tileHeader.tileRef, 0));
	}

	fclose(fp);

	return mesh;
}

void Navigation::Render(struct duDebugDraw* dd)
{
	duDebugDrawNavMeshWithClosedList(dd, *nav_mesh_, *nav_query_, DU_DRAWNAVMESH_CLOSEDLIST);

	for (int i = 0; i < nav_mesh_->getMaxTiles(); ++i)
	{
		const dtMeshTile* tile = nav_mesh_->getTile(i);
		if (!tile->header) continue;
		duDebugDrawBoxWire(dd, tile->header->bmin[0],
			tile->header->bmin[1], tile->header->bmin[2],
			tile->header->bmax[0], tile->header->bmax[1], 
			tile->header->bmax[2], duRGBA(110, 110, 100, 50), 1.0f);
	}
}

bool Navigation::FindRandomPointInCircle(const Vec& center, float radius, 
	float* dtrandom, const dtQueryFilter& ft)
{
	const float extents[3]{ 0,10,0 };
	float dtcenter[3] = { center.v3[0], center.v3[2], center.v3[1] };
	dtPolyRef startref;
	if (!dtStatusSucceed(nav_query_->findNearestPoly(dtcenter, extents, &ft, &startref, dtcenter)))
	{
		return false;
	}
	
	dtPolyRef destref;
	if (!dtStatusSucceed(nav_query_->findRandomPointAroundCircle(startref, dtcenter, radius, &ft,
		[]() {return RandomGenerator::Real(0.f, 1.f); }, &destref, dtrandom)))
	{
		return false;
	}

	return true;
}

bool Navigation::FindPath(const Vec& start, const Vec& end,
	List<Vec>& out_path, const dtQueryFilter& ft)
{
	float dtstart[3] = { start.v3[0], start.v3[2], start.v3[1] };
	float dtend[3] = { end.v3[0], end.v3[2], end.v3[1] };
	return FindPath(dtstart, dtend, out_path, ft);
}

bool Navigation::FindPath(float* dtstart, float* dtend, List<Vec>& out_path, const dtQueryFilter& ft)
{
	if (nav_query_)
	{
		dtPolyRef startref = 0;
		dtPolyRef endref = 0;
		dtRaycastHit hit;
		if (Raycast(dtstart, startref, dtend, hit, ft))
		{
			if (hit.t < 1.f)
			{
				//hit. should calculate path
				int polys;
				dtPolyRef path;
				if (dtStatusFailed(nav_query_->findPath(startref, endref, (float*)&dtstart,
					(float*)&dtend, &ft, &path, &polys, MAX_POLYS)))
				{
					return false;
				}
				float straightPath[MAX_POLYS * 3];
				unsigned char straightPathFlags[MAX_POLYS];
				dtPolyRef m_straightPathPolys[MAX_POLYS];
				int nstraightPath = 0;
				if (dtStatusFailed(nav_query_->findStraightPath((float*)&dtstart,
					(float*)&dtend, &path, polys, straightPath, straightPathFlags,
					m_straightPathPolys, &nstraightPath,
					MAX_POLYS, DT_STRAIGHTPATH_ALL_CROSSINGS)))
				{
					return false;
				}

				for (int i = 0; i < nstraightPath - 1; ++i)
				{
					//dtvec to b2vec
					out_path.push_back(Vec(
						straightPath[i * 3],
						straightPath[i * 3 + 2],
						straightPath[i * 3 + 1]));
				}
			}
			else
			{
				//no hit. can go straight
				out_path.push_back(Vec(
					dtend[0],
					dtend[2],
					dtend[1]));
			}
			return true;
		}
	}
	return false;
}

bool Navigation::Raycast(const Vec& start, dtPolyRef startref,
	const Vec& end, dtRaycastHit& hit, const dtQueryFilter& ft)
{
	float dtstart[3] = { start.v3[0], start.v3[2], start.v3[1]};
	float dtend[3] = { end.v3[0], end.v3[2], end.v3[1]};
	
	return Raycast(dtstart, startref, dtend, hit, ft);
}

bool Navigation::Raycast(float* dtstart, dtPolyRef startref, float* dtend,
	dtRaycastHit& hit, const dtQueryFilter& ft)
{
	if (nav_query_)
	{
		dtPolyRef endref;
		const float extents[3]{ 0,10,0 };
		if (dtStatusFailed(nav_query_->findNearestPoly(dtstart,
			extents, &ft, &startref, nullptr)))
		{
			return false;
		}
		if (dtStatusFailed(nav_query_->findNearestPoly(dtend,
			extents, &ft, &endref, nullptr)))
		{
			return false;
		}


		if (dtStatusFailed(nav_query_->raycast(startref, dtstart,
			dtend, &ft, DT_RAYCAST_USE_COSTS, &hit)))
		{
			return false;
		}
	}
	return true;
}
