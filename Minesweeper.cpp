#include <array>

#include <SFML/Graphics.hpp>

#pragma comment(lib, "sfml-graphics-d.lib")
#pragma comment(lib, "sfml-window-d.lib")
#pragma comment(lib, "sfml-system-d.lib")
#pragma comment(lib, "sfml-main-d.lib")

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr int CELL_WIDTH = 32;
constexpr int CELL_HEIGHT = 32;
constexpr int MAP_WIDTH = SCREEN_WIDTH / CELL_WIDTH;
constexpr int MAP_HEIGHT = SCREEN_HEIGHT / CELL_HEIGHT;

struct Cell
{
	bool mine = false;
	bool open = false;
	bool flag = false;
	int around = 0;
};

struct World
{
	Cell map[MAP_WIDTH * MAP_HEIGHT];

	bool inside(int x, int y) const
	{
		return x >= 0 && y >= 0 && x < MAP_WIDTH && y < MAP_HEIGHT;
	}

	Cell& get(int x, int y)
	{
		return map[y * MAP_WIDTH + x];
	}

	int totalMines = 0;
	int closedCells = 0;
	bool failed = false;
};

void NewGame(World& world, int totalMines)
{
	srand(time(NULL));
	for (int i = 0; i < MAP_WIDTH * MAP_HEIGHT; i++)
	{
		world.map[i].around = 0;
		world.map[i].flag = false;
		world.map[i].mine = false;
		world.map[i].open = false;
	}

	world.totalMines = totalMines;
	world.closedCells = MAP_WIDTH * MAP_HEIGHT;
	world.failed = false;

	for (int i = 0; i < world.totalMines; i++)
	{
		int x = rand() % MAP_WIDTH;
		int y = rand() % MAP_HEIGHT;

		if (world.get(x, y).mine)
			i--;
		else
		{
			world.get(x, y).mine = true;
			
			for (int dx = -1; dx < 2; dx++)
				for (int dy = -1; dy < 2; dy++)
				{
					if (world.inside(x + dx, y + dy))
						world.get(x + dx, y + dy).around++;
				}
		}
	}
}

void OpenFields(World& world, int x, int y)
{
	if (!world.inside(x, y) || world.get(x, y).open)
		return;

	world.get(x, y).open = true;
	world.closedCells--;

	if (world.get(x, y).around == 0)
	{
		for (int dx = -1; dx < 2; dx++)
			for (int dy = -1; dy < 2; dy++)
				OpenFields(world, x + dx, y + dy);
	}

	if (world.get(x, y).mine)
	{
		world.failed = true;

		for (int y = 0; y < MAP_HEIGHT; y++)
			for (int x = 0; x < MAP_WIDTH; x++)
				world.get(x, y).open = true;
	}
}

void ShowGame(World& world, sf::RenderWindow& window, sf::Font& font)
{
	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(CELL_WIDTH, CELL_HEIGHT));

	sf::Text text;
	text.setFont(font);

	auto drawCell = [&](int x, int y, const sf::Color& col)
	{
		rect.setPosition(x * CELL_WIDTH, y * CELL_HEIGHT);
		rect.setFillColor(col);
		window.draw(rect);
	};

	auto drawNumber = [&](int x, int y, int n, const sf::Color& col)
	{
		text.setFillColor(col);
		text.setString(std::to_string(n));
		text.setPosition((x + 0.25f) * CELL_WIDTH, y * CELL_HEIGHT);
		window.draw(text);
	};

	for (int y = 0; y < MAP_HEIGHT; y++)
		for (int x = 0; x < MAP_WIDTH; x++)
		{
			auto& cell = world.get(x, y);

			if (cell.open)
			{
				drawCell(x, y, sf::Color(0xC0C0C0FF)); // open field

				if (cell.mine)
					drawCell(x, y, sf::Color::Black);
				else if (cell.around > 0)
					drawNumber(x, y, cell.around, sf::Color::White);
			}
			else
			{
				drawCell(x, y, sf::Color(0x202020FF));

				if (cell.flag) // flag is 7 for now
					drawNumber(x, y, 7, sf::Color::Red);
			}
		}
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "Minesweeper!");

	World world;
	NewGame(world, 40);

	sf::Font font;
	font.loadFromFile("arial.ttf");
	
	sf::Mouse mouse;

	while (window.isOpen())
	{
		sf::Event evt;
		while (window.pollEvent(evt))
		{
			switch (evt.type)
			{
			case sf::Event::Closed: window.close(); break;

			case sf::Event::MouseButtonPressed:
			{
				switch (evt.mouseButton.button)
				{
				case sf::Mouse::Left:
				{
					if (world.failed)
						NewGame(world, 40);
					else
					{
						int x = evt.mouseButton.x / CELL_WIDTH;
						int y = evt.mouseButton.y / CELL_HEIGHT;

						if (world.inside(x, y) && !world.get(x, y).flag)
						{
							OpenFields(world, x, y);
						}
					}
				}
				break;

				case sf::Mouse::Right:
				{
					int x = evt.mouseButton.x / CELL_WIDTH;
					int y = evt.mouseButton.y / CELL_HEIGHT;

					if (world.inside(x, y))
						world.get(x, y).flag = !world.get(x, y).flag;
				}
				break;

				}
			}
			break;

			}
		}

		if (world.totalMines == world.closedCells)
			NewGame(world, 40);

		ShowGame(world, window, font);

		if (world.failed)
		{
			sf::Text text;
			text.setFont(font);
			text.setFillColor(sf::Color::Red);
			text.setStyle(sf::Text::Style::Bold);
			text.setString("Press LMB to play again!");
			text.setPosition(SCREEN_WIDTH * 0.3f, SCREEN_HEIGHT * 0.45f);
			window.draw(text);
		}

		window.display();
	}

	return 0;
}
