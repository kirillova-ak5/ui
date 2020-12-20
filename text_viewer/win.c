#include <stdio.h>
#include "win.h"

BOOL initWnd( HINSTANCE hInst, LPSTR lpszArgument,
              int nCmdShow, WNDPROC wndProc, HBRUSH *hBr )
{
    HWND hWnd;               /* This is the handle for our window */
    WNDCLASSEX wincl;// = {0};        /* Data structure for the windowclass */
    LPCSTR szClassName = "Woof";

    /* The Window structure */
    wincl.hInstance = hInst;
    wincl.lpszClassName = szClassName;
    wincl.lpfnWndProc = wndProc;      /* This function is called by windows */
    wincl.style = CS_OWNDC;                 /* Catch double-clicks */
    wincl.cbSize = sizeof(WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wincl.hIcon = NULL;//LoadIcon (NULL, IDI_APPLICATION);
    wincl.hIconSm = NULL;//LoadIcon (NULL, IDI_APPLICATION);
    wincl.hCursor = LoadCursor(NULL, IDC_ARROW);
    wincl.lpszMenuName = szClassName;
    wincl.cbClsExtra = 0;
    wincl.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */

    *hBr = CreateSolidBrush(RGB(0, 0, 0));
    wincl.hbrBackground = *hBr;
    /* Register the window class, and if it fails quit the program */
    if (!RegisterClassEx (&wincl))
        return FALSE;

    //HMENU hMenu = Load<em

    /* The class is registered, let's create the program*/
    hWnd = CreateWindowEx (
           0,                   /* Extended possibilites for variation */
           szClassName,         /* Classname */
           "Viewer",     /* Title Text */
           WS_OVERLAPPEDWINDOW |
           WS_VSCROLL |
           WS_HSCROLL, /* default window */
           CW_USEDEFAULT,       /* Windows decides the position */
           CW_USEDEFAULT,       /* where the window ends up on the screen */
           CW_USEDEFAULT,                 /* The programs width */
           CW_USEDEFAULT,                 /* and height in pixels */
           NULL,        /* The window is a child-window to desktop */
           NULL,                /* No menu */
           hInst,               /* Program Instance handler */
           lpszArgument
           );

    if (hWnd == NULL)
        return FALSE;

    /* Make the window visible on the screen */
    UpdateWindow(hWnd);
    ShowWindow(hWnd, nCmdShow);

    return TRUE;
}

int runMsgLoop( HBRUSH hBr )
{
    MSG msg;
    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&msg, NULL, 0, 0))
    {
        /* Translate virtual-key msg into character msg */
        TranslateMessage(&msg);
        /* Send message to WindowProcedure */
        DispatchMessage(&msg);
    }

    DeleteObject((HGDIOBJ)hBr);

    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    return msg.wParam;
}
