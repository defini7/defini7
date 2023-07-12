#include <SFML/Graphics.hpp>

#pragma comment(lib, "sfml-graphics-d.lib")
#pragma comment(lib, "sfml-window-d.lib")
#pragma comment(lib, "sfml-system-d.lib")
#pragma comment(lib, "sfml-main-d.lib")

constexpr int WORLD_WIDTH = 4;
constexpr int WORLD_HEIGHT = 4;

constexpr int CELL_SIZE = 96;

constexpr int SCREEN_WIDTH = WORLD_WIDTH * CELL_SIZE;
constexpr int SCREEN_HEIGHT = WORLD_HEIGHT * CELL_SIZE;

bool AddNew(uint16_t world[WORLD_HEIGHT][WORLD_WIDTH])
{
	int x, y, i = 0;

	do
	{
		x = rand() % WORLD_WIDTH;
		y = rand() % WORLD_HEIGHT;
		i++;
	} while (world[y][x] != 0 && i <= WORLD_WIDTH * WORLD_HEIGHT);

	if (i == WORLD_WIDTH * WORLD_HEIGHT)
		return false;

	world[y][x] = 2;
	return true;
}

bool UpdateWorld(uint16_t world[WORLD_HEIGHT][WORLD_WIDTH], int dx, int dy)
{
	if (dx < 0) // 0 -> w
	{
		for (int j = 0; j < WORLD_HEIGHT; j++)
			for (int i = 0; i < WORLD_WIDTH; i++)
			{
				int ni = i + dx;

				int k = 0;
				while (ni >= 0)
				{
					int oi = i + dx * k;
					bool abort = false;

					uint16_t val;
					if (world[j][ni] == 0)
						val = world[j][oi];
					else if (world[j][ni] == world[j][oi])
						abort = (val = world[j][ni] * 2);
					else
						break;

					world[j][ni] = val;
					world[j][oi] = 0;
					if (abort) break;

					ni += dx;
					k++;
				}
			}
	}

	if (dy < 0) // 0 -> h
	{
		for (int j = 0; j < WORLD_HEIGHT; j++)
			for (int i = 0; i < WORLD_WIDTH; i++)
			{
				int nj = j + dy;

				int k = 0;
				while (nj >= 0)
				{
					int oj = j + dy * k;
					bool abort = false;

					uint16_t val;
					if (world[nj][i] == 0)
						val = world[oj][i];
					else if (world[nj][i] == world[oj][i])
						abort = (val = world[nj][i] * 2);
					else
						break;

					world[nj][i] = val;
					world[oj][i] = 0;
					if (abort) break;

					nj += dy;
					k++;
				}
			}
	}

	if (dx > 0) // 0 <- w
	{
		for (int j = 0; j < WORLD_HEIGHT; j++)
			for (int i = WORLD_WIDTH - 1; i >= 0; i--)
			{
				int ni = i + dx;

				int k = 0;
				while (ni < WORLD_WIDTH)
				{
					int oi = i + dx * k;
					bool abort = false;

					uint16_t val;
					if (world[j][ni] == 0)
						val = world[j][oi];
					else if (world[j][ni] == world[j][oi])
						abort = (val = world[j][ni] * 2);
					else
						break;

					world[j][ni] = val;
					world[j][oi] = 0;
					if (abort) break;

					ni += dx;
					k++;
				}
			}
	}

	if (dy > 0) // 0 <- h
	{
		for (int j = WORLD_HEIGHT - 1; j >= 0; j--)
			for (int i = 0; i < WORLD_WIDTH; i++)
			{
				int nj = j + dy;

				int k = 0;
				while (nj < WORLD_HEIGHT)
				{
					int oj = j + dy * k;
					bool abort = false;

					uint16_t val;
					if (world[nj][i] == 0)
						val = world[oj][i];
					else if (world[nj][i] == world[oj][i])
						abort = (val = world[nj][i] * 2);
					else
						break;

					world[nj][i] = val;
					world[oj][i] = 0;
					if (abort) break;

					nj += dy;
					k++;
				}
			}
	}

	if (dx != 0 || dy != 0)
	{
		if (!AddNew(world))
			return false;
	}

	return true;
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(SCREEN_WIDTH, SCREEN_HEIGHT), "2048");

	sf::RectangleShape rect;
	rect.setSize(sf::Vector2f(CELL_SIZE, CELL_SIZE));

	sf::Font font;
	font.loadFromFile("arial.ttf");

	sf::Text text;
	text.setFont(font);
	text.setStyle(sf::Text::Style::Bold);

	auto drawRect = [&](int x, int y, const sf::Color& col)
	{
		rect.setPosition(x * CELL_SIZE, y * CELL_SIZE);
		rect.setFillColor(col);
		window.draw(rect);
	};

	auto drawNumber = [&](int x, int y, int n, const sf::Color& col)
	{
		text.setPosition((x + 0.35f) * CELL_SIZE, (y + 0.25f) * CELL_SIZE);
		text.setScale({ 0.75f, 0.75f });
		text.setFillColor(col);
		text.setString(std::to_string(n));
		window.draw(text);
	};

	uint16_t world[WORLD_HEIGHT][WORLD_WIDTH];
	memset(world, 0, sizeof(world));

	bool playing = true;

	srand(time(0));

	AddNew(world);
	AddNew(world);

	while (window.isOpen())
	{
		sf::Event evt;
		while (window.pollEvent(evt))
		{
			switch (evt.type)
			{
			case sf::Event::Closed: window.close(); break;
			case sf::Event::KeyPressed:
			{
				if (playing)
				{
					int dx = 0;
					int dy = 0;

					switch (evt.key.code)
					{
					case sf::Keyboard::Key::Left: dx = -1; break;
					case sf::Keyboard::Key::Right: dx = 1; break;
					case sf::Keyboard::Key::Up: dy = -1; break;
					case sf::Keyboard::Key::Down: dy = 1; break;
					}

					playing = UpdateWorld(world, dx, dy);
				}
				else
					playing = (evt.key.code == sf::Keyboard::Key::Space);
			}
			break;

			}
		}

		window.clear(sf::Color(0x202020FF));

		for (int j = 0; j < WORLD_HEIGHT; j++)
			for (int i = 0; i < WORLD_WIDTH; i++)
			{
				if (world[j][i] != 0)
				{
					drawRect(i, j, sf::Color::Red);
					drawNumber(i, j, world[j][i], sf::Color::White);
				}
			}

		if (!playing)
		{
			text.setPosition((float)SCREEN_WIDTH * 0.45f, (float)SCREEN_HEIGHT * 0.5f);
			text.setScale({ 1.0f, 1.0f });
			text.setFillColor(sf::Color::White);
			text.setString("Press SPACE to play again!");
			window.draw(text);
		}

		window.display();
	}

	return 0;
}
