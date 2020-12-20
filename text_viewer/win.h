#ifndef __WIN_H_INCLUDED
#define __WIN_H_INCLUDED

#include <windows.h>

/* Initialize window function.
 * ARGUMENTS:
 *   IN:
 *   - standard WinMain arguments.
 *   OUT:
 *   - new brush:
 *       HBRUSH *hBr;
 * RETURNS:
 *   (BOOL) TRUE if success, TRUE otherwise.
 */
BOOL initWnd( HINSTANCE hInst, LPSTR lpszArgument,
              int nCmdShow, WNDPROC wndProc, HBRUSH *hBr );

/* Run message loop function.
 * ARGUMENTS:
 *   IN:
 *   - background brush to delete in the end:
 *       HBRUSH hBr;
 * RETURNS:
 *   (int) system exit code.
 */
int runMsgLoop( HBRUSH hBr );

#endif // WIN_H_INCLUDED
