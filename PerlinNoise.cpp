#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

constexpr float BOUND = (3.14159265 / ~(~0u >> 1));

class Example : public olc::PixelGameEngine
{
public:
	Example()
	{
		sAppName = "Example";
	}

protected:
	olc::vf2d RandGradient(const olc::vi2d& vPos)
	{
		uint32_t nWidth = 8 * sizeof(uint32_t);
		uint32_t nRotated = nWidth / 2;

		uint64_t x = (uint64_t)vPos.x;
		uint64_t y = (uint64_t)vPos.y;

		x *= 3284157443;
		y ^= x << nRotated | x >> nWidth - nRotated;

		y *= 1911520717;
		x ^= y << nRotated | y >> nWidth - nRotated;

		x *= 2048419325;

		float fRand = (float)x * BOUND;

		return { cosf(fRand), sinf(fRand) };
	}

	float DotGradient(const olc::vi2d& vPosI, const olc::vf2d& vPosF)
	{
		olc::vf2d vGrad = RandGradient(vPosI);
		olc::vf2d vDist = vPosF - vPosI;

		return vDist.dot(vGrad);
	}

	float PerlinNoise2D(const olc::vf2d& vPos)
	{
		auto Inter = [](float fStart, float fEnd, float fStep)
		{
			return (fEnd - fStart) * fStep + fStart;
		};

		olc::vi2d p0 = vPos.floor();
		olc::vi2d p1 = vPos + olc::vu2d(1, 1);

		olc::vf2d s = vPos - p0;

		float n0, n1, ix0, ix1;

		n0 = DotGradient(p0, vPos);
		n1 = DotGradient({ p1.x, p0.y }, vPos);
		ix0 = Inter(n0, n1, s.x);

		n0 = DotGradient({ p0.x, p1.y }, vPos);
		n1 = DotGradient(p1, vPos);
		ix1 = Inter(n0, n1, s.x);

		return Inter(ix0, ix1, s.y) * 0.5f + 0.5f;
	}

	void ApplyPerlinNoise2D()
	{
		for (int32_t x = 0; x < ScreenWidth(); x++)
			for (int32_t y = 0; y < ScreenHeight(); y++)
			{
				size_t i = y * ScreenWidth() + x;
				pPerlinNoise[i] = PerlinNoise2D(olc::vf2d(x, y) / GetScreenSize());
			}
	}

public:
	bool OnUserCreate() override
	{
		pPerlinNoise = new float[ScreenWidth() * ScreenHeight()];

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		auto tStart = std::chrono::high_resolution_clock::now();
		ApplyPerlinNoise2D();
		auto tEnd = std::chrono::high_resolution_clock::now();

		double dElapsed = std::chrono::duration<double>(tEnd - tStart).count();

		for (int32_t x = 0; x < ScreenWidth(); x++)
			for (int32_t y = 0; y < ScreenHeight(); y++)
			{
				float n = pPerlinNoise[y * ScreenWidth() + x];

				Draw(x, y, olc::PixelF(n, 1.0f - n, 1.0f - n));
			}

		DrawString(10, 10, "Took " + std::to_string(dElapsed) + "s");

		return true;
	}

protected:
	float* pPerlinNoise;

};

int main()
{
	Example demo;

	if (demo.Construct(256, 240, 4, 4))
		demo.Start();

	return 0;
}
