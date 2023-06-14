#include <stdio.h>
#include <stdbool.h>

#include <SDL2/SDL.h>

#define TITLE "Breakout!"
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define TARGETS_CUP 128
#define CLEAR_COLOR 0x36, 0x36, 0x36

SDL_Window* window;
SDL_Renderer* render;

bool keys[SDL_NUM_SCANCODES];
const float delta_time = 1.0f / 60.0f;

typedef struct
{
    SDL_Rect rect;
    bool redundant;
    Sint8 id;
    union
    {
        Uint8 col[4];
        Uint32 rgba;
    };
} target;

target targets[TARGETS_CUP];
int targets_size = 0;

target* make_target(int x, int y, int w, int h, Uint32 rgba)
{
    target t = (target) {
        .rect = (SDL_Rect) {
            .x=x, .y=y,
            .w=w, .h=h
        },
        .rgba = rgba,
        .id = targets_size,
        .redundant = false
    };

    targets[targets_size++] = t;
    return &targets[targets_size - 1];
}

void create();
void update();

void core_init()
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0)
    {
        fprintf(stderr, SDL_GetError());
        exit(1);
    }

    window = SDL_CreateWindow(
        TITLE,
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        SCREEN_WIDTH, SCREEN_HEIGHT,
        SDL_WINDOW_SHOWN
    );

    if (!window) exit(1);

    render = SDL_CreateRenderer(
        window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE
    );

    if (!render) exit(1);

    for (int i = 0; i < SDL_NUM_SCANCODES; i++)
        keys[i] = false;
}

void core_update()
{
    create();

    bool app_running = true;
    while (app_running)
    {
        SDL_Event e;
        while (SDL_PollEvent(&e))
        {
            switch (e.type)
            {
            case SDL_QUIT:
                app_running = false;
                break;

            case SDL_KEYDOWN:
                keys[e.key.keysym.scancode] = true;
                break;

            case SDL_KEYUP:
                keys[e.key.keysym.scancode] = false;
                break;

            }
        }

        update();

        SDL_SetRenderDrawColor(render, CLEAR_COLOR, 0xff);
        SDL_RenderClear(render);

        for (int i = 0; i < targets_size; i++)
        {
            if (!targets[i].redundant)
            {
                SDL_SetRenderDrawColor(render, targets[i].col[3], targets[i].col[2], targets[i].col[1], targets[i].col[0]);
                SDL_RenderFillRect(render, &targets[i].rect);
            }
        }

        SDL_RenderPresent(render);
    }

    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

#define TILE_WIDTH 50
#define TILE_HEIGHT 30

target* blocks[3][10];
target* bat;
target* ball;

float bat_x = 350.0f, bat_y = 550.0f;
float ball_x = 350.0f, ball_y = 500.0f;

float ball_vx = 1.0f, ball_vy = 1.0f;

void create()
{
    int x = 140, y = 150;

    for (int j = 0; j < 3; j++, y++)
    {
        for (int i = 0; i < 10; i++, x++)
        {
            blocks[j][i] = make_target(
                x + i * TILE_WIDTH, y + j * TILE_HEIGHT,
                TILE_WIDTH, TILE_HEIGHT,
                0x828282ff
            );
        }

        x = 140;
    }

    bat = make_target(bat_x, bat_y, TILE_WIDTH, TILE_HEIGHT, 0xff0000ff);
    ball = make_target(ball_x, ball_y, TILE_HEIGHT, TILE_HEIGHT, 0xffff00ff);
}

bool rect_vs_rect(SDL_Rect r1, SDL_Rect r2)
{
    return r1.x < r2.x + r2.w && r1.y < r2.y + r2.h &&
        r1.x + r1.w > r2.x && r1.y + r1.h > r2.y;
}

SDL_Rect make_rect(int x, int y, int w, int h)
{
    return (SDL_Rect){ .x=x, .y=y, .w=w, .h=h };
}

void update()
{
    if (keys[SDL_SCANCODE_LEFT]) bat_x -= 5.0f * delta_time;
    if (keys[SDL_SCANCODE_RIGHT]) bat_x += 5.0f * delta_time;

    for (int i = 0; i < targets_size; i++)
    {
        if (i != ball->id && !targets[i].redundant)
        {
            bool x = rect_vs_rect(targets[i].rect, make_rect(ball_x + ball_vx * delta_time, ball_y, TILE_HEIGHT, TILE_HEIGHT));
            bool y = rect_vs_rect(targets[i].rect, make_rect(ball_x, ball_y + ball_vy * delta_time, TILE_HEIGHT, TILE_HEIGHT));
            
            if (x && !y) ball_vx *= -1.0f;
            if (y && !x) ball_vy *= -1.0f;

            if (x && y) ball_vy *= -1.0f;

            if ((x || y) && (bat->id != i))
                targets[i].redundant = true;

            if (x && y) break;
        }
    }

    if (ball_x + ball_vx * delta_time < 0.0f) ball_vx = 1.0f;
    if (ball_y + ball_vy * delta_time < 0.0f) ball_vy = 1.0f;

    if (ball_x + ball_vx * delta_time > SCREEN_WIDTH - TILE_HEIGHT) ball_vx = -1.0f;
    if (ball_y + ball_vy * delta_time > SCREEN_HEIGHT - TILE_HEIGHT) ball_vy = -1.0f;

    ball_x += ball_vx * delta_time * 5.0f;
    ball_y += ball_vy * delta_time * 5.0f;

    ball->rect.x = ball_x;
    ball->rect.y = ball_y;
    bat->rect.x = bat_x;
    bat->rect.y = bat_y;
}

int main(int argc, char* argv[])
{
    core_init();
    core_update();
    return 0;
}
