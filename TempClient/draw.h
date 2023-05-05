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

static const unsigned int startCol = bb::RGBA<128, 25, 0, 192>::VAL;
static const unsigned int endCol = bb::RGBA<51, 102, 0, 129>::VAL;
static const unsigned int pathCol = bb::RGBA<0, 0, 0, 64>::VAL;


class DebugDrawGLBB : public duDebugDraw
{
public:
	virtual unsigned int areaToCol(unsigned int area)
	{
		switch (area)
		{
			// Ground (0) : light blue
		case SAMPLE_POLYAREA_GROUND: return bb::RGBA<0, 192, 255, 255>::VAL;
			// Water : blue
		case SAMPLE_POLYAREA_WATER: return bb::RGBA<0, 0, 255, 255>::VAL;
			// Road : brown
		case SAMPLE_POLYAREA_ROAD: return bb::RGBA<50, 20, 12, 255>::VAL;
			// Door : cyan
		case SAMPLE_POLYAREA_DOOR: return bb::RGBA<0, 255, 255, 255>::VAL;
			// Grass : green
		case SAMPLE_POLYAREA_GRASS: return bb::RGBA<0, 255, 0, 255>::VAL;
			// Jump : yellow
		case SAMPLE_POLYAREA_JUMP: return bb::RGBA<255, 255, 0, 255>::VAL;
			// Unexpected : red
		default: return bb::RGBA<255, 0, 0, 255>::VAL;
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
    static void DebugCylinder(const Vec& center, float r, float h, float c, const unsigned int col)
    {
		auto pos = VecToDt(center);

		DebugDrawGLBB dd;
		dd.depthMask(false);

		// Agent dimensions.	
		duDebugDrawCylinderWire(&dd, pos[0] - r, pos[1] + 0.02f, pos[2] - r,
			pos[0] + r, pos[1] + h, pos[2] + r, col, 2.0f);

		duDebugDrawCircle(&dd, pos[0], pos[1] + c, pos[2], r, bb::RGBA<0, 0, 0, 64>::VAL, 1.0f);

		auto colb = bb::RGBA<0, 0, 0, 196>::VAL;
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
	static void DebugLine(const Vec& a, const Vec& b, const unsigned int col, float lineWidth)
	{
		auto dta = VecToDt(a);
		auto dtb = VecToDt(b);

		DebugDrawGLBB dd;
		dd.depthMask(false);
		dd.begin(DU_DRAW_LINES, lineWidth);

		dd.vertex(dta[0], dta[1], dta[2], col);
		dd.vertex(dtb[0], dtb[1], dtb[2], col);

		dd.end();
		dd.depthMask(true);
	}
	static void DebugCircle(const Vec& pos, const float r, const unsigned int col, float lineWidth)
	{
		auto dtpos = VecToDt(pos);

		DebugDrawGLBB dd;
		dd.depthMask(false);
		dd.begin(DU_DRAW_LINES, lineWidth);
		duAppendCircle(&dd, dtpos[0], dtpos[1], dtpos[2], r, col);
		dd.end();
		dd.depthMask(true);
	}
	static void DebugBoxWire(const Vec& min, const Vec& max, const unsigned int col, float lineWidth)
	{
		auto dtmin = VecToDt(min);
		auto dtmax = VecToDt(max);

		DebugDrawGLBB dd;
		dd.depthMask(false);
		dd.begin(DU_DRAW_LINES, lineWidth);
		duAppendBoxWire(&dd, dtmin[0], dtmin[1], dtmin[2], dtmax[0], dtmax[1], dtmax[2], col);
		dd.end();
		dd.depthMask(true);
	}
	static void DebugObbWire(const std::array<Vec, 4> vertices, const unsigned int col, float lineWidth)
	{
		DebugDrawGLBB dd;
		dd.depthMask(false);
		dd.begin(DU_DRAW_LINES, lineWidth);

		dd.vertex(vertices[0].v3.x, 1, vertices[0].v3.y, col);
		dd.vertex(vertices[1].v3.x, 1, vertices[1].v3.y, col);

		dd.vertex(vertices[1].v3.x, 1, vertices[1].v3.y, col);
		dd.vertex(vertices[2].v3.x, 1, vertices[2].v3.y, col);

		dd.vertex(vertices[2].v3.x, 1, vertices[2].v3.y, col);
		dd.vertex(vertices[3].v3.x, 1, vertices[3].v3.y, col);

		dd.vertex(vertices[3].v3.x, 1, vertices[3].v3.y, col);
		dd.vertex(vertices[0].v3.x, 1, vertices[0].v3.y, col);

		dd.end();
		dd.depthMask(true);
	}
	static void DebugSector(const Vec& center, short half_angle, short dir, float length,
		const unsigned int col, float lineWidth)
	{
		short min = dir - half_angle;
		short max = dir + half_angle;
		auto a = center.v2 + BBMath::DirLength(min, length).v2;
		auto b = center.v2 + BBMath::DirLength(max, length).v2;
		DebugDrawGLBB dd;
		dd.depthMask(false);
		dd.begin(DU_DRAW_LINES, lineWidth);

		dd.vertex(center.v2.x, 1, center.v2.y, col);
		dd.vertex(a.x, 1, a.y, col);

		dd.vertex(center.v2.x, 1, center.v2.y, col);
		dd.vertex(b.x, 1, b.y, col);

		auto angle = half_angle * 2;
		angle = angle / 10;
		for (int i = 0; i < 10; ++i)
		{
			short l = static_cast<short>(min + angle * i);
			short r = static_cast<short>(l + angle);
			auto minv = center.v2 + BBMath::DirLength(l, length).v2;
			auto maxv = center.v2 + BBMath::DirLength(r, length).v2;
			dd.vertex(minv.x, 1, minv.y, col);
			dd.vertex(maxv.x, 1, maxv.y, col);
		}

		dd.end();
		dd.depthMask(true);
	}
	static void DebugText(float x, float y, const char* str)
	{
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | 
			ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | 
			ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNav |
			ImGuiWindowFlags_NoInputs;
		ImGui::SetNextWindowPos(ImVec2(x, y));
		ImGui::SetNextWindowBgAlpha(0.f); // Transparent background
		//ImGui::SetWindowSize(ImVec2(200, 100));
		if (ImGui::Begin(str, (bool*)1, window_flags))
		{
			ImGui::SetWindowFontScale(0.7f);
			ImGui::Text(str);
			ImGui::SetWindowFontScale(1.f);
		}
		ImGui::End();
	}
};