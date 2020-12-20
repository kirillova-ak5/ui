#include <stdio.h>
#include <windows.h>
#include "wndproc.h"
#include "utils.h"

/* Reboot file function.
 * ARGUMENTS:
 *   IN:
 *   - window handler:
 *       HWND hWnd;
 *   OUT:
 *   - render representation:
 *       TEXTRNDDATA *trd;
 * RETURNS: None.
 */
void Reboot( HWND hWnd, TEXTRNDDATA *trd )
{
    trd->curLineInStr = 0;
    trd->xLeftUp = 0;
    trd->yLeftUp = 0;

    SetScrollPos(hWnd, SB_HORZ, 0, TRUE);
    SetScrollPos(hWnd, SB_VERT, 0, TRUE);
}

/* WM_PAINT callback function.
 * ARGUMENTS:
 *   IN:
 *   - window handler:
 *       HWND hWnd;
 *   - text representation:
 *       TEXTDATA *td;
 *   - render representation:
 *       TEXTRNDDATA *trd;
 *   - text metrics:
 *       TEXTMETRIC *tm;
 *   - mode:
 *       MODE m;
 * RETURNS: None.
 */
void OnPaint( HWND hWnd, TEXTDATA *td, TEXTRNDDATA *trd, TEXTMETRIC *tm, MODE m )
{
    int i;
    PAINTSTRUCT ps;
    BeginPaint(hWnd, &ps);

    if (m == MODE_ORIGINAL)
        for (i = trd->yLeftUp; i < min(trd->symsPerH + trd->yLeftUp, td->strCount); i++)
            TextOut(ps.hdc, 0, (i - trd->yLeftUp) * tm->tmHeight,
                    td->text + td->strOffsets[i] + trd->xLeftUp,
                    min(strTextLength(td, i) - trd->xLeftUp,
                        trd->symsPerW));
    else
    {
        int printed = 0; // how many strings are printed
        int overgone = 0;
        for (i = trd->yLeftUp; printed < trd->symsPerH && i < min(trd->symsPerH + trd->yLeftUp, td->strCount); i++)
        {
            int
                j = 0,
                strTL = strTextLength(td, i);
            int strCount = linesInCurStr(strTL, trd);
            for (j = (1 - overgone) * trd->curLineInStr/* + overgone * 0*/;
                    j < strCount - 1 &&
                    printed < trd->symsPerH; j++, printed++)
                TextOut(ps.hdc, 0, printed * tm->tmHeight,
                        td->text + td->strOffsets[i] + trd->symsPerW * j,
                        trd->symsPerW);

            for (; j == strCount - 1 && printed < trd->symsPerH; j++, printed++)
                TextOut(ps.hdc, 0, printed * tm->tmHeight,
                        td->text + td->strOffsets[i] + trd->symsPerW * j,
                        strTL - (strCount - 1) * trd->symsPerW);
            overgone = 1;
        }
    }
    EndPaint(hWnd, &ps);
}

/* WM_SIZE callback function.
 * ARGUMENTS:
 *   IN:
 *   - window handler:
 *       HWND hWnd;
 *   INOUT:
 *   - render representation:
 *       TEXTRNDDATA *trd;
 *   IN:
 *   - text metrics:
 *       TEXTMETRIC *tm;
 *   - new window metrics:
 *       int newW, newH;
 *   - mode:
 *       MODE m;
 * RETURNS: None.
 */
void OnSize( HWND hWnd, TEXTRNDDATA *trd, TEXTMETRIC *tm, int newW, int newH, MODE m )
{
    int
        oldW = trd->symsPerW,
        oldH = trd->symsPerH;
    RECT rc;

    trd->symsPerW = newW / tm->tmAveCharWidth;
    trd->symsPerH = newH / tm->tmHeight;

    if (m == MODE_ORIGINAL)
    {
        // vertical
        rc.top = 0;
        rc.bottom = max(newH, oldH);
        rc.left = (min(trd->symsPerW, oldW) - 1) * tm->tmAveCharWidth;
        rc.right = (max(trd->symsPerW, oldW) + 1) * tm->tmAveCharWidth ;
        InvalidateRect(hWnd, &rc, TRUE);

        // horizontal
        rc.top = (min(trd->symsPerH, oldH) - 1) * tm->tmHeight;
        rc.bottom = (max(trd->symsPerH, oldH) + 1) * tm->tmHeight;
        rc.left = 0;
        rc.right = newW;
        InvalidateRect(hWnd, &rc, TRUE);
    }
    else
        invalidateScreen(hWnd, trd, tm);
}

/* WM_KEYDOWN callback function.
 * ARGUMENTS:
 *   IN:
 *   - window handler:
 *       HWND hWnd;
 *   - text representation:
 *       TEXTDATA *td;
 *   INOUT:
 *   - render representation:
 *       TEXTRNDDATA *trd;
 *   IN:
 *   - text metrics:
 *       TEXTMETRIC *tm;
 *   - mode:
 *       MODE m;
 * RETURNS: None.
 */
void OnKeyDown( HWND hWnd, WPARAM wParam,
                TEXTDATA *td,
                TEXTRNDDATA *trd, TEXTMETRIC *tm, MODE m )
{
    int minScroll, maxScroll;
    GetScrollRange(hWnd, SB_VERT, &minScroll, &maxScroll);

    switch (wParam)
    {
    case VK_RIGHT:
        if (m == MODE_FORMATTED)
            break;
        trd->xLeftUp =
            (td->maxStrWidth - trd->symsPerW - 1 > 0) *
            min(trd->xLeftUp + 1, td->maxStrWidth - trd->symsPerW - 1);
        invalidateScreen(hWnd, trd, tm);
        break;
    case VK_LEFT:
        if (m == MODE_FORMATTED)
            break;
        trd->xLeftUp = max(0, trd->xLeftUp - 1);
        invalidateScreen(hWnd, trd, tm);
        break;
    case VK_UP:
        LineUp(m, td, trd);
        invalidateScreen(hWnd, trd, tm);
        break;
    case VK_DOWN:
        LineDown(m, td, trd);
        invalidateScreen(hWnd, trd, tm);
        break;
    case VK_PRIOR:
        PageUp(m, td, trd);
        invalidateScreen(hWnd, trd, tm);
        break;
    case VK_NEXT:
        PageDown(m, td, trd);
        invalidateScreen(hWnd, trd, tm);
        break;
    case VK_HOME:
        if (!GetAsyncKeyState(VK_CONTROL))
        {
            if (m == MODE_FORMATTED)
                break;
            trd->xLeftUp = 0;
        }
        else
        {
            trd->yLeftUp = 0;
            trd->curLineInStr = 0;
        }
        invalidateScreen(hWnd, trd, tm);
        break;
    case VK_END:
        if (!GetAsyncKeyState(VK_CONTROL))
        {
            int maxStrW = findMaxStrWidth(td, trd->yLeftUp, min(trd->yLeftUp + trd->symsPerH, td->strCount));
            trd->xLeftUp = max(0, maxStrW - trd->symsPerW);
        }
        else
        {
            int yUp, curLine;
            endOfDocument(m, td, trd, &yUp, &curLine);
            trd->yLeftUp = yUp;
            trd->curLineInStr = curLine;
        }
        invalidateScreen(hWnd, trd, tm);
        break;
    }

    SetScrollPos(hWnd, SB_VERT, textHeightToVScroll(td, trd, minScroll, maxScroll), TRUE);
    SetScrollPos(hWnd, SB_HORZ, textWidthToHScroll(td, trd, minScroll, maxScroll), TRUE);
}

/* WM_SCROLL->VSCROLL callback function.
 * ARGUMENTS:
 *   IN:
 *   - window handler:
 *       HWND hWnd;
 *   - scroll position:
 *       WPARAM wParam;
 *   - text representation:
 *       TEXTDATA *td;
 *   INOUT:
 *   - render representation:
 *       TEXTRNDDATA *trd;
 *   IN:
 *   - text metrics:
 *       TEXTMETRIC *tm;
 *   - mode:
 *       MODE m;
 * RETURNS: None.
 */
void OnVScroll( HWND hWnd, WPARAM wParam, TEXTDATA *td, TEXTRNDDATA *trd, TEXTMETRIC *tm, MODE m )
{
    int minScroll, maxScroll;
    int pos = HIWORD(wParam);
    int oldPos = GetScrollPos(hWnd, SB_VERT);
    GetScrollRange(hWnd, SB_VERT, &minScroll, &maxScroll);

    switch (LOWORD(wParam))
    {
    case SB_THUMBTRACK:
        if (oldPos != pos)
        {
            int endYLeftUp, endCurLine;
            endOfDocument(m, td, trd, &endYLeftUp, &endCurLine);

            trd->yLeftUp = vScrollToTextHeight(endYLeftUp, pos, minScroll, maxScroll);

            if (pos == maxScroll)
                trd->curLineInStr = endCurLine;
        }
        invalidateScreen(hWnd, trd, tm);
        SetScrollPos(hWnd, SB_VERT, textHeightToVScroll(td, trd, minScroll, maxScroll), TRUE);
        break;
    case SB_LINEDOWN:
        LineDown(m, td, trd);
        invalidateScreen(hWnd, trd, tm);
        SetScrollPos(hWnd, SB_VERT, textHeightToVScroll(td, trd, minScroll, maxScroll), TRUE);
        break;
    case SB_LINEUP:
        LineUp(m, td, trd);
        invalidateScreen(hWnd, trd, tm);
        SetScrollPos(hWnd, SB_VERT, textHeightToVScroll(td, trd, minScroll, maxScroll), TRUE);
        break;
    case SB_PAGEUP:
        PageUp(m, td, trd);
        invalidateScreen(hWnd, trd, tm);
        SetScrollPos(hWnd, SB_VERT, textHeightToVScroll(td, trd, minScroll, maxScroll), TRUE);
        break;
    case SB_PAGEDOWN:
        PageDown(m, td, trd);
        invalidateScreen(hWnd, trd, tm);
        SetScrollPos(hWnd, SB_VERT, textHeightToVScroll(td, trd, minScroll, maxScroll), TRUE);
        break;
    }
}

/* WM_SCROLL->HSCROLL callback function.
 * ARGUMENTS:
 *   IN:
 *   - window handler:
 *       HWND hWnd;
 *   - scroll position:
 *       WPARAM wParam;
 *   - text representation:
 *       TEXTDATA *td;
 *   INOUT:
 *   - render representation:
 *       TEXTRNDDATA *trd;
 *   IN:
 *   - text metrics:
 *       TEXTMETRIC *tm;
 *   - mode:
 *       MODE m;
 * RETURNS: None.
 */
void OnHScroll( HWND hWnd, WPARAM wParam, TEXTDATA *td, TEXTRNDDATA *trd, TEXTMETRIC *tm, MODE m )
{
    int minScroll, maxScroll;
    int pos = HIWORD(wParam);

    if (m == MODE_FORMATTED)
        return;

    GetScrollRange(hWnd, SB_HORZ, &minScroll, &maxScroll);

    switch (LOWORD(wParam))
    {
    case SB_THUMBTRACK:
        trd->xLeftUp = hScrollToTextWidth(td, trd, pos, minScroll, maxScroll);
        invalidateScreen(hWnd, trd, tm);
        SetScrollPos(hWnd, SB_HORZ, textWidthToHScroll(td, trd, minScroll, maxScroll), TRUE);
        break;
    case SB_LINEDOWN:
        trd->xLeftUp =
            (td->maxStrWidth - trd->symsPerW - 1 > 0) *
            min(trd->xLeftUp + 1, td->maxStrWidth - trd->symsPerW - 1);
        invalidateScreen(hWnd, trd, tm);
        SetScrollPos(hWnd, SB_HORZ, textWidthToHScroll(td, trd, minScroll, maxScroll), TRUE);
        break;
    case SB_LINEUP:
        trd->xLeftUp = max(0, trd->xLeftUp - 1);
        invalidateScreen(hWnd, trd, tm);
        SetScrollPos(hWnd, SB_HORZ, textWidthToHScroll(td, trd, minScroll, maxScroll), TRUE);
        break;
    }
}

/*  This function is called by the Windows function DispatchMessage()  */
LRESULT CALLBACK WindowProcedure( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    static TEXTDATA td = {0};
    static TEXTRNDDATA trd = {0};
    static TEXTMETRIC tm;
    static HFONT hFont;
    static int minScroll = 0, maxScroll = 2000;
    static int fontSize = 26;
    static MODE m = MODE_ORIGINAL;
    static OPENFILENAME ofn = {0};

    char szFile[100];

    HDC hDC;
    HMENU hMenu;
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = szFile ;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFileTitle = NULL ;
    ofn.nMaxFileTitle = 0 ;

    switch (message)                  /* handle the msg */
    {
    case WM_CREATE:
        hDC = GetDC(hWnd);

        hFont = CreateFont(
                             fontSize,
                             0,
                             0,
                             0,
                             FW_NORMAL,
                             FALSE,
                             FALSE,
                             FALSE,
                             ANSI_CHARSET,
                             OUT_TT_PRECIS,
                             CLIP_CHARACTER_PRECIS,
                             CLEARTYPE_QUALITY,
                             FIXED_PITCH,
                             TEXT("Consolas"));

        SelectObject(hDC, hFont);
        SetTextColor(hDC, RGB(0, 255, 0));
        SetBkColor(hDC, RGB(0, 0, 0));

        GetTextMetrics(hDC, &tm);
        readFile(((CREATESTRUCT *)lParam)->lpCreateParams, &td);
        SetScrollRange(hWnd, SB_VERT, minScroll, maxScroll, TRUE);
        SetScrollRange(hWnd, SB_HORZ, minScroll, maxScroll, TRUE);
        Reboot(hWnd, &trd);
        break;
    case WM_DESTROY:
        freeTextData(&td);
        PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
        break;
    case WM_PAINT:
        OnPaint(hWnd, &td, &trd, &tm, m);
        break;
    case WM_SIZE:
        OnSize(hWnd, &trd, &tm, LOWORD(lParam), HIWORD(lParam), m);
        break;
    case WM_CLOSE:
        freeTextData(&td);
        PostQuitMessage(0);       /* send a WM_QUIT to the message queue */
        break;
    case WM_KEYDOWN:
        OnKeyDown(hWnd, wParam, &td, &trd, &tm, m);
        break;
    case WM_VSCROLL:
        OnVScroll(hWnd, wParam, &td, &trd, &tm, m);
        break;
    case WM_HSCROLL:
        OnHScroll(hWnd, wParam, &td, &trd, &tm, m);
        break;
    case WM_COMMAND:
        hMenu = GetMenu(hWnd);
        switch (LOWORD(wParam))
        {
        case MENU_ORIGINAL:
            CheckMenuItem(hMenu, MENU_ORIGINAL, MF_CHECKED);
            CheckMenuItem(hMenu, MENU_FORMATTED, MF_UNCHECKED);
            m = MODE_ORIGINAL;
            ShowScrollBar(hWnd, SB_HORZ, TRUE);
            invalidateScreen(hWnd, &trd, &tm);
            break;
        case MENU_FORMATTED:
            CheckMenuItem(hMenu, MENU_ORIGINAL, MF_UNCHECKED);
            CheckMenuItem(hMenu, MENU_FORMATTED, MF_CHECKED);
            ShowScrollBar(hWnd, SB_HORZ, FALSE);
            m = MODE_FORMATTED;
            trd.curLineInStr = 0;
            invalidateScreen(hWnd, &trd, &tm);
            break;
        case MENU_OPEN:
            GetOpenFileName(&ofn);
            if (!readFile(ofn.lpstrFile, &td))
                MessageBox(hWnd, "Error while opening", NULL, MB_OK);
            Reboot(hWnd, &trd);
            invalidateScreen(hWnd, &trd, &tm);
            break;
        }
        break;
    case WM_QUIT:
        freeTextData(&td);
        break;
    default:                      /* for msg that we don't deal with */
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}
