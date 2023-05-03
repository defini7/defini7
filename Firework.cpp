#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

struct Particle
{
	float fOffsetX;
	float fOffsetY;

	float fDirX;
	float fDirY;

	float fSpeed;

	olc::Pixel pCol;
};

struct Firework
{
	float x, y;

	float fExplodedX;
	float fExplodedY;

	// Current time in milliseconds
	float fExplosionTimer;

	// Time when firework must explode
	float fExplosionTime;

	// Local timer to safe current lifetime
	float fLifeTimer;

	// How long particles will be alive
	float fLifeTime;
	
	std::vector<Particle> vecParticles;
};

class Fireworks : public olc::PixelGameEngine
{
public:
	Fireworks()
	{
		sAppName = "Fireworks";
	}

private:
	std::vector<Firework> vecFireworks;

	// Count time in milliseconds
	float fTimer;
	
	// When new firework will appear
	float fAppearTime;

	const float fFireworkSpeed = 100.0f;
	const float fParticleSpeed = 10.0f;

private:
	void AddFirework()
	{
		Firework f;

		f.x = ((rand() + int(float(ScreenWidth() - 1) * 0.25f)) % int(float(ScreenWidth() - 1) * 0.75f));
		f.y = ScreenHeight() - 1;

		f.fExplodedX = -1.0f;
		f.fExplodedY = -1.0f;

		f.fExplosionTimer = 0.0f;
		f.fExplosionTime = float((rand() % 2) + 1);

		f.fLifeTimer = 0.0f;
		f.fLifeTime = float((rand() % 4) + 2);

		int nParticles = (rand() + 20) % 40;

		for (int i = 0; i < nParticles; i++)
		{
			Particle p;

			p.fOffsetX = 0.0f;
			p.fOffsetY = 0.0f;

			p.fSpeed = float((rand() % 10) + 5);

			float fAngle = 2.0f * 3.1415926f * ((float)rand() / (float)RAND_MAX);

			p.fDirX = cosf(fAngle);
			p.fDirY = sinf(fAngle);

			p.pCol = olc::Pixel(rand() % 256, rand() % 256, rand() % 256);

			f.vecParticles.push_back(p);
		}

		vecFireworks.push_back(f);
	}

public:
	bool OnUserCreate() override
	{
		srand(time(NULL));

		fTimer = 0.0f;
		fAppearTime = 1.0f;

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		if (fTimer >= fAppearTime)
		{
			AddFirework();
			fTimer = 0.0f;
		}
		else fTimer += fElapsedTime;

		Clear(olc::BLACK);
		
		size_t i = 0;
		for (auto& f : vecFireworks)
		{
			if (f.fExplosionTimer >= f.fExplosionTime)
			{
				bool bJustExploded = f.fExplodedX < 0.0f;
				
				if (bJustExploded)
				{
					f.fExplodedX = f.x;
					f.fExplodedY = f.y;
				}

				for (auto& p : f.vecParticles)
				{
					p.fOffsetX += (fParticleSpeed + p.fSpeed) * p.fDirX * fElapsedTime;
					p.fOffsetY += (fParticleSpeed + p.fSpeed) * p.fDirY * fElapsedTime;

					Draw(f.fExplodedX + p.fOffsetX, f.fExplodedY + p.fOffsetY, p.pCol);
				}

				if (f.fLifeTimer > f.fLifeTime)
					vecFireworks.erase(vecFireworks.begin() + i);
				else
					f.fLifeTimer += fElapsedTime;
			}
			else
			{
				// Update timer
				f.fExplosionTimer += fElapsedTime;

				// Update position
				f.y -= fFireworkSpeed * fElapsedTime;

				// Draw path
				Draw((int)f.x, (int)f.y, olc::WHITE);
			}

			i++;
		}

		return true;
	}
};

int main()
{
	Fireworks demo;
	if (demo.Construct(256, 240, 4, 4))
		demo.Start();

	return 0;
}
