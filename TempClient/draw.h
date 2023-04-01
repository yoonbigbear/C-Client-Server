#pragma once
#include "share/detour/DebugDraw.h"

#include "bb_util.h"
#include "SDL_opengl.h"

enum SamplePolyAreas
{
	SAMPLE_POLYAREA_GROUND,
	SAMPLE_POLYAREA_WATER,
	SAMPLE_POLYAREA_ROAD,
	SAMPLE_POLYAREA_DOOR,
	SAMPLE_POLYAREA_GRASS,
	SAMPLE_POLYAREA_JUMP,
};

static const unsigned int startCol = BB::RGBA<128, 25, 0, 192>::VAL;
static const unsigned int endCol = BB::RGBA<51, 102, 0, 129>::VAL;
static const unsigned int pathCol = BB::RGBA<0, 0, 0, 64>::VAL;

class DebugDrawGLBB : public duDebugDraw
{
public:
	virtual unsigned int areaToCol(unsigned int area)
	{
		switch (area)
		{
			// Ground (0) : light blue
		case SAMPLE_POLYAREA_GROUND: return BB::RGBA<0, 192, 255, 255>::VAL;
			// Water : blue
		case SAMPLE_POLYAREA_WATER: return BB::RGBA<0, 0, 255, 255>::VAL;
			// Road : brown
		case SAMPLE_POLYAREA_ROAD: return BB::RGBA<50, 20, 12, 255>::VAL;
			// Door : cyan
		case SAMPLE_POLYAREA_DOOR: return BB::RGBA<0, 255, 255, 255>::VAL;
			// Grass : green
		case SAMPLE_POLYAREA_GRASS: return BB::RGBA<0, 255, 0, 255>::VAL;
			// Jump : yellow
		case SAMPLE_POLYAREA_JUMP: return BB::RGBA<255, 255, 0, 255>::VAL;
			// Unexpected : red
		default: return BB::RGBA<255, 0, 0, 255>::VAL;
		}
	}
	virtual void depthMask(bool state)
	{
		glDepthMask(state ? GL_TRUE : GL_FALSE);
	}
	virtual void texture(bool state)
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
	virtual void begin(duDebugDrawPrimitives prim, float size = 1.0f)

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
	virtual void vertex(const float* pos, unsigned int color)
	{
		glColor4ubv((GLubyte*)&color);
		glVertex3fv(pos);
	}

	virtual void vertex(const float x, const float y, const float z, unsigned int color)
	{
		glColor4ubv((GLubyte*)&color);
		glVertex3f(x, y, z);
	}
	virtual void vertex(const float* pos, unsigned int color, const float* uv)
	{
		glColor4ubv((GLubyte*)&color);
		glTexCoord2fv(uv);
		glVertex3fv(pos);
	}
	virtual void vertex(const float x, const float y, const float z,
		unsigned int color, const float u, const float v)
	{
		glColor4ubv((GLubyte*)&color);
		glTexCoord2f(u, v);
		glVertex3f(x, y, z);
	}

	virtual void end()
	{
		glEnd();
		glLineWidth(1.0f);
		glPointSize(1.0f);
	}

	void vertex(const double* pos, unsigned int color)
	{
		glColor4ubv((GLubyte*)&color);
		glVertex3dv(pos);
	}
	void vertex(const double x, const double y, const double z, unsigned int color)
	{
		glColor4ubv((GLubyte*)&color);
		glVertex3d(x, y, z);
	}
	void vertex(const double* pos, unsigned int color, const double* uv)
	{
		glColor4ubv((GLubyte*)&color);
		glTexCoord2dv(uv);
		glVertex3dv(pos);
	}
	void vertex(const double x, const double y, const double z, 
		unsigned int color, const double u, const double v)
	{
		glColor4ubv((GLubyte*)&color);
		glTexCoord2d(u, v);
		glVertex3d(x, y, z);
	}

};

class Draw
{
public:
    static void Cylinder(const float* pos, float r, float h, float c, const unsigned int col)
    {
		DebugDrawGLBB dd;
		dd.depthMask(false);

		// Agent dimensions.	
		duDebugDrawCylinderWire(&dd, pos[0] - r, pos[1] + 0.02f, pos[2] - r,
			pos[0] + r, pos[1] + h, pos[2] + r, col, 2.0f);

		duDebugDrawCircle(&dd, pos[0], pos[1] + c, pos[2], r, BB::RGBA<0, 0, 0, 64>::VAL, 1.0f);

		auto colb = BB::RGBA < 0, 0, 0, 196>::VAL;
		dd.begin(DU_DRAW_LINES);
		dd.vertex(pos[0], pos[1] - c, pos[2], colb);
		dd.vertex(pos[0], pos[1] + c, pos[2], colb);
		dd.vertex(pos[0] - r / 2, pos[1] + 0.02f, pos[2], colb);
		dd.vertex(pos[0] + r / 2, pos[1] + 0.02f, pos[2], colb);
		dd.vertex(pos[0], pos[1] + 0.02f, pos[2] - r / 2, colb);
		dd.vertex(pos[0], pos[1] + 0.02f, pos[2] + r / 2, colb);
		dd.end();

		dd.depthMask(true);
    }
	static void Line(const float* a, float lineWidth, const unsigned int col)
	{
		DebugDrawGLBB dd;
		dd.depthMask(false);
		dd.begin(DU_DRAW_LINES, lineWidth);

		dd.vertex(a[0], a[1], a[2], col);
		//dd.vertex(b[0], b[1], b[2], col);

		dd.end();
		dd.depthMask(true);
	}
	static void DrawCircle(const float x, const float y, const float z,
		const float r, const unsigned int col, float lineWidth)
	{
		DebugDrawGLBB dd;
		dd.depthMask(false);
		dd.begin(DU_DRAW_LINES, lineWidth);
		duAppendCircle(&dd, x, y, z, r, col);
		dd.end();
		dd.depthMask(true);
	}
	static void DrawBoxWire(float minx, float miny, float minz,
		float maxx, float maxy, float maxz, const unsigned int col, float lineWidth)
	{
		DebugDrawGLBB dd;
		dd.depthMask(false);
		dd.begin(DU_DRAW_LINES, lineWidth);
		duAppendBoxWire(&dd, minx, miny, minz, maxx, maxy, maxz, col);
		dd.end();
		dd.depthMask(true);
	}
};