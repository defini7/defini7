#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

constexpr int32_t NODE_SIZE = 12;

struct Shape;

struct Node
{
	Shape* parent;
	olc::vi2d pos;
};

struct Shape
{
	std::vector<Node> vecNodes;

	size_t nMaxNodes;
	bool bExpired = false;

	virtual void DrawYourself(olc::PixelGameEngine* pge) = 0;

	Node* GetNextNode(const olc::vf2d& pos)
	{
		if (vecNodes.size() == nMaxNodes)
			return nullptr;

		Node n;
		n.parent = this;
		n.pos = pos;
		vecNodes.push_back(n);
		
		return &vecNodes.back();
	}

	void DrawNodes(olc::PixelGameEngine* pge)
	{
		for (const auto& n : vecNodes)
			pge->FillCircle(n.pos, 2, olc::RED);
	}

	Node* HitNode(const olc::vi2d& vPos)
	{
		for (auto& n : vecNodes)
		{
			if (n.pos == vPos)
				return &n;
		}

		return nullptr;
	}
};

struct Line : Shape
{
	Line()
	{
		vecNodes.reserve(2);
		nMaxNodes = 2;
	}

	virtual void DrawYourself(olc::PixelGameEngine* pge) override
	{
		pge->FillCircle(vecNodes[0].pos, 2, olc::RED);
		pge->FillCircle(vecNodes[1].pos, 2, olc::RED);

		pge->DrawLine(vecNodes[0].pos, vecNodes[1].pos, olc::WHITE, 0xF0F0F0F0F);

		pge->DrawLine(vecNodes[0].pos, vecNodes[1].pos);
	}
};

struct Circle : Shape
{
	Circle()
	{
		vecNodes.reserve(2);
		nMaxNodes = 2;
	}

	virtual void DrawYourself(olc::PixelGameEngine* pge) override
	{
		pge->DrawLine(vecNodes[0].pos, vecNodes[1].pos, olc::WHITE, 0xF0F0F0F0F);

		uint32_t nRadius = uint32_t((vecNodes[1].pos - vecNodes[0].pos).mag());

		pge->DrawCircle(vecNodes[0].pos, nRadius);
	}
};

struct Rect : Shape
{
	Rect()
	{
		vecNodes.reserve(2);
		nMaxNodes = 2;
	}

	virtual void DrawYourself(olc::PixelGameEngine* pge) override
	{
		olc::vi2d p1 = vecNodes[0].pos;
		olc::vi2d p2 = vecNodes[1].pos;

		if (p1 > p2)
			std::swap(p1, p2);

		pge->DrawRect(p1, p2 - p1);
	}
};

struct Curve : Shape
{
	Curve()
	{
		vecNodes.reserve(3);
		nMaxNodes = 3;
	}

	virtual void DrawYourself(olc::PixelGameEngine* pge) override
	{
		if (vecNodes.size() == 2)
			pge->DrawLine(vecNodes[0].pos, vecNodes[1].pos, olc::WHITE, 0xF0F0F0F0F);

		if (vecNodes.size() == 3)
		{
			pge->DrawLine(vecNodes[0].pos, vecNodes[1].pos, olc::WHITE, 0xF0F0F0F0F);
			pge->DrawLine(vecNodes[1].pos, vecNodes[2].pos, olc::WHITE, 0xF0F0F0F0F);

			olc::vi2d op = vecNodes[0].pos;
			olc::vi2d np = op;

			for (float t = 0.0f; t <= 1.0f; t += 0.01f)
			{
				np = (1 - t) * (1 - t) * vecNodes[0].pos + 2 * (1 - t) * t * vecNodes[1].pos + t * t * vecNodes[2].pos;
				pge->DrawLine(op, np);
				op = np;
			}
		}
	}
};

class SimpleCAD : public olc::PixelGameEngine
{
public:
	SimpleCAD()
	{
		sAppName = "SimpleCAD";
	}

	virtual ~SimpleCAD()
	{
		for (auto& shape : vecShapes)
			delete shape;
	}

public:
	void DrawSelectedArea()
	{
		if (vSelectedArea.second != olc::vi2d(-1, -1))
		{
			olc::vi2d first = vSelectedArea.first;
			olc::vi2d second = vSelectedArea.second;

			if (first > second)
				std::swap(first, second);

			SetPixelMode(olc::Pixel::ALPHA);
			FillRect(first, second - first, olc::Pixel(255, 255, 255, 122));
			SetPixelMode(olc::Pixel::NORMAL);
		}
	}

	bool OnUserCreate() override
	{
		vTop.x = ScreenWidth() / 2 - (ScreenWidth() / 2) % NODE_SIZE;
		vTop.y = 0;

		vBottom.x = vTop.x;
		vBottom.y = ScreenHeight();

		vLeft.x = 0;
		vLeft.y = ScreenHeight() / 2 - (ScreenHeight() / 2) % NODE_SIZE;

		vRight.x = ScreenWidth();
		vRight.y = vLeft.y;

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		Clear(olc::VERY_DARK_BLUE);

		olc::vi2d vCursor;
		vCursor.x = GetMouseX() - GetMouseX() % NODE_SIZE;
		vCursor.y = GetMouseY() - GetMouseY() % NODE_SIZE;

		if (GetKey(olc::Key::L).bPressed)
		{
			tempShape = new Line();

			selected = tempShape->GetNextNode(vCursor);
			selected = tempShape->GetNextNode(vCursor);
		}

		if (GetKey(olc::Key::C).bPressed)
		{
			tempShape = new Circle();

			selected = tempShape->GetNextNode(vCursor);
			selected = tempShape->GetNextNode(vCursor);
		}

		if (GetKey(olc::Key::R).bPressed)
		{
			tempShape = new Rect();

			selected = tempShape->GetNextNode(vCursor);
			selected = tempShape->GetNextNode(vCursor);
		}

		if (GetKey(olc::Key::B).bPressed)
		{
			tempShape = new Curve();

			selected = tempShape->GetNextNode(vCursor);
			selected = tempShape->GetNextNode(vCursor);
		}

		if (GetMouse(1).bPressed)
		{
			for (const auto& shape : vecShapes)
			{
				Node* node = shape->HitNode(vCursor);

				if (node)
				{
					selected = node;
					break;
				}
			}

			if (!selected)
			{
				vSelectedArea.first = vCursor;
				vSelectedArea.second = { -1, -1 };
			}
		}

		if (GetMouse(1).bHeld)
		{
			if (!selected)
				vSelectedArea.second = vCursor;
		}

		if (GetKey(olc::Key::D).bPressed)
		{
			if (vSelectedArea.second != olc::vi2d(-1, -1))
			{
				for (int32_t x = vSelectedArea.first.x; x <= vSelectedArea.second.x; x += NODE_SIZE)
					for (int32_t y = vSelectedArea.first.y; y <= vSelectedArea.second.y; y += NODE_SIZE)
					{
						for (const auto& shape : vecShapes)
						{
							Node* node = shape->HitNode({ x, y });

							if (node)
								node->parent->bExpired = true;
						}
					}
				
				vSelectedArea.first = { -1, -1 };
				vSelectedArea.second = { -1, -1 };
			}

			if (tempShape)
			{
				delete tempShape;
				tempShape = nullptr;
			}
			else
			{
				if (selected)
				{
					selected->parent->bExpired = true;
					selected = nullptr;
				}
			}
		}

		if (selected)
			selected->pos = vCursor;

		if (GetMouse(0).bReleased)
		{
			if (tempShape)
			{
				selected = tempShape->GetNextNode(vCursor);
				if (!selected)
				{
					vecShapes.push_back(tempShape);
					tempShape = nullptr;
				}
			}
			else
				selected = nullptr;
		}

		DrawCircle(vCursor, 2, olc::DARK_GREY);

		for (int32_t x = 0; x < ScreenWidth(); x += NODE_SIZE)
			for (int32_t y = 0; y < ScreenHeight(); y += NODE_SIZE)
				Draw(x, y, olc::DARK_BLUE);

		DrawLine(vTop, vBottom, olc::GREY, 0xF0F0F0F0F);
		DrawLine(vLeft, vRight, olc::GREY, 0xF0F0F0F0F);

		for (size_t i = 0; i < vecShapes.size(); i++)
		{
			auto shape = vecShapes[i];

			if (shape->bExpired)
			{
				vecShapes.erase(vecShapes.begin() + i);
				continue;
			}

			shape->DrawYourself(this);
			shape->DrawNodes(this);
		}

		if (tempShape)
		{
			tempShape->DrawYourself(this);
			tempShape->DrawNodes(this);
		}

		DrawSelectedArea();

		return true;
	}

private:
	std::vector<Shape*> vecShapes;

	olc::vi2d vTop, vBottom, vLeft, vRight;

	Shape* tempShape = nullptr;
	Node* selected = nullptr;

	std::pair<olc::vi2d, olc::vi2d> vSelectedArea;
	
};

int main()
{
	SimpleCAD demo;

	if (demo.Construct(1280, 720, 1, 1))
		demo.Start();

	return 0;
}
