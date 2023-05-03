#include <Windows.h>
#include <gl/GL.h>

#include <math.h>

#pragma comment(lib, "opengl32.lib")

#define MAP_WIDTH 15
#define MAP_HEIGHT 15

typedef struct
{
    BOOL bMine;
    BOOL bFlag;
    BOOL bOpen;
    int nCellsAround;
} Cell;

Cell map[MAP_WIDTH * MAP_HEIGHT];

int nTotalMines;
int nClosedCells;

BOOL bFailed;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

void EnableOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC);
void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC);

BOOL IsCellInMap(int x, int y)
{
    return (x >= 0) && (y >= 0) && (x < MAP_WIDTH) && (y < MAP_HEIGHT);
}

void ScreenToOpenGL(HWND hWnd, int x, int y, int* ox, int* oy)
{
    RECT rct;
    GetClientRect(hWnd, &rct);
    *ox = (int)((float)x / (float)rct.right * (float)MAP_WIDTH);
    *oy = (int)(MAP_HEIGHT - (float)y / (float)rct.bottom * (float)MAP_WIDTH);
}

void GameNew()
{
    srand(time(NULL));
    memset(map, 0, sizeof(map));

    nTotalMines = 40;
    nClosedCells = MAP_WIDTH * MAP_HEIGHT;
    bFailed = FALSE;

    for (int i = 0; i < nTotalMines; i++)
    {
        int x = rand() % MAP_WIDTH;
        int y = rand() % MAP_HEIGHT;

        if (map[y * MAP_WIDTH + x].bMine) i--;
        else
        {
            map[y * MAP_WIDTH + x].bMine = TRUE;

            for (int dx = -1; dx < 2; dx++)
                for (int dy = -1; dy < 2; dy++)
                {
                    if (IsCellInMap(x + dx, y + dy))
                        map[(y + dy) * MAP_WIDTH + (x + dx)].nCellsAround += 1;
                }
        }
    }
}

void ShowMine()
{
    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLE_FAN);
        glVertex2f(0.3f, 0.3f);
        glVertex2f(0.7f, 0.3f);
        glVertex2f(0.7f, 0.7f);
        glVertex2f(0.3f, 0.7f);
    glEnd();
}

void ShowField()
{
    glBegin(GL_TRIANGLE_STRIP);
        glColor3f(0.8f, 0.8f, 0.8f); glVertex2f(0.0f, 1.0f);
        glColor3f(0.7f, 0.7f, 0.7f); glVertex2f(1.0f, 1.0f); glVertex2f(0.0f, 0.0f);
        glColor3f(0.6f, 0.6f, 0.6f); glVertex2f(1.0f, 0.0f);
    glEnd();
}

void ShowFieldOpen()
{
    glBegin(GL_TRIANGLE_STRIP);
        glColor3f(0.3f, 0.7f, 0.3f); glVertex2f(0.0f, 1.0f);
        glColor3f(0.3f, 0.6f, 0.3f); glVertex2f(1.0f, 1.0f); glVertex2f(0.0f, 0.0f);
        glColor3f(0.3f, 0.5f, 0.3f); glVertex2f(1.0f, 0.0f);
    glEnd();
}

void Line(float x1, float y1, float x2, float y2)
{
    glVertex2f(x1, y1);
    glVertex2f(x2, y2);
}

void ShowCount(float n)
{
    glLineWidth(3.0f);
    glColor3f(1.0f, 1.0f, 0.0f);

    glBegin(GL_LINES);

    if ((n != 1.0f) && (n != 4.0f))                               Line(0.3f, 0.85, 0.7f, 0.85f);
    if ((n != 0.0f) && (n != 1.0f) && (n != 7.0f))                Line(0.3f, 0.5, 0.7f, 0.5f);
    if ((n != 1.0f) && (n != 4.0f) && (n != 7.0f))                Line(0.3f, 0.15, 0.7f, 0.15f);

    if ((n != 5.0f) && (n != 6.0f))                               Line(0.7f, 0.5f, 0.7f, 0.85f);
    if (n != 2.0f)                                                Line(0.7f, 0.5f, 0.7f, 0.15f);

    if ((n != 1.0f) && (n != 2.0f) && (n != 3.0f) && (n != 7.0f)) Line(0.3f, 0.5f, 0.3f, 0.85f);
    if ((n == 0.0f) || (n == 2.0f) || (n == 6.0f) || (n == 8.0f)) Line(0.3f, 0.5f, 0.3f, 0.15f);

    glEnd();
}

void ShowFlag()
{
    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_TRIANGLES);
        glVertex2f(0.25f, 0.75f);
        glVertex2f(0.85f, 0.5f);
        glVertex2f(0.25f, 0.25f);
    glEnd();
    
    glLineWidth(5.0f);

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_LINES);
        glVertex2f(0.25f, 0.75f);
        glVertex2f(0.25f, 0.0f);
    glEnd();
}

void OpenFields(int x, int y)
{
    if (!IsCellInMap(x, y) || map[y * MAP_WIDTH + x].bOpen)
        return;

    map[y * MAP_WIDTH + x].bOpen = TRUE;
    nClosedCells--;

    if (map[y * MAP_WIDTH + x].nCellsAround == 0)
    {
        for (int dx = -1; dx < 2; dx++)
            for (int dy = -1; dy < 2; dy++)
                OpenFields(x + dx, y + dy);
    }

    if (map[y * MAP_WIDTH + x].bMine)
    {
        bFailed = TRUE;

        for (int y = 0; y < MAP_HEIGHT; y++)
            for (int x = 0; x < MAP_WIDTH; x++)
                map[y * MAP_WIDTH + x].bOpen = TRUE;
    }
}

void GameShow()
{
    glLoadIdentity();
    glScalef(2.0f / MAP_WIDTH, 2.0f / MAP_HEIGHT, 1.0f);
    glTranslatef(-MAP_WIDTH * 0.5f, -MAP_HEIGHT * 0.5f, 0);
    for (int y = 0; y < MAP_HEIGHT; y++)
    {
        for (int x = 0; x < MAP_WIDTH; x++)
        {
            glPushMatrix();
            glTranslatef(x, y, 0);

            if (map[y * MAP_WIDTH + x].bOpen)
            {
                ShowFieldOpen();

                if (map[y * MAP_WIDTH + x].bMine)
                    ShowMine();
                else if (map[y * MAP_WIDTH + x].nCellsAround > 0)
                    ShowCount(map[y * MAP_WIDTH + x].nCellsAround);
            }
            else
            {
                ShowField();

                if (map[y * MAP_WIDTH + x].bFlag)
                    ShowFlag();
            }

            glPopMatrix();
        }
    }
}

int WINAPI WinMain(HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpCmdLine,
    int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hWnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_OWNDC;
    wcex.lpfnWndProc = WindowProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = L"Minesweeper";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;

    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hWnd = CreateWindowEx(0,
        L"Minesweeper",
        L"Minesweeper in C",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        600,
        600,
        NULL,
        NULL,
        hInstance,
        NULL);

    ShowWindow(hWnd, nCmdShow);

    EnableOpenGL(hWnd, &hDC, &hRC);

    GameNew();

    while (!bQuit)
    {
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            if (msg.message == WM_QUIT) bQuit = TRUE;
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            if (nTotalMines == nClosedCells)
                GameNew();

            GameShow();

            SwapBuffers(hDC);

            Sleep(1);
        }
    }

    DisableOpenGL(hWnd, hDC, hRC);

    DestroyWindow(hWnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
    switch (nMsg)
    {
    case WM_CLOSE: PostQuitMessage(0);
    break;

    case WM_LBUTTONDOWN:
    {
        if (bFailed) GameNew();
        else
        {
            int x, y;
            ScreenToOpenGL(hWnd, LOWORD(lParam), HIWORD(lParam), &x, &y);

            if (IsCellInMap(x, y) && !map[y * MAP_WIDTH + x].bFlag)
                OpenFields(x, y);
        }
    }
    break;

    case WM_RBUTTONDOWN:
    {
        int x, y;
        ScreenToOpenGL(hWnd, LOWORD(lParam), HIWORD(lParam), &x, &y);

        if (IsCellInMap(x, y))
            map[y * MAP_WIDTH + x].bFlag = !map[y * MAP_WIDTH + x].bFlag;
    }
    break;

    case WM_DESTROY: return 0;

    case WM_KEYDOWN:
    {
        switch (wParam)
        {
        case VK_ESCAPE: PostQuitMessage(0);
            break;
        }
    }
    break;

    default:
        return DefWindowProc(hWnd, nMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hWnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    *hDC = GetDC(hWnd);

    int nFormat = ChoosePixelFormat(*hDC, &pfd);
    SetPixelFormat(*hDC, nFormat, &pfd);

    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL(HWND hWnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hWnd, hDC);
}
