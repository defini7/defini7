#include <windows.h>
#include <gl/gl.h>
#include <math.h>

LRESULT CALLBACK WindowProc(HWND, UINT, WPARAM, LPARAM);
void EnableOpenGL(HWND hwnd, HDC*, HGLRC*);
void DisableOpenGL(HWND, HDC, HGLRC);

typedef struct {
    float r;
    float g;
    float b;
} TColor;

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 768

TColor map[SCREEN_WIDTH * SCREEN_HEIGHT];

float fSetWidth = 11.0f;
int nMaxIterations = 100;

void InitMap()
{
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++)
    {
        TColor c;
        c.r = 0.0f;
        c.g = 0.0f;
        c.b = 0.0f;
        map[i] = c;
    }
}

void DoMandelbrotSet()
{
    float h = (fSetWidth * SCREEN_HEIGHT) / SCREEN_WIDTH;

    float xmin = -fSetWidth/2.0f;
    float ymin = -h/2.0f;

    float xmax = xmin + fSetWidth;
    float ymax = ymin + h;

    float dx = (xmax - xmin) / SCREEN_WIDTH;
    float dy = (ymax - ymin) / SCREEN_HEIGHT;

    float y = ymin;
    for (int j = 0; j < SCREEN_HEIGHT; j++)
    {
        float x = xmin;
        for (int i = 0; i < SCREEN_WIDTH; i++)
        {
            float a = x;
            float b = y;
            int n = 0;

            while (n < nMaxIterations)
            {
                float aa = a * a;
                float bb = b * b;
                float twoab = 2.0f * a * b;

                a = aa - bb + x;
                b = twoab + y;

                if (a*a + b*b > 16.0f)
                  break;

                n++;
            }

            TColor col;

            if (n == nMaxIterations)
            {
                col.r = 0.0f;
                col.g = 0.0f;
                col.b = 0.0f;
            }
            else
            {
                float c = sqrt((float)n / nMaxIterations);
                col.r = c;
                col.g = c;
                col.b = c;
            }

            map[j * SCREEN_HEIGHT + i] = col;

            x += dx;
        }
        y += dy;
    }
}

void DrawMandelbrotSet()
{
    glBegin(GL_POINTS);
    for (int x = 0; x < SCREEN_WIDTH; x++)
        for (int y = 0; y < SCREEN_HEIGHT; y++)
        {
            TColor c = map[y * SCREEN_WIDTH + x];

            glColor3f(c.r, c.g, c.b);
            glVertex2f(x, y);
        }
    glEnd();
}

int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow)
{
    WNDCLASSEX wcex;
    HWND hwnd;
    HDC hDC;
    HGLRC hRC;
    MSG msg;
    BOOL bQuit = FALSE;

    /* register window class */
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
    wcex.lpszClassName = "GLSample";
    wcex.hIconSm = LoadIcon(NULL, IDI_APPLICATION);;


    if (!RegisterClassEx(&wcex))
        return 0;

    /* create main window */
    hwnd = CreateWindowEx(0,
                          "GLSample",
                          "Mandelbrot Set",
                          WS_OVERLAPPEDWINDOW,
                          CW_USEDEFAULT,
                          CW_USEDEFAULT,
                          SCREEN_WIDTH,
                          SCREEN_HEIGHT,
                          NULL,
                          NULL,
                          hInstance,
                          NULL);

    ShowWindow(hwnd, nCmdShow);

    /* enable OpenGL for the window */
    EnableOpenGL(hwnd, &hDC, &hRC);

    glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, 1, 0);

    InitMap();

    /* program main loop */
    while (!bQuit)
    {
        /* check for messages */
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            /* handle or dispatch messages */
            if (msg.message == WM_QUIT)
            {
                bQuit = TRUE;
            }
            else
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        else
        {
            /* OpenGL animation code goes here */

            glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glPushMatrix();

            DoMandelbrotSet();
            DrawMandelbrotSet();

            glPopMatrix();

            SwapBuffers(hDC);

            Sleep (1);
        }
    }

    /* shutdown OpenGL */
    DisableOpenGL(hwnd, hDC, hRC);

    /* destroy the window explicitly */
    DestroyWindow(hwnd);

    return msg.wParam;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
        case WM_CLOSE:
            PostQuitMessage(0);
        break;

        case WM_DESTROY:
            return 0;

        case WM_KEYDOWN:
        {
            switch (wParam)
            {
                case VK_ESCAPE:
                    PostQuitMessage(0);
                break;

                case L'Q':
                    fSetWidth += 1.0f;
                break;

                case L'W':
                    fSetWidth -= 1.0f;
                break;

                case L'Z':
                    nMaxIterations++;
                break;

                case L'X':
                    nMaxIterations--;
                break;
            }

            if (fSetWidth < 10.0f)
                fSetWidth = 10.0f;
        }
        break;

        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }

    return 0;
}

void EnableOpenGL(HWND hwnd, HDC* hDC, HGLRC* hRC)
{
    PIXELFORMATDESCRIPTOR pfd;

    int iFormat;

    /* get the device context (DC) */
    *hDC = GetDC(hwnd);

    /* set the pixel format for the DC */
    ZeroMemory(&pfd, sizeof(pfd));

    pfd.nSize = sizeof(pfd);
    pfd.nVersion = 1;
    pfd.dwFlags = PFD_DRAW_TO_WINDOW |
                  PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType = PFD_TYPE_RGBA;
    pfd.cColorBits = 24;
    pfd.cDepthBits = 16;
    pfd.iLayerType = PFD_MAIN_PLANE;

    iFormat = ChoosePixelFormat(*hDC, &pfd);

    SetPixelFormat(*hDC, iFormat, &pfd);

    /* create and enable the render context (RC) */
    *hRC = wglCreateContext(*hDC);

    wglMakeCurrent(*hDC, *hRC);
}

void DisableOpenGL (HWND hwnd, HDC hDC, HGLRC hRC)
{
    wglMakeCurrent(NULL, NULL);
    wglDeleteContext(hRC);
    ReleaseDC(hwnd, hDC);
}
