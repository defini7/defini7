#include "defGameEngine.h"

class Example : public def::GameEngine
{
public:
	Example()
	{
		SetTitle("Sample");
		ShowFPS();
	}

private:
	std::vector<def::vi2d> nodes;

	def::vi2d* selected = nullptr;

protected:
	virtual bool OnUserCreate() override
	{
		nodes.resize(5);

		nodes[0] = { 10, 100 };
		nodes[1] = { 150, 30 };
		nodes[2] = { 200, 50 };
		nodes[3] = { 250, 50 };
		nodes[4] = { 200, 100 };

		return true;
	}

	virtual bool OnUserUpdate(float fDeltaTime) override
	{
		Clear(def::BLACK);

		for (size_t i = 0; i < nodes.size() - 1; i += 2)
		{
			def::vi2d op = nodes[i];
			def::vi2d np = op;

			for (float t = 0.0f; t <= 1.0f; t += 0.01f)
			{
				np = (1.0f - t) * (1.0f - t) * nodes[i] + 2.0f * (1.0f - t) * t * nodes[i + 1] + (t * t) * nodes[i + 2];
				DrawLine(op, np);
				op = np;
			}
		}

		if (GetMouse(0).bPressed)
		{
			for (size_t i = 0; i < nodes.size(); i++)
			{
				if (nodes[i] == GetMouse<int>())
				{
					selected = &nodes[i];
					break;
				}
			}
		}
		
		if (GetMouse(0).bReleased)
			selected = nullptr;

		if (selected)
			*selected = GetMouse<int>();

		for (size_t i = 0; i < nodes.size(); i++)
			Draw(nodes[i], def::RED);

		return true;
	}
};


int main()
{
	Example demo;
	demo.Construct(256, 240, 4, 4);
	demo.Run();
	return 0;
}
